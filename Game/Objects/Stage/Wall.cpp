/**
 * @file   Wall.cpp
 * @brief  壁クラス
 */
#include "pch.h"
#include "Game/Objects/Stage/Wall.h"
#include "Framework/Graphics.h"
#include "Libraries/MyLib/FollowCamera.h"

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="scale">サイズ</param>
/// <param name="position">座標</param>
/// <param name="type">壁タイプ</param>
Wall::Wall(
	const DirectX::SimpleMath::Vector3& scale,
	const DirectX::SimpleMath::Vector3& position,
	WallType type
)
	:
	m_camera{},
	m_graphics{ Graphics::GetInstance() },
	m_color{ static_cast<DirectX::SimpleMath::Vector4>(DirectX::Colors::DimGray) },
	m_isActive{}
{
	using namespace DirectX::SimpleMath;
	using namespace DirectX;

	auto context = m_graphics->GetDeviceResources()->GetD3DDeviceContext();

	// ジオメトリックプリミティブを生成する
	m_model = GeometricPrimitive::CreateBox(context, scale);

	// 座標の設定
	m_position = position;

	// ボックスコライダーの作成
	m_collider = std::make_unique<BoxCollider>();
	m_collider->CreateBoundingBox(position, scale);

	// 壁タイプの設定
	m_wallType = type;
}

/// <summary>
/// デストラクタ
/// </summary>
Wall::~Wall()
{
}

/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime">フレーム間の経過時間</param>
void Wall::Update(float elapsedTime)
{
	// 壁を上げる、もしくは下げる処理
	if (m_wallType == WallType::FIXED) { return; }
	if (m_isActive)
		this->RaiseWall(elapsedTime);
	else
		this->LowerWall(elapsedTime);
}

/// <summary>
/// 描画処理
/// </summary>
void Wall::Render()
{
	using namespace DirectX::SimpleMath;

	// 当たり判定の更新
	m_collider->Update(m_position);

	// 当たり判定の処理
	this->DetectCollision();

	Matrix world = Matrix::CreateTranslation(m_position);
	auto& view = m_graphics->GetViewMatrix();
	auto& proj = m_graphics->GetProjectionMatrix();

	// コライダーの描画
	m_collider->Render(DirectX::Colors::Black);

	// 壁の描画
	m_model->Draw(world, view, proj, m_color);
}

/// <summary>
/// 終了処理
/// </summary>
void Wall::Finalize()
{
}

/// <summary>
/// カメラとの衝突判定
/// </summary>
void Wall::DetectCollision()
{
	// カメラとの当たり判定
	m_camera->SetEyePosition(m_camera->GetEyePosition() + m_collider->CheckCollisionCollider(m_camera->GetBoundingSphere()));
}


/// <summary>
/// 壁をゆっくり上げる処理
/// </summary>
/// <param name="elapsedTime">フレーム間の経過時間</param>
void Wall::RaiseWall(float elapsedTime)
{
	if (m_position.y < 0.5f)
		m_position.y += 0.5f * elapsedTime;
}

/// <summary>
/// 壁をゆっくり下げる処理
/// </summary>
/// <param name="elapsedTime">フレーム間の経過時間</param>
void Wall::LowerWall(float elapsedTime)
{
	if (m_position.y >= -1.0f)
		m_position.y -= 0.5f * elapsedTime;
}
