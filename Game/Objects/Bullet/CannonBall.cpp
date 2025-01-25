#include "pch.h"
#include "Game/Objects/Bullet/CannonBall.h"
#include "Framework/Resources.h"
#include "Framework/Graphics.h"

#include "Libraries/MyLib/DebugLog.h"

// 砲弾速度を定義する
const DirectX::SimpleMath::Vector3 CannonBall::SPEED(0.0f, 0.0f, -0.12f);

// 砲弾にかかる重力を定義する
const DirectX::SimpleMath::Vector3 CannonBall::GRAVITY(0.0f, -0.05f, 0.0f);

// コンストラクタ
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

// デストラクタ
CannonBall::~CannonBall()
{
}

// 初期化する
void CannonBall::Initialize()
{
	using namespace DirectX::SimpleMath;

	// 砲弾モデルの作成
	m_bullet = DirectX::GeometricPrimitive::CreateSphere(m_graphics->GetDeviceResources()->GetD3DDeviceContext(),0.25f);

	// スフィアコライダーの作成
	m_collider = std::make_unique<SphereCollider>();
	m_collider->CreateBoundingSphere(m_position, 0.125f);
}

// 更新する 
void CannonBall::Update(float time)
{
	UNREFERENCED_PARAMETER(time);
	using namespace DirectX::SimpleMath;

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
	if (m_elapsedTime >= 5.0f)
	{
		SetBulletState(IBullet::USED);
	}

	// 速度を計算する（初速度）
	Vector3 initialVelocity = Vector3::Transform(SPEED, Matrix::CreateFromQuaternion(m_rotation));

	// 速度に重力の影響を加えて位置を計算する
	m_velocity = initialVelocity + (GRAVITY * m_elapsedTime);
	m_position += m_velocity;

	// コライダーの座標を更新
	m_collider->Update(m_position);

	// 地面より下に行ったら使用済みにする
	if (m_position.y <= 0)
	{
		SetBulletState(IBullet::USED);
	}
}

// 描画する 
void CannonBall::Render()
{
	using namespace DirectX::SimpleMath;

	// モデル描画のためのワールド行列を計算する
	//Quaternion rotationQuat = Quaternion::CreateFromYawPitchRoll(m_angleRL, m_angleUD, 0.0f);
	m_worldMatrix = Matrix::CreateRotationY(DirectX::XMConvertToRadians(180.0f)) *
		Matrix::CreateFromQuaternion(m_rotation) *
		Matrix::CreateTranslation(m_position);

	// 砲弾が未使用か使用済みの場合は描画しない
	if (m_bulletState == UNUSED || m_bulletState == USED)
		return;

	// 砲弾を描画する
	DrawBullet();
}

// 砲弾を描画する
void CannonBall::DrawBullet()
{
	using namespace DirectX::SimpleMath;

	Matrix view = m_graphics->GetViewMatrix();
	Matrix proj = m_graphics->GetProjectionMatrix();

	// コライダーの描画
	m_collider->Render();

	// 砲弾の描画
	m_bullet->Draw(m_worldMatrix, view, proj,DirectX::Colors::Black);
}

// Bulletオブジェクトの後処理をおこなう 
void CannonBall::Finalize()
{
}