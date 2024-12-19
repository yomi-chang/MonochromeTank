#include "pch.h"
#include "Game/Objects/Stage/Wall.h"
#include "Framework/Graphics.h"
#include "Libraries/MyLib/FollowCamera.h"

#include "Game/Objects/Tank/TankBase/Tank.h"
#include "Libraries/MyLib/DebugLog.h"

//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
Wall::Wall(
	DirectX::SimpleMath::Vector3 scale,
	DirectX::SimpleMath::Vector3 position,
	WallType type
)
	:
	m_tanks{},
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

// デストラクタ
Wall::~Wall()
{
}

//---------------------------------------------------------
// 更新処理
//---------------------------------------------------------
void Wall::Update(float elapsedTime)
{
	// 壁を上げる、もしくは下げる処理
	if (m_wallType == WallType::FIXED) { return; }
	if (m_isActive)
		RaiseWall(elapsedTime);
	else
		LowerWall(elapsedTime);
}

//---------------------------------------------------------
// 描画処理
//---------------------------------------------------------
void Wall::Render()
{
	using namespace DirectX::SimpleMath;

	// 当たり判定の更新
	m_collider->Update(m_position);

	// 当たり判定の処理
	DetectCollision();

	Matrix world = Matrix::CreateTranslation(m_position);
	auto view = m_graphics->GetViewMatrix();
	auto proj = m_graphics->GetProjectionMatrix();

	// コライダーの描画
	m_collider->Render(DirectX::Colors::Black);

	// 壁の描画
	m_model->Draw(world, view, proj, m_color);
}

//---------------------------------------------------------
// 終了処理
//---------------------------------------------------------
void Wall::Finalize()
{
}

//---------------------------------------------------------
// 衝突判定
//---------------------------------------------------------
void Wall::DetectCollision()
{
	// 戦車との当たり判定
	for (auto& tank : m_tanks)
	{
		tank->GetBody()->SetCollisionVel(m_collider->CheckCollisionCollider(tank->GetBoundingBox()));
		// 壁に触れている間重力を無効にする
		if(m_collider->CheckTriggerCollider(tank->GetBoundingBox()))
		tank->GetBody()->SetGravity(false);
	}

	// カメラとの当たり判定
	m_camera->SetEyePosition(m_camera->GetEyePosition() + m_collider->CheckCollisionCollider(m_camera->GetBoundingSphere()));
}

// 壁をゆっくり上げる処理
void Wall::RaiseWall(float elapsedTime)
{
	if (m_position.y < 0.5f)
		m_position.y += 0.5f * elapsedTime;
}

// 壁をゆっくりと下げる処理
void Wall::LowerWall(float elapsedTime)
{
	if (m_position.y > -0.6f)
		m_position.y -= 0.5f * elapsedTime;
}
