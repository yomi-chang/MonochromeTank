/**
 * @file   ProgressionManager.h
 * @brief  ゲームの進行管理クラス
 */
#include "pch.h"
#include "ProgressionManager.h"
#include "Message/Messenger.h"
#include "Game/Objects/Tank/EnemyTanks/EnemyTank.h"
#include "Game/Objects/Tank/PlayerTank.h"
#include "Game/EnemyAi/Patrol.h"
#include "Interface/IState.h"

/// <summary>
/// コンストラクタ
/// </summary>
ProgressionManager::ProgressionManager()
	:
	m_tanks{},
	m_messenger{},
	m_tankCount{},
	m_flag{},
	m_playerAlive{true}
{
}

/// <summary>
/// デストラクタ
/// </summary>
ProgressionManager::~ProgressionManager()
{
}

/// <summary>
/// 初期化処理
/// </summary>
/// <param name="tanks"></param>
/// <param name="playerTank">敵戦車情報</param>
void ProgressionManager::Initialize(std::vector<EnemyTank*> tanks,PlayerTank* playerTank)
{
	// 全戦車情報の設定
	m_tanks = tanks;

	// プレイヤー戦車
	m_playerTank = playerTank;

	// メッセンジャーの取得
	m_messenger = Messenger::GetInstance();
}

/// <summary>
/// 更新処理
/// </summary>
void ProgressionManager::Update()
{
	// 一騎打ちの場合巡回範囲を狭める
	if (m_tankCount == 2 && !m_flag && !m_playerAlive)
	{
		// 巡回範囲の縮小
		this->NarrowPatrolPoints();
		m_flag = true;
	}

	// プレイヤーとの一騎打ちの場合
	if (m_tankCount == 1 && m_playerAlive)
	{
		this->HandleEndgamePhase();
	}
}

/// <summary>
/// 巡回範囲を狭める
/// </summary>
void ProgressionManager::NarrowPatrolPoints()
{
	using namespace DirectX::SimpleMath;

	for (auto& tank : m_tanks)
	{
		// 倒されている戦車はスキップ
		if (tank->GetDead()) { continue; }

		// 巡回ルートの受け取り
		std::vector<Vector3> patrolPoints = tank->GetPatrol()->GetPatrolPoints();

		// 巡回地点を狭める
		for (auto& point : patrolPoints)
		{
			point.x *= 0.5f;
			point.z *= 0.5f;
		}

		// 新しい巡回ルートを設定する
		tank->GetPatrol()->SetPatrolPoints(patrolPoints);
	}
}

/// <summary>
/// ゲーム終盤
/// </summary>
void ProgressionManager::HandleEndgamePhase()
{
	// 生存している戦車を探す
	EnemyTank* aliveTank = nullptr;
	for (auto& tank : m_tanks)
	{
		if (!tank->GetDead())
		{
			aliveTank = tank;
		}
	}
	
	// 存在しないならか生存している戦車が巡回行動をしていないなら早期リターン
	if (aliveTank == nullptr || aliveTank->GetCurrentState()->GetStateID() != IState::StateID::PATROL) { return; }

	// プレイヤーを追跡対象にする
	aliveTank->SetTargetTank(m_playerTank->GetTank());
	// 追跡行動にする
	Messenger::GetInstance()->Dispatch(aliveTank->GetTankNumber(), Message::TRACKING);
}
