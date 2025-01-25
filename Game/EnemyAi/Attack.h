#pragma once
#include "IEnemyAction.h"

class Attack : public IEnemyAction 
{
public:
	Attack();
	~Attack() override = default;

	void Initialize(Tank* tank) override;
	void Update(float elapsedTime) override;

private:
	enum Action { SHOT,MOVE };
	const float TANK_SPEED = 2.0f;

	const float MOVE_TIME = 5.0f;
	const float SHOT_TIME = 1.0f;

private:
	// ©‹@
	Tank* m_tank;
	Tank* m_targetTank;

	// ŠÔ
	float m_time;

	// Œ»İ‚Ìs“®
	Action m_currentAction;

public:
	// ’ÇÕ‘ÎÛ‚ÌíÔ‚Ìİ’è
	void SetTargetTank(Tank* tank) { m_targetTank = tank; }

private:
	// ’ÇÕ‘ÎÛ‚Ì•ûŒü‚ÉŒü‚­
	void LookTargetTank(float elapsedTime);

	// ˆÚ“®
	void MoveAction(float elapsedTime);

	// UŒ‚
	void ShotAction(float elapsedTime);
};