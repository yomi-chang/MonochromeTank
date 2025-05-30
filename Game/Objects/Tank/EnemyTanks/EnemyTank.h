/**
 * @file   EnemyTank.h
 * @brief  敵戦車クラス
 */
#pragma once
#include "Interface/IObject.h"
#include "Game/Objects/Tank/TankBase/Tank.h"
#include "Game/UserInterface/EnemyHpGauge.h"
#include "Interface/IState.h"

class Tank;
class EnemyHpGauge;

class Patrol;
class Tracking;
class Attack;
class AvoidWall;
class SphereCollider;

class EnemyTank : public IObject
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
	void Finalize() override;
	// 破壊されているか
	bool GetDead() override { return m_tank->GetDead(); }
	// 戦車の取得
	Tank* GetTank() override { return m_tank.get(); }
	// 他の戦車情報の設定
	void SetOtherTanks(std::vector<Tank*> tanks) override;

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
	std::unique_ptr<Patrol> m_patrol;				// 巡回行動
	std::unique_ptr<Tracking> m_tracking;			// 追跡行動
	std::unique_ptr<Attack> m_attack;				// 攻撃行動
	std::unique_ptr<AvoidWall> m_avoidWall;			// 壁回避行動

	// 現在の状態
	IState* m_currentState;
	// 前回の状態
	IState* m_prevState;


public:
	// 戦車番号の取得
	int GetTankNumber() { return m_tankNumber; }
	// 座標の取得
	const DirectX::SimpleMath::Vector3& GetPosition() { return m_position; }
	// 回転角の取得
	const DirectX::SimpleMath::Quaternion& GetAngle() { return m_angle; }
	// 座標の受け取り
	void SetPosition(const DirectX::SimpleMath::Vector3& position);
	// Tankの所有権の移動
	std::unique_ptr<Tank> ReleaseTank() { return std::move(m_tank); }
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
	// 巡回行動の取得
	Patrol* GetPatrol() { return m_patrol.get(); }
};