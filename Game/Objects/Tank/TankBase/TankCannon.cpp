#include "pch.h"
#include "Game/Objects/Tank/TankBase/TankCannon.h"
#include "Game/Objects/Tank/TankBase/Tank.h"

#include "Game/UserInterface/DrawTexture.h"
#include "Game/Objects/Stage/Wall.h"

#include "Game/Objects/Bullet/Bullet.h"
#include "Game/Objects/Bullet/CannonBall.h"

#include "Libraries/MyLib/DebugLog.h"

//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
TankCannon::TankCannon(
	Tank* tank,
	const DirectX::SimpleMath::Vector3& initialPosition,
	const float& initialAngle
)
	:
	m_graphics{ m_graphics = Graphics::GetInstance() },
	m_initialPosition{ initialPosition },
	m_initialRotation{ DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3::Up, initialAngle) },
	m_currentPosition{},
	m_currentRotation{},
	m_tankParts{},
	m_worldMatrix{},
	m_model{},
	m_cannonRotation{},
	m_bullets{},
	m_cannonBall{},
	m_bulletType{},
	m_reloadCount{},
	m_isReload{},
	m_reloadBulletType{},
	m_drawTexture{},
	m_tank{},
	m_walls{}
{
	// 戦車情報の受け取り
	m_tank = tank;
}

//---------------------------------------------------------
// デストラクタ
//---------------------------------------------------------
TankCannon::~TankCannon()
{
}

//---------------------------------------------------------
// 初期化処理
//---------------------------------------------------------
void TankCannon::Initialize()
{
	// モデルの取得
	m_model = Resources::GetInstance()->GetTankCannonModel();

	// 戦車に砲身情報を渡す
	m_tank->SetCannon(this);

	// 連射弾配列を作成する
	//m_bullets.resize(20);
	//// 配列に連射弾を格納する
	//for (int index = 0; index < 20; index++)
	//{
	//	// 連射弾を生成する
	//	m_bullets[index] = std::make_unique<Bullet>(IBullet::UNUSED);
	//	// 連射弾を初期化する
	//	m_bullets[index]->Initialize();
	//}
	/// 連射弾の生成
	for (int i = 0; i < 20; i++)
	{
		m_bullets.push_back(std::make_unique<Bullet>(IBullet::UNUSED));
		m_bullets[i]->Initialize();
	}

	// 砲弾を生成する
	m_cannonBall = std::make_unique<CannonBall>(IBullet::UNUSED);
	m_cannonBall->Initialize();

	// テクスチャ描画クラスの生成
	m_drawTexture = std::make_unique<DrawTexture>();
	// 初期画像読み込み
	m_drawTexture->SetTexture(Resources::GetInstance()->GetTargetTexture());

	// 最初に発射できる弾を砲弾に設定する
	m_bulletType = BulletType::CANNONBALL;
	m_reloadBulletType = BulletType::CANNONBALL;

	// デバッグ用モデルの描画
	auto context = m_graphics->GetDeviceResources()->GetD3DDeviceContext();
	m_box = DirectX::GeometricPrimitive::CreateBox(context, DirectX::SimpleMath::Vector3(0.1f,0.1f,0.1f));
}

//---------------------------------------------------------
// 更新処理
//---------------------------------------------------------
void TankCannon::Update(
	float elapsedTime,
	const DirectX::SimpleMath::Vector3& currentPosition,
	const DirectX::SimpleMath::Quaternion& currentRotation
)
{
	// 現在位置の更新
	m_currentPosition = currentPosition + m_initialPosition;
	m_currentRotation = currentRotation * m_initialRotation;

	// 弾の更新
	for (auto& bullet : m_bullets)
	{
		bullet->Update(elapsedTime);
	}
	m_cannonBall->Update(elapsedTime);

	// タイマーを減らす
	if (m_shotTimer > 0.0f)
	{
		m_shotTimer -= elapsedTime;
	}

	// リロード処理
	Reload(elapsedTime);
}

//---------------------------------------------------------
// 描画処理
//---------------------------------------------------------
void TankCannon::Render()
{
	using namespace DirectX::SimpleMath;

	// 弾の描画
	for (auto& bullet : m_bullets)
	{
		bullet->Render();
	}
	m_cannonBall->Render();

	// ワールド行列の生成
	m_worldMatrix = Matrix::CreateScale(0.09f) *
		Matrix::CreateFromQuaternion(m_cannonRotation) *
		Matrix::CreateTranslation(Vector3(0.0f, 0.0f, -0.0f)) *
		Matrix::CreateFromQuaternion(m_currentRotation) *
		Matrix::CreateTranslation(m_currentPosition);

	// 「砲身」の描画
	m_graphics->DrawModel(m_model, m_worldMatrix);

	// 照準の描画
	DisplaySight();

	// デバッグ用のモデルの描画(消しておく)
	auto view = m_graphics->GetViewMatrix();
	auto proj = m_graphics->GetProjectionMatrix();
	Matrix boxMatrix = Matrix::CreateTranslation(this->GetMuzzlePosition());
	//m_box->Draw(boxMatrix, view, proj, DirectX::Colors::Red);
}

//---------------------------------------------------------
// 終了処理
//---------------------------------------------------------
void TankCannon::Finalize()
{
}

//---------------------------------------------------------
// 砲身の回転
//---------------------------------------------------------
void TankCannon::RotateCannon(float angle)
{
	using namespace DirectX::SimpleMath;

	// クォータニオンに変換して適用
	m_cannonRotation = Quaternion::CreateFromYawPitchRoll(0.0f, angle, 0.0f);
}

//---------------------------------------------------------
// 弾の発射処理
//---------------------------------------------------------
void TankCannon::ShootBullet(IBullet* bullet)
{
	// 「砲弾」位置を設定する
	bullet->SetPosition(this->GetMuzzlePosition());
	// コライダー座標の更新
	bullet->SetColliderPosition(this->GetMuzzlePosition());
	// 「砲弾」角度を設定する
	bullet->SetRotation(m_cannonRotation * m_currentRotation);
	// 「砲弾」を発射する
	bullet->SetBulletState(IBullet::FLYING);
}

//---------------------------------------------------------
// 弾の発射呼び出し
//---------------------------------------------------------
void TankCannon::Shoot()
{
	if (m_shotTimer > 0.0f)
		return;

	switch (m_bulletType)
	{
		// 「連射弾」を発射する
		case BulletType::BULLET:
			for (auto& bullet : m_bullets)
			{
				// 使用されていない弾は発射できる
				if (bullet->GetBulletState() == IBullet::UNUSED)
				{
					//「連射弾」を発射する
					ShootBullet(bullet.get());
					// 発射砲弾数をインクリメントする
					//m_shotBulletNumber++;
					break;
				}
			}
			break;
		// 「砲弾」を発射する
		case BulletType::CANNONBALL:
			if (m_cannonBall->GetBulletState() == IBullet::UNUSED)
			{
				// 「砲弾」を発射する
				ShootBullet(m_cannonBall.get());
			}
			break;
	}
	// 発射インターバルを設定する
	m_shotTimer = SHOT_INTERVAL;
}

//---------------------------------------------------------
// 弾の変更
//---------------------------------------------------------
void TankCannon::ChangeBullet()
{
	// リロード開始
	if (m_isReload) { return; }
	m_bulletType = (m_bulletType == BulletType::CANNONBALL) ? BulletType::BULLET : BulletType::CANNONBALL;
}


//---------------------------------------------------------
// リロード開始
//---------------------------------------------------------
void TankCannon::StartReload()
{
	// リロード開始
	if (m_isReload) { return; }

	// どの弾をリロードするか
	switch (m_bulletType)
	{
		// 連射弾
		case BulletType::BULLET:
			for (auto& bullet : m_bullets)
			{
				// 弾が1発でも使用されていたらリロード可能
				if (bullet->GetBulletState() == IBullet::USED)
				{
					m_reloadCount = BULLET_RELOAD_TIME;
					m_reloadBulletType = BulletType::BULLET;
					m_isReload = true;
					mylib::DebugLog("連射弾のリロード開始");
					return;
				}
			}
			break;
		// 砲弾
		case BulletType::CANNONBALL:
			if (m_cannonBall->GetBulletState() == IBullet::USED)
			{
				m_reloadCount = CANNONBALL_RELOAD_TIME;
				m_reloadBulletType = BulletType::CANNONBALL;
				m_isReload = true;
				mylib::DebugLog("砲弾のリロード開始");
			}
			break;
		default:
			break;
	}
}


//---------------------------------------------------------
// リロード処理
//---------------------------------------------------------
void TankCannon::Reload(float elapsedTime)
{
	// リロード中でないなら早期リターン
	if (!m_isReload) { return; }

	// カウントダウン
	m_reloadCount -= elapsedTime;

	// リロード完了
	if (m_reloadCount <= 0.0f)
	{
		m_reloadCount = 0.0f;
		switch (m_reloadBulletType)
		{
			case BulletType::BULLET:
				for (auto& bullet : m_bullets)
				{
					bullet->SetBulletState(IBullet::UNUSED);
				}
				break;
			case BulletType::CANNONBALL:
				m_cannonBall->SetBulletState(IBullet::UNUSED);
				break;
			default:
				break;
		}

		m_isReload = false;
	}
}

//---------------------------------------------------------
// 砲身の先端座標取得
//---------------------------------------------------------
DirectX::SimpleMath::Vector3 TankCannon::GetMuzzlePosition()
{
	using namespace DirectX::SimpleMath;

	// 砲身の先端に対するオフセットベクトル
	DirectX::SimpleMath::Vector3 muzzleOffset = DirectX::SimpleMath::Vector3(0.0f, 0.5f, -0.8f);
	// Quaternion から Matrix を作成して Transform を適用
	Matrix rotationMatrix = Matrix::CreateFromQuaternion(m_cannonRotation * m_currentRotation);
	// 回転をオフセットに適用し、砲身の先端座標を計算
	return Vector3::Transform(muzzleOffset, rotationMatrix) + m_currentPosition;
}

//---------------------------------------------------------
// 照準の表示
//---------------------------------------------------------
void TankCannon::DisplaySight()
{
	using namespace DirectX::SimpleMath;

	// 壁の情報を持っていないならこれ以降の処理をしない
	if (m_walls.empty()) { return; }

	// 戦車の向いている方向
	Quaternion rotation = m_cannonRotation * m_currentRotation;
	Matrix matrix = Matrix::CreateFromQuaternion(rotation);

	//// 描画開始
	//m_graphics->DrawPrimitiveBegin(m_graphics->GetViewMatrix(), m_graphics->GetProjectionMatrix());
	//// レーザーサイト
	//m_graphics->DrawLine(GetMuzzlePosition(), { matrix.Forward() * MAX_RANGE}, DirectX::Colors::Red);
	//// 描画終了
	//m_graphics->DrawPrimitiveEnd();

	// レイを飛ばして着弾方向の表示
	// 射程距離
	float range = MAX_RANGE;
	// レイの距離
	float rayDistance = 0.0f;
	// レイの距離記録用変数
	float minDistance = range;
	
	// レイの作成
	Vector3 rayDirection = matrix.Forward();
	rayDirection.Normalize();
	Ray ray{ this->GetMuzzlePosition(), rayDirection };

	// 壁のボックスコライダーとの衝突判定を取る
	for (auto& wall : m_walls)
	{
		// 壁との衝突判定
		bool isHit = ray.Intersects(*wall->GetBoundingBox(), rayDistance);
		
		// 射程範囲外かつ当たっている
		if (rayDistance <= range && isHit)
		{
			// より短い距離を記録する
			minDistance = std::min(minDistance, rayDistance);
		}
	}
	
	// 衝突座標
	Vector3 hitPosition = Vector3::Zero;
	// 一回でも壁との衝突が取れていたら
	if (minDistance < range)
	{
		// 衝突点計算
		hitPosition = Vector3{ ray.position + ray.direction * minDistance - ray.direction };
		// 赤い照準を出す
		m_drawTexture->SetTexture(Resources::GetInstance()->GetTargetLockTexture());
	}
	else
	{
		// 照準画像の表示場所計算
		hitPosition = Vector3{ ray.position + ray.direction * range - ray.direction };

		// 黒い照準を出す
		m_drawTexture->SetTexture(Resources::GetInstance()->GetTargetTexture());

		// 床に埋まらないようにする
		if (hitPosition.y <= 0.0f)
		{
			hitPosition.y = 0.0f;
			// 赤い照準を出す
			m_drawTexture->SetTexture(Resources::GetInstance()->GetTargetLockTexture());
		}
		
	}

	// 照準画像の表示
	m_drawTexture->Render(hitPosition);
}

//---------------------------------------------------------
// 壁情報の削除
//---------------------------------------------------------
void TankCannon::DeleteWall()
{
	m_walls.clear();
}
