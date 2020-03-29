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
	virtual void OnThink() override;
	virtual void OnPainSound(int channel) override;
	virtual void OnKnifeSound(int channel, const char* pszName) override;
	virtual void OnDieSound(int channel) override;
	virtual void OnIdleSound();
	//virtual void OnKilled(ModifiableWrappedEntity* pKiller, int& shouldgib) override;
	virtual void SetMaxspeed() override;

private:
	float m_LastPainTime;
	float m_NextIdleSoundTime;
};

void PrecacheZombie();