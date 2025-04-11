/*
	@file	Shot.h
	@brief	敵の射撃処理クラス
*/
#pragma once
#include "Interface/IState.h"
class Shot : public IState
{
public:
	// コンストラクタ
	Shot();
	// デストラクタ
	~Shot() override;
	// 初期化処理
	void Initialize(Tank* tank) override;
	// 更新処理
	void Update(float elapsedTime) override;
	// 追跡対象の戦車の取得
	Tank* GetTargetTank() override { return m_targetTank; }
	// 追跡対象の戦車の設定
	void SetTargetTank(Tank* targetTank) override { m_targetTank = targetTank; }

private:
	// 自機の情報
	Tank* m_tank;
	// 追跡対象の戦車
	Tank* m_targetTank;

private:
	// 追跡対象の方向に向く
	void LookTargetTank(float elapsedTime);
};

