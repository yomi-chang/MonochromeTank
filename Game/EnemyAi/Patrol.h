/*
	@file	Patrol.h
	@brief	敵の巡回行動処理クラス
*/
#pragma once
#include "Interface/IState.h"
#include "Game/Objects/Tank/TankBase/Tank.h"

class Patrol : public IState
{
public:
	// コンストラクタ
	Patrol();
	// デストラクタ
	~Patrol() override;
	// 初期化処理
	void Initialize(Tank* tank) override;
	// 更新処理
	void Update(float elapsedTime) override;
	// 追跡対象の戦車の取得
	Tank* GetTargetTank() override { return m_targetTank; }
	// 追跡対象の戦車の設定
	void SetTargetTank(Tank* targetTank) override { m_targetTank = targetTank; }

private:
	// 巡回地点
	std::vector<DirectX::SimpleMath::Vector3> m_patrolPoints;
	// 巡回地点の数
	int m_patrolPointVelue;
	// 現在の巡回番号
	unsigned int m_currentPoint;
	// 自機の情報
	Tank* m_tank;
	// 追跡対象の戦車
	Tank* m_targetTank;
	// 他戦車情報
	std::vector<Tank*> m_otherTanks;
	// タイマー
	float m_time;

public:
	// 他戦車情報の設定
	void SetOtherTanks(std::vector<Tank*> tanks) { m_otherTanks = tanks; }

	// 巡回地点の追加
	void AddPatrolPoint(const DirectX::SimpleMath::Vector3& point);

	// 巡回地点の削除
	void ClearPatrolPoints();

	// 巡回地点の登録
	void SetPatrolPoints(const std::vector<DirectX::SimpleMath::Vector3>& patrolPoints);

	// 巡回地点の受け取り
	const std::vector<DirectX::SimpleMath::Vector3>& GetPatrolPoints() { return m_patrolPoints; }

private:
	// 索敵処理
	void ScoutOtherTank();
};