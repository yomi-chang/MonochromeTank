/**
 * @file   TankCannon.h
 * @brief  砲身クラス
 */
#include "pch.h"
#include "Game/Objects/Tank/TankBase/TankCannon.h"
#include "Game/Objects/Tank/TankBase/Tank.h"

#include "Game/UserInterface/DrawTexture.h"
#include "Game/Objects/Stage/Wall.h"

#include "Game/Objects/Bullet/Bullet.h"
#include "Game/Objects/Bullet/CannonBall.h"

#include "Libraries/MyLib/Utils.h"

#include "Game/Other/SharedData.h"
#include "Libraries/MyLib/SoundManager.h"
#include "Game/Other/Sounds.h"


/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="tank">戦車の情報</param>
/// <param name="initialPosition">初期座標</param>
/// <param name="initialAngle">初期角度</param>
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
	m_bulletBlurRadius{},
	m_drawTexture{},
	m_tank{},
	m_displaySight{},
	m_isShot{},
	m_shotTimer{}
{
	// 戦車情報の受け取り
	m_tank = tank;
}

/// <summary>
/// デストラクタ
/// </summary>
TankCannon::~TankCannon()
{
}

/// <summary>
/// 初期化処理
/// </summary>
void TankCannon::Initialize()
{
	// パラメータの受け取り
	auto parameter = Parameter::GetInstance();

	// モデルの取得
	m_model = Resources::GetInstance()->GetTankCannonModel();

	// 戦車に砲身情報を渡す
	m_tank->SetCannon(this);

	/// 連射弾の生成
	for (int i = 0; i < parameter->GetBulletCount(); i++)
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

	// 標準表示にする
	m_displaySight = true;
}

/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime">フレーム間の経過時間</param>
/// <param name="currentPosition">現在の座標</param>
/// <param name="currentRotation">現在の角度</param>
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

	// 弾を発射しているなら少しずつ弾が散っていくようにする
	if (m_isShot)
	{
		m_bulletBlurRadius += elapsedTime;
	}
	else
	{
		// 弾が散らない状況にする
		m_bulletBlurRadius = 0.0f;
	}

	// リロード処理
	this->Reload(elapsedTime);
}

/// <summary>
/// 描画処理
/// </summary>
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
	m_worldMatrix = Matrix::CreateScale(Tank::TANK_SIZE) *
		Matrix::CreateFromQuaternion(m_cannonRotation) *
		Matrix::CreateTranslation(Vector3(0.0f, 0.0f, 0.0f)) *
		Matrix::CreateFromQuaternion(m_currentRotation) *
		Matrix::CreateTranslation(m_currentPosition);

	// 「砲身」の描画
	m_graphics->DrawModel(m_model, m_worldMatrix);
}

/// <summary>
/// 終了処理
/// </summary>
void TankCannon::Finalize()
{
}

/// <summary>
/// 砲身の回転
/// </summary>
/// <param name="angle">角度</param>
void TankCannon::RotateCannon(float angle)
{
	using namespace DirectX::SimpleMath;

	// クォータニオンに変換して適用
	m_cannonRotation = Quaternion::CreateFromYawPitchRoll(0.0f, angle, 0.0f);
}

/// <summary>
/// 弾の発射処理
/// </summary>
/// <param name="bullet">弾の情報</param>
void TankCannon::ShootBullet(IBullet* bullet)
{
	// 「砲弾」位置を設定する
	bullet->SetPosition(GetMuzzlePosition());
	// コライダー座標の更新
	bullet->SetColliderPosition(GetMuzzlePosition());
	// 「砲弾」角度を設定する
	bullet->SetRotation(GetShotRotation());
	// 「砲弾」を発射する
	bullet->SetBulletState(IBullet::FLYING);
}

/// <summary>
/// 弾の発射呼び出し
/// </summary>
void TankCannon::Shoot()
{
	// 弾を撃っている状態にする
	m_isShot = true;

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
					// SEの再生
					if(m_tank->GetTankNumber() == 0)
						SharedData::GetInstance()->GetSoundManager()->PlaySE(XACT_WAVEBANK_SOUNDS_BULLET_SE);
					break;
				}
			}
			break;
		// 「砲弾」を発射する
		case BulletType::CANNONBALL:
			if (m_cannonBall->GetBulletState() == IBullet::UNUSED)
			{
				//「砲弾」を発射する
				ShootBullet(m_cannonBall.get());
				// SEの再生
				if (m_tank->GetTankNumber() == 0)
					SharedData::GetInstance()->GetSoundManager()->PlaySE(XACT_WAVEBANK_SOUNDS_CANNONBALL_SE);
			}
			else
			{
				// SEの再生
				if (m_tank->GetTankNumber() == 0)
					SharedData::GetInstance()->GetSoundManager()->PlaySE(XACT_WAVEBANK_SOUNDS_NONEBULLETS_SE);
			}
			break;
	}
	// 発射インターバルを設定する
	m_shotTimer = Parameter::GetInstance()->GetShotInterval();
}

/// <summary>
/// 弾の変更
/// </summary>
void TankCannon::ChangeBullet()
{
	// リロード開始
	if (m_isReload) { return; }
	m_bulletType = (m_bulletType == BulletType::CANNONBALL) ? BulletType::BULLET : BulletType::CANNONBALL;
}


/// <summary>
/// リロード開始
/// </summary>
void TankCannon::StartReload()
{
	// リロード開始
	if (m_isReload) { return; }

	// パラメータの受け取り
	auto parameter = Parameter::GetInstance();

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
					m_reloadCount = parameter->GetBulletReloadTime();
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
				m_reloadCount = parameter->GetCannonBallReloadTime();
				m_reloadBulletType = BulletType::CANNONBALL;
				m_isReload = true;
				mylib::DebugLog("砲弾のリロード開始");
			}
			break;
		default:
			break;
	}
}

/// <summary>
/// 照準の描画
/// </summary>
void TankCannon::DrawSight()
{
	// 照準画像の表示
	if (m_tank->GetTankNumber() == 0 && m_displaySight)
		m_drawTexture->Render(m_hitPosition);
}

/// <summary>
/// リロード処理
/// </summary>
/// <param name="elapsedTime">フレーム間の経過時間</param>
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

		// SEの再生
		if (m_tank->GetTankNumber() == 0)
			SharedData::GetInstance()->GetSoundManager()->PlaySE(XACT_WAVEBANK_SOUNDS_RELOAD_SE);
	}
}

/// <summary>
/// ずらした射撃方向の取得
/// </summary>
/// <returns></returns>
DirectX::SimpleMath::Quaternion TankCannon::GetShotRotation()
{
	using namespace DirectX::SimpleMath;
	using namespace DirectX;

	Quaternion shotRotation = m_cannonRotation * m_currentRotation;

	// 角度をランダムな値を利用してずらす
	/*shotRotation *= Quaternion::CreateFromYawPitchRoll(
		mylib::Random(XMConvertToRadians(-m_bulletBlurRadius), XMConvertToRadians(m_bulletBlurRadius)),
		mylib::Random(XMConvertToRadians(-m_bulletBlurRadius), XMConvertToRadians(m_bulletBlurRadius)),
		0.0f
	);*/

	return shotRotation;
}

/// <summary>
/// 砲身の先端座標取得
/// </summary>
/// <returns>先端座標</returns>
DirectX::SimpleMath::Vector3 TankCannon::GetMuzzlePosition()
{
	using namespace DirectX::SimpleMath;

	// 砲身の先端に対するオフセットベクトル
	Vector3 muzzleOffset(0.0f, 0.5f, -0.8f);
	// Quaternion から Matrix を作成して Transform を適用
	Matrix rotationMatrix = Matrix::CreateFromQuaternion(m_cannonRotation * m_currentRotation);
	// 回転をオフセットに適用し、砲身の先端座標を計算
	return Vector3::Transform(muzzleOffset, rotationMatrix) + m_currentPosition;
}

/// <summary>
/// Rayの衝突情報の設定
/// </summary>
/// <param name="isHit">衝突しているかどうか</param>
/// <param name="hitPosition">衝突地点</param>
void TankCannon::SetRayInfo(bool isHit, DirectX::SimpleMath::Vector3 hitPosition)
{
	// 衝突しているかに応じたテクスチャの設定
	if (isHit)
		m_drawTexture->SetTexture(Resources::GetInstance()->GetTargetLockTexture());
	else
		m_drawTexture->SetTexture(Resources::GetInstance()->GetTargetTexture());

	// 衝突座標の設定
	m_hitPosition = hitPosition;
}

