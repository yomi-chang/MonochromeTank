#include "pch.h"
#include "Game/Objects/Bullet/Bullet.h"
#include "Framework/Resources.h"
#include "Framework/Graphics.h"
#include "Libraries/Microsoft/DebugDraw.h"
#include "Game/Objects/Bullet/BulletTrail.h"


// 砲弾速度を定義する
const DirectX::SimpleMath::Vector3 Bullet::SPEED(0.0f, 0.0f, -0.3f);

// コンストラクタ
Bullet::Bullet(IBullet::BulletState bulletState)
	:
	m_graphics{Graphics::GetInstance()},
	m_position{},
	m_rotation{},
	m_velocity{},
	m_gravity(DirectX::SimpleMath::Vector3(0.0f, -9.8f, 0.0f)),
	m_worldMatrix{},
	m_bulletState(bulletState),
	m_collider{},
	m_bullet{},
	m_count{}
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
	m_bullet = DirectX::GeometricPrimitive::CreateSphere(m_graphics->GetDeviceResources()->GetD3DDeviceContext(), 0.1f);

	// スフィアコライダーの作成
	m_collider = std::make_unique<SphereCollider>();
	m_collider->CreateBoundingSphere(m_position, 0.05f);

	// トレイルの作成
	m_trail = std::make_unique<BulletTrail>();
	m_trail->Initialize(2);

	m_count = SURVIVAL_TIME;
}

// 更新する 
void Bullet::Update(float time)
{
	UNREFERENCED_PARAMETER(time);
	using namespace DirectX::SimpleMath;

	// 使用可能もしくは使用済みの場合
	if (m_bulletState == USED)
	{

		return;
	}
	else if(m_bulletState == UNUSED)
	{
		// 生存時間の設定
		m_count = SURVIVAL_TIME;
		return;
	}

	// 速度を計算する
	m_velocity = Vector3::Transform(SPEED,m_rotation);

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

// 描画する 
void Bullet::Render()
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

	// トレイルの描画
	Vector3 head = m_position;
	head.y += 0.2f;
	Vector3 tail = m_position;
	tail.y -= 0.2f;
	m_trail->SetPosition(head, tail);
	m_trail->Render();
}

// 砲弾を描画する
void Bullet::DrawBullet()
{
	using namespace DirectX::SimpleMath;

	Matrix view = m_graphics->GetViewMatrix();
	Matrix proj = m_graphics->GetProjectionMatrix();

	// コライダーの描画
	m_collider->Render();

	// 弾丸の描画
	m_bullet->Draw(m_worldMatrix, view, proj, DirectX::Colors::Silver);
}

// Bulletオブジェクトの後処理をおこなう 
void Bullet::Finalize()
{
}