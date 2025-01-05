#include"pch.h"
#include"Game/Objects/Tank/EnemyTanks/SimpleTank.h"

#include "Game/Objects/Tank/TankBase/Tank.h"
#include "Game/Objects/Tank/TankBase/TankBody.h"
#include "Game/Objects/Tank/TankBase/TankTurret.h"
#include "Game/Objects/Tank/TankBase/TankCannon.h"

#include "Game/UserInterface/EnemyHpGauge.h"
#include "Game/Collider/SphereCollider.h"

#include "Framework/InputManager.h"
#include "Libraries/MyLib/Math.h"

#include "Game/EnemyAi/SelectAction.h"
#include "Game/EnemyAi/Patrol.h"
#include "Game/EnemyAi/Tracking.h"

EnemyTank::EnemyTank(
	int tankNumber,
	DirectX::SimpleMath::Vector3 position
)
	:
	m_tankNumber{ tankNumber },
	m_position{ position },
	m_angle{},
	m_hpGauge{},
	m_damage{},
	m_isDead{},
	m_isTracking{},
	m_time{},
	m_collider{},
	m_targetTank{},
	m_tank{},
	m_patrolPoint{},
	m_currentPoint{},
	m_patrol{},
	m_tracking{}
{
}

EnemyTank::~EnemyTank()
{
}

void EnemyTank::Initialize()
{
	using namespace DirectX::SimpleMath;

	// 戦車の生成
	m_tank = std::make_unique<Tank>(m_tankNumber,m_position, DirectX::XMConvertToRadians(180.0f));
	m_tank->Initialize();

	// 敵体力ゲージを生成
	m_hpGauge = std::make_unique<EnemyHpGauge>();
	m_hpGauge->SetMaxHp(10.0f);

	m_patrolPoint.emplace_back(Vector3{ 3.0f, 0.0f,  3.0f });
	m_patrolPoint.emplace_back(Vector3{ -3.0f, 0.0f,  3.0f });
	m_patrolPoint.emplace_back(Vector3{ -3.0f, 0.0f, -3.0f });
	m_patrolPoint.emplace_back(Vector3{ 3.0f, 0.0f, -3.0f });

	// デバッグ用モデルの描画
	auto context = Graphics::GetInstance()->GetDeviceResources()->GetD3DDeviceContext();
	m_box = DirectX::GeometricPrimitive::CreateBox(context, DirectX::SimpleMath::Vector3(0.3f, 0.3f, 0.3f));

	// 索敵用コライダーの作成
	m_collider = std::make_unique<SphereCollider>();
	m_collider->CreateBoundingSphere(m_position,3.0f);

	//m_tank->GetCannon()->ChangeBullet();

	// 敵AIの生成
	m_selectAction = std::make_unique<SelectAction>();
	m_selectAction->Initialize(m_tank.get(), m_hpGauge.get());

	m_patrol = std::make_unique<Patrol>();
	m_patrol->Initialize(m_patrolPoint, m_tank.get());

	m_tracking = std::make_unique<Tracking>();
	m_tracking->Initialize(m_targetTank, m_tank.get());
}

void EnemyTank::Update(float elapsedTime)
{
	using namespace DirectX::SimpleMath;


	// 戦車の更新
	m_tank->Update(elapsedTime);

	// 座標と回転角の更新
	m_position = m_tank->GetPosition();
	m_angle = m_tank->GetRotation();
	
	// 索敵用コライダーの座標更新
	m_collider->Update(m_position);

	// ダメージの初期化
	m_damage = 0.0f;
	// 衝突判定
	if(m_tank->DetectCollisionTankAndNomalBullets()) 
	{
		m_damage += 0.5f;
		m_isTracking = true;
	}
	if (m_tank->DetectCollisionTankAndCannonBall()) 
	{
		m_damage += 3.0f;
		m_isTracking = true;
	}
	m_tank->DetectCollisionTankAndOtherTanks();
	// ダメージ処理
	m_hpGauge->Damage(m_damage);

	// 追跡中の戦車
	m_targetTank = m_tanks.at(0);

	// 追跡行動
	m_tracking->SetTargetTank(m_targetTank);
	
	m_selectAction->Update();
	switch (m_selectAction->GetAction())
	{
		case SelectAction::Action::PATROL:
			m_patrol->Update(elapsedTime);
			mylib::DebugLog("パトロール");
			break;
		case SelectAction::Action::TRACKING:
			m_tracking->Update(elapsedTime);
			mylib::DebugLog("追跡行動");
			break;
		default:
			break;
	}

	//// 追跡中かどうか
	//if (m_isTracking)
	//{
	//	// 追跡中の敵の方向を向く
	//	// 敵の方向ベクトルの計算
	//	Vector3 delta = m_position - m_targetTank->GetPosition();
	//	float angleRadians = atan2(delta.x, delta.z);

	//	// 車体の回転を考慮して目標の角度を計算
	//	float targetAngle = angleRadians - m_tank->GetRotation().ToEuler().y;

	//	// 砲塔回転の制限
	//	targetAngle = mylib::Clamp(targetAngle, DirectX::XMConvertToRadians(-45.0f), DirectX::XMConvertToRadians(45.0f));

	//	// 現在の砲塔の回転角度
	//	float currentAngle = m_tank->GetTurret()->GetTurretRotation().ToEuler().y;

	//	// 目標角度と現在の角度との差を求め、Lerp補間で回転
	//	float angleDifference = targetAngle - currentAngle;

	//	// ゆっくり回転するための速度制御
	//	float rotationSpeed = 0.9f;
	//	float t = rotationSpeed * elapsedTime;

	//	// 補間後の回転角度
	//	float newAngle = currentAngle + angleDifference * t;

	//	// 砲塔の回転
	//	m_tank->GetTurret()->RotateTurret(newAngle);

	//	// 射撃処理
	//	m_tank->GetCannon()->StartReload();
	//	m_tank->GetCannon()->Shoot();
	//}
	//else
	//{
	//	// 探している挙動
	//	m_time += elapsedTime;
	//	m_tank->GetTurret()->RotateTurret(sinf(m_time) / 2.0f);
	//}
	
	// 巡回行動
	//Patrol(elapsedTime);
}

void EnemyTank::Render()
{
	// 戦車の描画
	m_tank->Render();

	// 索敵用コライダーの描画
	m_collider->Render();

	// HPゲージ
	m_hpGauge->Render(m_position);

	/*auto view = Graphics::GetInstance()->GetViewMatrix();
	auto proj = Graphics::GetInstance()->GetProjectionMatrix();
	DirectX::SimpleMath::Matrix boxMatrix;
	switch (m_currentPoint)
	{
		case 0:
			boxMatrix = DirectX::SimpleMath::Matrix::CreateTranslation(3.0f, 0.5f, 3.0f);
			break;
		case 1:
			boxMatrix = DirectX::SimpleMath::Matrix::CreateTranslation(-3.0f, 0.5f, 3.0f);
			break;
		case 2:
			boxMatrix = DirectX::SimpleMath::Matrix::CreateTranslation(-3.0f, 0.5f, -3.0f);
			break;
		case 3:
			boxMatrix = DirectX::SimpleMath::Matrix::CreateTranslation(3.0f, 0.5f, -3.0f);
			break;
		default:
			break;
	}
	
	m_box->Draw(boxMatrix, view, proj, DirectX::Colors::Red);*/
}

void EnemyTank::Finalize()
{
}

// 死亡情報を渡す
bool EnemyTank::GetDead()
{
	// 体力が0なら死亡判定
	if (m_hpGauge->GetHp() <= 0.0f)
	{
		return true;
	}
	return false;
}

// 座標情報の受け取り
void EnemyTank::SetPosition(DirectX::SimpleMath::Vector3 position)
{
	m_tank->GetBody()->SetCollisionVel(position);
}

void EnemyTank::SetOtherTanks(std::vector<Tank*> tanks)
{
	m_tanks = tanks;
	m_tank->SetOtherTanks(tanks);
	m_selectAction->SetOtherTanks(tanks);
}

//void EnemyTank::Patrol(float elapsedTime)
//{
//	using namespace DirectX::SimpleMath;
//
//	// 進行方向ベクトル
//	Vector3 heading = Vector3::Transform(Vector3::Forward * TANK_SPEED * elapsedTime, m_tank->GetRotation());
//
//	// ゴールへ向かうベクトル
//	Vector3 toGoal = m_patrolPoint[m_currentPoint] - m_position;
//
//	// 自機からターゲットへ向かうベクトル
//	Vector3 toTarget = m_targetTank->GetPosition() - m_position;
//
//
//	// 視界に入るまでの最短距離
//	float distance = 8.0f;
//	// 追跡距離
//	float maxRange = 60.0f;
//	// 追跡対象の戦車と自機の距離の平方
//	float distSqTargetTankToTank = (m_targetTank->GetPosition() - m_position).LengthSquared();
//
//
//	// 一定範囲内に入ったら追跡開始
//	if (distSqTargetTankToTank < distance)
//	{
//		// 追跡開始
//		m_isTracking = true;
//	}
//
//	// 一定範囲外なら追跡終了
//	if (m_isTracking && distSqTargetTankToTank >= maxRange)
//	{
//		// 追跡終了
//		m_isTracking = false;
//	}
//
//	// 追跡中でないなら巡回ルートを移動
//	if (!m_isTracking)
//	{
//		toTarget = toGoal;
//	}
//
//	// 移動処理
//	m_tank->GetBody()->Move(heading);
//	//mylib::DebugLog("距離", distSqTargetTankToTank);
//
//	/*
//		自機をターゲットの方向へ徐々に回転する
//	*/
//	// 「自機の進行方向ベクトル」と「ターゲットの方向」からcosθを計算する
//	float cosTheta = heading.Dot(toTarget) / (toTarget.Length() * heading.Length());
//
//	// acosの引数で指定できる範囲は「-1～1」なので、値を補正する
//	cosTheta = std::max(-1.0f, std::min(cosTheta, 1.0f));
//
//	// cosθからθを計算する
//	// acosの結果は「0～π」
//	float theta = std::acos(cosTheta);
//
//	//１フレームでの回転角を制限値以内に補正する
//	theta = std::min(10.0f, theta);
//
//	// 右側に行きたい場合は角度の符号を付け替える
//	// ZX平面上にあるベクトルの向きはYのプラスマイナスで判断する
//	if (heading.Cross(toTarget).y < 0.0f)
//	{
//		theta *= (-1.0f);
//	}
//
//	// 角度を更新する
//	m_tank->GetBody()->Rotate(Quaternion::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(theta), 0.0f, 0.0f));
//		
//
//	// ゴールに達したら、ゴール情報を更新する
//	if (m_isTracking == false && toTarget.Length() < 1.0f)
//	{
//		m_currentPoint++;
//		m_currentPoint %= 4;
//	}
//}
