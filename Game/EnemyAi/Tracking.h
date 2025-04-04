/*
	@file	Tracking.h
	@brief	敵の追跡行動クラス
*/
#pragma once
#include "Interface/IEnemyAction.h"

class Tracking : public IEnemyAction
{
private:
	// 戦車の速度
	static constexpr float TANK_SPEED = 2.0f;

public:
	Tracking();
	~Tracking() override = default;

	void Initialize(Tank* tank) override;
	void Update(float elapsedTime) override;

private:
	// 追跡対象の戦車
	Tank* m_targetTank;

	// 自機の情報
	Tank* m_tank;

public:
	// 追跡対象の戦車の設定
	void SetTargetTank(Tank* tank) { m_targetTank = tank; }
};