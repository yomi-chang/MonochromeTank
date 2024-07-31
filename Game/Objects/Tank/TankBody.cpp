#include "pch.h"
#include "Game/Objects/Tank/TankBase.h"
#include "Game/Objects/Tank/TankBody.h"
#include "Game/Objects/Tank/TankTurret.h"
#include "Framework/Resources.h"
#include "Framework/InputManager.h"

// コンストラクタ
TankBody::TankBody(
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
	m_tankType{}
{
}

// デストラクタ
TankBody::~TankBody()
{
	Finalize();
}

// 初期化処理
void TankBody::Initialize(Type type)
{
	using namespace DirectX::SimpleMath;

	// タイプの確定
	m_tankType = type;

	// モデル情報の受け取り
	m_model = Resources::GetInstance()->GetTankBodyModel();

	// 砲塔の生成
	Attach(std::make_unique<TankTurret>(this, Vector3{ 0.0f,0.75f,0.0f },0.0f));
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

	// パーツの更新
	for (auto& tankPart : m_tankParts)
	{
		tankPart->Update(elapsedTime, currentPosition, currentAngleRL);
	}
}

// 描画処理
void TankBody::Render()
{
	using namespace DirectX::SimpleMath;

	// ワールド行列を生成する
	m_worldMatrix = Matrix::CreateScale(0.5f) *
		Matrix::CreateRotationY(m_currentAngleRL + m_initialAngle) *
		Matrix::CreateTranslation(m_currentPosition + m_initialPosition);

	// プリミティブ描画を開始する
	m_graphics->DrawPrimitiveBegin(m_graphics->GetViewMatrix(), m_graphics->GetProjectionMatrix());
	// 「砲塔下部」を描画する
	m_graphics->DrawModel(m_model, m_worldMatrix);
	// プリミティブ描画を終了する
	m_graphics->DrawPrimitiveEnd();

	// パーツの描画
	for (auto& tankPart : m_tankParts)
	{
		tankPart->Render();
	}
}

// 終了処理
void TankBody::Finalize()
{
	// 削除する部品をリセットする
	m_tankParts.clear();
}

/// <summary>
/// パーツの追加
/// </summary>
/// <param name="part">パーツ</param>
void TankBody::Attach(std::unique_ptr<IComponent> part)
{
	// パーツの初期化
	part->Initialize(m_tankType);
	// パーツの追加
	m_tankParts.emplace_back(std::move(part));
}

/// <summary>
/// パーツの削除
/// </summary>
/// <param name="part">パーツ</param>
void TankBody::Detach(std::unique_ptr<IComponent> part)
{
}