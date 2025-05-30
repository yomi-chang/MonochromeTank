/**
 * @file   Attack.h
 * @brief  敵の攻撃処理クラス
 */
#pragma once
#include "Interface/IState.h"

class Attack : public IState
{
private:
	enum Action { SHOT, MOVE };
	static constexpr float MOVE_TIME = 3.0f;
	static constexpr float SHOT_TIME = 3.0f;

public:
	// コンストラクタ
	Attack();
	// デストラクタ
	~Attack() override;
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
	// 自機
	Tank* m_tank;
	// 追跡対象の戦車
	Tank* m_targetTank;
	// 時間
	float m_time;
	// 現在の行動
	Action m_currentAction;
	// 移動時間
	float m_moveTime;
	// 攻撃時間
	float m_shotTime;
	// 射撃を一回以上を行っているかどうか
	bool m_isShot;


private:
	// 追跡対象の方向に向く
	void LookTargetTank(float elapsedTime);
	// 移動処理
	void MoveAction(float elapsedTime);
	// 射撃処理
	void ShotAction(float elapsedTime);
	// 追跡対象の戦車が離れている判定及び処理
	void IsTargetTankFar();
};