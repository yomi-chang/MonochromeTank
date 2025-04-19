/*
	@file	FixedTurret.cpp
	@brief	固定砲台クラス
*/
#include "pch.h"
#include "Framework/Resources.h"
#include "Framework/Graphics.h"
#include "Game/Objects/FixedTurret/FixedTurret.h"
#include "Game/Objects/Bullet/Bullet.h"
#include "Game/Objects/Tank/TankBase/Tank.h"

#include "Framework/Graphics.h"

//-------------------------------------------------------------------
// コンストラクタ
//-------------------------------------------------------------------
FixedTurret::FixedTurret(const DirectX::SimpleMath::Vector3& position)
	:
	m_position{},
	m_angle{},
	m_tanks{},
	m_targetTank{},
	m_bullets{},
	m_model{},
	m_shotTimer{},
	m_reloadCount{},
	m_isReload{}
{
	// 座標の受け取り
	m_position = position;
}

//-------------------------------------------------------------------
// 初期化処理
//-------------------------------------------------------------------
void FixedTurret::Initialize()
{
	using namespace DirectX::SimpleMath;

	// 固定砲台モデルの取得
	m_model = Resources::GetInstance()->GetFixedTurretModel();

	// 連射弾の生成
	for (int i = 0; i <= BULLET_CAPACITY; i++)
	{
		m_bullets.push_back(std::make_unique<Bullet>(IBullet::UNUSED));
		m_bullets[i]->Initialize();
	}

	// 初期の回転角設定
	m_angle = Quaternion::CreateFromYawPitchRoll(
		DirectX::XMConvertToRadians(180),
		DirectX::XMConvertToRadians(-20), 0);

	// 追跡対象の戦車
	m_targetTank = nullptr;
}

//-------------------------------------------------------------------
// 更新処理
//-------------------------------------------------------------------
void FixedTurret::Update(float elapsedTime)
{
	using namespace DirectX::SimpleMath;

	// 弾の更新
	for (auto& bullet : m_bullets)
	{
		bullet->Update(elapsedTime);
	}

	// 追跡対象の戦車の設定及び変更
	ChangeTargetTank();

	// 追跡対象の戦車がいないなら処理しない
	if (m_targetTank == nullptr) { return; }

	mylib::DebugLog("追跡対象の番号",m_targetTank->GetTankNumber());

	// 射程距離外なら処理しない
	float distance = (m_targetTank->GetPosition() - m_position).LengthSquared();
	if (distance >= MAX_RANGE) { return; }

	// 追跡中の戦車の方向に向く処理
	Vector3 delta = m_position - m_targetTank->GetPosition();
	float angle1 = atan2(delta.x, delta.z);
	float angle2 = atan2(delta.y, fabs(delta.x) + fabs(delta.z));
	Quaternion target = Quaternion::CreateFromYawPitchRoll(angle1, -angle2, 0.0f);

	// ゆっくりと回転するようにする
	// 回転速度
	float t = ROTATION_SPEED * elapsedTime;
	m_angle = DirectX::XMQuaternionSlerp(m_angle, target, t);

	// 発射処理
	Shot();
	//Reload(elapsedTime);
	//StartReload();

	if(m_shotTimer > 0)
		m_shotTimer -= elapsedTime;
}

//-------------------------------------------------------------------
// 描画処理
//-------------------------------------------------------------------
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
}

//-------------------------------------------------------------------
// 弾の発射
//-------------------------------------------------------------------
void FixedTurret::ShootBullet(IBullet* bullet)
{
	// 「砲弾」位置を設定する
	bullet->SetPosition(GetMuzzlePosition());
	// コライダー座標の更新
	bullet->SetColliderPosition(GetMuzzlePosition());
	// 「砲弾」角度を設定する
	bullet->SetRotation(m_angle);
	// 「砲弾」を発射する
	bullet->SetBulletState(IBullet::FLYING);
}

//-------------------------------------------------------------------
// 砲身先端座標の取得
//-------------------------------------------------------------------
DirectX::SimpleMath::Vector3 FixedTurret::GetMuzzlePosition()
{
	using namespace DirectX::SimpleMath;

	// 砲身の先端に対するオフセットベクトル
	Vector3 muzzleOffset = Vector3(0.0f, 0.0f, -1.1f);
	// Quaternion から Matrix を作成して Transform を適用
	Matrix rotationMatrix = Matrix::CreateFromQuaternion(m_angle);
	// 回転をオフセットに適用し、砲身の先端座標を計算
	return Vector3::Transform(muzzleOffset, rotationMatrix) + m_position;
}

//-------------------------------------------------------------------
// 発射処理
//-------------------------------------------------------------------
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

//-------------------------------------------------------------------
// リロード処理
//-------------------------------------------------------------------
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
	}
}

//-------------------------------------------------------------------
// リロード開始
//-------------------------------------------------------------------
void FixedTurret::StartReload()
{
	// リロード開始
	if (m_isReload) { return; }

	for (auto& bullet : m_bullets)
	{
		// 弾が1発でも使用されていたらリロード可能
		if (bullet->GetBulletState() == IBullet::USED)
		{
			m_reloadCount = RELOAD_TIME;
			m_isReload = true;
			return;
		}
	}
}

//-------------------------------------------------------------------
// 追跡対象の戦車を変更及び設定する
//-------------------------------------------------------------------
void FixedTurret::ChangeTargetTank()
{
	// 一番近い戦車を追跡対象にする
	float minDistance = std::numeric_limits<float>::max();
	for (auto& tank : m_tanks)
	{
		// 固定砲台と戦車の距離を調べる
		float distance = (tank->GetPosition() - m_position).LengthSquared();

		// 一定範囲以内でかつ一番近いなら
		if (distance <= MAX_RANGE &&
			minDistance > distance)
		{
			minDistance = distance;
			m_targetTank = tank;
		}
	}
}

