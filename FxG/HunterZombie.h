#pragma once

#include "Zombie.h"
#include "extdll.h"

class HunterZombie : public Zombie
{
public:
	HunterZombie(Player* pPlayer);
	virtual int OnKnifeDeploy(WrappedEntity* pKnife) override;
	virtual void SetProperty() override;
	virtual void OnThink() override;
	virtual void SetMaxspeed() override;
	bool IsTouchedWall();
	Vector TruncateLength(const Vector& vec, float maxLength);

private:
	float m_LastLeapTime;
};

void PrecacheHunterZombie();