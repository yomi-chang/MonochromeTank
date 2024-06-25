#include "pch.h"
#include "Game/Objects/Tank/TankBase.h"
#include "Game/Objects/Tank/TankTurret.h"
#include "Game/Objects/Tank/TankCannon.h"
#include "Framework/Resources.h"

// コンストラクタ
TankTurret::TankTurret(
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
	m_turretAngle{},
	m_tankType{ type }
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
	Attach(std::make_unique<TankCannon>(this, Vector3{ 0.0f,0.75f,0.0f }, 0.0f, m_tankType));

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

	// キーボードステートの取得
	DirectX::Keyboard::State keyboardState = DirectX::Keyboard::Get().GetState();
	DirectX::Mouse::State mouseState = DirectX::Mouse::Get().GetState();

	//if (m_tankType == TankType::Player)
	//{
	//	// 砲塔の回転
	//	if (keyboardState.Left)
	//	{
	//		m_turretAngle += DirectX::XMConvertToRadians(0.2f);
	//	}
	//	else if (keyboardState.Right)
	//	{
	//		m_turretAngle -= DirectX::XMConvertToRadians(0.2f);
	//	}
	//}

	// 最初の回転角を設定
	//m_turretAngle = DirectX::XMConvertToRadians(90);
	m_turretAngle = DirectX::XMConvertToRadians (1280.0 / 10.0);

	// マウス座標に応じて回転
	m_turretAngle -= DirectX::XMConvertToRadians(static_cast<float>(mouseState.x) / 5.0f);

	// 回転の制限
	m_turretAngle = TankBase::Clamp(m_turretAngle, DirectX::XMConvertToRadians(-90.0f), DirectX::XMConvertToRadians(90.0f));

	// 現在の位置の更新
	m_currentPosition = currentPosition;
	// 現在の回転角の更新
	m_currentAngleRL = currentAngleRL;
	// 「砲身」の更新
	TankBase::Update(elapsedTime, currentPosition, currentAngleRL + m_turretAngle);

	
}

// 描画処理
void TankTurret::Render()
{
	using namespace DirectX::SimpleMath;

	// ワールド行列を生成する
	m_worldMatrix = Matrix::CreateScale(0.5f) *
		Matrix::CreateRotationY(m_currentAngleRL + GetInitialAngleRL() + m_turretAngle) *
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