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
	m_tankParts{}
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
	Attach(std::make_unique<TankBody>(this, Vector3(0.0f, 0.0f, 0.0f), 0.0f));
}

// 更新処理
void Tank::Update(
	float elapsedTime,
	const DirectX::SimpleMath::Vector3& currentPosition,
	const float& currentAngleRL
)
{
	UNREFERENCED_PARAMETER(elapsedTime);

	// 砲塔部品を更新する
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