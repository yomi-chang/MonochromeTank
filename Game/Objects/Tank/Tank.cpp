#include "pch.h"
#include "Game/Objects/Tank/Tank.h"
#include "Game/Objects/Tank/TankBody.h"

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="parent">親オブジェクト</param>
/// <param name="initialPosition">初期座標</param>
/// <param name="initialAngleRL">初期回転角</param>
/// <param name="type">戦車が敵かプレイヤーか</param>
Tank::Tank(
	ITankComponent* parent,
	const DirectX::SimpleMath::Vector3& initialPosition,
	const float& initialAngleRL,
	TankType type
)
	:
	TankBase(parent, initialPosition, initialAngleRL, type),
	m_graphics{Graphics::GetInstance()},
	m_currentPosition{},
	m_currentAngleRL{},
	m_tankParts{},
	m_worldMatrix{},
	m_bullets{},
	m_tankType{ type },
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
void Tank::Initialize()
{
	using namespace DirectX::SimpleMath;

	// 初期座標、初期回転角の取得
	m_currentPosition = GetInitialPosition();
	m_currentAngleRL = GetInitialAngleRL();

	// 車体の生成
	Attach(std::make_unique<TankBody>(this, Vector3(0.0f, 0.5f, 0.0f), 0.0f, m_tankType));

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

/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime">時間</param>
/// <param name="currentPosition">現在の座標</param>
/// <param name="currentAngleRL">現在の回転</param>
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
		case TankBase::Player:
			PlayerAction();
			break;
		// 敵の行動
		case TankBase::Enemy:
			EnemyAction();
			break;
	}
	
	// パーツの更新
	TankBase::Update(elapsedTime, m_currentPosition , m_currentAngleRL);

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
	DetectCollisionTurretAndBullets();

	// 当たり判定の更新
	m_collider->Update(m_currentPosition);
}

/// <summary>
/// 自身を描画しない描画処理(Tank用)
/// </summary>
void Tank::Render()
{
	// パーツの描画
	TankBase::Render();

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
	m_tankParts.clear();
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
		tunkVelocity += Matrix::CreateRotationY(m_currentAngleRL + TankBase::GetInitialAngleRL()).Forward() * 0.05f;
		m_currentPosition += tunkVelocity;
	}
	else if (keyboardState.S)
	{
		tunkVelocity -= Matrix::CreateRotationY(m_currentAngleRL + TankBase::GetInitialAngleRL()).Forward() * 0.05f;
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
	tunkVelocity -= Matrix::CreateRotationY(m_currentAngleRL + TankBase::GetInitialAngleRL()).Forward() * 0.01f;
	m_currentPosition += tunkVelocity;
}

/// <summary>
/// 弾丸と戦車の当たり判定
/// </summary>
void Tank::DetectCollisionTurretAndBullets()
{
	m_hit = false;

	// 弾丸と戦車の当たり判定
	for (auto& bullet : m_otherTank->GetBullets())
	{
		// 弾丸が飛んでいる、かつ当たっているなら
		if (bullet->GetBulletState() == IBullet::FLYING &&
			m_collider->ChackHitBoundingBox(bullet->GetWorldBoundingBox()))
		{
			m_hit = true;
		}
	}
}
