#include "pch.h"
#include "Game/Objects/Stage/Wall.h"
#include "Framework/Graphics.h"
#include "Libraries/MyLib/FollowCamera.h"

#include "Game/Objects/Tank/TankBase/Tank.h"

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
	m_color{ static_cast<DirectX::SimpleMath::Vector4>(DirectX::Colors::DimGray) }
{
	using namespace DirectX::SimpleMath;
	using namespace DirectX;

	auto context = m_graphics->GetDeviceResources()->GetD3DDeviceContext();

	// ジオメトリックプリミティブを生成する
	m_model = GeometricPrimitive::CreateBox(context, scale);

	// ワールド座標の設定
	m_world = Matrix::CreateTranslation(position);

	// ボックスコライダーの作成
	m_collider = std::make_unique<BoxCollider>();
	m_collider->CreateBoundingBox(position, scale);
}

// デストラクタ
Wall::~Wall()
{
}

//---------------------------------------------------------
// 描画処理
//---------------------------------------------------------
void Wall::Render()
{
	// 当たり判定の処理
	DetectCollision();

	auto view = m_graphics->GetViewMatrix();
	auto proj = m_graphics->GetProjectionMatrix();

	// コライダーの描画
	m_collider->Render(DirectX::Colors::Black);

	// 壁の描画
	m_model->Draw(m_world, view, proj, m_color);
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
	}

	// カメラとの当たり判定
	m_camera->SetEyePosition(m_camera->GetEyePosition() + m_collider->CheckCollisionCollider(m_camera->GetBoundingSphere()));
}
