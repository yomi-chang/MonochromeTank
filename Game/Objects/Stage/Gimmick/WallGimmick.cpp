#include "pch.h"
#include "Game/Objects/Stage/Gimmick/WallGimmick.h"
#include "Game/Collider/BoxCollider.h"
#include "Game/Objects/Tank/TankBase/Tank.h"
#include "Framework/Graphics.h"

WallGimmick::WallGimmick()
	:
	m_position{}
{
}

void WallGimmick::Initialize()
{
	// デバッグ用モデルの描画
	auto context = Graphics::GetInstance()->GetDeviceResources()->GetD3DDeviceContext();
	m_box = DirectX::GeometricPrimitive::CreateBox(context, DirectX::SimpleMath::Vector3(5.0f, 5.0f, 5.0f));

	m_collider = std::make_unique<BoxCollider>();
	m_collider->CreateBoundingBox(DirectX::SimpleMath::Vector3::Zero, DirectX::SimpleMath::Vector3(5.0f, 5.0f, 5.0f));
}

void WallGimmick::Update()
{
	// 弾があたったなら壁を下から出す命令
	for (auto& tank : m_tanks)
	{
		// 自機の場合は判定を行わない
		if (tank->GetCannon()->GetCannonBall()->GetBulletState() == IBullet::FLYING &&
			m_collider->CheckTriggerCollider(tank->GetCannon()->GetCannonBall()->GetBoundingSphere()))
		{
			tank->GetCannon()->GetCannonBall()->SetBulletState(IBullet::USED);
			// ここに壁の処理
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
}
