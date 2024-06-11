#include "pch.h"
#include "Game/Objects/Tank/TankBody.h"
#include "Framework/Resources.h"

// コンストラクタ
TankBody::TankBody(
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
TankBody::~TankBody()
{
	Finalize();
}

// 初期化処理
void TankBody::Initialize()
{
	// モデル情報の受け取り
	m_model = Resources::GetInstance()->GetTankBodyModel();

	// 車体の生成
}

// 更新処理
void TankBody::Update(
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
void TankBody::Render()
{
	// パーツの描画
	TankBase::Render();
}

// 終了処理
void TankBody::Finalize()
{
	// 削除する部品をリセットする
	m_tankParts.clear();
}