/**
 * @file   EnemyTank.cpp
 * @brief  敵戦車クラス
 */
#include"pch.h"
#include"Game/Objects/Tank/EnemyTanks/EnemyTank.h"

#include "Game/Objects/Tank/TankBase/Tank.h"
#include "Game/Objects/Tank/TankBase/TankBody.h"
#include "Game/Objects/Tank/TankBase/TankTurret.h"
#include "Game/Objects/Tank/TankBase/TankCannon.h"

#include "Game/UserInterface/EnemyHpGauge.h"
#include "Game/Collider/SphereCollider.h"

#include "Game/EnemyAi/Patrol.h"
#include "Game/EnemyAi/Tracking.h"
#include "Game/EnemyAi/Attack.h"
#include "Game/EnemyAi/RetreatAttack.h"
#include "Game/EnemyAi/AvoidWall.h"

#include "Message/Messenger.h"

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="tankNumber">戦車番号</param>
/// <param name="position">座標</param>
EnemyTank::EnemyTank(
	int tankNumber,
	const DirectX::SimpleMath::Vector3& position
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
	m_avoidWall{},
	m_currentState{},
	m_prevState{}
{
	// 戦車番号と戦車の登録
	Messenger::GetInstance()->Register(m_tankNumber, this);
}

/// <summary>
/// デストラクタ
/// </summary>
EnemyTank::~EnemyTank()
{
}

/// <summary>
/// 初期化処理
/// </summary>
void EnemyTank::Initialize()
{
	using namespace DirectX::SimpleMath;

	// 戦車の生成
	m_tank = std::make_unique<Tank>(m_tankNumber,m_position, DirectX::XMConvertToRadians(180.0f));
	m_tank->Initialize();
	m_tank->SetMaxHp(Parameter::GetInstance()->GetEnemyHp());
	// 砲身角度の調整
	m_tank->GetCannon()->RotateCannon(0.1f);

	// 敵体力ゲージを生成
	m_hpGauge = std::make_unique<EnemyHpGauge>();

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

	// 後退攻撃行動の生成
	m_retreatAttack = std::make_unique<RetreatAttack>();
	m_retreatAttack->Initialize(m_tank.get());

	// 壁回避行動の生成
	m_avoidWall = std::make_unique<AvoidWall>();
	m_avoidWall->Initialize(m_tank.get());

	// 初期状態を追跡行動に
	m_currentState = m_patrol.get();
	m_prevState = m_currentState;
}

/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime">フレーム間の経過時間</param>
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

	// 行動の更新処理
	m_currentState->Update(elapsedTime);
}

/// <summary>
/// 描画処理
/// </summary>
void EnemyTank::Render()
{
	// 戦車の描画
	m_tank->Render();
}

/// <summary>
/// 終了処理
/// </summary>
void EnemyTank::Finalize()
{
}

/// <summary>
/// 座標情報の設定
/// </summary>
/// <param name="position">座標</param>
void EnemyTank::SetPosition(const DirectX::SimpleMath::Vector3& position)
{
	m_tank->GetBody()->SetCollisionVel(position);
}

/// <summary>
/// 他の戦車情報の設定
/// </summary>
/// <param name="tanks">戦車情報</param>
void EnemyTank::SetOtherTanks(std::vector<Tank*> tanks)
{
	m_tank->SetOtherTanks(tanks);
	m_patrol->SetOtherTanks(tanks);
}

/// <summary>
/// 体力ゲージの描画
/// </summary>
void EnemyTank::DrawHpGauge()
{
	if (m_tank->GetHp() <= 0.0f) { return; }

	m_hpGauge->Render(m_position, m_tank->GetHpRatio());
}

/// <summary>
/// 弾の描画
/// </summary>
void EnemyTank::DrawBullet()
{
	if (m_tank->GetHp() <= 0.0f) { return; }

	m_tank->GetCannon()->DrawBullet();
}

/// <summary>
/// メッセージの取得
/// </summary>
/// <param name="messageID">メッセージID</param>
void EnemyTank::OnMessegeAccepted(Message::MessageID messageID)
{
	// 取得したメッセージに応じた処理
	switch (messageID)
	{
	case Message::NONE:
		// NONEが返されたら前回の行動に遷移する
		this->ChangeState(m_prevState);
		break;
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
	case Message::RETREAT_ATTACK:
		// 追跡対象の戦車の取得
		m_targetTank = m_currentState->GetTargetTank();
		// 後退攻撃行動に遷移
		this->ChangeState(m_retreatAttack.get());
		break;
	case Message::AVOIDWALL:
		// 前回の行動の設定
		m_prevState = m_currentState;
		// 壁回避行動に遷移
		this->ChangeState(m_avoidWall.get());
		break;
	default:
		break;
	}

	// 追跡対象の戦車の設定
	m_currentState->SetTargetTank(m_targetTank);

	// 状態遷移時に呼ばれる関数
	m_currentState->Enter();
}


