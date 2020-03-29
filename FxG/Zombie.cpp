#include "Zombie.h"

#include <algorithm>

#include "Player.h"
#include "meta_api.h"
#include "Utilities.h"
#include "CstrikeDatas.h"
#include "AmxxApi.h"

#include "FastZombie.h"
#include "FatZombie.h"
#include "HunterZombie.h"

DECLARE_PLAYER_CLASS(Zombie);

static const char* s_IdleSounds[] =
{
	"tig_zombie/idle/breathing/breathing01.wav",
	"tig_zombie/idle/breathing/breathing09.wav",
	"tig_zombie/idle/breathing/breathing16.wav",
	"tig_zombie/idle/breathing/breathing18.wav",
};

static const char* s_PainSounds[] =
{
	"tig_zombie/been_shot/been_shot_09.wav",
	"tig_zombie/been_shot/been_shot_13.wav",
	"tig_zombie/been_shot/been_shot_14.wav",
	"tig_zombie/been_shot/male/been_shot_31.wav",
};

static const char* s_KnifeHitSounds[] =
{
	"zombie/claw_strike1.wav",
	"zombie/claw_strike2.wav",
	"zombie/claw_strike3.wav",
};

static const char* s_KnifeMissSounds[] =
{
	"zombie/claw_miss1.wav",
	"zombie/claw_miss2.wav",
};

static const char* s_KnifeHitWallSound = "weapons/cbar_hitbod2.wav";

static const char* s_DieSounds[] =
{
	"tig_zombie/die/death_23.wav",
	"tig_zombie/die/death_35.wav",
	"tig_zombie/die/death_38.wav",
	"tig_zombie/die/male/death_49.wav",
};

static std::vector<std::pair<std::string, ZombieAttributes>>& ZombieClassVector()
{
	static std::vector<std::pair<std::string, ZombieAttributes>> v;
	return v;
}

void PrecacheZombie()
{
	PRECACHE_MODEL("models/player/tig_zombie_headcrab/tig_zombie_headcrab.mdl");
	PRECACHE_MODEL("models/tig_zombie/v_knife_zombie.mdl");
	PRECACHE_MODEL("models/v_knife_r.mdl");

	PRECACHE_SOUND_ARRAY(s_IdleSounds);
	PRECACHE_SOUND_ARRAY(s_PainSounds);
	PRECACHE_SOUND_ARRAY(s_KnifeHitSounds);
	PRECACHE_SOUND_ARRAY(s_KnifeMissSounds);
	PRECACHE_SOUND_ARRAY(s_DieSounds);
	PRECACHE_SOUND(s_KnifeHitWallSound);

	PrecacheFastZombie();
	PrecacheFatZombie();
	PrecacheHunterZombie();
}

Zombie::Zombie(Player* pPlayer) : PlayerClass(pPlayer)
{
	m_LastPainTime = 0;
	m_NextIdleSoundTime = 0;
}

int Zombie::OnKnifeDeploy(WrappedEntity* pKnife)
{
	m_pPlayer->GetEdict()->v.viewmodel = MAKE_STRING("models/tig_zombie/v_knife_zombie.mdl");
	m_pPlayer->GetEdict()->v.weaponmodel = MAKE_STRING("");
	return 1;
}

void Zombie::Become()
{
	int index = m_pPlayer->GetNextZombieClass();
	if (index >= 0 && m_pPlayer->GetClassName() == "Zombie")
	{
		// Become() is included
		m_pPlayer->ChangeClass(ZombieClassVector().at(index).first.c_str());
		m_pPlayer->GetClass()->Become();
		return; // stop here
	}

	m_pPlayer->ChangeTeam(TEAM_T, CS_DONTCHANGE);
	UTIL_StripUserWeapons(m_pPlayer->GetEdict());
	UTIL_GiveItem(m_pPlayer->GetEdict(), "weapon_knife");

	m_NextIdleSoundTime = gpGlobals->time;

	this->SetProperty();
}

void Zombie::OnTouchWeapon(WrappedEntity* pWeapon)
{
	_RETURN_STATE(MRES_SUPERCEDE);
}

void Zombie::OnSpawn()
{
	this->Become();
}

void Zombie::OnThink()
{
	if (gpGlobals->time >= m_NextIdleSoundTime)
	{
		this->OnIdleSound();
		m_NextIdleSoundTime = gpGlobals->time + RANDOM_FLOAT(10.0, 20.0);
	}
}

void Zombie::OnPainSound(int channel)
{
	if (gpGlobals->time >= m_LastPainTime + 0.75f)
	{
		EMIT_SOUND_DYN2(m_pPlayer->GetEdict(), channel, RANDOM_SOUND_ARRAY(s_PainSounds), VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
		m_LastPainTime = gpGlobals->time;
		_RETURN_STATE(MRES_SUPERCEDE);
	}
}

void Zombie::OnKnifeSound(int channel, const char* pszName)
{
	if (pszName[0] == 'h' || pszName[3] == 'b') // hit or stab
	{
		if (pszName[3] == 'w') // hit wall
		{
			EMIT_SOUND_DYN2(m_pPlayer->GetEdict(), channel, s_KnifeHitWallSound, VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
			_RETURN_STATE(MRES_SUPERCEDE);
		}
		else // hit body
		{
			EMIT_SOUND_DYN2(m_pPlayer->GetEdict(), channel, RANDOM_SOUND_ARRAY(s_KnifeHitSounds), VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
			_RETURN_STATE(MRES_SUPERCEDE);
		}
	}
	else if (pszName[0] == 's') // slash (miss)
	{
		EMIT_SOUND_DYN2(m_pPlayer->GetEdict(), channel, RANDOM_SOUND_ARRAY(s_KnifeMissSounds), VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
		_RETURN_STATE(MRES_SUPERCEDE);
	}
}

void Zombie::OnDieSound(int channel)
{
	EMIT_SOUND_DYN2(m_pPlayer->GetEdict(), channel, RANDOM_SOUND_ARRAY(s_DieSounds), VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
	_RETURN_STATE(MRES_SUPERCEDE);
}

void Zombie::OnIdleSound()
{
	EMIT_SOUND_DYN2(m_pPlayer->GetEdict(), CHAN_VOICE, RANDOM_SOUND_ARRAY(s_IdleSounds), VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
}

void Zombie::SetMaxspeed()
{
	m_pPlayer->SetMaxSpeed(m_pPlayer->GetMaxSpeed() * 0.95);
}

void Zombie::SetProperty()
{
	m_pPlayer->SetGravity(0.95f);
	m_pPlayer->SetHealth(1200);
	m_pPlayer->SetMaxHealth(m_pPlayer->GetHealth());
	m_pPlayer->SetArmorValue(0.0f);
	m_pPlayer->SetModel("tig_zombie_headcrab", false);
	m_pPlayer->ResetMaxspeed();
}

const std::vector<std::pair<std::string, ZombieAttributes>>& GetZombieClassVector()
{
	return ZombieClassVector();
}

ZombieClassHelper::ZombieClassHelper(const char* pszName, const ZombieAttributes& attributes)
{
	ZombieClassVector().push_back(std::make_pair(pszName, attributes));
}