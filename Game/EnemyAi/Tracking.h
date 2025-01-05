#pragma once

class Tank;

class Tracking
{
public:
	Tracking();
	~Tracking() = default;

	void Initialize(
		Tank* targetTank,
		Tank* tank
	);
	void Update(float elapsedTime);

private:
	// ’ÇÕ‘ÎÛ‚ÌíÔ
	Tank* m_targetTank;

	// ©‹@‚Ìî•ñ
	Tank* m_tank;

	// íÔ‚Ì‘¬“x
	const float TANK_SPEED = 2.0f;

public:
	// ’ÇÕ‘ÎÛ‚ÌíÔ‚Ìİ’è
	void SetTargetTank(Tank* tank) { m_targetTank = tank; }
};