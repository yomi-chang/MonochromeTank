#include"pch.h"
#include"Game/Objects/Tank/EnemyTanks/EnemyTank.h"

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
#include "Game/EnemyAi/Attack.h"

#include "Game/Particle/Smoke.h"

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
	m_time{},
	m_collider{},
	m_targetTank{},
	m_tank{},
	m_patrolPoint{},
	m_patrolPoint2{},
	m_patrolPoint3{},
	m_currentPoint{},
	m_patrol{},
	m_tracking{},
	m_attack{}
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
	m_tank->SetMaxHp(100);

	// 敵体力ゲージを生成
	m_hpGauge = std::make_unique<EnemyHpGauge>();
	m_hpGauge->SetMaxHp(10.0f);

	m_patrolPoint.emplace_back(Vector3{ 3.0f, 0.0f,  3.0f });
	m_patrolPoint.emplace_back(Vector3{ -3.0f, 0.0f,  3.0f });
	m_patrolPoint.emplace_back(Vector3{ -3.0f, 0.0f, -3.0f });
	m_patrolPoint.emplace_back(Vector3{ 3.0f, 0.0f, -3.0f });

	// 仮のデータ
	m_patrolPoint2.emplace_back(Vector3{ -3.0f, 0.0f,  -3.0f });
	m_patrolPoint2.emplace_back(Vector3{ -3.0f, 0.0f,  3.0f });
	//m_patrolPoint2.emplace_back(Vector3{ 1.5f, 0.0f, 3.0f });

	m_patrolPoint3.emplace_back(Vector3{ 3.0f, 0.0f,  3.0f });
	m_patrolPoint3.emplace_back(Vector3{ -3.0f, 0.0f,  3.0f });
	m_patrolPoint3.emplace_back(Vector3{ 1.5f, 0.0f, -3.0f });

	// 索敵用コライダーの作成
	m_collider = std::make_unique<SphereCollider>();
	m_collider->CreateBoundingSphere(m_position,3.0f);

	//m_tank->GetCannon()->ChangeBullet();

	// 敵AIの生成
	m_selectAction = std::make_unique<SelectAction>();
	m_selectAction->Initialize(m_tank.get(), m_hpGauge.get());

	m_patrol = std::make_unique<Patrol>();
	switch (m_tankNumber)
	{
		case 1:
			m_patrol->Initialize(m_tank.get());
			m_patrol->SetPatrolPoints(m_patrolPoint);
			break;
		case 2:
			m_patrol->Initialize(m_tank.get());
			m_patrol->SetPatrolPoints(m_patrolPoint2);
			break;
		case 3:
			m_patrol->Initialize(m_tank.get());
			m_patrol->SetPatrolPoints(m_patrolPoint3);
			break;
		default:
			break;
	}
	//m_patrol->Initialize(m_patrolPoint, m_tank.get());

	m_tracking = std::make_unique<Tracking>();
	m_tracking->Initialize(m_tank.get());

	m_attack = std::make_unique<Attack>();
	m_attack->Initialize(m_tank.get());

	// やられたときの演出作成
	m_smokeEffect = std::make_unique<Smoke>();
	m_smokeEffect->Initialize();
}

void EnemyTank::Update(float elapsedTime)
{
	using namespace DirectX::SimpleMath;

	// やられているかの判定
	if (m_tank->GetHp() <= 0)
	{
		m_smokeEffect->Update(elapsedTime);
		return;
	}

	// 戦車の更新
	m_tank->Update(elapsedTime);

	// 座標と回転角の更新
	m_position = m_tank->GetPosition();
	m_angle = m_tank->GetRotation();
	
	// 索敵用コライダーの座標更新
	m_collider->Update(m_position);

	//// ダメージの初期化
	//m_damage = 0.0f;
	//// 衝突判定
	//if(m_tank->DetectCollisionTankAndNomalBullets()) 
	//{
	//	m_damage += 0.5f;
	//}
	//if (m_tank->DetectCollisionTankAndCannonBall()) 
	//{
	//	m_damage += 3.0f;
	//}
	//m_tank->DetectCollisionTankAndOtherTanks();
	//// ダメージ処理
	//m_hpGauge->Damage(m_damage);

	// 巡回行動中なら一定範囲にいる敵を追跡対象にする
	if (m_selectAction->GetAction() == SelectAction::Action::PATROL)
	{
		for (auto& tank : m_tanks)
		{
			// 自機では判定しない
			if (tank->GetTankNumber() == m_tank->GetTankNumber()) { continue; }

			float distance = (tank->GetPosition() - m_tank->GetPosition()).LengthSquared();
			if (distance <= 5.0f)
			{
				m_targetTank = tank;
				m_selectAction->SetTargetTank(tank);
			}
		}
	}
	
	m_selectAction->Update();
	m_tracking->SetTargetTank(m_targetTank);
	switch (m_selectAction->GetAction())
	{
		case  SelectAction::Action::NONE:
			break;
		case SelectAction::Action::PATROL:
			m_patrol->Update(elapsedTime);
			//mylib::DebugLog("パトロール");
			break;
		case SelectAction::Action::TRACKING:
			m_tracking->Update(elapsedTime);
			//mylib::DebugLog("追跡行動");
		case SelectAction::Action::ATTACK:
			m_attack->Update(elapsedTime);
			break;
		default:
			break;
	}
}

void EnemyTank::Render()
{
	// やられているかの判定
	if (m_tank->GetHp() <= 0)
	{
		// 演出表示
		m_smokeEffect->Render(m_position);

		// 演出が終了したら破壊
		if (m_smokeEffect->GetFinishEffect())
		{
			m_isDead = true;
		}

		return;
	}

	// やられていたら更新を行わない
	if (m_isDead) { return; }

	// 戦車の描画
	m_tank->Render();

	// 索敵用コライダーの描画
	m_collider->Render();

	// HPゲージ
	mylib::DebugLog("hp", m_tank->GetHp());
	m_hpGauge->Render(m_position,m_tank->GetHpRatio());
}

void EnemyTank::Finalize()
{
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

void EnemyTank::ChangeTargetTank()
{
	
}


