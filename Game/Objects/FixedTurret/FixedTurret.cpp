#include "pch.h"
#include "Framework/Resources.h"
#include "Framework/Graphics.h"
#include "Game/Objects/FixedTurret/FixedTurret.h"
#include "Game/Objects/Bullet/Bullet.h"
#include "Game/Objects/Tank/TankBase/Tank.h"

#include "Framework/Graphics.h"

FixedTurret::FixedTurret()
	:
	m_position{},
	m_angle{},
	m_tanks{},
	m_targetTank{},
	m_bullets{},
	m_model{},
	m_shotTimer{},
	m_reloadCount{},
	m_isReload{},
	m_box{}
{
}

void FixedTurret::Initialize()
{
	using namespace DirectX::SimpleMath;

	// 固定砲台モデルの取得
	m_model = Resources::GetInstance()->GetFixedTurretModel();

	// 連射弾の生成
	for (int i = 0; i <= 20; i++)
	{
		m_bullets.push_back(std::make_unique<Bullet>(IBullet::UNUSED));
		m_bullets[i]->Initialize();
	}

	m_position = Vector3{ -10.0f,2.0f,-10.0f };

	// デバッグ用モデルの描画
	auto context = Graphics::GetInstance()->GetDeviceResources()->GetD3DDeviceContext();
	m_box = DirectX::GeometricPrimitive::CreateBox(context, DirectX::SimpleMath::Vector3(0.1f, 0.1f, 0.1f));
}

void FixedTurret::Update(float elapsedTime)
{
	using namespace DirectX::SimpleMath;

	// 弾の更新
	for (auto& bullet : m_bullets)
	{
		bullet->Update(elapsedTime);
	}

	// 追跡対象の戦車の選択
	m_targetTank = m_tanks.at(0);
	// 追跡中の戦車の方向に向く処理
	Vector3 delta = m_position - m_targetTank->GetPosition();
	float angle1 = atan2(delta.x, delta.z);
	float angle2 = atan2(delta.y, fabs(delta.x) + fabs(delta.z));
	Quaternion target = Quaternion::CreateFromYawPitchRoll(angle1, -angle2, 0.0f);

	// ゆっくりと回転するようにする
	// 回転速度
	float rotationSpeed = 0.9f;
	float t = rotationSpeed * elapsedTime;
	m_angle = DirectX::XMQuaternionSlerp(m_angle, target, t);

	// 発射処理
	Shot();

	// タイマーを減らす
	if (m_shotTimer > 0.0f)
	{
		m_shotTimer -= elapsedTime;
	}
}

void FixedTurret::Render()
{
	using namespace DirectX::SimpleMath;

	// 弾の描画
	for (auto& bullet : m_bullets)
	{
		bullet->Render();
	}

	Matrix world = Matrix::CreateScale(1.5f);
	world *= Matrix::CreateFromQuaternion(m_angle);
 	world *= Matrix::CreateTranslation(m_position);
	// 固定砲台の描画
	Graphics::GetInstance()->DrawModel(m_model, world);

	// デバッグ用のモデルの描画(消しておく)
	auto view = Graphics::GetInstance()->GetViewMatrix();
	auto proj = Graphics::GetInstance()->GetProjectionMatrix();
	Matrix boxMatrix = Matrix::CreateTranslation(this->GetMuzzlePosition());
	m_box->Draw(boxMatrix, view, proj, DirectX::Colors::Red);
}

// 弾の発射
void FixedTurret::ShootBullet(IBullet* bullet)
{
	// 「砲弾」位置を設定する
	bullet->SetPosition(this->GetMuzzlePosition());
	// コライダー座標の更新
	bullet->SetColliderPosition(this->GetMuzzlePosition());
	// 「砲弾」角度を設定する
	bullet->SetAngle(m_angle);
	// 「砲弾」を発射する
	bullet->SetBulletState(IBullet::FLYING);
}

// 砲身先端座標の取得
DirectX::SimpleMath::Vector3 FixedTurret::GetMuzzlePosition()
{
	using namespace DirectX::SimpleMath;

	// 砲身の先端に対するオフセットベクトル
	DirectX::SimpleMath::Vector3 muzzleOffset = DirectX::SimpleMath::Vector3(0.0f, 0.0f, -1.1f);
	// Quaternion から Matrix を作成して Transform を適用
	Matrix rotationMatrix = Matrix::CreateFromQuaternion(m_angle);
	// 回転をオフセットに適用し、砲身の先端座標を計算
	return Vector3::Transform(muzzleOffset, rotationMatrix) + m_position;
}

// 発射処理
void FixedTurret::Shot()
{
	// 弾の発射処理
	for (auto& bullet : m_bullets)
	{
		// 使用されていない弾は発射できる
		if (bullet->GetBulletState() == IBullet::UNUSED)
		{
			// クールタイム中なら発射しない
			if (m_shotTimer > 0.0f)
				return;

			//「連射弾」を発射する
			ShootBullet(bullet.get());

			// 発射クールタイムの設定
			m_shotTimer = SHOT_INTERVAL;
		}
	}
}

// リロード処理
void FixedTurret::Reload(float elapsedTime)
{


	// カウントダウン
	m_reloadCount -= elapsedTime;

	// リロード完了
	if (m_reloadCount <= RELOAD_TIME)
	{
		for (auto& bullet : m_bullets)
		{
			bullet->SetBulletState(IBullet::UNUSED);
		}
		m_reloadCount = RELOAD_TIME;
	}
}
