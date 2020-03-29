#include "FatZombie.h"
#include "Player.h"
#include "meta_api.h"

static const ZombieAttributes s_ZombieAttributes{
	"Fat Zombie", // name
	"Slower and More HP", // desc
	3000.0f, // health
	220.0f, // maxspeed
	0.95f, // gravity
	0.75f, // knockback
};

DECLARE_ZOMBIE_CLASS(FatZombie, s_ZombieAttributes);

FatZombie::FatZombie(Player* pPlayer) : Zombie(pPlayer)
{
}

void PrecacheFatZombie()
{
	PRECACHE_MODEL("models/player/tig_zombie_headcrab/tig_zombie_headcrab.mdl");
}

void FatZombie::SetProperty()
{
	m_pPlayer->SetGravity(s_ZombieAttributes.gravity);
	m_pPlayer->SetHealth(s_ZombieAttributes.health);
	m_pPlayer->SetMaxHealth(m_pPlayer->GetHealth());
	m_pPlayer->SetArmorValue(0.0f);
	m_pPlayer->SetModel("tig_zombie_headcrab", false);
	m_pPlayer->ResetMaxspeed();
}

void FatZombie::SetMaxspeed()
{
	m_pPlayer->SetMaxSpeed(s_ZombieAttributes.maxspeed);
}