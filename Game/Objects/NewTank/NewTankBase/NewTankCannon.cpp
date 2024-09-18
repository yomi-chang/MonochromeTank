#include "pch.h"
#include "Game/Objects/NewTank/NewTankBase/NewTankCannon.h"
#include "Game/Objects/NewTank/NewTankBase/NewTank.h"

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
	if (m_shotTimer > 0.0f)
	{
		// タイマーを減らす
		m_shotTimer -= elapsedTime;
	}


	// 現在位置の更新
	m_currentPosition = currentPosition + m_initialPosition;
	m_currentAngle = currentAngle * m_initialAngle;

	// 弾の更新
	for (auto& bullet : m_bullets)
	{
		bullet->Update(elapsedTime);
	}
	m_cannonBall->Update(elapsedTime);
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
