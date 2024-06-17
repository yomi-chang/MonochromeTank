#include "pch.h"
#include "Game/Objects/Bullet/Bullet.h"
#include "Framework/Resources.h"
#include "Framework/Graphics.h"
#include "Utilities/DebugDraw.h"

// 砲弾モデルスケール
const DirectX::SimpleMath::Vector3 Bullet::BULLET_MODEL_SCALE(0.003f);
// 砲弾境界ボックススケール
const DirectX::SimpleMath::Vector3 Bullet::BULLET_BOUNDINGBOX_SCALE(0.003);
// 境界(最小)
const float Bullet::MIN_BOUNDARY = -300.0f;
// 境界(最大)
const float Bullet::MAX_BOUNDARY = 300.0f;

// 直径 (m)
const float Bullet::DIAMETER = 0.2f;
// 空気抵抗係数(Cd)
const float Bullet::DRAG_COEFFICIENT = 0.2f;
// 空気密度(ρ) kg/m^3
const float Bullet::AIR_DENSITY = 1.22f;
//砲弾直径断面積( m^2)
const float Bullet::CROSS_SECTION = 0.25f * (DirectX::XM_PI * DIAMETER * DIAMETER);
// 空気抵抗係数(k)  砲弾に働く空気抵抗は砲弾の速度の二乗に比例する
const float Bullet::AIR_RESISTANCE = -0.5f * AIR_DENSITY * CROSS_SECTION * DRAG_COEFFICIENT;
// 砲弾速度を定義する
const DirectX::SimpleMath::Vector3 Bullet::SPEED(0.0f, 0.0f, 1.0f);

// コンストラクタ
Bullet::Bullet(IBullet::BulletState bulletState)
	:
	m_model{},
	m_position{},
	m_angleUD(0.0f),
	m_angleRL(0.0f),
	m_velocity{},
	m_acceralation(DirectX::SimpleMath::Vector3(0.0f, -9.8f, 0.0f)),
	m_worldMatrix{},
	m_boundingBox{},
	m_bulletState(bulletState)
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

	// 砲弾モデルをロードする
	m_model = Resources::GetInstance()->GetBulletModel();

	// モデルのエフェクトを更新する
	m_model->UpdateEffects([&](DirectX::IEffect* effect)
		{
			// ベイシックエフェクトを取得する
			auto basicEffect = dynamic_cast<DirectX::BasicEffect*>(effect);
			// ディフューズを設定する
			basicEffect->SetDiffuseColor(DirectX::Colors::Silver);
			// スペキュラカラーを設定する
			basicEffect->SetSpecularColor(DirectX::Colors::Silver);
			// スペキュラパワーを設定する
			basicEffect->SetSpecularPower(10.0f);
			// エミッションカラーを設定する
			basicEffect->SetEmissiveColor(DirectX::XMVECTOR{0.1, 0.1, 0.1, 0.1});
		});

	//	モデルから境界ボックスを取得する
	for (auto it = m_model->meshes.cbegin(); it != m_model->meshes.cend(); it++)
	{
		if (it == m_model->meshes.cbegin())
		{
			// モデルから境界ボックスを設定する
			m_boundingBox = (*it)->boundingBox;
		}
		else
		{
			// モデルから境界ボックスをマージする
			DirectX::BoundingBox::CreateMerged(m_boundingBox, m_boundingBox, (*it)->boundingBox);
		}
	}
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
	
	// 「砲弾」を再生させる領域を設定する
	if (m_position.x < MIN_BOUNDARY || m_position.x > MAX_BOUNDARY ||
		m_position.z < MIN_BOUNDARY || m_position.z > MAX_BOUNDARY)
	{
		// 境界領域を超えたら「砲弾」を使用済みにする
		SetBulletState(IBullet::USED);
	}
}

// 描画する 
void Bullet::Render()
{
	using namespace DirectX::SimpleMath;

	// モデル描画のためのワールド行列を計算する
	Quaternion rotationQuat = Quaternion::CreateFromYawPitchRoll(m_angleRL, m_angleUD, 0.0f);
	m_worldMatrix = Matrix::CreateScale(Bullet::BULLET_MODEL_SCALE) *
		Matrix::CreateTranslation(Vector3(0.0f, 14.2f, 10.0f)) *
		Matrix::CreateFromQuaternion(rotationQuat) *
		Matrix::CreateTranslation(m_position);

	// 砲弾が未使用か使用済みの場合は描画しない
	if (m_bulletState == UNUSED || m_bulletState == USED)
		return;

	// 境界ボックスをトランスフォームする
	m_boundingBox.Transform(m_worldBoundingBox, m_worldMatrix);

	// 砲弾を描画する
	DrawBullet(m_worldMatrix);

#ifdef _DEBUG
	// 境界ボックスを描画する
	DrawBoundingBox(m_worldMatrix);
#endif
}

// 砲弾を描画する
void Bullet::DrawBullet(const DirectX::SimpleMath::Matrix& matrix)
{
	m_model->UpdateEffects([&](DirectX::IEffect* effect)
		{
			// ベイシックエフェクトを取得する
			auto basicEffect = dynamic_cast<DirectX::BasicEffect*>(effect);
			// ディフューズカラーを設定する
			basicEffect->SetDiffuseColor(DirectX::Colors::LightGray);
			// スペキュラカラーを設定する
			basicEffect->SetSpecularColor(DirectX::Colors::LightGray);
			// スペキュラパワーを設定する
			basicEffect->SetSpecularPower(50.0f);
			// エミッションカラーを設定する
			basicEffect->SetEmissiveColor(DirectX::XMVECTOR{ 0.3, 0.3, 0.3, 0.1 });
		});
	// プリミティブ描画を開始する
	m_graphics->DrawPrimitiveBegin(m_graphics->GetViewMatrix(), m_graphics->GetProjectionMatrix());
	// 「砲弾」を描画する
	m_graphics->DrawModel(m_model, matrix);
	// プリミティブ描画を終了する
	m_graphics->DrawPrimitiveEnd();
}

// 境界ボックスを描画する
void Bullet::DrawBoundingBox(const DirectX::SimpleMath::Matrix& matrix)
{
	// プリミティブ描画を開始する
	m_graphics->DrawPrimitiveBegin(m_graphics->GetViewMatrix(), m_graphics->GetProjectionMatrix());
	// 境界ボックスを描画する
	DX::Draw(m_graphics->GetPrimitiveBatch(), m_worldBoundingBox, DirectX::Colors::White);
	// プリミティブ描画を終了する
	m_graphics->DrawPrimitiveEnd();
}

// Bulletオブジェクトの後処理をおこなう 
void Bullet::Finalize()
{
}