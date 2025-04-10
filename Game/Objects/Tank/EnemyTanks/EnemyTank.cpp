/*
	@file	EnemyTank.cpp
	@brief	敵戦車クラス
*/
#include"pch.h"
#include"Game/Objects/Tank/EnemyTanks/EnemyTank.h"

#include "Game/Objects/Tank/TankBase/Tank.h"
#include "Game/Objects/Tank/TankBase/TankBody.h"
#include "Game/Objects/Tank/TankBase/TankTurret.h"
#include "Game/Objects/Tank/TankBase/TankCannon.h"

#include "Game/UserInterface/EnemyHpGauge.h"
#include "Game/Collider/SphereCollider.h"

#include "Game/EnemyAi/SelectAction.h"
#include "Game/EnemyAi/Patrol.h"
#include "Game/EnemyAi/Tracking.h"
#include "Game/EnemyAi/Attack.h"

#include "Message/Messenger.h"

//-------------------------------------------------------------------
// コンストラクタ
//-------------------------------------------------------------------
EnemyTank::EnemyTank(
	int tankNumber,
	DirectX::SimpleMath::Vector3 position
)
	:
	m_tankNumber{ tankNumber },
	m_position{ position },
	m_angle{},
	m_hpGauge{},
	m_time{},
	m_targetTank{},
	m_tank{},
	m_patrol{},
	m_tracking{},
	m_attack{},
	m_currentState{}
{
	// 戦車番号と戦車の登録
	Messenger::GetInstance()->Register(m_tankNumber, this);
}

//-------------------------------------------------------------------
// デストラクタ
//-------------------------------------------------------------------
EnemyTank::~EnemyTank()
{
}

//-------------------------------------------------------------------
// 初期化処理
//-------------------------------------------------------------------
void EnemyTank::Initialize()
{
	using namespace DirectX::SimpleMath;

	// 戦車の生成
	m_tank = std::make_unique<Tank>(m_tankNumber,m_position, DirectX::XMConvertToRadians(180.0f));
	m_tank->Initialize();
	m_tank->SetMaxHp(Parameter::GetInstance()->GetEnemyHp());

	// 敵体力ゲージを生成
	m_hpGauge = std::make_unique<EnemyHpGauge>();

	// 敵AIの生成
	m_selectAction = std::make_unique<SelectAction>();
	m_selectAction->Initialize(m_tank.get());

	// 巡回行動の生成
	auto parameter = Parameter::GetInstance();
	m_patrol = std::make_unique<Patrol>();
	m_patrol->Initialize(m_tank.get());
	m_patrol->SetPatrolPoints(parameter->GetPatrolRoute(m_tankNumber - 1));

	// 追跡行動の生成
	m_tracking = std::make_unique<Tracking>();
	m_tracking->Initialize(m_tank.get());

	// 攻撃行動の生成
	m_attack = std::make_unique<Attack>();
	m_attack->Initialize(m_tank.get());

	// 初期状態を追跡行動に
	m_currentState = m_patrol.get();
}

//-------------------------------------------------------------------
// 更新処理
//-------------------------------------------------------------------
void EnemyTank::Update(float elapsedTime)
{
	using namespace DirectX::SimpleMath;
	auto parameter = Parameter::GetInstance();

	// 戦車の更新
	m_tank->Update(elapsedTime);

	// 破壊されているなら早期リターン
	if (m_tank->GetHp() <= 0.0f) { return; }

	// 座標と回転角の更新
	m_position = m_tank->GetPosition();
	m_angle = m_tank->GetRotation();

	// 攻撃を受けていた場合追跡対象の戦車を切り替える
	if (m_tank->GetTargetTank() != nullptr)
	{
		m_targetTank = m_tank->GetTargetTank();
		Messenger::GetInstance()->Dispatch(m_tank->GetTankNumber(), Message::TRACKING);
		m_tank->SetTargetTank(nullptr);
	}
		
	// 追跡対象が破壊されたら巡回行動に移行
	if (m_targetTank != nullptr)
	{
		if (m_targetTank->GetHp() <= 0 || m_targetTank->GetDead())
		{
			m_tank->SetTargetTank(nullptr);
			m_targetTank = nullptr;
			Messenger::GetInstance()->Dispatch(m_tank->GetTankNumber(), Message::PATROL);
		}
	}

	// 壁回避行動
	if (m_tank->GetAvoidWall())
	{
		// 回避行動
		float speed = elapsedTime * parameter->GetEnemySpeed();
		float angle = DirectX::XMConvertToRadians(0.7f);
		Vector3 velocity = Vector3::Transform(Vector3::Forward * speed, m_tank->GetRotation());
		// 移動させる
		m_tank->GetBody()->Move(velocity);
		// 回転させる
		m_tank->GetBody()->Rotate(Quaternion::CreateFromYawPitchRoll(angle, 0.0f, 0.0f));
		return;
	}

	// 行動の更新処理
	m_currentState->Update(elapsedTime);
}

//-------------------------------------------------------------------
// 描画処理
//-------------------------------------------------------------------
void EnemyTank::Render()
{
	// 戦車の描画
	m_tank->Render();

	// HPゲージ
	if (m_tank->GetHp() <= 0.0f) { return; }
	m_hpGauge->Render(m_position,m_tank->GetHpRatio());
}

//-------------------------------------------------------------------
// 終了処理
//-------------------------------------------------------------------
void EnemyTank::Finalize()
{
}

//-------------------------------------------------------------------
// 座標情報の受け取り
//-------------------------------------------------------------------
void EnemyTank::SetPosition(DirectX::SimpleMath::Vector3 position)
{
	m_tank->GetBody()->SetCollisionVel(position);
}

//-------------------------------------------------------------------
// 他の戦車情報の設定
//-------------------------------------------------------------------
void EnemyTank::SetOtherTanks(std::vector<Tank*> tanks)
{
	m_tanks = tanks;
	m_tank->SetOtherTanks(tanks);
	m_selectAction->SetOtherTanks(tanks);
	m_patrol->SetOtherTanks(tanks);
}

//-------------------------------------------------------------------
// メッセージの取得
//-------------------------------------------------------------------
void EnemyTank::OnMessegeAccepted(Message::MessageID messageID)
{
	// 取得したメッセージに応じた処理
	switch (messageID)
	{
	case Message::PATROL:
		// 追跡対象の戦車の登録解除(念の為行っておく)
		m_targetTank = nullptr;
		// 巡回行動に遷移
		this->ChangeState(m_patrol.get());
		break;
	case Message::TRACKING:
		// 追跡対象の戦車の取得
		if (m_targetTank == nullptr)
		{
			m_targetTank = m_currentState->GetTargetTank();
		}
		// 追跡行動に遷移
		this->ChangeState(m_tracking.get());
		break;
	case Message::ATTACK:
		// 追跡対象の戦車の取得
		m_targetTank = m_currentState->GetTargetTank();
		// 攻撃行動に遷移
		this->ChangeState(m_attack.get());
		break;
	}

	// 追跡対象の戦車の設定
	m_currentState->SetTargetTank(m_targetTank);
}


