/*
	@file	TankBody.cpp
	@brief	車体クラス
*/
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
	m_initialRotation{ DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3::Up, initialAngle) },
	m_currentPosition{},
	m_currentRotation{},
	m_tankParts{},
	m_worldMatrix{},
	m_model{},
	m_bodyRotation{},
	m_tank{},
	m_gravityEnabled{}
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

	// 砲塔の生成
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
	const DirectX::SimpleMath::Quaternion& currentRotation
)
{
	// 重力をかける
	if (m_currentPosition.y > 0.0f && m_gravityEnabled)
		m_currentPosition.y -= 0.1f;
	// 重力を有効にする
	m_gravityEnabled = true;

	// 現在回転角の更新
	m_currentRotation = currentRotation * m_initialRotation * m_bodyRotation;

	// 部品の更新
	for (auto& part : m_tankParts)
	{
		part->Update(elapsedTime, m_currentPosition, m_currentRotation);
	}
}

//---------------------------------------------------------
// 描画処理
//---------------------------------------------------------
void TankBody::Render()
{
	using namespace DirectX::SimpleMath;

	// ワールド行列の生成
	m_worldMatrix = Matrix::CreateScale(Tank::TANK_SIZE) *
		Matrix::CreateFromQuaternion(m_currentRotation) *
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
void TankBody::Rotate(DirectX::SimpleMath::Quaternion rotation)
{
	// 回転を加える
	m_bodyRotation *= rotation;
}
