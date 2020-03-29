#pragma once

#include "PlayerClass.h"
#include "ZombieAttributes.h"

#include <vector>
#include <string>

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
	virtual void SetProperty() override;

private:
	float m_LastPainTime;
	float m_NextIdleSoundTime;
};

void PrecacheZombie();

#define DECLARE_ZOMBIE_CLASS(classname, attr) \
	DECLARE_PLAYER_CLASS(classname); \
	static ZombieClassHelper __s_ZombieClassHelper(#classname, attr);

class ZombieClassHelper
{
public:
	ZombieClassHelper(const char* pszName, const ZombieAttributes& attributes);
};

const std::vector<std::pair<std::string, ZombieAttributes>>& GetZombieClassVector();