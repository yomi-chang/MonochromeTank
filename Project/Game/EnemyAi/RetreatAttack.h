/**
 * @file   RetreatAttack.h
 * @brief  敵の後退攻撃処理クラス
 */
#pragma once
#include "Interface/IState.h"

class RetreatAttack : public IState
{
private:
    static constexpr float BACK_MOVE_TIME = 3.0f;
    static constexpr float SHOT_INTERVAL = 0.5f;

public:
    // コンストラクタ
    RetreatAttack();
    // デストラクタ
    ~RetreatAttack() override;
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
    // 移動時間
    float m_moveTime;
    // 攻撃時間
    float m_shotInterval;

private:
    // 追跡対象の方向に向く
    void LookAtTarget(float elapsedTime);
    // 後退移動処理
    void RetreatMove(float elapsedTime);
    // 射撃処理
    void ShotAction(float elapsedTime);
};