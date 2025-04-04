/*
	@file	Patrol.h
	@brief	敵の巡回行動処理クラス
*/
#pragma once
#include "Interface/IEnemyAction.h"

class Patrol : public IEnemyAction
{
private:
	static constexpr float TANK_SPEED = 2.0f;

public:
	Patrol();
	~Patrol() override = default;

	void Initialize(Tank* tank) override;
	void Update(float elapsedTime) override;

private:
	// 巡回地点
	std::vector<DirectX::SimpleMath::Vector3> m_patrolPoints;
	// 巡回地点の数
	int m_patrolPointVelue;
	// 現在の巡回番号
	unsigned int m_currentPoint;
	// 自機の情報
	Tank* m_tank;
	// タイマー
	float m_time;

public:
	// 巡回地点の追加
	void AddPatrolPoint(DirectX::SimpleMath::Vector3 point);

	// 巡回地点の削除
	void ClearPatrolPoints();

	// 巡回地点の登録
	void SetPatrolPoints(std::vector<DirectX::SimpleMath::Vector3> patrolPoints);
};