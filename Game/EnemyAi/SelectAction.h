/*
	@file	SelectAction.h
	@brief	敵の行動選択クラス
*/
#pragma once

class Tank;

class SelectAction
{
public:
	// 行動の種類
	enum Action
	{
		NONE,
		PATROL,
		TRACKING,
		ATTACK
	};

private:
	// 評価型
	enum Evaluation
	{
		HIGH,
		MEDIUM,
		LOW
	};

	// 距離閾値
	const float DISTANCE_HIGH = 50.0f, DISTANCE_LOW = 10.0f;

	// 体力閾値
	const float HP_HIGH = 70.0f, HP_LOW = 30.0f;

public:
	SelectAction();
	~SelectAction() = default;

	void Initialize(Tank* tank);

	void Update();

private:
	// 行動
	Action m_action;

	// 自機の情報
	Tank* m_tank;
	// 全戦車の情報
	std::vector<Tank*> m_otherTanks;

	// 一番近い戦車との距離
	Evaluation m_distance;
	// 自機の体力
	Evaluation m_hp;

	// 追跡対象の戦車
	Tank* m_targetTank;

	// 追跡するようになる距離
	float m_trackingDistance;

public:
	// 行動方法を返す
	Action GetAction() { return m_action; }

	// 戦車情報の設定
	void SetOtherTanks(std::vector<Tank*> tanks) { m_otherTanks = tanks; }

	// 追跡対象の戦車の取得
	Tank* GetTargetTank() { return m_targetTank; }

	// 追跡対象の戦車の設定
	void SetTargetTank(Tank* targetTank) { m_targetTank = targetTank; }

	// 追跡するようになる距離の設定
	void SetTrackingDistance(float dis) { m_trackingDistance = dis; }

private:
	// ステータスの評価
	Evaluation EvaluateStates(float value, float high, float low);

	// 行動選択
	Action Select();
};