#include "pch.h"
#include "Game/Objects/Tank/Tank.h"
#include "Game/Objects/Tank/TankBody.h"

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="parent">親オブジェクト</param>
/// <param name="initialPosition">初期座標</param>
/// <param name="initialAngleRL">初期回転角</param>
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
	m_hit{}
{
}

/// <summary>
/// デストラクタ
/// </summary>
Tank::~Tank()
{
	Finalize();
}

/// <summary>
/// 初期化処理
/// </summary>
void Tank::Initialize(Type type)
{
	using namespace DirectX::SimpleMath;

	// タイプの確定
	m_tankType = type;

	// 現在の座標を設定
	m_currentPosition = m_initialPosition;

	// 車体の生成
	Attach(std::make_unique<TankBody>(this, Vector3(0.0f, 0.5f, 0.0f), 0.0f));

	// 砲弾配列を作成する
	m_bullets.resize(100);

	// 配列に砲弾を格納する
	for (int index = 0; index < 100; index++)
	{
		// 砲弾を生成する
		m_bullets[index] = std::make_unique<Bullet>(IBullet::UNUSED);
		// 砲弾を初期化する
		m_bullets[index]->Initialize();
	}

	// 境界球
	m_collider = std::make_unique<SphereCollider>();
	m_collider->CreateBoundingSphere(m_currentPosition, 1.0f);
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
	}

	// 当たり判定の更新
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

	// 弾丸と戦車の当たり判定
	DetectCollisionTankAndBullets();

	// 戦車と戦車の当たり判定
	DetectCollisionTankAndOtherTanks();

	// パーツの更新
	for (auto& tankPart : m_tankParts)
	{
		tankPart->Update(elapsedTime, m_currentPosition, m_currentAngleRL);
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

	// 飛弾中の砲弾を描画する
	for (auto& bullet : m_bullets)
	{
		// 飛弾中の砲弾を描画する
		if (bullet->GetBulletState() == IBullet::FLYING)
		{
			// 砲弾を描画する
			bullet->Render();
		}
	}
}

/// <summary>
/// 終了処理
/// </summary>
void Tank::Finalize()
{
	// 削除する部品をリセットする
	//m_tankParts.clear();
}

/// <summary>
/// パーツの追加
/// </summary>
/// <param name="part">パーツ</param>
void Tank::Attach(std::unique_ptr<IComponent> part)
{
	// パーツの初期化
	part->Initialize(m_tankType);
	// パーツの追加
	m_tankParts.emplace_back(std::move(part));
}

/// <summary>
/// パーツの削除
/// </summary>
/// <param name="part">パーツ</param>
void Tank::Detach(std::unique_ptr<IComponent> part)
{
}

/// <summary>
/// プレイヤーの操作
/// </summary>
void Tank::PlayerAction()
{
	using namespace DirectX::SimpleMath;

	// キーボードステートの取得
	DirectX::Keyboard::State keyboardState = DirectX::Keyboard::Get().GetState();

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
}

/// <summary>
/// 敵の行動　ToDo:敵AIは別でクラス化
/// </summary>
void Tank::EnemyAction()
{
	using namespace DirectX::SimpleMath;

	// 速度の初期化
	Vector3 tunkVelocity = Vector3::Zero;

	// プレイヤーの方向を向く
	Vector3 delta = m_currentPosition - m_otherTank->GetTankPosition();
	float angleRadians = atan2(delta.x, delta.z);
	m_currentAngleRL = angleRadians;
	tunkVelocity -= Matrix::CreateRotationY(m_currentAngleRL + m_initialAngle).Forward() * 0.01f;
	m_currentPosition += tunkVelocity;
}

/// <summary>
/// 弾丸と戦車の当たり判定
/// </summary>
void Tank::DetectCollisionTankAndBullets()
{
	m_hit = false;

	// 弾丸と戦車の当たり判定
	//for (auto& bullet : m_otherTank->GetBullets())
	//{
	//	// 弾丸が飛んでいる、かつ当たっているなら
	//	if (bullet->GetBulletState() == IBullet::FLYING &&
	//		m_collider->ChackHitBoundingBox(bullet->GetWorldBoundingBox()))
	//	{
	//		m_hit = true;
	//	}
	//}
}

/// <summary>
/// 戦車と戦車の当たり判定
/// </summary>
void Tank::DetectCollisionTankAndOtherTanks()
{
	m_hit = false;
	m_currentPosition += m_collider->CheckCollisionCollider(m_otherTank->GetBoundingSphere());
}
