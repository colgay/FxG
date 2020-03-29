#include "HunterZombie.h"
#include "Player.h"
#include "meta_api.h"

DECLARE_PLAYER_CLASS(HunterZombie);

static const char* s_LeapSounds[] =
{
	"tig_zombie/hunter_leap1.wav",
	"tig_zombie/hunter_leap2.wav",
};

void PrecacheHunterZombie()
{
	PRECACHE_MODEL("models/player/tig_zombie_hunter/tig_zombie_hunter.mdl");
	PRECACHE_SOUND_ARRAY(s_LeapSounds);
}

HunterZombie::HunterZombie(Player* pPlayer) : Zombie(pPlayer), m_LastLeapTime(0)
{
}

void HunterZombie::SetProperty()
{
	m_pPlayer->SetGravity(0.55f);
	m_pPlayer->SetHealth(700);
	m_pPlayer->SetMaxHealth(m_pPlayer->GetHealth());
	m_pPlayer->SetArmorValue(0.0f);
	m_pPlayer->SetModel("tig_zombie_hunter", false);
	m_pPlayer->ResetMaxspeed();
}

void HunterZombie::OnThink()
{
	if ((m_pPlayer->GetButton() & IN_DUCK) && (m_pPlayer->GetButton() & IN_JUMP))
	{
		if (((m_pPlayer->GetFlags() & FL_ONGROUND) && (gpGlobals->time >= m_LastLeapTime + 1.5)) ||
			(this->IsTouchedWall() && (gpGlobals->time >= m_LastLeapTime + 0.5)))
		{
			Vector velocity = m_pPlayer->GetVelocity();
			Vector angle = m_pPlayer->GetViewAngle();

			if (angle.x > -25.0)
				angle.x = -25.0;

			MAKE_VECTORS(angle);

			velocity = velocity + gpGlobals->v_forward * 450.0f;
			m_pPlayer->SetVelocity(velocity);

			EMIT_SOUND_DYN2(m_pPlayer->GetEdict(), CHAN_VOICE, RANDOM_SOUND_ARRAY(s_LeapSounds), VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
		
			m_LastLeapTime = gpGlobals->time;
		}
	}
}

void HunterZombie::SetMaxspeed()
{
	m_pPlayer->SetMaxSpeed(m_pPlayer->GetMaxSpeed() * 0.95);
}

bool HunterZombie::IsTouchedWall()
{
	Vector start = m_pPlayer->GetOrigin();

	Vector angle = m_pPlayer->GetViewAngle();
	MAKE_VECTORS(angle);

	Vector end = start + gpGlobals->v_forward * 8.0f;

	TraceResult ptr;
	TRACE_HULL(start, end, ignore_monsters, (m_pPlayer->GetFlags() & FL_DUCKING) ? head_hull : human_hull, m_pPlayer->GetEdict(), &ptr);

	if (ptr.flFraction < 1.0f)
		return true;

	end = start + gpGlobals->v_forward * -8.0f;
	TRACE_HULL(start, end, ignore_monsters, (m_pPlayer->GetFlags() & FL_DUCKING) ? head_hull : human_hull, m_pPlayer->GetEdict(), &ptr);
	
	if (ptr.flFraction < 1.0f)
		return true;

	return false;
}