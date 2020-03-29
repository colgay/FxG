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
	PRECACHE_MODEL("models/tig_zombie/v_knife_hunter.mdl");
	PRECACHE_SOUND_ARRAY(s_LeapSounds);
}

HunterZombie::HunterZombie(Player* pPlayer) : Zombie(pPlayer), m_LastLeapTime(0)
{
}

int HunterZombie::OnKnifeDeploy(WrappedEntity* pKnife)
{
	m_pPlayer->GetEdict()->v.viewmodel = MAKE_STRING("models/tig_zombie/v_knife_hunter.mdl");
	m_pPlayer->GetEdict()->v.weaponmodel = MAKE_STRING("");
	return 1;
}

void HunterZombie::SetProperty()
{
	m_pPlayer->SetGravity(0.5f);
	m_pPlayer->SetHealth(700);
	m_pPlayer->SetMaxHealth(m_pPlayer->GetHealth());
	m_pPlayer->SetArmorValue(0.0f);
	m_pPlayer->SetModel("tig_zombie_hunter", false);
	m_pPlayer->ResetMaxspeed();
}

void HunterZombie::OnThink()
{
	if ((m_pPlayer->GetButton() & IN_DUCK) && (m_pPlayer->GetButton() & IN_JUMP) && !(m_pPlayer->GetOldButton() & IN_JUMP))
	{
		bool isOnGround = (m_pPlayer->GetFlags() & FL_ONGROUND);
		bool isTouched = this->IsTouchedWall();

		if ((isOnGround && gpGlobals->time >= m_LastLeapTime + 1.5f) || (!isOnGround && isTouched && gpGlobals->time >= m_LastLeapTime + 0.5f))
		{
			Vector velocity = m_pPlayer->GetVelocity();
			Vector angle = m_pPlayer->GetViewAngle();

			if (isOnGround && angle.x > -25.0f)
				angle.x = -25.0f;

			MAKE_VECTORS(angle);

			if (isOnGround)
			{
				velocity = velocity + gpGlobals->v_forward * 450.0f;
				velocity = this->TruncateLength(velocity, 450.0f); // quick speed fix
			}
			else
			{
				velocity = velocity + gpGlobals->v_forward * 500.0f;
				velocity = this->TruncateLength(velocity, 500.0f); // quick speed fix
			}

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

Vector HunterZombie::TruncateLength(const Vector& vec, float maxLength)
{
	float f;
	f = maxLength / vec.Length2D();
	f = f < 1.0f ? f : 1.0f;

	Vector v = vec * f;
	v.z = vec.z;

	return v;
}