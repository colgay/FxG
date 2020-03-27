#include "Human.h"
#include "amxxmodule.h"
#include "Player.h"
#include "WrappedEntity.h"
#include "Utilities.h"
#include "CstrikeDatas.h"

DECLARE_PLAYER_CLASS(Human);

Human::Human(Player* pPlayer) : PlayerClass(pPlayer), m_IsPoisoning(false), m_LastPoisonTime(0), m_PoisonAttacker(nullptr)
{

}

void Human::SetMaxspeed()
{
	//m_pPlayer->SetMaxSpeed(m_pPlayer->GetMaxSpeed() * 1.0);
}

int Human::OnTakeDamage(WrappedEntity* inflictor, WrappedEntity* attacker, float damage, int damageType)
{
	if (inflictor == attacker && attacker->IsPlayer())
	{
		Player* pAttacker = GetPlayerHandler()->GetPlayer(attacker->GetEdict());

		if (pAttacker->HasClass() && pAttacker->GetClass()->IsZombie() && pAttacker->GetCurrentWeapon() == CSW_KNIFE)
		{
			if (m_pPlayer->GetArmorValue() <= 0.0)
			{
				this->MakePoisoning(pAttacker);
			}
		}
	}
}

void Human::OnThink()
{
	if (m_pPlayer->IsAlive())
	{

	}
}

void Human::OnKilled(WrappedEntity* pKiller)
{
}

void Human::Become()
{
	m_pPlayer->SetGravity(1.0);
	m_pPlayer->SetHealth(100);
	m_pPlayer->SetMaxHealth(m_pPlayer->GetHealth());
	m_pPlayer->SetArmorValue(0.0f);
	m_pPlayer->ChangeTeam(TEAM_CT, CS_DONTCHANGE);
	m_pPlayer->ResetModel();
	m_pPlayer->ResetMaxspeed();
}

void Human::MakePoisoning(Player* pAttacker)
{
	m_IsPoisoning = true;
	m_LastPoisonTime = gpGlobals->time;
	m_PoisonAttacker = pAttacker;
}
