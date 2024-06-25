#include "pch.h"
#include "Game/Objects/Tank/TankBase.h"
#include "Game/Objects/Tank/TankBody.h"
#include "Game/Objects/Tank/TankTurret.h"
#include "Framework/Resources.h"

// コンストラクタ
TankBody::TankBody(
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
	m_model{},
	m_worldMatrix{},
	m_tankType{ type }
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
	using namespace DirectX::SimpleMath;

	// モデル情報の受け取り
	m_model = Resources::GetInstance()->GetTankBodyModel();

	// モデルをセットする
	TankBase::SetModel(m_model);

	// 砲塔の生成
	Attach(std::make_unique<TankTurret>(this, Vector3{ 0.0f,0.75f,0.0f },0.0f,m_tankType));
}

// 更新処理
void TankBody::Update(
	float elapsedTime,
	const DirectX::SimpleMath::Vector3& currentPosition,
	const float& currentAngleRL
)
{
	UNREFERENCED_PARAMETER(elapsedTime);

	// 現在の位置の更新
	m_currentPosition = currentPosition;
	// 現在の回転角の更新
	m_currentAngleRL = currentAngleRL;
	// 「砲塔」の更新
	TankBase::Update(elapsedTime, currentPosition, currentAngleRL);

	//// パーツの更新
	//for (auto& turretPart : m_tankParts)
	//{
	//	turretPart->Update(elapsedTime, currentPosition, currentAngleRL);
	//}
}

// 描画処理
void TankBody::Render()
{
	using namespace DirectX::SimpleMath;

	// ワールド行列を生成する
	m_worldMatrix = Matrix::CreateScale(0.5f) *
		Matrix::CreateRotationY(m_currentAngleRL + GetInitialAngleRL()) *
		Matrix::CreateTranslation(m_currentPosition + GetInitialPosition());

	// パーツの描画
	TankBase::Render(m_worldMatrix);
}

// 終了処理
void TankBody::Finalize()
{
	// 削除する部品をリセットする
	m_tankParts.clear();
}