#include "Zombie.h"
#include "Player.h"
#include "Utilities.h"
#include "CstrikeDatas.h"

DECLARE_PLAYER_CLASS(Zombie);

void PrecacheZombie()
{
	PRECACHE_MODEL("models/v_knife_r.mdl");
}

Zombie::Zombie(Player* pPlayer) : PlayerClass(pPlayer)
{
}

void Zombie::OnKnifeDeploy(WrappedEntity* pKnife)
{
	m_pPlayer->GetEdict()->v.viewmodel = MAKE_STRING("models/v_knife_r.mdl");
	m_pPlayer->GetEdict()->v.weaponmodel = MAKE_STRING("");
}

void Zombie::Become()
{
	m_pPlayer->SetGravity(0.95f);
	m_pPlayer->SetHealth(1200);
	m_pPlayer->SetMaxHealth(m_pPlayer->GetHealth());
	m_pPlayer->SetArmorValue(0.0f);
	m_pPlayer->ChangeTeam(TEAM_T, CS_DONTCHANGE);
	m_pPlayer->SetModel("vip", false);
	m_pPlayer->ResetMaxspeed();

	UTIL_StripUserWeapons(m_pPlayer->GetEdict());
	UTIL_GiveItem(m_pPlayer->GetEdict(), "weapon_knife");
}

void Zombie::SetMaxspeed()
{
	m_pPlayer->SetMaxSpeed(m_pPlayer->GetMaxSpeed() * 0.95);
}
