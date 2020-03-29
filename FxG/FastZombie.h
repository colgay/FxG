#pragma once

#include "Zombie.h"

class FastZombie : public Zombie
{
public:
	FastZombie(Player* pPlayer);
	//virtual int OnKnifeDeploy(WrappedEntity* pKnife) override;
	virtual void SetProperty() override;
	//virtual void OnThink() override;
	virtual void SetMaxspeed() override;
};

void PrecacheFastZombie();