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
	// Ž©‹@
	Tank* m_tank;
};