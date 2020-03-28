#pragma once

#include "PlayerClass.h"

void PrecacheHuman();

class Human : public PlayerClass
{
public:
	Human(Player* pPlayer);

	virtual bool IsZombie() const override { return false; }
	virtual void SetMaxspeed();
	virtual int OnTakeDamage(ModifiableWrappedEntity* inflictor, ModifiableWrappedEntity* attacker, float& damage, int& damageType) override;
	virtual void OnThink() override;
	virtual void OnKilled(ModifiableWrappedEntity* pKiller, int& shouldgibs) override;
	virtual void Become() override;
	virtual void OnSpawn() override;

	void MakePoisoning(Player* pAttacker);
	void CleanPoisoning();
	void PoisoningHurt();

private:
	bool m_IsPoisoning;
	float m_LastPoisonTime;
	float m_LastCoughTime;
	Player* m_pPoisonAttacker;
};