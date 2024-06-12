#include "pch.h"
#include "Game/Objects/Tank/TankBase.h"
#include "Game/Objects/Tank/TankTurret.h"
#include "Game/Objects/Tank/TankCannon.h"
#include "Framework/Resources.h"

// コンストラクタ
TankTurret::TankTurret(
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
TankTurret::~TankTurret()
{
	Finalize();
}

// 初期化処理
void TankTurret::Initialize()
{
	using namespace DirectX::SimpleMath;

	// モデル情報の受け取り
	m_model = Resources::GetInstance()->GetTankTurretModel();

	// 砲身の生成
	Attach(std::make_unique<TankCannon>(this, Vector3{ 0,0,0 }, 0.0f));

	// モデルをセットする
	TankBase::SetModel(m_model);
}

// 更新処理
void TankTurret::Update(
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
void TankTurret::Render()
{
	// パーツの描画
	TankBase::Render();
}

// 終了処理
void TankTurret::Finalize()
{
	// 削除する部品をリセットする
	m_tankParts.clear();
}