#pragma once
#include "Interface/IEnemyAction.h"

class Tracking : public IEnemyAction
{
public:
	Tracking();
	~Tracking() override = default;

	void Initialize(Tank* tank) override;
	void Update(float elapsedTime) override;

private:
	// íÔ‚Ì‘¬“x
	const float TANK_SPEED = 2.0f;

private:
	// ’ÇÕ‘ÎÛ‚ÌíÔ
	Tank* m_targetTank;

	// ©‹@‚Ìî•ñ
	Tank* m_tank;

public:
	// ’ÇÕ‘ÎÛ‚ÌíÔ‚Ìİ’è
	void SetTargetTank(Tank* tank) { m_targetTank = tank; }
};