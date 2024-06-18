#include "pch.h"
#include "TankBase.h"

// コンストラクタ
TankBase::TankBase(
	ITankComponent* parent,
	const DirectX::SimpleMath::Vector3& initialPosition,
	const float& initialAngleRL
)
	:
	m_graphics{ Graphics::GetInstance() },
	m_parent{ parent },
	m_initialPosition{ initialPosition },
	m_initialAngleRL{ initialAngleRL },
	m_tankParts{},
	m_model{}
{
}

// デストラクタ
TankBase::~TankBase()
{
}

// 初期化処理
void TankBase::Initialize()
{
}

// モデルのセット
void TankBase::SetModel(DirectX::Model* model)
{
	// モデル情報を受け取り
	m_model = model;
}

// 更新処理
void TankBase::Update(
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
void TankBase::Render()
{
	// パーツを描画
	for (auto& tankPart : m_tankParts)
	{
		tankPart->Render();
	}
}

// 描画処理
void TankBase::Render(const DirectX::SimpleMath::Matrix& matrix)
{
	// プリミティブ描画を開始
	m_graphics->DrawPrimitiveBegin(m_graphics->GetViewMatrix(), m_graphics->GetProjectionMatrix());

	// パーツの描画
	m_graphics->DrawModel(m_model, matrix);

	// プリミティブ描画を終了
	m_graphics->DrawPrimitiveEnd();

	// パーツが存在するかどうかの判断
	if (m_tankParts.size() == 0) { return; }

	// パーツを描画
	for (auto& turretPart : m_tankParts)
	{
		turretPart->Render();
	}
}

// パーツの追加
void TankBase::Attach(std::unique_ptr<ITankComponent> tankPart)
{
	// 初期化する
	tankPart->Initialize();

	// 部品を追加する
	m_tankParts.emplace_back(std::move(tankPart));
}

// クランプ関数
float TankBase::Clamp(float value, float min, float max)
{
	return std::max(min, std::min(value, max));
}
