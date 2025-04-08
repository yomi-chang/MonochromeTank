/*
	@file	CannonBall.cpp
	@brief	砲弾クラス
*/
#include "pch.h"
#include "Game/Objects/Bullet/CannonBall.h"
#include "Framework/Resources.h"
#include "Framework/Graphics.h"

#include "Libraries/MyLib/DebugLog.h"
//-------------------------------------------------------------------
// コンストラクタ
//-------------------------------------------------------------------
CannonBall::CannonBall(IBullet::BulletState bulletState)
	:
	m_position{},
	m_rotation{},
	m_velocity{},
	m_worldMatrix{},
	m_bulletState(bulletState),
	m_graphics{Graphics::GetInstance()},
	m_collider{},
	m_bullet{},
	m_elapsedTime{}
{
}

//-------------------------------------------------------------------
// デストラクタ
//-------------------------------------------------------------------
CannonBall::~CannonBall()
{
}

//-------------------------------------------------------------------
// 初期化処理
//-------------------------------------------------------------------
void CannonBall::Initialize()
{
	using namespace DirectX::SimpleMath;

	// 砲弾モデルの作成
	m_bullet = DirectX::GeometricPrimitive::CreateSphere(m_graphics->GetDeviceResources()->GetD3DDeviceContext(),0.25f);

	// スフィアコライダーの作成
	m_collider = std::make_unique<SphereCollider>();
	m_collider->CreateBoundingSphere(m_position, Parameter::GetInstance()->GetCannonBallColliderRadius());
}

//-------------------------------------------------------------------
// 更新処理
//-------------------------------------------------------------------
void CannonBall::Update(float time)
{
	UNREFERENCED_PARAMETER(time);
	using namespace DirectX::SimpleMath;
	auto parameter = Parameter::GetInstance();
	Vector3 speed = parameter->GetCannonBallSpeed() * time;
	Vector3 gravity = parameter->GetCannonBallGravity() * time;

	// 使用可能もしくは使用済みの場合
	if (m_bulletState == USED)
	{
		return;
	}
	else if (m_bulletState == UNUSED)
	{
		// 経過時間のリセット
		m_elapsedTime = 0.0f;
		return;
	}

	// 経過時間を記録
	m_elapsedTime += time;

	// 一定時間経過していたら使用済みにする
	if (m_elapsedTime >= parameter->GetCannonBallSurvivalTime())
	{
		SetBulletState(IBullet::USED);
	}

	// 速度を計算する（初速度）
	Vector3 initialVelocity = Vector3::Transform(speed, Matrix::CreateFromQuaternion(m_rotation));

	// 速度に重力の影響を加えて位置を計算する
	m_velocity = initialVelocity + (gravity * m_elapsedTime);
	m_position += m_velocity;

	// コライダーの座標を更新
	m_collider->Update(m_position);

	// 地面より下に行ったら使用済みにする
	if (m_position.y <= 0)
	{
		SetBulletState(IBullet::USED);
	}
}

//-------------------------------------------------------------------
// 描画処理
//-------------------------------------------------------------------
void CannonBall::Render()
{
	using namespace DirectX::SimpleMath;

	// モデル描画のためのワールド行列を計算する
	m_worldMatrix = Matrix::CreateRotationY(DirectX::XMConvertToRadians(180.0f)) *
		Matrix::CreateFromQuaternion(m_rotation) *
		Matrix::CreateTranslation(m_position);

	// 砲弾が未使用か使用済みの場合は描画しない
	if (m_bulletState == UNUSED || m_bulletState == USED)
		return;

	// 砲弾を描画する
	DrawBullet();
}

//-------------------------------------------------------------------
// 砲弾の描画
//------------------------------------------------------------------
void CannonBall::DrawBullet()
{
	using namespace DirectX::SimpleMath;

	Matrix view = m_graphics->GetViewMatrix();
	Matrix proj = m_graphics->GetProjectionMatrix();

	// コライダーの描画
	m_collider->Render();

	// 砲弾の描画
	m_bullet->Draw(m_worldMatrix, view, proj,DirectX::Colors::LightGray);
}

//-------------------------------------------------------------------
// 終了処理
//-------------------------------------------------------------------
void CannonBall::Finalize()
{
}