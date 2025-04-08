/*
	@file	Bullet.cpp
	@brief	連射弾クラス
*/
#include "pch.h"
#include "Game/Objects/Bullet/Bullet.h"
#include "Framework/Resources.h"
#include "Framework/Graphics.h"
#include "Libraries/Microsoft/DebugDraw.h"
#include "Game/Objects/Bullet/BulletTrail.h"

//-------------------------------------------------------------------
// コンストラクタ
//-------------------------------------------------------------------
Bullet::Bullet(IBullet::BulletState bulletState)
	:
	m_graphics{Graphics::GetInstance()},
	m_position{},
	m_rotation{},
	m_velocity{},
	m_worldMatrix{},
	m_bulletState{ bulletState },
	m_collider{},
	m_bullet{},
	m_count{}
{
}

//-------------------------------------------------------------------
// デストラクタ
//-------------------------------------------------------------------
Bullet::~Bullet()
{
}

//-------------------------------------------------------------------
// 初期化処理
//-------------------------------------------------------------------
void Bullet::Initialize()
{
	using namespace DirectX::SimpleMath;

	// 弾モデルの作成
	m_bullet = DirectX::GeometricPrimitive::CreateSphere(m_graphics->GetDeviceResources()->GetD3DDeviceContext(), 0.1f);

	// スフィアコライダーの作成
	m_collider = std::make_unique<SphereCollider>();
	m_collider->CreateBoundingSphere(m_position, Parameter::GetInstance()->GetBulletColliderRadius());

	// トレイルの作成
	m_trail = std::make_unique<BulletTrail>();
	m_trail->Initialize(2);

	// 弾の生存時間の設定
	m_count = Parameter::GetInstance()->GetBulletSurvivalTime();
}

//-------------------------------------------------------------------
// 更新処理
//-------------------------------------------------------------------
void Bullet::Update(float time)
{
	UNREFERENCED_PARAMETER(time);
	using namespace DirectX::SimpleMath;
	auto parameter = Parameter::GetInstance();
	Vector3 speed = parameter->GetBulletSpeed() * time;

	// 使用可能もしくは使用済みの場合
	if (m_bulletState == USED)
	{

		return;
	}
	else if(m_bulletState == UNUSED)
	{
		// 生存時間の設定
		m_count = parameter->GetBulletSurvivalTime();
		return;
	}

	// 速度を計算する
	m_velocity = Vector3::Transform(speed, m_rotation);

	// 位置を計算する
	m_position += m_velocity;
	// コライダーの座標更新
	m_collider->Update(m_position);
	// 生存カウントを減らす
	m_count -= time;

	// 床より下または生存カウントが0になったら使用済みにする
	if (m_position.y <= 0.0f ||
		m_count <= 0.0f)
	{
		SetBulletState(IBullet::USED);
	}
}

//-------------------------------------------------------------------
// 描画処理
//-------------------------------------------------------------------
void Bullet::Render()
{
	using namespace DirectX::SimpleMath;

	// モデル描画のためのワールド行列を計算する
	m_worldMatrix = Matrix::CreateRotationY(DirectX::XMConvertToRadians(180.0f)) *
		Matrix::CreateFromQuaternion(m_rotation) *
		Matrix::CreateTranslation(m_position);

	// 弾が未使用か使用済みの場合は描画しない
	if (m_bulletState == UNUSED || m_bulletState == USED)
		return;

	// 弾を描画する
	DrawBullet();

	// トレイルの描画
	Vector3 head = m_position;
	head.y += 0.2f;
	Vector3 tail = m_position;
	tail.y -= 0.2f;
	m_trail->SetPosition(head, tail);
	//m_trail->Render();
}

//-------------------------------------------------------------------
// 弾の描画
//-------------------------------------------------------------------
void Bullet::DrawBullet()
{
	using namespace DirectX::SimpleMath;

	Matrix view = m_graphics->GetViewMatrix();
	Matrix proj = m_graphics->GetProjectionMatrix();

	// コライダーの描画
	m_collider->Render();

	// 弾の描画
	m_bullet->Draw(m_worldMatrix, view, proj, DirectX::Colors::Silver);
}

//-------------------------------------------------------------------
// 終了処理
//-------------------------------------------------------------------
void Bullet::Finalize()
{
}