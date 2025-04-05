/*
	@file	Attack.h
	@brief	“G‚ÌUŒ‚ˆ—ƒNƒ‰ƒX
*/
#pragma once
#include "Interface/IState.h"

class Attack : public IState
{
private:
	enum Action { SHOT, MOVE };
	static constexpr float TANK_SPEED = 2.0f;
	static constexpr float MOVE_TIME = 5.0f;
	static constexpr float SHOT_TIME = 3.0f;

public:
	Attack();
	~Attack() override = default;

	void Initialize(Tank* tank) override;
	void Update(float elapsedTime) override;

private:
	// ©‹@
	Tank* m_tank;
	Tank* m_targetTank;

	// ŠÔ
	float m_time;

	// Œ»İ‚Ìs“®
	Action m_currentAction;

	// ˆÚ“®ŠÔ
	float m_moveTime;

	// UŒ‚ŠÔ
	float m_shotTime;

public:
	// ’ÇÕ‘ÎÛ‚ÌíÔ‚Ìİ’è
	void SetTargetTank(Tank* tank) { m_targetTank = tank; }

private:
	// ’ÇÕ‘ÎÛ‚Ì•ûŒü‚ÉŒü‚­
	void LookTargetTank(float elapsedTime);

	// ˆÚ“®
	void MoveAction(float elapsedTime);

	// ËŒ‚
	void ShotAction(float elapsedTime);
};