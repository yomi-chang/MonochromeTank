/**
 * @file   TankTurret.cpp
 * @brief  砲塔クラス
 */
#include "pch.h"
#include "Game/Objects/Tank/TankBase/TankTurret.h"
#include "Game/Objects/Tank/TankBase/TankCannon.h"
#include "Game/Objects/Tank/TankBase/Tank.h"

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="tank">戦車の情報</param>
/// <param name="initialPosition">初期座標</param>
/// <param name="initialAngle">初期角度</param>
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

/// <summary>
/// デストラクタ
/// </summary>
TankTurret::~TankTurret()
{
}

/// <summary>
/// 初期化処理
/// </summary>
void TankTurret::Initialize()
{
	using namespace DirectX::SimpleMath;

	// 砲身の生成
	this->Attach(std::make_unique<TankCannon>(m_tank, Vector3(0.0f, 0.0f, 0.0f), 0.0f));

	// モデルの取得
	m_model = Resources::GetInstance()->GetTankModel(m_tank->GetTankNumber(), Resources::TURRET);

	// 戦車に砲塔情報を渡す
	m_tank->SetTurret(this);
}

/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime">フレーム間の経過時間</param>
/// <param name="currentPosition">現在の座標</param>
/// <param name="currentRotation">現在の角度</param>
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

/// <summary>
/// 描画処理
/// </summary>
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

/// <summary>
/// 終了処理
/// </summary>
void TankTurret::Finalize()
{
}

/// <summary>
/// パーツの追加
/// </summary>
/// <param name="parts">パーツ</param>
void TankTurret::Attach(std::unique_ptr<IParts> parts)
{
	// パーツの初期化
	parts->Initialize();
	// パーツの追加
	m_tankParts.emplace_back(std::move(parts));
}

/// <summary>
/// パーツの削除
/// </summary>
/// <param name="parts">パーツ</param>
void TankTurret::Detach(std::unique_ptr<IParts> parts)
{
}

/// <summary>
/// 砲塔の回転
/// </summary>
/// <param name="angle">角度</param>
void TankTurret::RotateTurret(float angle)
{
	using namespace DirectX::SimpleMath;

	// クォータニオンに変換して適用
	m_turretRotation = Quaternion::CreateFromYawPitchRoll(angle, 0.0f, 0.0f);
}