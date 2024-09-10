#include "pch.h"
#include "Game/Objects/Bullet/Bullet.h"
#include "Framework/Resources.h"
#include "Framework/Graphics.h"
#include "Utilities/DebugDraw.h"


// 砲弾速度を定義する
const DirectX::SimpleMath::Vector3 Bullet::SPEED(0.0f, 0.0f, -0.3f);

// コンストラクタ
Bullet::Bullet(IBullet::BulletState bulletState)
	:
	m_graphics{Graphics::GetInstance()},
	m_position{},
	m_angleUD(0.0f),
	m_angleRL(0.0f),
	m_velocity{},
	m_gravity(DirectX::SimpleMath::Vector3(0.0f, -9.8f, 0.0f)),
	m_worldMatrix{},
	m_bulletState(bulletState),
	m_collider{},
	m_bullet{}
{
}

// デストラクタ
Bullet::~Bullet()
{
}

// 初期化する
void Bullet::Initialize()
{
	using namespace DirectX::SimpleMath;

	// 砲弾モデルの作成
	m_bullet = DirectX::GeometricPrimitive::CreateSphere(m_graphics->GetDeviceResources()->GetD3DDeviceContext(), 0.05f);

	// スフィアコライダーの作成
	m_collider = std::make_unique<SphereCollider>();
	m_collider->CreateBoundingSphere(m_position, 0.025f);
}

// 更新する 
void Bullet::Update(float time)
{
	UNREFERENCED_PARAMETER(time);
	using namespace DirectX::SimpleMath;

	// クォータニオンを生成する
 	Quaternion rotationQuat = Quaternion::CreateFromYawPitchRoll(m_angleRL, m_angleUD, 0.0f);
	
	// 速度を計算する
	m_velocity = Vector3::Transform(SPEED, rotationQuat);
	
	// 位置を計算する
	m_position += m_velocity;

	// コライダーの座標更新
	m_collider->Update(m_position);

	// 床より下なら使用済みに
	if (m_position.y <= 0.0f)
	{
		SetBulletState(IBullet::USED);
	}
}

// 描画する 
void Bullet::Render()
{
	using namespace DirectX::SimpleMath;

	// モデル描画のためのワールド行列を計算する
	Quaternion rotationQuat = Quaternion::CreateFromYawPitchRoll(m_angleRL, m_angleUD, 0.0f);
	m_worldMatrix = Matrix::CreateRotationY(DirectX::XMConvertToRadians(180.0f)) *
		//Matrix::CreateTranslation(Vector3(0.0f, 0.75f, -0.8f)) *
		Matrix::CreateFromQuaternion(rotationQuat) *
		Matrix::CreateTranslation(m_position);

	// 砲弾が未使用か使用済みの場合は描画しない
	if (m_bulletState == UNUSED || m_bulletState == USED)
		return;


	// 砲弾を描画する
	DrawBullet(m_worldMatrix);
}

// 砲弾を描画する
void Bullet::DrawBullet(const DirectX::SimpleMath::Matrix& matrix)
{
	using namespace DirectX::SimpleMath;
	 
	Matrix view = m_graphics->GetViewMatrix();
	Matrix proj = m_graphics->GetProjectionMatrix();

	// コライダーの描画
	m_collider->Render();

	// 弾丸の描画
	m_bullet->Draw(m_worldMatrix, view, proj, DirectX::Colors::Black);
}

// Bulletオブジェクトの後処理をおこなう 
void Bullet::Finalize()
{
}