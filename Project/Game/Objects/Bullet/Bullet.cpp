/**
 * @file   Bullet.cpp
 * @brief  連射弾クラス
 */
#include "pch.h"
#include "Game/Objects/Bullet/Bullet.h"
#include "Framework/Resources.h"
#include "Framework/Graphics.h"
#include "Libraries/Microsoft/DebugDraw.h"
#include "Game/Objects/Bullet/BulletTrail.h"

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="bulletState">弾の状態</param>
Bullet::Bullet(IBullet::BulletState bulletState)
	:
	m_graphics{ Graphics::GetInstance() },
	m_position{},
	m_rotation{},
	m_velocity{},
	m_worldMatrix{},
	m_bulletState{ bulletState },
	m_collider{},
	m_bullet{},
	m_trail{},
	m_count{}
{
}

/// <summary>
/// デストラクタ
/// </summary>
Bullet::~Bullet()
{
}

/// <summary>
/// 初期化処理
/// </summary>
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
	m_trail->Initialize(Parameter::GetInstance()->GetBulletMaxTrail());

	// 弾の生存時間の設定
	m_count = Parameter::GetInstance()->GetBulletSurvivalTime();
}

/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime">フレーム間の経過時間</param>
void Bullet::Update(float elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);
	using namespace DirectX::SimpleMath;
	auto parameter = Parameter::GetInstance();
	Vector3 speed = parameter->GetBulletSpeed() * elapsedTime;

	// 使用可能もしくは使用済みの場合
	if (m_bulletState == USED)
	{
		// トレイルの座標の削除
		m_trail->DeletePosArray();
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
	m_count -= elapsedTime;

	// 床より下または生存カウントが0になったら使用済みにする
	if (m_position.y <= 0.0f ||
		m_count <= 0.0f)
	{
		SetBulletState(IBullet::USED);
	}
}

/// <summary>
/// 描画処理
/// </summary>
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
	this->DrawBullet();

	// トレイルの描画
	Vector3 width = Matrix::CreateFromQuaternion(m_rotation).Right() * Parameter::GetInstance()->GetBulletWidth();
	Vector3 right = m_position + width;
	Vector3 left = m_position - width;
	m_trail->SetPosition(right, left);
	m_trail->Render();
}

/// <summary>
/// 弾の描画
/// </summary>
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

/// <summary>
/// 終了処理
/// </summary>
void Bullet::Finalize()
{
}