#include "amxxmodule.h"

//#include <map>
//#include <string>

#include "Hacks.h"
#include "offsets.h"
#include "hook.h"
#include "HamSandwich.h"
#include "CEvent.h"
#include "CLogEvent.h"
#include "Timer.h"
#include "Utilities.h"
#include "AmxxApi.h"
#include "Player.h"
#include "PlayerHandler.h"
#include "WrappedEntity.h"
#include "Human.h"
#include "Zombie.h"
#include "GameRules.h"

#include <amtl/am-string.h>
#include <amtl/am-vector.h>

OffsetManager Offsets;

extern ke::Vector<Hook*> g_Hooks[HAM_LAST_ENTRY_DONT_USE_ME_LOL];

//std::map<std::string, BasePlayerClassHelper*> g_PlayerClassHelpers;
//std::map<std::string, BaseGameModeHelper*> g_GameModeHelpers;

//std::vector<std::pair<std::string, BasePlayerClassHelper*>> g_PlayerClassVector;
//std::vector<std::pair<std::string, BaseGameModeHelper*>> g_GameModeVector;

int ReadConfig(void);
void CloseConfigFiles(void);
int OnPlayerTakeDamage(Hook* hook, void* pthis, entvars_t* inflictor, entvars_t* attacker, float damage, int damagebits);
void OnWeaponTouch(Hook* hook, void* pthis, void* pother);
int OnKnifeDeploy(Hook* hook, void* pthis);
void OnPlayerSpawn(Hook* hook, void* pthis);
void OnPlayerKilled(Hook* , void* , entvars_t* , int );
void OnPlayerResetMaxspeed(Hook* hook, void* pthis);
void OnCheckWinConditions(Hook* hook, void* pthis);
void OnGameRulesThink(Hook* hook, void* pthis);
void OnRoundStart(void);
void OnNewRound(int);

void OnAmxxAttach()
{
	if (ReadConfig())
	{
		if (!Offsets.IsValid())
		{
			MF_Log("Error: Offsets are invalid!");
		}
	}
	else
	{
		MF_Log("Error: Cannot read config file!");
	}

	RegisterNatives();
}


void OnAmxxDetach()
{
	g_logevents.clearLogEvents();
	g_events.clearEvents();
	g_TimerManager.Clean();

	CloseConfigFiles();
}

void ServerActivate_Post(edict_t* pEdictList, int edictCount, int clientMax)
{
	RegisterEvent("HLTV", &OnNewRound, "a", 2, "1=0", "2=0");
	RegisterLogEvent(&OnRoundStart, 2, 1, "1=Round_Start");

	RegisterHam(Ham_TakeDamage, "player", reinterpret_cast<void*>(OnPlayerTakeDamage));
	RegisterHam(Ham_Touch, "weaponbox", reinterpret_cast<void*>(OnWeaponTouch));
	RegisterHam(Ham_Item_Deploy, "weapon_knife", reinterpret_cast<void*>(OnKnifeDeploy));
	RegisterHam(Ham_Spawn, "player", reinterpret_cast<void*>(OnPlayerSpawn));
	RegisterHam(Ham_Killed, "player", reinterpret_cast<void*>(OnPlayerKilled));
	RegisterHam(Ham_CS_Player_ResetMaxSpeed, "player", reinterpret_cast<void*>(OnPlayerResetMaxspeed));
	
	RegisterGameRules(GR_CheckWinConditions, reinterpret_cast<void*>(OnCheckWinConditions));
	RegisterGameRules(GR_GameRules_Think, reinterpret_cast<void*>(OnGameRulesThink));

	ToggleHooks(true);

	g_GameRules.OnServerActivated();

	RETURN_META(MRES_IGNORED);
}

void ServerDeactivate_Post()
{
	g_events.clearEvents();
	g_logevents.clearLogEvents();
	g_TimerManager.Clean();

	ToggleHooks(false);
}

void OnPluginsLoaded()
{
	PrecacheHuman();
	PrecacheZombie();
	g_GameRules.OnPrecache();

	g_TypeConversion.init();

	RegisterForwards();
}

void OnPluginsUnloaded()
{
	for (size_t i = 0; i < HAM_LAST_ENTRY_DONT_USE_ME_LOL; i++)
	{
		for (size_t j = 0; j < g_Hooks[i].length(); ++j)
		{
			delete g_Hooks[i].at(j);
		}

		g_Hooks[i].clear();
	}
}

void ClientDisconnect(edict_t* pEnt)
{
	GetPlayerHandler()->Disconnect(pEnt);
	RETURN_META(MRES_IGNORED);
}

void ClientKill(edict_t* pEnt)
{
	RETURN_META(MRES_IGNORED);
}

void ClientUserInfoChanged_Post(edict_t* pEntity, char* infobuffer)
{
	int index = g_TypeConversion.edict_to_id(pEntity);

	if (MF_IsPlayerBot(index) && GetPlayerHandler()->HasPlayer(index) && GetPlayerHandler()->GetPlayer(index)->GetEdict() == nullptr)
	{
		// Support for bot
		ClientPutInServer(pEntity);
	}

	RETURN_META(MRES_IGNORED);
}

void ClientPutInServer(edict_t* pEnt)
{
	MF_Log("!!!!!!!!!!!!!!! PLAYER ClientPutInServer !!!!!!!!!!!!!!!!\n");
	GetPlayerHandler()->PutInServer(pEnt);
	RETURN_META(MRES_IGNORED);
}

void ClientCommand(edict_t* pEnt)
{
	const char *pszCmd = CMD_ARGV(0);

	if (strcmp(pszCmd, "testzombie") == 0)
	{
		// how to become a zombie?
		Player* pPlayer = GetPlayerHandler()->GetPlayer(pEnt);
		if (pPlayer != nullptr)
		{
			PlayerClass* pPlayerClass = pPlayer->ChangeClass("Zombie");
			if (pPlayerClass != nullptr)
			{
				pPlayerClass->Become();
				UTIL_ClientPrintAll(HUD_PRINTTALK, UTIL_VarArgs("testzombie: IsZombie(): %s\n", (pPlayerClass->IsZombie() ? "true" : "false")));
			}
			else
			{
				UTIL_ClientPrintAll(HUD_PRINTTALK, "PlayerClass is nullptr\n");
			}
		}
		else
		{
			UTIL_ClientPrintAll(HUD_PRINTTALK, "Player not found.\n");
		}

		RETURN_META(MRES_SUPERCEDE);
	}
	else if (strcmp(pszCmd, "testhunter") == 0)
	{
		// how to become a zombie?
		Player* pPlayer = GetPlayerHandler()->GetPlayer(pEnt);
		if (pPlayer != nullptr)
		{
			PlayerClass* pPlayerClass = pPlayer->ChangeClass("HunterZombie");
			if (pPlayerClass != nullptr)
			{
				pPlayerClass->Become();
				UTIL_ClientPrintAll(HUD_PRINTTALK, UTIL_VarArgs("testhunter: IsZombie(): %s\n", (pPlayerClass->IsZombie() ? "true" : "false")));
			}
			else
			{
				UTIL_ClientPrintAll(HUD_PRINTTALK, "PlayerClass is nullptr\n");
			}
		}
		else
		{
			UTIL_ClientPrintAll(HUD_PRINTTALK, "Player not found.\n");
		}

		RETURN_META(MRES_SUPERCEDE);
	}
	else if (strcmp(pszCmd, "testhuman") == 0)
	{
		// how to become a zombie?
		Player* pPlayer = GetPlayerHandler()->GetPlayer(pEnt);
		if (pPlayer != nullptr)
		{
			PlayerClass* pPlayerClass = pPlayer->ChangeClass("Human");
			if (pPlayerClass != nullptr)
			{
				pPlayerClass->Become();
				UTIL_ClientPrintAll(HUD_PRINTTALK, UTIL_VarArgs("testhuman: IsZombie(): %s\n", (pPlayerClass->IsZombie() ? "true" : "false")));
			}
			else
			{
				UTIL_ClientPrintAll(HUD_PRINTTALK, "PlayerClass is nullptr\n");
			}
		}
		else
		{
			UTIL_ClientPrintAll(HUD_PRINTTALK, "Player not found.\n");
		}

		RETURN_META(MRES_SUPERCEDE);
	}
	
	RETURN_META(MRES_IGNORED);
}

void PlayerPreThink(edict_t* pEnt)
{
	Player* pPlayer = GetPlayerHandler()->GetPlayer(pEnt);
	pPlayer->CleanRecyleBinOfClasses();
	if (pPlayer->HasClass())
	{
		pPlayer->GetClass()->OnThink();
	}

	RETURN_META(MRES_IGNORED);
}

const char* GetGameDescription(void)
{
	RETURN_META_VALUE(MRES_SUPERCEDE, "TiG");
}

void C_AlertMessage(ALERT_TYPE atype, const char* szFmt, ...)
{
	if (atype != at_logged)
	{
		RETURN_META(MRES_IGNORED);
	}

	/* There are also more messages but we want only logs
	at_notice,
	at_console,		// same	as at_notice, but forces a ConPrintf, not a	message	box
	at_aiconsole,	// same	as at_console, but only	shown if developer level is	2!
	at_warning,
	at_error,
	at_logged		// Server print to console ( only in multiplayer games ).
	*/

	cell retVal = 0;

	// execute logevents and plugin_log forward
	if (g_logevents.logEventsExist())
	{
		va_list	logArgPtr;
		va_start(logArgPtr, szFmt);
		g_logevents.setLogString(szFmt, logArgPtr);
		va_end(logArgPtr);
		g_logevents.parseLogString();

		if (g_logevents.logEventsExist())
		{
			g_logevents.executeLogEvents();
		}
	}

	if (retVal)
	{
		RETURN_META(MRES_SUPERCEDE);
	}

	RETURN_META(MRES_IGNORED);
}

void CmdStart(const edict_t* pPlayer, const struct usercmd_s* pCmd, unsigned int random_seed)
{
	RETURN_META(MRES_IGNORED);
}

void C_MessageBegin_Post(int msg_dest, int msg_type, const float* pOrigin, edict_t* ed)
{
	int playerIndex;
	edict_t* pPlayer;
	if (ed)
	{
		playerIndex = ENTINDEX(ed);
		pPlayer = ed;
	}
	else {
		playerIndex = 0;
		pPlayer = 0;
	}

	if (msg_type < 0 || msg_type >= MAX_REG_MSGS)
		msg_type = 0;

	//mState = 0;
	//function = modMsgs[msg_type];
	//endfunction = modMsgsEnd[msg_type];

	g_events.parserInit(msg_type, &gpGlobals->time, pPlayer, playerIndex);

	RETURN_META(MRES_IGNORED);
}

void C_WriteByte_Post(int iValue)
{
	g_events.parseValue(iValue);

	RETURN_META(MRES_IGNORED);
}

void C_WriteChar_Post(int iValue)
{
	g_events.parseValue(iValue);

	RETURN_META(MRES_IGNORED);
}

void C_WriteShort_Post(int iValue)
{
	g_events.parseValue(iValue);

	RETURN_META(MRES_IGNORED);
}

void C_WriteLong_Post(int iValue)
{
	g_events.parseValue(iValue);

	RETURN_META(MRES_IGNORED);
}

void C_WriteAngle_Post(float flValue)
{
	g_events.parseValue(flValue);

	RETURN_META(MRES_IGNORED);
}

void C_WriteCoord_Post(float flValue)
{
	g_events.parseValue(flValue);

	RETURN_META(MRES_IGNORED);
}

void C_WriteString_Post(const char* sz)
{
	g_events.parseValue(sz);

	RETURN_META(MRES_IGNORED);
}

void C_WriteEntity_Post(int iValue)
{
	g_events.parseValue(iValue);

	RETURN_META(MRES_IGNORED);
}

void C_MessageEnd_Post(void)
{
	g_events.executeEvents();

	RETURN_META(MRES_IGNORED);
}

void OnRoundStart()
{
	g_GameRules.OnRoundStart();
}

void OnNewRound(int msg)
{
	g_GameRules.OnRoundRestart();
}

int DispatchSpawn(edict_t* pent)
{
	RETURN_META_VALUE(MRES_IGNORED, g_GameRules.OnEntitySpawn(pent));
}

int OnPlayerTakeDamage(Hook* hook, void* pthis, entvars_t* inflictor, entvars_t* attacker, float damage, int damagebits)
{
	_FUNC_BEGIN_V(int, 0, MRES_UNSET);

	edict_t* pPlayerEnt = g_TypeConversion.cbase_to_edict(pthis);

	ModifiableWrappedEntity modifiableInflictor(g_TypeConversion.entvar_to_edict(inflictor)),
		modifiableAttacker(g_TypeConversion.entvar_to_edict(attacker));

	Player *pPlayer = GetPlayerHandler()->GetPlayer(pPlayerEnt);
	if (pPlayer->HasClass())
	{
		_CALL_FUNC(pPlayer->GetClass()->OnTakeDamage(
			&modifiableInflictor,
			&modifiableAttacker,
			damage, damagebits));
	}

	_FUNC_CHECK_STATE()
	{
		_RETURN_FUNC_END_V();
	}

#if defined(_WIN32)
	_FUNC_ORIG_RET = reinterpret_cast<int(__fastcall*)(void*, int, entvars_t*, entvars_t*, float, int)>(hook->func)(
		pthis, 0, modifiableInflictor.GetPev(), modifiableAttacker.GetPev(), damage, damagebits);
#elif defined(__linux__) || defined(__APPLE__)
	_FUNC_ORIG_RET = reinterpret_cast<int (*)(void*, entvars_t*, entvars_t*, float, int)>(hook->func)(
		pthis, modifiableInflictor.GetPev(), modifiableAttacker.GetPev(), damage, damagebits);
#endif

	_RETURN_FUNC_END_V();
}


void OnWeaponTouch(Hook* hook, void* pthis, void* pother)
{
	_FUNC_BEGIN(MRES_UNSET);

	edict_t* pEdict = g_TypeConversion.cbase_to_edict(pother);

	Player* pPlayer = GetPlayerHandler()->GetPlayer(pEdict);
	if (pPlayer != nullptr && pPlayer->HasClass())
	{
		Zombie *pZombie = dynamic_cast<Zombie*>(pPlayer->GetClass());
		if (pZombie != nullptr)
			pZombie->OnTouchWeapon(&WrappedEntity(pEdict)); // void type no need 
	}

	_FUNC_CHECK_STATE()
	{
		_RETURN_FUNC_END();
	}

#if defined(_WIN32)
	reinterpret_cast<void(__fastcall*)(void*, int, void*)>(hook->func)(pthis, 0, pother);
#elif defined(__linux__) || defined(__APPLE__)
	reinterpret_cast<void (*)(void*, void*)>(hook->func)(pthis, pother);
#endif

	_RETURN_FUNC_END();
}

int OnKnifeDeploy(Hook* hook, void* pthis)
{
	_FUNC_BEGIN_V(int, 0, MRES_UNSET);

	// ...

#if defined(_WIN32)
	_FUNC_ORIG_RET = reinterpret_cast<int(__fastcall*)(void*, int)>(hook->func)(pthis, 0);
#elif defined(__linux__) || defined(__APPLE__)
	_FUNC_ORIG_RET = reinterpret_cast<int (*)(void*)>(hook->func)(pthis);
#endif

	GET_OFFSET("CBasePlayerItem", m_pPlayer, offsetPlayer);

	edict_t* pKnife = g_TypeConversion.cbase_to_edict(pthis);
	void* pCBase = get_pdata<void*>(pthis, offsetPlayer);
	Player* pPlayer = GetPlayerHandler()->GetPlayer(g_TypeConversion.cbase_to_edict(pCBase));

	// Check if owner is a player
	if (pPlayer != nullptr && pPlayer->HasClass())
	{
		PlayerClass* pPlayerClass = pPlayer->GetClass();
		// player is zombie
		if (pPlayerClass->IsZombie())
		{
			// call event
			Zombie* pZombie = dynamic_cast<Zombie*>(pPlayerClass);
			if (pZombie != nullptr)
				_CALL_FUNC(pZombie->OnKnifeDeploy(&WrappedEntity(pKnife)));
		}
	}

	_RETURN_FUNC_END_V();
}

void OnPlayerSpawn(Hook* hook, void* pthis)
{
#if defined(_WIN32)
	reinterpret_cast<void(__fastcall*)(void*, int)>(hook->func)(pthis, 0);
#elif defined(__linux__) || defined(__APPLE__)
	reinterpret_cast<void (*)(void*)>(hook->func)(pthis);
#endif

	int index = g_TypeConversion.cbase_to_id(pthis);
	//if (MF_IsPlayerIngame(index) && MF_IsPlayerAlive(index))
	if (GetPlayerHandler()->HasPlayer(index) && MF_IsPlayerIngame(index) && MF_IsPlayerAlive(index))
	{
		if (GetPlayerHandler()->GetPlayer(index)->HasClass())
		{
			GetPlayerHandler()->GetPlayer(index)->GetClass()->OnSpawn();
		}
	}
}

void OnPlayerKilled(Hook* hook, void* pthis, entvars_t* killer, int shouldgibs)
{
	_FUNC_BEGIN(MRES_UNSET);

	ModifiableWrappedEntity modifiableKiller(g_TypeConversion.entvar_to_edict(killer));

	edict_t* pVictim = g_TypeConversion.cbase_to_edict(pthis);
	if (GetPlayerHandler()->GetPlayer(pVictim)->HasClass())
	{
		GetPlayerHandler()->GetPlayer(pVictim)->GetClass()->OnKilled(&modifiableKiller, shouldgibs);
	}

	_FUNC_CHECK_STATE()
	{
		_RETURN_FUNC_END();
	}

#if defined(_WIN32)
	reinterpret_cast<void(__fastcall*)(void*, int, entvars_t*, int)>(hook->func)(pthis, 0, modifiableKiller.GetPev(), shouldgibs);
#elif defined(__linux__) || defined(__APPLE__)
	reinterpret_cast<void (*)(void*, entvars_t*, int)>(hook->func)(pthis, modifiableKiller.GetPev(), shouldgibs);
#endif

	_RETURN_FUNC_END();
}

void OnPlayerResetMaxspeed(Hook* hook, void* pthis)
{
#if defined(_WIN32)
	reinterpret_cast<void(__fastcall*)(void*, int)>(hook->func)(pthis, 0);
#elif defined(__linux__) || defined(__APPLE__)
	reinterpret_cast<void (*)(void*)>(hook->func)(pthis);
#endif

	int index = g_TypeConversion.cbase_to_id(pthis);
	if (MF_IsPlayerIngame(index) && MF_IsPlayerAlive(index))
	{
		Player* pPlayer = GetPlayerHandler()->GetPlayer(index);
		if (pPlayer->HasClass())
			pPlayer->GetClass()->SetMaxspeed();
	}
}

void OnCheckWinConditions(Hook* hook, void* pthis)
{
	g_GameRules.OnCheckWinConditions(pthis);
}

void OnGameRulesThink(Hook* hook, void* pthis)
{
	g_GameRules.OnThink(pthis);

#if defined(_WIN32)
	reinterpret_cast<void(__fastcall*)(void*, int)>(hook->func)(pthis, 0);
#elif defined(__linux__) || defined(__APPLE__)
	reinterpret_cast<void (*)(void*)>(hook->func)(pthis);
#endif
}

void EmitSound(edict_t* entity, int channel, const char* sample, /*int*/float volume, float attenuation, int fFlags, int pitch)
{
	_FUNC_BEGIN(MRES_IGNORED);

	Player* pPlayer = GetPlayerHandler()->GetPlayer(entity);
	if (pPlayer != nullptr && pPlayer->HasClass())
	{
		if (strlen(sample) > 7 && strncmp(sample + 7, "bhit", 4) == 0)
		{
			pPlayer->GetClass()->OnPainSound(channel);
		}
		else if (strlen(sample) > 14 && strncmp(sample + 8, "knife", 5) == 0)
		{
			char type[16];
			strcpy(type, (sample + 14));
			type[strlen(type) - 4] = '\0';

			pPlayer->GetClass()->OnKnifeSound(channel, type);
		}
		else if (strncmp(sample + 7, "die", 3) == 0 || strncmp(sample + 7, "death", 5) == 0)
		{
			pPlayer->GetClass()->OnDieSound(channel);
		}
	}

	//SERVER_PRINT(UTIL_VarArgs("STATE = %d, ", _FUNC_STATE));
	_RETURN_META_END();
}

void StartFrame_Post(void)
{
	g_TimerManager.Executes();
	RETURN_META(MRES_IGNORED);
}