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
	const float& initialAngleRL
)
	:
	m_parent{ parent },
	m_initialPosition{ initialPosition },
	m_initialAngle{ initialAngleRL },
	m_graphics{Graphics::GetInstance()},
	m_currentPosition{},
	m_currentAngleRL{},
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
	m_bulletType{ BulletType::CANNONBALL }
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
	m_bullets.resize(100);
	// 配列に連射弾を格納する
	for (int index = 0; index < 100; index++)
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
}


/// 更新処理
void Tank::Update(
	float elapsedTime,
	const DirectX::SimpleMath::Vector3& currentPosition,
	const float& currentAngleRL
)
{
	UNREFERENCED_PARAMETER(elapsedTime);

	// プレイヤー又は敵の行動
	switch (m_tankType)
	{
		// プレイヤーの行動
		case Type::PLAYER:
			PlayerAction();
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

	// 飛弾中の砲弾を更新する
	for (auto& bullet : m_bullets)
	{
		// 砲弾が飛弾している場合 砲弾を更新する
		if (bullet->GetBulletState() == IBullet::FLYING)
		{
			// 砲弾を更新する
			bullet->Update(elapsedTime);
		}
	}
	if (m_cannonBall->GetBulletState() == IBullet::FLYING)
	{
		m_cannonBall->Update(elapsedTime);
	}

	// ダメージの初期化
	m_damage = 0.0f;

	// 弾丸と戦車の当たり判定
	DetectCollisionTankAndBullets();
	// 戦車と戦車の当たり判定
	DetectCollisionTankAndOtherTanks();

	// パーツの更新
	for (auto& tankPart : m_tankParts)
	{
		tankPart->Update(elapsedTime, m_currentPosition, m_currentAngleRL);
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

	//mylib::DebugLog("ダメージ",m_damage);
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

	// 飛弾中の砲弾を描画する
	for (auto& bullet : m_bullets)
	{
		// 飛弾中の砲弾を描画する
		//if (bullet->GetBulletState() == IBullet::FLYING)
		//{
			// 砲弾を描画する
			bullet->Render();
		//}
	}
	//if (m_cannonBall->GetBulletState() == IBullet::FLYING)
	//{
		m_cannonBall->Render();
	//}

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
void Tank::PlayerAction()
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
		tunkVelocity += Matrix::CreateRotationY(m_currentAngleRL + m_initialAngle).Forward() * 0.05f;
		m_currentPosition += tunkVelocity;
	}
	else if (keyboardState.S)
	{
		tunkVelocity -= Matrix::CreateRotationY(m_currentAngleRL + m_initialAngle).Forward() * 0.05f;
		m_currentPosition += tunkVelocity;
	}
	// 左右回転
	if (keyboardState.A)
	{
		m_currentAngleRL += DirectX::XMConvertToRadians(0.5f);
	}
	else if (keyboardState.D)
	{
		m_currentAngleRL -= DirectX::XMConvertToRadians(0.5f);
	}

	// 弾の種類を変更
	if (keyboardTracker->IsKeyPressed(DirectX::Keyboard::Space))
	{
		switch (m_bulletType)
		{
			case BulletType::BULLET:
				m_bulletType = BulletType::CANNONBALL;
				break;
			case BulletType::CANNONBALL:
				m_bulletType = BulletType::BULLET;
				break;
		}
	}

	// リロード
	if (mouseTracker->rightButton == mouseTracker->PRESSED)
	{
		switch (m_bulletType)
		{
			case BulletType::BULLET:
				for (auto& bullet : m_bullets)
				{
					if(bullet->GetBulletState() == IBullet::USED)
					// ToDo　時間のかかるリロードに変更
					bullet->SetBulletState(IBullet::UNUSED);
				}
				break;
			case BulletType::CANNONBALL:
				// ToDo　時間のかかるリロードに変更
				if (m_cannonBall->GetBulletState() == IBullet::USED)
				m_cannonBall->SetBulletState(IBullet::UNUSED);
				break;
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
	Vector3 delta = m_currentPosition - m_otherTanks[0]->GetTankPosition();
	float angleRadians = atan2(delta.x, delta.z);
	m_currentAngleRL = angleRadians;
	tunkVelocity -= Matrix::CreateRotationY(m_currentAngleRL + m_initialAngle).Forward() * 0.01f;
	m_currentPosition += tunkVelocity;
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
