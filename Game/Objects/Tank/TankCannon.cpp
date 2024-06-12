#include "pch.h"
#include "Game/Objects/Tank/TankCannon.h"
#include "Framework/Resources.h"

// コンストラクタ
TankCannon::TankCannon(
	ITankComponent* parent,
	const DirectX::SimpleMath::Vector3& initialPosition,
	const float& initialAngleRL
)
	:
	TankBase(parent, initialPosition, initialAngleRL),
	m_graphics{Graphics::GetInstance()},
	m_initialPosition{ initialPosition },
	m_initialAngleRL{ initialAngleRL },
	m_tankParts{},
	m_model{}
{
}

// デストラクタ
TankCannon::~TankCannon()
{
	Finalize();
}

// 初期化処理
void TankCannon::Initialize()
{
	// モデル情報の受け取り
	m_model = Resources::GetInstance()->GetTankCannonModel();

	// モデルをセットする
	TankBase::SetModel(m_model);
}

// 更新処理
void TankCannon::Update(
	float elapsedTime,
	const DirectX::SimpleMath::Vector3& currentPosition,
	const float& currentAngleRL
)
{
	UNREFERENCED_PARAMETER(elapsedTime);

	// パーツの更新
	for (auto& turretPart : m_tankParts)
	{
		turretPart->Update(elapsedTime, currentPosition, currentAngleRL);
	}
}

// 自身を描画しない描画処理(Tank用)
void TankCannon::Render()
{
	// パーツの描画
	TankBase::Render();
}

// 終了処理
void TankCannon::Finalize()
{
	// 削除する部品をリセットする
	m_tankParts.clear();
}