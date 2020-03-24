#include "Player.h"
#include "offsets.h"
#include "CstrikeDatas.h"
#include "AmxxApi.h"
#include "HamSandwich.h"

#include "amxxmodule.h"
#include <HLTypeConversion.h>

extern HLTypeConversion g_TypeConversion;

void Player::PutInServer(int index)
{
	m_index = index;
	m_pEdict = g_TypeConversion.id_to_edict(index);
	this->ChangeClass("Human");
}

void Player::Disconnect()
{
	m_index = 0;
	m_pEdict = NULL;
}

int Player::GetTeam() const
{
	GET_OFFSET("CBasePlayer", m_iTeam, offsetTeam);
	return get_pdata<int>(m_pEdict, offsetTeam);
}

void Player::ChangeTeam(int team, int model, bool send_teaminfo)
{
	GET_OFFSET("CBasePlayer", m_iTeam, offsetTeam);
	GET_OFFSET("CBasePlayer", m_iModelName, offsetModel);

	set_pdata<int>(m_pEdict, offsetTeam, team);

	if (model > 0)
	{
		set_pdata<int>(m_pEdict, offsetModel, model);
	}

	if (model >= 0)
	{
		this->ResetModel();
	}

	char teaminfo[32];

	switch (team)
	{
	case TEAM_UNASSIGNED: strcpy(teaminfo, "UNASSIGNED"); break;
	case TEAM_T:          strcpy(teaminfo, "TERRORIST");  break;
	case TEAM_CT:         strcpy(teaminfo, "CT");         break;
	case TEAM_SPECTATOR:  strcpy(teaminfo, "SPECTATOR");  break;
	default:              sprintf(teaminfo, "TEAM_%i", team);
	}

	if (send_teaminfo)
	{
		static int msgTeamInfo = -1;
		if (msgTeamInfo == -1)
			msgTeamInfo = GET_USER_MSG_ID(NULL, "TeamInfo", NULL);

		MESSAGE_BEGIN(MSG_ALL, msgTeamInfo);
		WRITE_BYTE(this->GetIndex());
		WRITE_STRING(teaminfo);
		MESSAGE_END();
	}

	MF_SetPlayerTeamInfo(this->GetIndex(), team, team <= TEAM_SPECTATOR ? teaminfo : nullptr);
}

void Player::SetModel(const char* model, bool updateIndex)
{
	CS_SetPlayerModel(m_pEdict, model, updateIndex);
}

void Player::ResetModel()
{
	CS_ResetPlayerModel(m_pEdict);
}

void Player::ResetMaxspeed()
{
	void *pv = g_TypeConversion.id_to_cbase(m_index);
	void* pFunc = GetHamFunction(pv, Ham_CS_Player_ResetMaxSpeed);

#if defined(_WIN32)
	reinterpret_cast<void(__fastcall*)(void*, int)>(pFunc)(pv, 0);
#elif defined(__linux__) || defined(__APPLE__)
	reinterpret_cast<void (*)(void*)>(pFunc)(pv);
#endif
}

PlayerClass* Player::ChangeClass(const char* pszClassName)
{
	BasePlayerClassHelper* pHelper = GetPlayerClassHelperByName(pszClassName);
	if (pHelper == nullptr)
	{
		MF_Log("Player::ChangeClass(): PlayerClass %s is not found\n", pszClassName);
		return nullptr;
	}

	if (m_pPlayerClass != nullptr)
	{
		delete m_pPlayerClass;
		m_pPlayerClass = nullptr;
	}

	m_pPlayerClass = pHelper->Instantiate(this);

	return m_pPlayerClass;
}