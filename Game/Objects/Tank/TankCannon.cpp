#include "pch.h"
#include "Game/Objects/Tank/TankCannon.h"
#include "Framework/Resources.h"
#include "Framework/InputManager.h"

#include "Game/Objects/Stage/Wall.h"
#include "Game/UserInterface/DrawTexture.h"
#include "Libraries/MyLib/Math.h"
#include "Libraries/MyLib/DebugLog.h"
#include "Framework/Resources.h"

// コンストラクタ
TankCannon::TankCannon(
	IComponent* parent,
	const DirectX::SimpleMath::Vector3& initialPosition,
	const float& initialAngle
)
	:
	m_parent{ parent },
	m_graphics{Graphics::GetInstance()},
	m_initialPosition{ initialPosition },
	m_initialAngle(DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3::Up, initialAngle)),
	m_currentPosition{},
	m_currentAngle{},
	m_tankParts{},
	m_model{},
	m_worldMatrix{},
	m_cannonAngle{},
	m_shotTimer(SHOT_INTERVAL),
	m_tankType{}
{
	// 砲塔へのポインタを取得する
	m_tank = dynamic_cast<Tank*>(parent->GetParent()->GetParent());
}

// デストラクタ
TankCannon::~TankCannon()
{
	Finalize();
}

// 初期化処理
void TankCannon::Initialize(Type type)
{
	using namespace DirectX::SimpleMath;

	// タイプの確定
	m_tankType = type;

	// モデル情報の受け取り
	m_model = Resources::GetInstance()->GetTankCannonModel();

	// 画像表示クラス
	m_drawTexture = std::make_unique<DrawTexture>();

	// 初期画像読み込み
	m_drawTexture->SetTexture(Resources::GetInstance()->GetTargetTexture());
}

// 更新処理
void TankCannon::Update(
	float elapsedTime,
	const DirectX::SimpleMath::Vector3& currentPosition,
	const DirectX::SimpleMath::Quaternion& currentAngle
)
{
	using namespace DirectX::SimpleMath;

	UNREFERENCED_PARAMETER(elapsedTime);

	// マウスステートの取得
	const auto& mouseState = InputManager::GetInstance()->GetMouseState();

	if (m_tankType == Type::PLAYER)
	{
		//マウスの移動量を取得して回転させる
		//float cannonAngle = 0.0f;
		//cannonAngle -= static_cast<float>(mouseState.y)/* * 0.001f*/;
		//m_cannonAngle *= Quaternion::CreateFromYawPitchRoll(0.0f, DirectX::XMConvertToRadians(cannonAngle), 0.0f);
		// 砲身の向きを制限する
		//m_cannonAngle.x = mylib::Clamp(m_cannonAngle.x, CANON_ANGLEUD_MIN, CANON_ANGLEUD_MAX);

		// マウスの移動からX軸回転角を計算
		float rotationX = static_cast<float>(mouseState.y) * 0.001f;
		// 現在の砲身角度をクォータニオンからオイラー角に変換
		DirectX::SimpleMath::Vector3 eulerAngles = m_cannonAngle.ToEuler();
		// 砲身のX軸回転を更新
		eulerAngles.x -= rotationX;
		// X軸の回転範囲をクランプ（範囲制限）
		eulerAngles.x = mylib::Clamp(eulerAngles.x, CANON_ANGLEUD_MIN, CANON_ANGLEUD_MAX);
		// クランプされたオイラー角をクォータニオンに変換して適用
		m_cannonAngle = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(eulerAngles.y, eulerAngles.x, eulerAngles.z);
		

		// 弾の発射
		if (mouseState.leftButton)
		{
			// 発射タイマーが0.0より大きい場合は発射タイマーを減らす
			if (m_shotTimer > 0.0f)
			{
				// タイマーを減らす
				m_shotTimer -= elapsedTime;
			}
			else
			{
				// 「連射弾」を発射する
				switch (m_tank->GetBulletType())
				{
					case Tank::BulletType::BULLET:
						for (auto& bullet : m_tank->GetBullets())
						{
							// 使用されていない弾は発射できる
							if (bullet->GetBulletState() == IBullet::UNUSED)
							{
								//「連射弾」を発射する
								Shoot(bullet.get());
								// 発射砲弾数をインクリメントする
								//m_shotBulletNumber++;
								break;
							}
						}
						break;
					case Tank::BulletType::CANNONBALL:
						// 「砲弾」を発射する
						if (m_tank->GetCannonBall()->GetBulletState() == IBullet::UNUSED)
						{
							// 「砲弾」を発射する
							Shoot(m_tank->GetCannonBall().get());
							m_tank->ShakeCamera();
						}
						break;
				}
				// 初期値を設定する
				m_shotTimer = SHOT_INTERVAL;
			}
		}
	}

	// 敵の弾発射処理
	if (m_tankType == Type::ENEMY)
	{
		// 発射タイマーが0.0より大きい場合は発射タイマーを減らす
		if (m_shotTimer > 0.0f)
		{
			// タイマーを減らす
			m_shotTimer -= elapsedTime;
		}
		else
		{
			for (auto& bullet : m_tank->GetBullets())
			{
				// 使用されていない弾は発射できる
				if (bullet->GetBulletState() == IBullet::UNUSED)
				{
					//「連射弾」を発射する
					//Shoot(bullet.get());
					// 発射砲弾数をインクリメントする
					//m_shotBulletNumber++;
					break;
				}
			}
			// 初期値を設定する
			m_shotTimer = ENEMY_SHOT_INTERVAL;
		}
	}

	// 現在の位置を更新する
	m_currentPosition = currentPosition + m_initialPosition;
	// 現在の回転角を更新する
	m_currentAngle = currentAngle * m_initialAngle;
}

// 描画処理
void TankCannon::Render()
{
	using namespace DirectX::SimpleMath;

	// ワールド行列を生成する
	m_worldMatrix = Matrix::CreateScale(0.5f);
	//m_worldMatrix *= Matrix::CreateRotationX(m_cannonAngle);
	
	m_worldMatrix *= Matrix::CreateFromQuaternion(m_cannonAngle);
	m_worldMatrix *= Matrix::CreateTranslation(Vector3(0.0f, 0.0f, -0.3f));
	m_worldMatrix *= Matrix::CreateFromQuaternion(m_currentAngle);
	m_worldMatrix *= Matrix::CreateTranslation(m_currentPosition);
	
	// 「砲身」を描画する
	m_graphics->DrawModel(m_model, m_worldMatrix);


	// プリミティブ描画を開始する
	m_graphics->DrawPrimitiveBegin(m_graphics->GetViewMatrix(), m_graphics->GetProjectionMatrix());

	// 照準の描画
	Quaternion rotation = m_cannonAngle * m_currentAngle;
	Matrix matrix = Matrix::CreateFromQuaternion(rotation);
	//m_graphics->DrawLine(GetMuzzlePosition(), { matrix.Forward() * 10.0f}, DirectX::Colors::Red);
	
	// プリミティブ描画を終了する
	m_graphics->DrawPrimitiveEnd();


	if (m_tankType == Type::ENEMY) { return; }
	// Rayを飛ばして着弾方向の表示
	// Rayの距離設定
	float maxDistance = 5.0f;
	// Rayの作成
	Ray ray{ this->GetMuzzlePosition(), matrix.Forward() };
	// 壁のボックスコライダーとの衝突判定を取る
	std::vector<Wall*> walls = m_tank->GetWalls();
	Vector3 hitPosition = Vector3::Zero;
	for (auto& wall : walls)
	{
		float distance = 0.0f;
		// 壁との衝突判定
		bool isHit = ray.Intersects(*wall->GetBoundingBox(), distance);
		// 射程範囲外かつ当たっている
		if (distance <= maxDistance && isHit)
		{
			// 衝突点計算
			hitPosition = Vector3{ ray.position + ray.direction * distance - ray.direction * 1.5f};
			mylib::DebugLog(hitPosition);
			// 赤い照準を出す
			m_drawTexture->SetTexture(Resources::GetInstance()->GetTargetLockTexture());
			break;
		}
		// 射程範囲外または当たっていない
		else
		{
			// 照準画像の表示場所計算
			hitPosition = Vector3{ ray.position + ray.direction * maxDistance - ray.direction * 1.5f };
			mylib::DebugLog("out of range");
			// 黒い照準を出す
			m_drawTexture->SetTexture(Resources::GetInstance()->GetTargetTexture());
		}
	}
	// 照準画像の表示
	m_drawTexture->Render(hitPosition);
}

// 終了処理
void TankCannon::Finalize()
{
}

void TankCannon::Shoot(IBullet* bullet)
{
	// 「砲弾」位置を設定する
	bullet->SetPosition(this->GetMuzzlePosition());
	// コライダー座標の更新
	bullet->SetColliderPosition(this->GetMuzzlePosition());
	// 「砲弾」角度を設定する
	bullet->SetAngle(m_currentAngle * m_cannonAngle);
	// 「砲弾」を発射する
	bullet->SetBulletState(IBullet::FLYING);
}

// 砲身の先端座標を求める
DirectX::SimpleMath::Vector3 TankCannon::GetMuzzlePosition()
{
	using namespace DirectX::SimpleMath;

	// 砲身の先端に対するオフセットベクトル
	DirectX::SimpleMath::Vector3 muzzleOffset = DirectX::SimpleMath::Vector3(0.0f, 0.0f, -0.8f);

	// Quaternion から Matrix を作成して Transform を適用
	Matrix rotationMatrix = Matrix::CreateFromQuaternion(m_cannonAngle * m_currentAngle);

	// 回転をオフセットに適用し、砲身の先端座標を計算
	return Vector3::Transform(muzzleOffset, rotationMatrix) + m_currentPosition;
}