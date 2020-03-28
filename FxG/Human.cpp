#include "Human.h"
#include "amxxmodule.h"
#include "offsets.h"
#include "HamSandwich.h"
#include "Player.h"
#include "WrappedEntity.h"
#include "Utilities.h"
#include "CstrikeDatas.h"
#include "AmxxApi.h"

DECLARE_PLAYER_CLASS(Human);

void PrecacheHuman()
{
	PRECACHE_SOUND("scientist/cough.wav");
}

Human::Human(Player* pPlayer) : PlayerClass(pPlayer), m_IsPoisoning(false), m_LastPoisonTime(0), m_LastCoughTime(0), m_pPoisonAttacker(nullptr)
{

}

void Human::SetMaxspeed()
{
	//m_pPlayer->SetMaxSpeed(m_pPlayer->GetMaxSpeed() * 1.0);
}

int Human::OnTakeDamage(ModifiableWrappedEntity* inflictor, ModifiableWrappedEntity* attacker, float& damage, int& damageType)
{
	if (inflictor->IsEqualTo(attacker) && attacker->IsPlayer())
	{
		Player* pAttacker = GetPlayerHandler()->GetPlayer(attacker->GetEdict());

		if (pAttacker->HasClass() && pAttacker->GetClass()->IsZombie() && pAttacker->GetCurrentWeapon() == CSW_KNIFE)
		{
			if (m_pPlayer->GetArmorValue() <= 0.0f)
			{
				this->MakePoisoning(pAttacker);
				return 1;
			}
		}
	}

	return 0;
}

void Human::OnThink()
{
	if (m_pPlayer->IsAlive() && m_IsPoisoning)
	{
		this->PoisoningHurt();
	}
}

void Human::OnKilled(ModifiableWrappedEntity* pKiller, int& shouldgibs)
{
	/*
	if (m_IsPoisoning)
	{
		this->CleanPoisoning(); // no need? because deleted jo
		m_pPlayer->ChangeClass("Zombie");
		m_pPlayer->GetClass()->Become();

		_RETURN_HAM(HAM_SUPERCEDE);
	}
	*/
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

void Human::OnSpawn()
{
	this->CleanPoisoning();
	this->Become();
}

void Human::MakePoisoning(Player* pAttacker)
{
	m_IsPoisoning = true;
	m_LastPoisonTime = gpGlobals->time;
	m_LastCoughTime = gpGlobals->time;
	m_pPoisonAttacker = pAttacker;

	//EMIT_SOUND_DYN2(m_pPlayer->GetEdict(), CHAN_BODY, "scientist/cough.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
}

void Human::CleanPoisoning()
{
	m_IsPoisoning = false;
	m_LastPoisonTime = 0;
	m_LastCoughTime = 0;
	m_pPoisonAttacker = nullptr;
}

void Human::PoisoningHurt()
{
	if (gpGlobals->time >= m_LastCoughTime + 3.0)
	{
		UTIL_Damage(m_pPlayer->GetEdict(), 0, 1, DMG_NERVEGAS, Vector());

		EMIT_SOUND_DYN2(m_pPlayer->GetEdict(), CHAN_VOICE, "scientist/cough.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
		m_LastCoughTime = gpGlobals->time;
	}

	if (gpGlobals->time >= m_LastPoisonTime + 1.0)
	{
		if (m_pPlayer->GetHealth() - 1 < 1)
		{
			this->CleanPoisoning();
			m_pPlayer->ChangeClass("Zombie");
			m_pPlayer->GetClass()->Become();
		}
		else
		{
			m_pPlayer->SetHealth(m_pPlayer->GetHealth() - 1);

			// set pain shock after take damage
			GET_OFFSET("CBasePlayer", m_flVelocityModifier, offsetModifier);
			set_pdata<float>(m_pPlayer->GetEdict(), offsetModifier, 0.9);

			m_LastPoisonTime = gpGlobals->time;
		}
	}
}
