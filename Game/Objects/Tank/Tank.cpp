#include "pch.h"
#include "Game/Objects/Tank/Tank.h"
#include "Game/Objects/Tank/TankBody.h"
#include "Game/Objects/Tank/TankArmor.h"
#include "Framework/InputManager.h"

#include "Libraries/MyLib/DebugLog.h"

/// コンストラクタ
Tank::Tank(
	IComponent* parent,
	const DirectX::SimpleMath::Vector3& initialPosition,
	const float& initialAngle
)
	:
	m_parent{ parent },
	m_initialPosition{ initialPosition },
	m_initialAngle(DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3::Up, initialAngle)),
	m_graphics{Graphics::GetInstance()},
	m_currentPosition{},
	m_currentAngle{},
	m_tankParts{},
	m_worldMatrix{},
	m_bullets{},
	m_tankType{},
	m_collider{},
	m_hit{},
	m_hpGauge{},
	m_enemyHpGauge{},
	m_damage{},
	m_otherTanks{},
	m_bulletType{},
	m_reloadCount{},
	m_isReload{},
	m_reloadBulletType{},
	m_camera{}
{
}

// デストラクタ
Tank::~Tank()
{
	Finalize();
}


// 初期化処理
void Tank::Initialize(Type type)
{
	using namespace DirectX::SimpleMath;

	// タイプの確定
	m_tankType = type;

	// 現在の座標を設定
	m_currentPosition = m_initialPosition;

	// 車体の生成
	Attach(std::make_unique<TankBody>(this, Vector3(0.0f, 0.5f, 0.0f), 0.0f));
	// 装甲の生成
	Attach(std::make_unique<TankArmor>(this, Vector3(0.0f, 0.5f, 0.0f), 0.0f));

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

	// コライダーの作成
	m_collider = std::make_unique<SphereCollider>();
	m_collider->CreateBoundingSphere(m_currentPosition, 1.0f);

	// 体力ゲージ関係の設定
	switch (m_tankType)
	{
		case IComponent::PLAYER:
			// 体力ゲージを生成
			m_hpGauge = std::make_unique<HpGauge>();
			m_hpGauge->Initialize(Vector2{ 200,50 });
			break;
		case IComponent::ENEMY:
			// 敵体力ゲージを生成
			m_enemyHpGauge = std::make_unique<EnemyHpGauge>();
			m_enemyHpGauge->SetMaxHp(10.0f);
			break;
		default:
			break;
	}

	// 最初に発射できる弾を砲弾に設定する
	m_bulletType = BulletType::CANNONBALL;
	m_reloadBulletType = BulletType::CANNONBALL;
}


/// 更新処理
void Tank::Update(
	float elapsedTime,
	const DirectX::SimpleMath::Vector3& currentPosition,
	const DirectX::SimpleMath::Quaternion& currentAngle
)
{
	UNREFERENCED_PARAMETER(elapsedTime);

	// プレイヤー又は敵の行動
	switch (m_tankType)
	{
		// プレイヤーの行動
		case Type::PLAYER:
			PlayerAction(elapsedTime);
			break;
		// 敵の行動
		case Type::ENEMY:
			EnemyAction();
			break;
		default:
			break;
	}

	// コライダーの更新
	m_collider->Update(m_currentPosition);

	// 弾の更新
	for (auto& bullet : m_bullets)
	{
		bullet->Update(elapsedTime);
	}
	m_cannonBall->Update(elapsedTime);

	// ダメージの初期化
	m_damage = 0.0f;

	// 弾丸と戦車の当たり判定
	DetectCollisionTankAndBullets();
	// 戦車と戦車の当たり判定
	DetectCollisionTankAndOtherTanks();

	// パーツの更新
	for (auto& tankPart : m_tankParts)
	{
		tankPart->Update(elapsedTime, m_currentPosition, m_currentAngle);
	}

	// ダメージ処理
	switch (m_tankType)
	{
		case Type::PLAYER:
			m_hpGauge->Damage(m_damage);
			break;
		case Type::ENEMY:
			m_enemyHpGauge->Damage(m_damage);
			break;
		default:
			break;
	}
}

/// 描画処理
void Tank::Render()
{
	// パーツの描画
	for (auto& tankPart : m_tankParts)
	{
		tankPart->Render();
	}

	// コライダーの表示
	m_collider->Render();

	// 弾の描画
	for (auto& bullet : m_bullets)
	{
		bullet->Render();
	}
	m_cannonBall->Render();

	//体力ゲージの描画
	switch (m_tankType)
	{
		case IComponent::PLAYER:
			m_hpGauge->Render();
			break;
		case IComponent::ENEMY:
			m_enemyHpGauge->Render(m_currentPosition);
			break;
		default:
			break;
	}
}

// 終了処理
void Tank::Finalize()
{
	// 削除する部品をリセットする
	//m_tankParts.clear();
}

// パーツの追加
void Tank::Attach(std::unique_ptr<IComponent> part)
{
	// パーツの初期化
	part->Initialize(m_tankType);
	// パーツの追加
	m_tankParts.emplace_back(std::move(part));
}


// パーツの削除
void Tank::Detach(std::unique_ptr<IComponent> part)
{
}


// プレイヤーの操作
void Tank::PlayerAction(float elapsedTime)
{
	using namespace DirectX::SimpleMath;
	// キーボードステートの取得
	const auto& keyboardState = InputManager::GetInstance()->GetKeyboardState();
	const auto& keyboardTracker = InputManager::GetInstance()->GetKeyboardTracker();
	// マウスステートの取得
	const auto& mouseTracker = InputManager::GetInstance()->GetMouseTracker();

	// 速度の初期化
	Vector3 tunkVelocity = Vector3::Zero;

	// 前後移動
	if (keyboardState.W)
	{
		tunkVelocity += Vector3::Transform(Vector3::Forward * elapsedTime * 3.0f, m_currentAngle);
		m_currentPosition += tunkVelocity;
	}
	else if (keyboardState.S)
	{
		tunkVelocity += Vector3::Transform(Vector3::Backward * elapsedTime * 3.0f, m_currentAngle);
		m_currentPosition += tunkVelocity;
	}
	// 左右回転
	if (keyboardState.A)
	{
		m_currentAngle *= Quaternion::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(0.5f), 0.0f, 0.0f);
	}
	else if (keyboardState.D)
	{
		m_currentAngle *= Quaternion::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(-0.5f), 0.0f, 0.0f);
	}

	// 弾の種類を変更
	if (keyboardTracker->IsKeyPressed(DirectX::Keyboard::Space))
	{
		m_bulletType = (m_bulletType == BulletType::CANNONBALL) ?  BulletType::BULLET : BulletType::CANNONBALL;
	}

	// リロード開始
	if (mouseTracker->rightButton == mouseTracker->PRESSED &&
		!m_isReload)
	{
		switch (m_bulletType)
		{
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

	// リロード処理
	if (m_isReload)
	{
		// カウントダウン
		m_reloadCount -= elapsedTime;

		// リロード完了
		if (m_reloadCount <= 0.0f)
		{
			switch (m_reloadBulletType)
			{
				case Tank::BULLET:
					for (auto& bullet : m_bullets)
					{
						bullet->SetBulletState(IBullet::UNUSED);
					}
					break;
				case Tank::CANNONBALL:
					m_cannonBall->SetBulletState(IBullet::UNUSED);
					break;
				default:
					break;
			}

			m_isReload = false;
		}
	}
}


// 敵の行動　ToDo:敵AIは別でクラス化
void Tank::EnemyAction()
{
	using namespace DirectX::SimpleMath;

	// 速度の初期化
	Vector3 tunkVelocity = Vector3::Zero;

	// プレイヤーの方向を向く m_otherTanks[0]はプレイヤー　ToDo:EnemyのAiクラスを作成
	/*Vector3 delta = m_currentPosition - m_otherTanks[0]->GetTankPosition();
	float angleRadians = atan2(delta.x, delta.z);
	m_currentAngleRL = angleRadians;
	tunkVelocity -= Matrix::CreateRotationY(m_currentAngleRL + m_initialAngle).Forward() * 0.01f;
	m_currentPosition += tunkVelocity;*/
}

// 弾丸と戦車の当たり判定
void Tank::DetectCollisionTankAndBullets()
{
	m_hit = false;

	// 弾丸と戦車の当たり判定
	for (auto& otherTank : m_otherTanks)
	{
		// 連射弾
		for (auto& bullet : otherTank->GetBullets())
		{
			// 弾丸が飛んでいる、かつ当たっているなら
			if (bullet->GetBulletState() == IBullet::FLYING &&
				m_collider->CheckTriggerCollider(bullet->GetBoundingSphere()))
			{
				bullet->SetBulletState(IBullet::USED);
				m_damage += 0.5f;
			}
		}
		// 砲弾
		if (otherTank->GetCannonBall()->GetBulletState() == IBullet::FLYING &&
			m_collider->CheckTriggerCollider(otherTank->GetCannonBall()->GetBoundingSphere()))
		{
			otherTank->GetCannonBall()->SetBulletState(IBullet::USED);
			m_damage += 3.0f;
		}
	}
}

// 戦車と戦車の当たり判定
void Tank::DetectCollisionTankAndOtherTanks()
{
	m_hit = false;
	for (auto& otherTank : m_otherTanks)
	{
		if (m_collider->CheckTriggerCollider(otherTank->GetBoundingSphere()))
		{
			m_hit = true;
			m_currentPosition += m_collider->CheckCollisionCollider(otherTank->GetBoundingSphere());
			//m_damage += 0.0001f;
		}
	}
}

// 死亡しているかどうか
bool Tank::GetDead()
{
	switch (m_tankType)
	{
		case IComponent::PLAYER:
			return m_hpGauge->GetDead();
			break;
		case IComponent::ENEMY:
			return m_enemyHpGauge->GetDead();
			break;
		default:
			break;
	}
}
