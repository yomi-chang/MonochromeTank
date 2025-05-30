/**
 * @file   Tracking.h
 * @brief  敵の追跡行動クラス
 */
#pragma once
#include "Interface/IState.h"

class Tracking : public IState
{
public:
	// コンストラクタ
	Tracking();
	// デストラクタ
	~Tracking() override;
	// 初期化処理
	void Initialize(Tank* tank) override;
	// 更新処理
	void Update(float elapsedTime) override;
	// 行動状態遷移をした際に呼び出される関数
	void Enter() override;
	// 追跡対象の戦車の取得
	Tank* GetTargetTank() override { return m_targetTank; }
	// 追跡対象の戦車の設定
	void SetTargetTank(Tank* targetTank) override { m_targetTank = targetTank; }
	// 状態番号の取得
	StateID GetStateID() override { return m_stateID; }

private:
	// 状態番号
	StateID m_stateID;
	// 追跡対象の戦車
	Tank* m_targetTank;
	// 自機の情報
	Tank* m_tank;

private:
	// 追跡対象の戦車に接近している判定及び処理
	void IsTargetTankNear();
};