#pragma once

class Tank;
class EnemyHpGauge;

class SelectAction
{
public:
	// 行動の種類
	enum Action
	{
		NONE,
		PATROL,
		TRACKING,
		ESCAPE
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
	const float DISTANCE_HIGH = 60.0f, DISTANCE_LOW = 8.0f;

	// 体力閾値
	const float HP_HIGH = 7.0f, HP_LOW = 3.0f;

public:
	SelectAction();
	~SelectAction() = default;

	void Initialize(Tank* tank, EnemyHpGauge* hp);

	void Update();

private:
	// 行動
	Action m_action;

	// 自機の情報
	Tank* m_tank;
	// 体力ゲージ
	EnemyHpGauge* m_hpGauge;
	// 全戦車の情報
	std::vector<Tank*> m_otherTanks;

	// 一番近い戦車との距離
	Evaluation m_distance;
	// 自機の体力
	Evaluation m_hp;

public:
	// 行動方法を返す
	Action GetAction() { return m_action; }

	// 戦車情報を受け取る
	void SetOtherTanks(std::vector<Tank*> tanks) { m_otherTanks = tanks; }

private:
	// ステータスの評価
	Evaluation EvaluateStates(float value, float high, float low);

	// 行動選択
	Action Select();
};