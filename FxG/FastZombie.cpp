#include "FastZombie.h"
#include "Player.h"
#include "meta_api.h"

static const ZombieAttributes s_ZombieAttributes{
	"Fast Zombie", // name
	"Move Faster", // desc
	1000.0f, // health
	320.0f, // maxspeed
	1.0f, // gravity
	1.5f, // knockback
};

DECLARE_ZOMBIE_CLASS(FastZombie, s_ZombieAttributes);

FastZombie::FastZombie(Player* pPlayer) : Zombie(pPlayer)
{
}

void PrecacheFastZombie()
{
	PRECACHE_MODEL("models/player/tig_zombie_fast/tig_zombie_fast.mdl");
}

void FastZombie::SetProperty()
{
	m_pPlayer->SetGravity(s_ZombieAttributes.gravity);
	m_pPlayer->SetHealth(s_ZombieAttributes.health);
	m_pPlayer->SetMaxHealth(m_pPlayer->GetHealth());
	m_pPlayer->SetArmorValue(0.0f);
	m_pPlayer->SetModel("tig_zombie_fast", false);

	SERVER_PRINT("FAST::BEFORE SET SPEED\n");
	m_pPlayer->ResetMaxspeed();
}

void FastZombie::SetMaxspeed()
{
	SERVER_PRINT("FAST::SET SPEED\n");
	m_pPlayer->SetMaxSpeed(s_ZombieAttributes.maxspeed);
}