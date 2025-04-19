/*
	@file	EnemyTank.h
	@brief	敵戦車クラス
*/
#pragma once
#include "Interface/IObject.h"
#include "Game/Objects/Tank/TankBase/Tank.h"
#include "Game/UserInterface/EnemyHpGauge.h"
#include "Interface/IState.h"

class Tank;
class EnemyHpGauge;

class SelectAction;
class Patrol;
class Tracking;
class Attack;
class Shot;
class SphereCollider;

class EnemyTank : IObject
{
public:
	// コンストラクタ
	EnemyTank(
		int tankNumber,
		const DirectX::SimpleMath::Vector3& position
	);
	// デストラクタ
	~EnemyTank() override;
	// 初期化処理
	void Initialize()override;
	// 更新処理
	void Update(float elapsedTime) override;
	// 描画処理
	void Render()override;
	// 終了処理
	void Finalize()override;

private:
	// 戦車番号
	int m_tankNumber;
	// 座標
	DirectX::SimpleMath::Vector3 m_position;
	// 回転角
	DirectX::SimpleMath::Quaternion m_angle;
	// 戦車
	std::unique_ptr<Tank> m_tank;
	// HPゲージ
	std::unique_ptr<EnemyHpGauge> m_hpGauge;
	// タイマー
	float m_time;
	// 追跡対象の戦車
	Tank* m_targetTank;

	// AI関係
	std::unique_ptr<SelectAction> m_selectAction;	// 行動選択
	std::unique_ptr<Patrol> m_patrol;				// 巡回行動
	std::unique_ptr<Tracking> m_tracking;			// 追跡行動
	std::unique_ptr<Attack> m_attack;				// 攻撃行動
	std::unique_ptr<Shot> m_shot;					// 射撃行動

	// 現在の状態
	IState* m_currentState;

public:
	// 座標の取得
	const DirectX::SimpleMath::Vector3& GetPosition() { return m_position; }
	// 回転角の取得
	const DirectX::SimpleMath::Quaternion& GetAngle() { return m_angle; }
	// 死亡情報を渡す
	bool GetDead() { return m_tank->GetDead(); }
	// 座標の受け取り
	void SetPosition(const DirectX::SimpleMath::Vector3& position);
	// 戦車情報の取得
	Tank* GetTank() { return m_tank.get(); }
	// Tankの所有権を移動する（新しいメソッド）
	std::unique_ptr<Tank> ReleaseTank() { return std::move(m_tank); }
	// 他戦車情報の受け渡し
	void SetOtherTanks(std::vector<Tank*> tanks);
	// HPの取得
	int GetHP() { return m_tank->GetHp(); }
	// 追跡対象の戦車の設定
	void SetTargetTank(Tank* targetTank) { m_targetTank = targetTank; }
	// 状態の変更
	void ChangeState(IState* newState) { m_currentState = newState; }
	// 状態の取得
	IState* GetCurrentState() { return m_currentState; }
	// メッセージの取得
	void OnMessegeAccepted(Message::MessageID messageID);
};