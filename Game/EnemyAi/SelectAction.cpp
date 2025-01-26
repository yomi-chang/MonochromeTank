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
	m_hp{},
	m_targetTank{}
{
}

void SelectAction::Initialize(Tank* tank, EnemyHpGauge* hp)
{
	// 自機の取得
	m_tank = tank;

	// HPゲージの取得
	m_hpGauge = hp;

	// 最初の行動は巡回行動
	m_action = Action::PATROL;
}

void SelectAction::Update()
{
	// ターゲットがいないときは巡回行動に
	if (m_targetTank == nullptr) 
	{
		// 巡回行動を設定
		mylib::DebugLog("情報なし");
		m_action = Action::PATROL;
		return; 
	}

	// 追跡対象の戦車との距離を調べる
	float distance = (m_targetTank->GetPosition() - m_tank->GetPosition()).LengthSquared();
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
	// ステータスが低閾値以上ならMEDIUMを返す
	if (value >= low) return Evaluation::MEDIUM;
	// それ以外ならLOWを返す
	return Evaluation::LOW;
}

// 行動選択
SelectAction::Action SelectAction::Select()
{
	/*if (m_hp == Evaluation::HIGH)
	{
		if (m_distance == Evaluation::HIGH)		{ return Action::TRACKING; }
		if (m_distance == Evaluation::MEDIUM)	{ return Action::TRACKING; }
		if (m_distance == Evaluation::LOW)		{ return Action::ATTACK;   }
	}
	if (m_hp == Evaluation::MEDIUM)
	{
		if (m_distance == Evaluation::HIGH)		{ return Action::TRACKING; }
		if (m_distance == Evaluation::MEDIUM)	{ return Action::TRACKING; }
		if (m_distance == Evaluation::LOW)		{ return Action::ATTACK;   }
	}
	if (m_hp == Evaluation::LOW)
	{
		if (m_distance == Evaluation::HIGH)		{ return Action::TRACKING; }
		if (m_distance == Evaluation::MEDIUM)	{ return Action::TRACKING; }
		if (m_distance == Evaluation::LOW)		{ return Action::ATTACK;   }
	}*/
	return Action::ATTACK;
}
