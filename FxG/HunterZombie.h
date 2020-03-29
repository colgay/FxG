#pragma once

#include "Zombie.h"

class HunterZombie : public Zombie
{
public:
	HunterZombie(Player* pPlayer);
	//virtual int OnKnifeDeploy(WrappedEntity* pKnife) override;
	virtual void SetProperty() override;
	virtual void OnThink() override;
	virtual void SetMaxspeed() override;
	virtual bool IsTouchedWall();

private:
	float m_LastLeapTime;
};

void PrecacheHunterZombie();