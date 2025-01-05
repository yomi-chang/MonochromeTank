#include "pch.h"
#include "Game/EnemyAi/SelectAction.h"
#include "Game/Objects/Tank/TankBase/Tank.h"
#include "Game/UserInterface/EnemyHpGauge.h"

SelectAction::SelectAction()
	:
	m_action{},
	m_tank{},
	m_otherTanks{},
	m_distance{},
	m_hp{}
{
}

void SelectAction::Initialize(Tank* tank, EnemyHpGauge* hp)
{
	// 自機の取得
	m_tank = tank;

	// HPゲージの取得
	m_hpGauge = hp;
}

void SelectAction::Update()
{
	// 最も近い敵を探す
	float distance = std::numeric_limits<float>::max(); // 最大値で初期化
	for (auto& tank : m_otherTanks)
	{
		// 自機では判定しない
		if (tank->GetTankNumber() == m_tank->GetTankNumber()) { continue; }
		
		// 距離の取得
		float d = (tank->GetPosition() - m_tank->GetPosition()).LengthSquared();
		distance = std::min(distance, d); // 小さい値を格納
	}
	mylib::DebugLog("距離", distance);

	// 体力の取得
	float hp = m_hpGauge->GetHp();

	// ステータスの評価
	m_distance = EvaluateStates(distance, DISTANCE_HIGH, DISTANCE_LOW);
	m_hp = EvaluateStates(hp, HP_HIGH, HP_LOW);

	// 行動選択
	m_action = Select();
}

// ステータスを評価する
SelectAction::Evaluation SelectAction::EvaluateStates(float value, float high, float low)
{
	// ステータスが高閾値以上ならHIGHを返す
	if (value >= high) return Evaluation::HIGH;
	// ステータスが低閾値以上ならMIDIUMを返す
	if (value >= low) return Evaluation::MEDIUM;
	// それ以外ならLOWを返す
	return Evaluation::LOW;
}

// 行動選択
SelectAction::Action SelectAction::Select()
{
	if (m_hp == Evaluation::HIGH)
	{
		if (m_distance == Evaluation::HIGH)		{ return Action::PATROL; }
		if (m_distance == Evaluation::MEDIUM)	{ return Action::TRACKING; }
		if (m_distance == Evaluation::LOW)		{ return Action::PATROL; }
	}
	if (m_hp == Evaluation::MEDIUM)
	{
		if (m_distance == Evaluation::HIGH)		{ return Action::PATROL; }
		if (m_distance == Evaluation::MEDIUM)	{ return Action::TRACKING; }
		if (m_distance == Evaluation::LOW)		{ return Action::PATROL; }
	}
	if (m_hp == Evaluation::LOW)
	{
		if (m_distance == Evaluation::HIGH)		{ return Action::PATROL; }
		if (m_distance == Evaluation::MEDIUM)	{ return Action::PATROL; }
		if (m_distance == Evaluation::LOW)		{ return Action::PATROL; }
	}
}
