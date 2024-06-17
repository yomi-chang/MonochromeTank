#include "pch.h"
#include "Game/Objects/Tank/Tank.h"
#include "Game/Objects/Tank/TankBody.h"

// コンストラクタ
Tank::Tank(
	ITankComponent* parent,
	const DirectX::SimpleMath::Vector3& initialPosition,
	const float& initialAngleRL
)
	:
	TankBase(parent, initialPosition, initialAngleRL),
	m_graphics{Graphics::GetInstance()},
	m_currentPosition{},
	m_currentAngleRL{},
	m_tankParts{},
	m_worldMatrix{}
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

	// 車体の生成
	Attach(std::make_unique<TankBody>(this, Vector3(0.0f, 0.5, 0.0f), 0.0f));
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

	// 前後移動
	if (keyboardState.W)
	{
		tunkVelocity += Matrix::CreateRotationY(m_currentAngleRL + TankBase::GetInitialAngleRL()).Forward() * 0.1f;
	}
	else if (keyboardState.S)
	{
		tunkVelocity -= Matrix::CreateRotationY(m_currentAngleRL + TankBase::GetInitialAngleRL()).Forward() * 0.1f;
	}

	// 左右回転
	if (keyboardState.A)
	{
		m_currentAngleRL += DirectX::XMConvertToRadians(1.0f);
	}
	else if (keyboardState.D)
	{
		m_currentAngleRL -= DirectX::XMConvertToRadians(1.0f);
	}

	m_currentPosition += tunkVelocity;

	// パーツの更新
	TankBase::Update(elapsedTime, m_currentPosition + GetInitialPosition(), m_currentAngleRL + GetInitialAngleRL());
}

// 自身を描画しない描画処理(Tank用)
void Tank::Render()
{
	// パーツの描画
	TankBase::Render();
}

// 終了処理
void Tank::Finalize()
{
	// 削除する部品をリセットする
	m_tankParts.clear();
}