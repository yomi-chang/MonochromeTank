/*
	@file	AvoidWall.h
	@brief	敵の壁回避クラス
*/
#pragma once
#include "Interface/IState.h"

class AvoidWall : public IState
{
public:
	// コンストラクタ
	AvoidWall();
	// デストラクタ
	~AvoidWall() override;
	// 初期化処理
	void Initialize(Tank* tank) override;
	// 更新処理
	void Update(float elapsedTime) override;
	// 追跡対象の戦車の取得
	Tank* GetTargetTank() override { return m_targetTank; }
	// 追跡対象の戦車の設定
	void SetTargetTank(Tank* targetTank) override { m_targetTank = targetTank; }

private:
	// 自機
	Tank* m_tank;
	// 追跡対象の戦車
	Tank* m_targetTank;
	// 時間
	float m_time;
	// 前回の状態
	IState* m_prevState;
};