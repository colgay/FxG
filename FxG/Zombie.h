#pragma once

#include "PlayerClass.h"

class Zombie : public PlayerClass
{
public:
	Zombie(Player* pPlayer);

	virtual bool IsZombie() const override { return true; }
	virtual int OnKnifeDeploy(WrappedEntity *pKnife);
	virtual void Become() override;
	virtual void OnTouchWeapon(WrappedEntity *pWeapon);
	virtual void OnSpawn() override;
	//virtual void OnKilled(ModifiableWrappedEntity* pKiller, int& shouldgib) override;
	virtual void SetMaxspeed() override;
};

void PrecacheZombie();