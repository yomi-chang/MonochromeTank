/*
	@file	Tracking.h
	@brief	敵の追跡行動クラス
*/
#pragma once
#include "Interface/IState.h"

class Tracking : public IState
{
private:
	// 戦車の速度
	static constexpr float TANK_SPEED = 2.0f;

public:
	Tracking();
	~Tracking() override = default;

	// 初期化処理
	void Initialize(Tank* tank) override;
	// 更新処理
	void Update(float elapsedTime) override;
	// 追跡対象の戦車の取得
	Tank* GetTargetTank() override { return m_targetTank; }
	// 追跡対象の戦車の設定
	void SetTargetTank(Tank* targetTank) override { m_targetTank = targetTank; }

private:
	// 追跡対象の戦車
	Tank* m_targetTank;

	// 自機の情報
	Tank* m_tank;

private:
	// 追跡対象の戦車に接近
	void IsTargetTankNear();
};