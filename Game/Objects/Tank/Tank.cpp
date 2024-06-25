#include "pch.h"
#include "Game/Objects/Tank/Tank.h"
#include "Game/Objects/Tank/TankBody.h"

// コンストラクタ
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
	m_tankType{ type }
{
}

// デストラクタ
Tank::~Tank()
{
	Finalize();
}

// 初期化処理
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
}

// 更新処理
void Tank::Update(
	float elapsedTime,
	const DirectX::SimpleMath::Vector3& currentPosition,
	const float& currentAngleRL
)
{
	using namespace DirectX::SimpleMath;
	UNREFERENCED_PARAMETER(elapsedTime);

	// キーボードステートの取得
	DirectX::Keyboard::State keyboardState = DirectX::Keyboard::Get().GetState();

	// 速度の初期化
	Vector3 tunkVelocity = Vector3::Zero;
	
	//プレイヤーの場合
	if (m_tankType == TankType::Player)
	{
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

	if (m_tankType == TankType::Enemy)
	{
		// プレイヤーの方向を向く
		Vector3 delta = m_currentPosition - m_otherTank->GetTankPosition();
		float angleRadians = atan2(delta.x, delta.z);
		m_currentAngleRL = angleRadians;

		tunkVelocity -= Matrix::CreateRotationY(m_currentAngleRL + TankBase::GetInitialAngleRL()).Forward() * 0.01f;
		m_currentPosition += tunkVelocity;
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
}

// 自身を描画しない描画処理(Tank用)
void Tank::Render()
{
	// パーツの描画
	TankBase::Render();

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

// 終了処理
void Tank::Finalize()
{
	// 削除する部品をリセットする
	m_tankParts.clear();
}


// プレイヤーの操作
void Tank::PlayerAction()
{
}

void Tank::EnemyAction()
{
}
