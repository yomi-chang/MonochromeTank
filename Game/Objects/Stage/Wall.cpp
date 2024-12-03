#include "pch.h"
#include "Game/Objects/Stage/Wall.h"
#include "Framework/Graphics.h"
#include "Libraries/MyLib/FollowCamera.h"
#include "Game/Objects/Tank/PlayerTank.h"
#include "Game/Objects/Tank/EnemyTanks/SimpleTank.h"

using namespace DirectX;

//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
Wall::Wall(
	DirectX::SimpleMath::Vector3 scale,
	DirectX::SimpleMath::Vector3 movePosition
)
	:
	m_camera{},
	m_graphics{ Graphics::GetInstance() },
	m_color{ static_cast<DirectX::SimpleMath::Vector4>(Colors::DimGray) }
{
	using namespace DirectX::SimpleMath;

	auto context = m_graphics->GetDeviceResources()->GetD3DDeviceContext();

	// ジオメトリックプリミティブを生成する
	m_model = GeometricPrimitive::CreateBox(context, scale);

	// ワールド座標の設定
	m_world = Matrix::CreateTranslation(movePosition);

	// ボックスコライダーの作成
	m_collider = std::make_unique<BoxCollider>();
	m_collider->CreateBoundingBox(movePosition, scale);
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
	// プレイヤーとの当たり判定
	m_playerTank->SetPosition(m_collider->CheckCollisionCollider(m_playerTank->GetBoundingBox()));

	// カメラとの当たり判定
	m_camera->SetEyePosition(m_camera->GetEyePosition() + m_collider->CheckCollisionCollider(m_camera->GetBoundingSphere()));

	// 敵戦車との当たり判定
	for (auto& enemyTank : m_enemyTanks)
	{
		enemyTank->SetPosition(m_collider->CheckCollisionCollider(enemyTank->GetBoundingBox()));
	}
}
