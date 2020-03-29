#pragma once

#include "Zombie.h"

class FatZombie : public Zombie
{
public:
	FatZombie(Player* pPlayer);
	//virtual int OnKnifeDeploy(WrappedEntity* pKnife) override;
	virtual void SetProperty() override;
	//virtual void OnThink() override;
	virtual void SetMaxspeed() override;
};

void PrecacheFatZombie();