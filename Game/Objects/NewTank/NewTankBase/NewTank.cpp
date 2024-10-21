#include "pch.h"
#include "Game/Objects/NewTank/NewTankBase/NewTank.h"
#include "Game/Objects/NewTank/NewTankBase/NewTankBody.h"

//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
NewTank::NewTank(
	const DirectX::SimpleMath::Vector3& initialPosition,
	const float& initialAngle
)
	:
	m_graphics{ Graphics::GetInstance() },
	m_initialPosition{initialPosition},
	m_initialAngle{ DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3::Up, initialAngle) },
	m_currentPosition{},
	m_currentAngle{},
	m_tankParts{},
	m_worldMatrix{}
{
}

//---------------------------------------------------------
// デストラクタ
//---------------------------------------------------------
NewTank::~NewTank()
{
}

//---------------------------------------------------------
// 初期化処理
//---------------------------------------------------------
void NewTank::Initialize()
{
	using namespace DirectX::SimpleMath;

	// 車体の生成(地面から0.5f高い座標に生成)
	Attach(std::make_unique<NewTankBody>(this,Vector3(0.0f, 0.5f, 0.0f), 0.0f));

	// 現在位置の更新
	m_currentPosition = m_initialPosition;
	m_currentAngle = m_initialAngle;
}

//---------------------------------------------------------
// 更新処理
//---------------------------------------------------------
void NewTank::Update(float elapsedTime)
{
	// 部品の更新
	for (auto& part : m_tankParts)
	{
		part->Update(elapsedTime, m_currentPosition, m_currentAngle);
	}
}

//---------------------------------------------------------
// 描画処理
//---------------------------------------------------------
void NewTank::Render()
{
	// 部品の描画
	for (auto& part : m_tankParts)
	{
		part->Render();
	}
}

//---------------------------------------------------------
// 終了処理
//---------------------------------------------------------
void NewTank::Finalize()
{
}

//---------------------------------------------------------
// パーツの追加
//---------------------------------------------------------
void NewTank::Attach(std::unique_ptr<IObject> part)
{
	// パーツの初期化
	part->Initialize();
	// パーツの追加
	m_tankParts.emplace_back(std::move(part));
}

//---------------------------------------------------------
// パーツの削除
//---------------------------------------------------------
void NewTank::Detach(std::unique_ptr<IObject> part)
{
}
