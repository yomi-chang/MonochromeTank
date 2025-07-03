/**
 * @file   ProgressionManager.h
 * @brief  ゲームの進行管理クラス
 */
#include "pch.h"
#include "ProgressionManager.h"
#include "Message/Messenger.h"
#include "Game/Objects/Tank/EnemyTanks/EnemyTank.h"
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
	m_flag{}
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
/// <param name="tanks">敵戦車情報</param>
void ProgressionManager::Initialize(std::vector<EnemyTank*> tanks)
{
	// 全戦車情報の設定
	m_tanks = tanks;

	// メッセンジャーの取得
	m_messenger = Messenger::GetInstance();
}

/// <summary>
/// 更新処理
/// </summary>
void ProgressionManager::Update()
{
	// 一騎打ちの場合巡回範囲を狭める
	if (m_tankCount == 2 && !m_flag)
	{
		this->NarrowPatrolPoints();
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

	m_flag = true;
}

/// <summary>
/// ゲーム終盤
/// </summary>
void ProgressionManager::HandleEndgamePhase()
{
	// 全ての戦車が巡回行動かどうかを調べる
	bool allTanksPatrolState = true;
	// 生存している戦車
	std::vector<EnemyTank*> aliveTanks;
	for (auto& tank : m_tanks)
	{
		// 撃破済みの戦車はスキップ
		if (tank->GetDead()) { continue; }

		aliveTanks.push_back(tank);
	}

	// 巡回行動でない戦車が存在している場合
	for (auto& aliveTank : aliveTanks)
	{
		if (aliveTank->GetCurrentState()->GetStateID() != IState::StateID::PATROL)
		{
			allTanksPatrolState = false;
			break;
		}
	}
	
	// 全員が巡回行動でないなら早期リターン
	if (!allTanksPatrolState) { return; }

	// 全員が巡回行動の場合の行動
	//for (auto& tank : m_tanks)
	//{
	//	// 撃破されていない敵を探す
	//	if (!tank->GetDead())
	//	{
	//		
	//	}
	//}
}
