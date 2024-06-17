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
	m_currentPosition{},
	m_currentAngleRL{},
	m_tankParts{},
	m_model{},
	m_worldMatrix{},
	m_currentAngleUD{}
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

	// 現在の位置を更新する
	m_currentPosition = currentPosition;
	// 現在の回転角を更新する
	m_currentAngleRL = currentAngleRL;

	// キーボードステートの取得
	DirectX::Keyboard::State keyboardState = DirectX::Keyboard::Get().GetState();

	// 砲身の上下
	if (keyboardState.Up)
	{
		m_currentAngleUD += DirectX::XMConvertToRadians(0.2f);
	}
	else if ( keyboardState.Down)
	{
		m_currentAngleUD -= DirectX::XMConvertToRadians(0.2f);
	}
}

// 自身を描画しない描画処理(Tank用)
void TankCannon::Render()
{
	using namespace DirectX::SimpleMath;

	// ワールド行列を生成する
	m_worldMatrix = Matrix::CreateScale(1.0f);
	m_worldMatrix *= Matrix::CreateRotationX(m_currentAngleUD);
	m_worldMatrix *= Matrix::CreateTranslation(Vector3(0.0f, 0.0f, -0.6f));
	m_worldMatrix *= Matrix::CreateRotationY(m_currentAngleRL + GetInitialAngleRL());
	m_worldMatrix *= Matrix::CreateTranslation(m_currentPosition + GetInitialPosition());
	
	// 描画を行う
	TankBase::Render(m_worldMatrix);
}

// 終了処理
void TankCannon::Finalize()
{
}