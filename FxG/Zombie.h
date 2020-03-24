#pragma once

#include "Player.h"
#include "PlayerHandler.h"

class Zombie : public PlayerClass
{
public:
	Zombie(Player* pPlayer);

	virtual bool IsZombie() const override { return true; }
	virtual void OnKnifeDeploy(WrappedEntity *pKnife);
	virtual void Become() override;
	//virtual void OnRespawn() override;
	//virtual void OnKilled(WrappedEntity* pKiller) override;
	virtual void SetMaxspeed() override;
};

void PrecacheZombie();