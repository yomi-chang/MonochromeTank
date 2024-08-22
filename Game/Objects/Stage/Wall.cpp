#include "pch.h"
#include "Game/Objects/Stage/Wall.h"
#include "Framework/Graphics.h"
#include "Libraries/MyLib/FollowCamera.h"

using namespace DirectX;
// コンストラクタ
Wall::Wall(
	DirectX::SimpleMath::Vector3 scale,
	DirectX::SimpleMath::Vector3 movePosition
)
	:
	//m_camera{},
	m_graphics{ Graphics::GetInstance() },
	m_color{ static_cast<DirectX::SimpleMath::Vector4>(Colors::Silver) }
{
	using namespace DirectX::SimpleMath;

	auto context = m_graphics->GetDeviceResources()->GetD3DDeviceContext();

	// ジオメトリックプリミティブを生成する
	m_model = GeometricPrimitive::CreateBox(context, scale);

	// ワールド座標の設定
	m_world = Matrix::CreateTranslation(movePosition);

	// ボックスコライダーの作成
	m_boxCollider = std::make_unique<BoxCollider>();
	m_boxCollider->CreateBoundingBox(movePosition, scale);
}

// デストラクタ
Wall::~Wall()
{
}


// 描画処理
void Wall::Render()
{
	// プレイヤーとの当たり判定
	m_player->SetTankPosition(m_player->GetTankPosition() + m_boxCollider->CheckCollisionCollider(m_player->GetBoundingSphere()));

	// カメラとの当たり判定
	/*if (m_boxCollider->CheckTriggerCollider(m_camera->GetCollider()))
	{
		m_color.w = 0.9f;
	}
	else
	{
		m_color = DirectX::Colors::SaddleBrown;
	}*/

	auto view = m_graphics->GetViewMatrix();
	auto proj = m_graphics->GetProjectionMatrix();

	// コライダーの描画
	m_boxCollider->Render();

	// 壁の描画
	m_model->Draw(m_world, view, proj, m_color);
}

// 終了処理
void Wall::Finalize()
{
}
