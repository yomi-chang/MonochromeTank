#include "pch.h"
#include "Game/Objects/Tank/TankBase.h"
#include "Game/Objects/Tank/TankTurret.h"
#include "Game/Objects/Tank/TankCannon.h"
#include "Framework/Resources.h"

// コンストラクタ
TankTurret::TankTurret(
	IComponent* parent,
	const DirectX::SimpleMath::Vector3& initialPosition,
	const float& initialAngleRL
)
	:
	m_parent{ parent },
	m_graphics{Graphics::GetInstance()},
	m_initialPosition{ initialPosition },
	m_initialAngle{ initialAngleRL },
	m_currentPosition{},
	m_currentAngleRL{},
	m_tankParts{},
	m_model{},
	m_worldMatrix{},
	m_turretAngle{},
	m_tankType{}
{
}

// デストラクタ
TankTurret::~TankTurret()
{
	Finalize();
}

// 初期化処理
void TankTurret::Initialize(Type type)
{
	using namespace DirectX::SimpleMath;

	// タイプの確定
	m_tankType = type;

	// モデル情報の受け取り
	m_model = Resources::GetInstance()->GetTankTurretModel();

	// 砲身の生成
	Attach(std::make_unique<TankCannon>(this, Vector3{ 0.0f,0.75f,0.0f }, 0.0f));
}

// 更新処理
void TankTurret::Update(
	float elapsedTime,
	const DirectX::SimpleMath::Vector3& currentPosition,
	const float& currentAngleRL
)
{
	UNREFERENCED_PARAMETER(elapsedTime);

	// キーボードステートの取得
	DirectX::Keyboard::State keyboardState = DirectX::Keyboard::Get().GetState();
	DirectX::Mouse::State mouseState = DirectX::Mouse::Get().GetState();

	if (m_tankType == Type::PLAYER)
	{
		// 砲塔の回転
		if (keyboardState.Left)
		{
			m_turretAngle += DirectX::XMConvertToRadians(0.5f);
		}
		else if (keyboardState.Right)
		{
			m_turretAngle -= DirectX::XMConvertToRadians(0.5f);
		}
	}

	// 最初の回転角を設定
	//m_turretAngle = DirectX::XMConvertToRadians (1280.0f / 10.0f);
	// マウス座標に応じて回転
	//m_turretAngle -= DirectX::XMConvertToRadians(static_cast<float>(mouseState.x) / 5.0f);

	// 回転の制限
	//m_turretAngle = TankBase::Clamp(m_turretAngle, DirectX::XMConvertToRadians(-90.0f), DirectX::XMConvertToRadians(90.0f));

	// 現在の位置の更新
	m_currentPosition = currentPosition;
	// 現在の回転角の更新
	m_currentAngleRL = currentAngleRL;

	// パーツの更新
	for (auto& tankPart : m_tankParts)
	{
		tankPart->Update(elapsedTime, currentPosition, currentAngleRL + m_turretAngle);
	}
	
}

// 描画処理
void TankTurret::Render()
{
	using namespace DirectX::SimpleMath;

	// ワールド行列を生成する
	m_worldMatrix = Matrix::CreateScale(0.5f) *
		Matrix::CreateRotationY(m_currentAngleRL + m_initialAngle + m_turretAngle) *
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
void TankTurret::Finalize()
{
	// 削除する部品のリセット
	m_tankParts.clear();
}

/// <summary>
/// パーツの追加
/// </summary>
/// <param name="part">パーツ</param>
void TankTurret::Attach(std::unique_ptr<IComponent> part)
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
void TankTurret::Detach(std::unique_ptr<IComponent> part)
{
}