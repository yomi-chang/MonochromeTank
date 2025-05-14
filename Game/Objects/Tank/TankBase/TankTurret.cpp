/*
	@file	TankTurret.cpp
	@brief	砲塔クラス
*/
#include "pch.h"
#include "Game/Objects/Tank/TankBase/TankTurret.h"
#include "Game/Objects/Tank/TankBase/TankCannon.h"
#include "Game/Objects/Tank/TankBase/Tank.h"

//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
TankTurret::TankTurret(
	Tank* tank,
	const DirectX::SimpleMath::Vector3& initialPosition,
	const float& initialAngle
)
	:
	m_graphics{ m_graphics = Graphics::GetInstance() },
	m_initialPosition{ initialPosition },
	m_initialRotation{ DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3::Up, initialAngle) },
	m_currentPosition{},
	m_currentRotation{},
	m_tankParts{},
	m_worldMatrix{},
	m_model{},
	m_turretRotation{},
	m_tank{}
{
	// 戦車情報の受け取り
	m_tank = tank;
}

//---------------------------------------------------------
// デストラクタ
//---------------------------------------------------------
TankTurret::~TankTurret()
{
}

//---------------------------------------------------------
// 初期化処理
//---------------------------------------------------------
void TankTurret::Initialize()
{
	using namespace DirectX::SimpleMath;

	// 砲身の生成
	this->Attach(std::make_unique<TankCannon>(m_tank, Vector3(0.0f, 0.0f, 0.0f), 0.0f));

	// モデルの取得
	m_model = Resources::GetInstance()->GetTankTurretModel();

	// 戦車に砲塔情報を渡す
	m_tank->SetTurret(this);
}

//---------------------------------------------------------
// 更新処理
//---------------------------------------------------------
void TankTurret::Update(
	float elapsedTime,
	const DirectX::SimpleMath::Vector3& currentPosition,
	const DirectX::SimpleMath::Quaternion& currentRotation
)
{
	// 現在位置の更新
	m_currentPosition = currentPosition + m_initialPosition;
	m_currentRotation =  currentRotation * m_initialRotation * m_turretRotation;

	// 部品の更新
	for (auto& part : m_tankParts)
	{
		part->Update(elapsedTime, m_currentPosition, m_currentRotation);
	}
}

//---------------------------------------------------------
// 描画処理
//---------------------------------------------------------
void TankTurret::Render()
{
	using namespace DirectX::SimpleMath;

	// ワールド行列の生成
	m_worldMatrix = Matrix::CreateScale(Tank::TANK_SIZE) *
		Matrix::CreateFromQuaternion(m_currentRotation) *
		Matrix::CreateTranslation(m_currentPosition);

	// 「砲塔」の描画
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
void TankTurret::Finalize()
{
}

//---------------------------------------------------------
// パーツの追加
//---------------------------------------------------------
void TankTurret::Attach(std::unique_ptr<IParts> parts)
{
	// パーツの初期化
	parts->Initialize();
	// パーツの追加
	m_tankParts.emplace_back(std::move(parts));
}

//---------------------------------------------------------
// パーツの削除
//---------------------------------------------------------
void TankTurret::Detach(std::unique_ptr<IParts> parts)
{
}

//---------------------------------------------------------
// 砲塔の回転
//---------------------------------------------------------
void TankTurret::RotateTurret(float angle)
{
	using namespace DirectX::SimpleMath;

	// クォータニオンに変換して適用
	m_turretRotation = Quaternion::CreateFromYawPitchRoll(angle, 0.0f, 0.0f);
}