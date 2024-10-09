#include "pch.h"
#include "Game/Objects/NewTank/NewTankBase/NewTankCannon.h"
#include "Game/Objects/NewTank/NewTankBase/NewTank.h"

#include "Game/UserInterface/DrawTexture.h"
#include "Game/Objects/Stage/Wall.h"

#include "Game/Objects/Bullet/Bullet.h"
#include "Game/Objects/Bullet/CannonBall.h"

//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
NewTankCannon::NewTankCannon(
	NewTank* tank,
	const DirectX::SimpleMath::Vector3& initialPosition,
	const float& initialAngle
)
	:
	m_graphics{},
	m_initialPosition{ initialPosition },
	m_initialAngle{ DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3::Up, initialAngle) },
	m_currentPosition{},
	m_currentAngle{},
	m_tankParts{},
	m_worldMatrix{},
	m_model{},
	m_cannonAngle{},
	m_bullets{},
	m_cannonBall{},
	m_bulletType{},
	m_reloadCount{},
	m_isReload{},
	m_reloadBulletType{},
	m_drawTexture{},
	m_tank{}
{
	// グラフィックスの取得
	m_graphics = Graphics::GetInstance();

	// 戦車情報の受け取り
	m_tank = tank;
}

//---------------------------------------------------------
// デストラクタ
//---------------------------------------------------------
NewTankCannon::~NewTankCannon()
{
}

//---------------------------------------------------------
// 初期化処理
//---------------------------------------------------------
void NewTankCannon::Initialize()
{
	// モデルの取得
	m_model = Resources::GetInstance()->GetTankCannonModel();

	// 戦車に砲身情報を渡す
	m_tank->SetCannon(this);

	// 連射弾配列を作成する
	m_bullets.resize(20);
	// 配列に連射弾を格納する
	for (int index = 0; index < 20; index++)
	{
		// 連射弾を生成する
		m_bullets[index] = std::make_unique<Bullet>(IBullet::UNUSED);
		// 連射弾を初期化する
		m_bullets[index]->Initialize();
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
}

//---------------------------------------------------------
// 更新処理
//---------------------------------------------------------
void NewTankCannon::Update(
	float elapsedTime,
	const DirectX::SimpleMath::Vector3& currentPosition,
	const DirectX::SimpleMath::Quaternion& currentAngle
)
{
	// 現在位置の更新
	m_currentPosition = currentPosition + m_initialPosition;
	m_currentAngle = currentAngle * m_initialAngle;

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
void NewTankCannon::Render()
{
	using namespace DirectX::SimpleMath;

	// 弾の描画
	for (auto& bullet : m_bullets)
	{
		bullet->Render();
	}
	m_cannonBall->Render();

	// ワールド行列の生成
	m_worldMatrix = Matrix::CreateScale(0.5f) *
		Matrix::CreateFromQuaternion(m_cannonAngle) *
		Matrix::CreateTranslation(Vector3(0.0f, 0.0f, -0.3f)) *
		Matrix::CreateFromQuaternion(m_currentAngle) *
		Matrix::CreateTranslation(m_currentPosition);

	// 「砲身」の描画
	m_graphics->DrawModel(m_model, m_worldMatrix);

	// 照準の描画
	DisplaySight();
}

//---------------------------------------------------------
// 終了処理
//---------------------------------------------------------
void NewTankCannon::Finalize()
{
}

//---------------------------------------------------------
// 砲身の回転
//---------------------------------------------------------
void NewTankCannon::RotateCannon(float angle)
{
	using namespace DirectX::SimpleMath;

	// クォータニオンに変換して適用
	m_cannonAngle = Quaternion::CreateFromYawPitchRoll(0.0f, angle, 0.0f);
}

//---------------------------------------------------------
// 弾の発射処理
//---------------------------------------------------------
void NewTankCannon::ShootBullet(IBullet* bullet)
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

//---------------------------------------------------------
// 弾の発射呼び出し
//---------------------------------------------------------
void NewTankCannon::Shoot()
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
	// 初期値を設定する
	m_shotTimer = SHOT_INTERVAL;
}

//---------------------------------------------------------
// 弾の変更
//---------------------------------------------------------
void NewTankCannon::ChangeBullet()
{
	m_bulletType = (m_bulletType == BulletType::CANNONBALL) ? BulletType::BULLET : BulletType::CANNONBALL;
}


//---------------------------------------------------------
// リロード開始
//---------------------------------------------------------
void NewTankCannon::StartReload()
{
	// リロード開始
	if (m_isReload)
		return;

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
void NewTankCannon::Reload(float elapsedTime)
{
	// リロード中でないなら早期リターン
	if (!m_isReload)
		return;

	// カウントダウン
	m_reloadCount -= elapsedTime;

	// リロード完了
	if (m_reloadCount <= 0.0f)
	{
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
DirectX::SimpleMath::Vector3 NewTankCannon::GetMuzzlePosition()
{
	using namespace DirectX::SimpleMath;

	// 砲身の先端に対するオフセットベクトル
	DirectX::SimpleMath::Vector3 muzzleOffset = DirectX::SimpleMath::Vector3(0.0f, 0.0f, -0.8f);
	// Quaternion から Matrix を作成して Transform を適用
	Matrix rotationMatrix = Matrix::CreateFromQuaternion(m_cannonAngle * m_currentAngle);
	// 回転をオフセットに適用し、砲身の先端座標を計算
	return Vector3::Transform(muzzleOffset, rotationMatrix) + m_currentPosition;
}

//---------------------------------------------------------
// 照準の表示
//---------------------------------------------------------
void NewTankCannon::DisplaySight()
{
	using namespace DirectX::SimpleMath;

	// プリミティブ描画を開始する
	m_graphics->DrawPrimitiveBegin(m_graphics->GetViewMatrix(), m_graphics->GetProjectionMatrix());

	// 照準の描画
	Quaternion rotation = m_cannonAngle * m_currentAngle;
	Matrix matrix = Matrix::CreateFromQuaternion(rotation);
	//m_graphics->DrawLine(GetMuzzlePosition(), { matrix.Forward() * 10.0f}, DirectX::Colors::Red);

	// プリミティブ描画を終了する
	m_graphics->DrawPrimitiveEnd();


	// Rayを飛ばして着弾方向の表示
	// Rayの距離設定
	float maxDistance = 10.0f;
	// Rayの作成
	Ray ray{ this->GetMuzzlePosition(), matrix.Forward() };
	// 壁のボックスコライダーとの衝突判定を取る
	Vector3 hitPosition = Vector3::Zero;
	for (auto& wall : m_walls)
	{
		float distance = 0.0f;
		// 壁との衝突判定
		bool isHit = ray.Intersects(*wall->GetBoundingBox(), distance);
		
		// 射程範囲外かつ当たっている
		if (distance <= maxDistance && isHit)
		{
			// 衝突点計算
			hitPosition = Vector3{ ray.position + ray.direction * distance - ray.direction };
			// 赤い照準を出す
			m_drawTexture->SetTexture(Resources::GetInstance()->GetTargetLockTexture());
			break;
		}
		// 射程範囲外または当たっていない
		else
		{
			// 照準画像の表示場所計算
			hitPosition = Vector3{ ray.position + ray.direction * maxDistance - ray.direction };
			// 黒い照準を出す
			m_drawTexture->SetTexture(Resources::GetInstance()->GetTargetTexture());
		}
	}
	// 照準画像の表示
	m_drawTexture->Render(hitPosition);
}
