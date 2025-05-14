/*
	@file	ProgressionManager.h
	@brief	ゲームの進行管理クラス
*/
#include "pch.h"
#include "ProgressionManager.h"
#include "Message/Messenger.h"
#include "Game/Objects/Tank/EnemyTanks/EnemyTank.h"
#include "Game/EnemyAi/Patrol.h"

//-------------------------------------------------------------------
// コンストラクタ
//-------------------------------------------------------------------
ProgressionManager::ProgressionManager()
	:
	m_tanks{},
	m_messenger{},
	m_duelFlag{}
{
}

//-------------------------------------------------------------------
// デストラクタ
//-------------------------------------------------------------------
ProgressionManager::~ProgressionManager()
{
}

//-------------------------------------------------------------------
// 初期化処理
//-------------------------------------------------------------------
void ProgressionManager::Initialize(std::vector<EnemyTank*> tanks)
{
	// 全戦車情報の設定
	m_tanks = tanks;

	// メッセンジャーの取得
	m_messenger = Messenger::GetInstance();
}

//-------------------------------------------------------------------
// 更新処理
//-------------------------------------------------------------------
void ProgressionManager::Update()
{
	
	// 一騎打ちの場合巡回範囲を狭める
	if (m_tankCount <= 2 && !m_duelFlag)
	{
		this->NarrowPatrolPoints();
	}

	


}

//-------------------------------------------------------------------
// 巡回範囲を狭める
//-------------------------------------------------------------------
void ProgressionManager::NarrowPatrolPoints()
{
	using namespace DirectX::SimpleMath;

	for (auto& tank : m_tanks)
	{
		// 倒されていたら早期リターン
		if (tank->GetDead()) { return; }

		// 巡回ルートの受け取り
		std::vector<Vector3> patrolPoints = tank->GetPatrol()->GetPatrolPoints();

		// 巡回地点を中央に寄せる
		for (auto& point : patrolPoints)
		{
			point.x *= 0.5f;
			point.z *= 0.5f;
		}

		// 新しい巡回ルートを設定する
		tank->GetPatrol()->SetPatrolPoints(patrolPoints);
	}
}
