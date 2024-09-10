#include "pch.h"
#include "Game/Objects/Tank/TankCannon.h"
#include "Framework/Resources.h"
#include "Framework/InputManager.h"

#include "Libraries/MyLib/Math.h"

// コンストラクタ
TankCannon::TankCannon(
	IComponent* parent,
	const DirectX::SimpleMath::Vector3& initialPosition,
	const float& initialAngleRL
)
	:
	m_parent{ parent },
	m_graphics{Graphics::GetInstance()},
	m_initialPosition{ initialPosition },
	m_initialAngle{ initialAngleRL },
	m_currentPosition{},
	m_currentAngleRL{},
	m_tankParts{},
	m_model{},
	m_worldMatrix{},
	m_cannonAngle{},
	m_currentAngleUD{},
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
}

// 更新処理
void TankCannon::Update(
	float elapsedTime,
	const DirectX::SimpleMath::Vector3& currentPosition,
	const float& currentAngleRL
)
{
	UNREFERENCED_PARAMETER(elapsedTime);
	using namespace DirectX;

	// 現在の位置を更新する
	m_currentPosition = currentPosition;
	// 現在の回転角を更新する
	m_currentAngleRL = currentAngleRL;

	// キーボードステートの取得
	const auto& keyboardState = InputManager::GetInstance()->GetKeyboardState();
	// マウスステートの取得
	const auto& mouseState = InputManager::GetInstance()->GetMouseState();
	const auto& mouseTracker = InputManager::GetInstance()->GetMouseTracker();

	if (m_tankType == Type::PLAYER)
	{
		// 砲身の上下
		/*if (keyboardState.Up)
		{
			m_cannonAngle += DirectX::XMConvertToRadians(0.5f);
		}
		else if (keyboardState.Down)
		{
			m_cannonAngle -= DirectX::XMConvertToRadians(0.5f);
		}*/

		// 最初の回転角を設定
		m_cannonAngle = DirectX::XMConvertToRadians(40.0f);
		// マウス座標に応じて回転
		m_cannonAngle -= DirectX::XMConvertToRadians(static_cast<float>(mouseState.y) / 10.0f);

		// 砲身の向きを制限する
		m_cannonAngle = mylib::Clamp(m_cannonAngle, CANON_ANGLEUD_MIN, CANON_ANGLEUD_MAX);
		m_currentAngleUD = m_cannonAngle;

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
							// 経過時間のリセット
							m_tank->GetCannonBall()->ResetElapsedTime();
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
}

// 自身を描画しない描画処理(Tank用)
void TankCannon::Render()
{
	using namespace DirectX::SimpleMath;

	// ワールド行列を生成する
	m_worldMatrix = Matrix::CreateScale(0.5f);
	m_worldMatrix *= Matrix::CreateRotationX(m_cannonAngle);
	m_worldMatrix *= Matrix::CreateTranslation(Vector3(0.0f, 0.0f, -0.3f));
	m_worldMatrix *= Matrix::CreateRotationY(m_currentAngleRL + m_initialAngle);
	m_worldMatrix *= Matrix::CreateTranslation(m_currentPosition + m_initialPosition);
	
	// 「砲身」を描画する
	m_graphics->DrawModel(m_model, m_worldMatrix);

	// プリミティブ描画を開始する
	m_graphics->DrawPrimitiveBegin(m_graphics->GetViewMatrix(), m_graphics->GetProjectionMatrix());

	// 照準の描画
	Matrix matrix = Matrix::CreateRotationX(m_cannonAngle) * Matrix::CreateRotationY(m_currentAngleRL + m_initialAngle);
	m_graphics->DrawLine(GetMuzzlePosition(), { matrix.Forward()}, DirectX::Colors::Red);
	
	// プリミティブ描画を終了する
	m_graphics->DrawPrimitiveEnd();
}

// 終了処理
void TankCannon::Finalize()
{
}

void TankCannon::Shoot(IBullet* bullet)
{
	// 「砲弾」位置を設定する
	//bullet->SetPosition(m_currentPosition);
	bullet->SetPosition(this->GetMuzzlePosition());
	// コライダー座標の更新
	bullet->SetColliderPosition(this->GetMuzzlePosition());
	// 「砲弾」初期左右角を設定する
	bullet->SetAngleRL(m_currentAngleRL);
	// 「砲弾」初期上下角を設定する
	bullet->SetAngleUD(m_currentAngleUD);
	// 「砲弾」を発射する
	bullet->SetBulletState(IBullet::FLYING);
}

// 砲塔の先端座標を求める
DirectX::SimpleMath::Vector3 TankCannon::GetMuzzlePosition()
{
	using namespace DirectX::SimpleMath;

	Vector3 position = m_currentPosition + m_initialPosition;
	float angle = m_currentAngleRL + m_initialAngle;

	DirectX::SimpleMath::Matrix rotationX = DirectX::SimpleMath::Matrix::CreateRotationX(m_cannonAngle);
	DirectX::SimpleMath::Matrix rotationY = DirectX::SimpleMath::Matrix::CreateRotationY(angle);
	DirectX::SimpleMath::Matrix combinedRotation = rotationX * rotationY;

	DirectX::SimpleMath::Vector3 muzzleOffset = DirectX::SimpleMath::Vector3(0.0f, 0.0f, -0.8f);

	// 軸の位置に回転を適用して砲身の先端の座標を求める
	return DirectX::SimpleMath::Vector3::Transform(muzzleOffset, combinedRotation) + position;
}