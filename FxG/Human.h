#pragma once

#include "PlayerClass.h"

class Human : public PlayerClass
{
public:
	Human(Player* pPlayer);

	virtual bool IsZombie() const override { return false; }
	virtual void SetMaxspeed();
	virtual int OnTakeDamage(WrappedEntity* pInflictor, WrappedEntity* pAttacker, float damage, int damageType) override;
	virtual void OnThink() override;
	virtual void OnKilled(WrappedEntity* pKiller) override;
	virtual void Become() override;

	void MakePoisoning(Player* pAttacker);

private:
	bool m_IsPoisoning;
	float m_LastPoisonTime;
	Player* m_PoisonAttacker;
};