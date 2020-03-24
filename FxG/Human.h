#pragma once

#include "Player.h"
#include "PlayerHandler.h"

class Human : public PlayerClass
{
public:
	Human(Player* pPlayer);

	virtual bool IsZombie() const override { return false; }
	virtual void SetMaxspeed();

	virtual void Become() override;
};