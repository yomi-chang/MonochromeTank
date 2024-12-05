#include "pch.h"
#include "Game/Objects/Tank/TankBase/TankBody.h"
#include "Game/Objects/Tank/TankBase/TankTurret.h"
#include "Game/Objects/Tank/TankBase/Tank.h"

//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
TankBody::TankBody(
	Tank* tank,
	const DirectX::SimpleMath::Vector3& initialPosition,
	const float& initialAngle
)
	:
	m_graphics{ Graphics::GetInstance() },
	m_initialPosition{ initialPosition },
	m_initialAngle{ DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3::Up, initialAngle) },
	m_currentPosition{},
	m_currentAngle{},
	m_tankParts{},
	m_worldMatrix{},
	m_model{},
	m_bodyAngle{},
	m_tank{}
{
	// 戦車情報の受け取り
	m_tank = tank;
}

//---------------------------------------------------------
// デストラクタ
//---------------------------------------------------------
TankBody::~TankBody()
{
}

//---------------------------------------------------------
// 初期化処理
//---------------------------------------------------------
void TankBody::Initialize()
{
	using namespace DirectX::SimpleMath;

	// 砲塔の生成(車体の中心から0.3f高い座標に生成)
	Attach(std::make_unique<TankTurret>(m_tank,Vector3(0.0f, 0.0f, 0.0f), 0.0f));

	// モデルの取得
	m_model = Resources::GetInstance()->GetTankBodyModel();

	// 戦車に車体情報を渡す
	m_tank->SetBody(this);

	// 初期座標の確定
	m_currentPosition = m_initialPosition + m_tank->GetInitialPosition();
}

//---------------------------------------------------------
// 更新処理
//---------------------------------------------------------
void TankBody::Update(
	float elapsedTime,
	const DirectX::SimpleMath::Vector3& currentPosition,
	const DirectX::SimpleMath::Quaternion& currentAngle
)
{
	// 現在回転角の更新
	m_currentAngle = currentAngle * m_initialAngle * m_bodyAngle;

	// 部品の更新
	for (auto& part : m_tankParts)
	{
		part->Update(elapsedTime, m_currentPosition, m_currentAngle);
	}
}

//---------------------------------------------------------
// 描画処理
//---------------------------------------------------------
void TankBody::Render()
{
	using namespace DirectX::SimpleMath;

	// ワールド行列の生成
	m_worldMatrix = Matrix::CreateScale(0.09f) *
		Matrix::CreateFromQuaternion(m_currentAngle) *
		Matrix::CreateTranslation(m_currentPosition);

	// 「車体」の描画
	m_graphics->DrawModel(m_model, m_worldMatrix);

	// 部品の描画
	for (auto& part : m_tankParts)
	{
		part->Render();
	}
}

//---------------------------------------------------------
// 終了処理
//---------------------------------------------------------
void TankBody::Finalize()
{
}

//---------------------------------------------------------
// パーツの追加
//---------------------------------------------------------
void TankBody::Attach(std::unique_ptr<IParts> parts)
{
	// パーツの初期化
	parts->Initialize();
	// パーツの追加
	m_tankParts.emplace_back(std::move(parts));
}

//---------------------------------------------------------
// パーツの削除
//---------------------------------------------------------
void TankBody::Detach(std::unique_ptr<IParts> parts)
{
}

//---------------------------------------------------------
// 移動処理
//---------------------------------------------------------
void TankBody::Move(DirectX::SimpleMath::Vector3 velocity)
{
	// 速度の加算
	m_currentPosition += velocity;
}

//---------------------------------------------------------
// 回転処理
//---------------------------------------------------------
void TankBody::Rotate(DirectX::SimpleMath::Quaternion angle)
{
	// 回転を加える
	m_bodyAngle *= angle;
}
