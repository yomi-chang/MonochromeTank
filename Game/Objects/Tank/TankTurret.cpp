#include "pch.h"
#include "Game/Objects/Tank/TankTurret.h"
#include "Game/Objects/Tank/TankCannon.h"
#include "Framework/Resources.h"
#include "Framework/InputManager.h"

#include "Libraries/MyLib/Math.h"

// コンストラクタ
TankTurret::TankTurret(
	IComponent* parent,
	const DirectX::SimpleMath::Vector3& initialPosition,
	const float& initialAngle
)
	:
	m_parent{ parent },
	m_graphics{Graphics::GetInstance()},
	m_initialPosition{ initialPosition },
	m_initialAngle(DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3::Up, initialAngle)),
	m_currentPosition{},
	m_currentAngle{},
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
	const DirectX::SimpleMath::Quaternion& currentAngle
)
{
	UNREFERENCED_PARAMETER(elapsedTime);
	using namespace DirectX::SimpleMath;

	// マウスステートの取得
	const auto& mouseState = InputManager::GetInstance()->GetMouseState();

	if (m_tankType == Type::PLAYER)
	{
		//// マウスの移動量を取得して回転させる
		//float turretAngle = 0.0f;
		//turretAngle += static_cast<float>(mouseState.x) * 0.0001f;
		//m_turretAngle *= Quaternion::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(turretAngle), 0.0f, 0.0f);
		//// 回転の制限
		//m_turretAngle.y = mylib::Clamp(m_turretAngle.y, TURRET_ANGLEUD_MIN, TURRET_ANGLEUD_MAX);

		// マウスの移動からX軸回転角を計算
		float rotationY = static_cast<float>(mouseState.x) * 0.001f;
		// 現在の砲身角度をクォータニオンからオイラー角に変換
		DirectX::SimpleMath::Vector3 eulerAngles = m_turretAngle.ToEuler();
		// 砲身のX軸回転を更新
		eulerAngles.y -= rotationY;
		// X軸の回転範囲をクランプ（範囲制限）
		eulerAngles.y = mylib::Clamp(eulerAngles.y, TURRET_ANGLEUD_MIN, TURRET_ANGLEUD_MAX);
		// クランプされたオイラー角をクォータニオンに変換して適用
		m_turretAngle = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(eulerAngles.y, eulerAngles.x, eulerAngles.z);
	}

	// 現在の位置の更新
	m_currentPosition = currentPosition + m_initialPosition;
	// 現在の回転角の更新
	m_currentAngle = m_turretAngle * currentAngle * m_initialAngle;

	// パーツの更新
	for (auto& tankPart : m_tankParts)
	{
		tankPart->Update(elapsedTime, currentPosition, m_currentAngle);
	}
}

// 描画処理
void TankTurret::Render()
{
	using namespace DirectX::SimpleMath;

	// ワールド行列を生成する
	m_worldMatrix = Matrix::CreateScale(0.5f) *
		Matrix::CreateFromQuaternion(m_currentAngle) *
		Matrix::CreateTranslation(m_currentPosition);

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