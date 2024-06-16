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
	m_currentPosition{},
	m_currentAngleRL{},
	m_tankParts{},
	m_model{},
	m_worldMatrix{}
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
	Attach(std::make_unique<TankCannon>(this, Vector3{ 0.0f,1.0f,-0.6f }, 0.0f));

	// モデルのセット
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

	// 現在の位置の更新
	m_currentPosition = currentPosition;
	// 現在の回転角の更新
	m_currentAngleRL = currentAngleRL;
	// 「砲身」の更新
	TankBase::Update(elapsedTime, currentPosition, currentAngleRL);


	//// パーツの更新
	//for (auto& turretPart : m_tankParts)
	//{
	//	turretPart->Update(elapsedTime, currentPosition, currentAngleRL);
	//}
}

// 描画処理
void TankTurret::Render()
{
	using namespace DirectX::SimpleMath;

	// ワールド行列を生成する
	m_worldMatrix = Matrix::CreateScale(1.0f) *
		Matrix::CreateRotationY(m_currentAngleRL + GetInitialAngleRL()) *
		Matrix::CreateTranslation(m_currentPosition + GetInitialPosition());

	// パーツの描画
	TankBase::Render(m_worldMatrix);
}

// 終了処理
void TankTurret::Finalize()
{
	// 削除する部品のリセット
	m_tankParts.clear();
}