/**
 * @file   TankBody.cpp
 * @brief  車体クラス
 */
#include "pch.h"
#include "Game/Objects/Tank/TankBase/TankBody.h"
#include "Game/Objects/Tank/TankBase/TankTurret.h"
#include "Game/Objects/Tank/TankBase/Tank.h"

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="tank">戦車情報</param>
/// <param name="initialPosition">初期座標</param>
/// <param name="initialAngle">初期角度</param>
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

/// <summary>
/// デストラクタ
/// </summary>
TankBody::~TankBody()
{
}

/// <summary>
/// 初期化処理
/// </summary>
void TankBody::Initialize()
{
	using namespace DirectX::SimpleMath;
	using namespace DirectX;

	// 砲塔の生成
	this->Attach(std::make_unique<TankTurret>(m_tank,Vector3(0.0f, 0.0f, 0.0f), 0.0f));

	// モデルの取得
	m_model = Resources::GetInstance()->GetTankModel(m_tank->GetTankNumber(), Resources::BODY);

	// 戦車に車体情報を渡す
	m_tank->SetBody(this);

	// 初期座標の確定
	m_currentPosition = m_initialPosition + m_tank->GetInitialPosition();
}

/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime">フレーム間の経過時間</param>
/// <param name="currentPosition">現在の座標</param>
/// <param name="currentRotation">現在の角度</param>
void TankBody::Update(
	float elapsedTime,
	const DirectX::SimpleMath::Vector3& currentPosition,
	const DirectX::SimpleMath::Quaternion& currentRotation
)
{
	UNREFERENCED_PARAMETER(currentPosition);

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

/// <summary>
/// 描画処理
/// </summary>
void TankBody::Render()
{
	using namespace DirectX;
	using namespace DirectX::SimpleMath;

	// ワールド行列の生成
	m_worldMatrix = Matrix::CreateScale(Tank::TANK_SIZE) *
		Matrix::CreateFromQuaternion(m_currentRotation) *
		Matrix::CreateTranslation(m_currentPosition);
	
	// テクスチャの貼り付け
	/*m_model->UpdateEffects([](IEffect* effect)
		{
			BasicEffect* basicEffect = dynamic_cast<BasicEffect*>(effect);
			if (basicEffect)
			{
				basicEffect->SetTextureEnabled(true);
				basicEffect->SetTexture(Resources::GetInstance()->GetTank1Texture());

				basicEffect->SetLightingEnabled(true);
				basicEffect->SetDiffuseColor(DirectX::Colors::White);
				basicEffect->SetPerPixelLighting(true);
			}
		});*/

	// モデルの描画
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
void TankBody::Finalize()
{
}

/// <summary>
/// パーツの追加
/// </summary>
/// <param name="parts">パーツ</param>
void TankBody::Attach(std::unique_ptr<IParts> parts)
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
void TankBody::Detach(std::unique_ptr<IParts> parts)
{
}

/// <summary>
/// 移動処理
/// </summary>
/// <param name="velocity">速度</param>
void TankBody::Move(const DirectX::SimpleMath::Vector3& velocity)
{
	// 速度の加算
	m_currentPosition += velocity;
}

/// <summary>
/// 回転処理
/// </summary>
/// <param name="rotation">回転角</param>
void TankBody::Rotate(const DirectX::SimpleMath::Quaternion& rotation)
{
	// 回転を加える
	m_bodyRotation *= rotation;
}
