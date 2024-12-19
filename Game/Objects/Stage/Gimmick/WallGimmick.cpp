#include "pch.h"
#include "Game/Objects/Stage/Gimmick/WallGimmick.h"
#include "Game/Collider/BoxCollider.h"
#include "Game/Objects/Tank/TankBase/Tank.h"
#include "Framework/Graphics.h"
#include "Libraries/MyLib/DebugLog.h"

WallGimmick::WallGimmick()
	:
	m_position{},
	m_isHit{}
{
}

void WallGimmick::Initialize()
{
	using namespace DirectX::SimpleMath;

	// デバッグ用モデルの描画
	auto context = Graphics::GetInstance()->GetDeviceResources()->GetD3DDeviceContext();
	m_box = DirectX::GeometricPrimitive::CreateBox(context, Vector3(1.0f, 1.0f, 1.0f));

	m_position = Vector3{ 0.0f,2.0f,0.0f };
	m_collider = std::make_unique<BoxCollider>();
	m_collider->CreateBoundingBox(Vector3{0.0f,2.0f,0.0f}, DirectX::SimpleMath::Vector3(1.0f, 1.0f, 1.0f));
}

void WallGimmick::Update()
{
	// 当たっている情報をリセット
	m_isHit = false;
	// 弾があたったなら壁を下から出す命令
	for (auto& tank : m_tanks)
	{
		// 自機の場合は判定を行わない
		if (tank->GetCannon()->GetCannonBall()->GetBulletState() == IBullet::FLYING &&
			m_collider->CheckTriggerCollider(tank->GetCannon()->GetCannonBall()->GetBoundingSphere()))
		{
			// 弾を消す
			tank->GetCannon()->GetCannonBall()->SetBulletState(IBullet::USED);
			// 当たった情報
			m_isHit = true;
		}
	}
}

void WallGimmick::Render()
{
	using namespace DirectX::SimpleMath;
	Matrix matrix = Matrix::CreateTranslation(m_position);
	auto view = Graphics::GetInstance()->GetViewMatrix();
	auto proj = Graphics::GetInstance()->GetProjectionMatrix();

	m_box->Draw(matrix, view, proj, DirectX::Colors::Red);
	m_collider->Render();
}
