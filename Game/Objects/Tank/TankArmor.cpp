#include "pch.h"
#include "Game/Objects/Tank/TankArmor.h"
#include "Game/Objects/Tank/TankTurret.h"
#include "Framework/Resources.h"
#include "Framework/InputManager.h"

// コンストラクタ
TankArmor::TankArmor(
	IComponent* parent,
	const DirectX::SimpleMath::Vector3& initialPosition,
	const float& initialAngleRL
)
	:
	m_parent{ parent },
	m_graphics{Graphics::GetInstance()},
	m_initialPosition{initialPosition},
	m_initialAngle{initialAngleRL},
	m_currentPosition{},
	m_currentAngleRL{},
	m_tankParts{},
	m_model{},
	m_worldMatrix{},
	m_tankType{},
	m_color{ static_cast<DirectX::SimpleMath::Vector4>(DirectX::Colors::Silver) }
{
}

// デストラクタ
TankArmor::~TankArmor()
{
	Finalize();
}

// 初期化処理
void TankArmor::Initialize(Type type)
{
	using namespace DirectX::SimpleMath;

	// タイプの確定
	m_tankType = type;

	// 装甲モデル（仮）
	m_model = DirectX::GeometricPrimitive::CreateSphere(m_graphics->GetDeviceResources()->GetD3DDeviceContext(), 3.0f);

	// カラー
	m_color.w = 0.3f;
}

// 更新処理
void TankArmor::Update(
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

	// パーツの更新
	for (auto& tankPart : m_tankParts)
	{
		tankPart->Update(elapsedTime, currentPosition, currentAngleRL);
	}
}

// 描画処理
void TankArmor::Render()
{
	using namespace DirectX::SimpleMath;

	// ワールド行列を生成する
	m_worldMatrix = Matrix::CreateScale(0.5f) *
		Matrix::CreateRotationY(m_currentAngleRL + m_initialAngle) *
		Matrix::CreateTranslation(m_currentPosition + m_initialPosition);

	//// プリミティブ描画を開始する
	//m_graphics->DrawPrimitiveBegin(m_graphics->GetViewMatrix(), m_graphics->GetProjectionMatrix());
	//// 「砲塔下部」を描画する
	//m_graphics->DrawModel(m_model, m_worldMatrix);
	//// プリミティブ描画を終了する
	//m_graphics->DrawPrimitiveEnd();

	auto view = m_graphics->GetViewMatrix();
	auto proj = m_graphics->GetProjectionMatrix();

	m_model->Draw(m_worldMatrix, view, proj, m_color);

	// パーツの描画
	for (auto& tankPart : m_tankParts)
	{
		tankPart->Render();
	}
}

// 終了処理
void TankArmor::Finalize()
{
	// 削除する部品をリセットする
	m_tankParts.clear();
}