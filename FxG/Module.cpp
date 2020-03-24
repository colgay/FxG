#include "amxxmodule.h"
#include "Player.h"
#include "PlayerHandler.h"
#include "Zombie.h"
#include "offsets.h"
#include "hook.h"
#include "HamSandwich.h"
#include "Utilities.h"
#include "AmxxApi.h"
#include "CEvent.h"
#include "CLogEvent.h"
#include "Timer.h"

#include <vector>
#include <string>
#include <any>

#include <HLTypeConversion.h>
#include <amtl/am-string.h>
#include <amtl/am-vector.h>

HLTypeConversion g_TypeConversion;

OffsetManager Offsets;

extern ke::Vector<Hook*> g_Hooks[HAM_LAST_ENTRY_DONT_USE_ME_LOL];

std::vector<std::pair<std::string, BasePlayerClassHelper*>> g_PlayerClassVector;

int ReadConfig(void);
int OnPlayerTakeDamage(Hook* hook, void* pthis, entvars_t* inflictor, entvars_t* attacker, float damage, int damagebits);
void OnWeaponTouch(Hook* hook, void* pthis, void* pother);
int OnKnifeDeploy(Hook* hook, void* pthis);
void OnPlayerSpawn(Hook* hook, void* pthis);
void OnPlayerResetMaxspeed(Hook* hook, void* pthis);
void OnRoundStart(void);
void OnNewRound(int);
void OnGameCountDown(Timer* pTimer);
void OnGameModeStart();

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
}


void OnAmxxDetach()
{
	g_logevents.clearLogEvents();
}

void ServerActivate_Post(edict_t* pEdictList, int edictCount, int clientMax)
{
	PrecacheZombie();

	RegisterEvent("HLTV", &OnNewRound, "a", 2, "1=0", "2=0");
	RegisterLogEvent(&OnRoundStart, 2, 1, "1=Round_Start");

	RegisterHam(Ham_TakeDamage, "player", reinterpret_cast<void*>(OnPlayerTakeDamage));
	RegisterHam(Ham_Touch, "weaponbox", reinterpret_cast<void*>(OnWeaponTouch));
	RegisterHam(Ham_Item_Deploy, "weapon_knife", reinterpret_cast<void*>(OnKnifeDeploy));
	RegisterHam(Ham_Spawn, "player", reinterpret_cast<void*>(OnPlayerSpawn));
	RegisterHam(Ham_CS_Player_ResetMaxSpeed, "player", reinterpret_cast<void*>(OnPlayerResetMaxspeed));
}

void ServerDeactivate_Post()
{
	g_events.clearEvents();
	g_logevents.clearLogEvents();
	g_TimerManager.Clean();
}

void OnPluginsLoaded()
{
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

void ClientPutInServer(edict_t* pEnt)
{
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
				UTIL_ClientPrintAll(HUD_PRINTTALK, "PlayerClass is nullptr");
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
				UTIL_ClientPrintAll(HUD_PRINTTALK, UTIL_VarArgs("testzombie: IsZombie(): %s\n", (pPlayerClass->IsZombie() ? "true" : "false")));
			}
			else
			{
				UTIL_ClientPrintAll(HUD_PRINTTALK, "PlayerClass is nullptr");
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
	SERVER_PRINT("OnRoundStart() called\n");
}

void OnNewRound(int msg)
{
	SERVER_PRINT("OnNewRound() called\n");

	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		if (!MF_IsPlayerIngame(i))
			continue;

		Player* pPlayer = GetPlayerHandler()->GetPlayer(i);
		if (pPlayer != nullptr)
		{
			if (pPlayer->GetClass()->IsZombie())
			{
				pPlayer->ChangeClass("Human");
			}
		}
	}

	g_TimerManager.Remove(0);

	TimerParam_t params;
	params.push_back(20);
	g_TimerManager.Create(1.0, 0, &OnGameCountDown, params, true);
}

void OnGameCountDown(Timer* pTimer)
{
	TimerParam_t &params = pTimer->GetParams();
	
	int &count = std::any_cast<int&>(params[0]);
	if (count > 0)
	{
		count--;

		if (count <= 10)
		{
			char word[32];
			UTIL_IntToString(count, word);

			for (int i = 1; i <= gpGlobals->maxClients; i++)
			{
				if (MF_IsPlayerIngame(i))
					CLIENT_COMMAND(g_TypeConversion.id_to_edict(i), "spk fvox/%s\n", word);
			}
		}

		hudtextparms_t textparms;

		textparms.r2 = 255;
		textparms.g2 = 255;
		textparms.b2 = 250;
		textparms.r1 = 0;
		textparms.g1 = 255;
		textparms.b1 = 0;
		textparms.x = -1.0;
		textparms.y = 0.25;
		textparms.effect = 0;
		textparms.fxTime = 0.0;
		textparms.holdTime = 1.0;
		textparms.fadeinTime = 0.0;
		textparms.fadeoutTime = 0.5;
		textparms.channel = 4;

		UTIL_HudMessageAll(textparms, UTIL_VarArgs("Covid-19 coronavirus is spreading (%d)...", count));
	}
	else
	{
		OnGameModeStart();
		pTimer->MarkAsDeleted(true);
	}
}

void OnGameModeStart()
{
	hudtextparms_t textparms;

	textparms.r2 = 255;
	textparms.g2 = 255;
	textparms.b2 = 250;
	textparms.r1 = 0;
	textparms.g1 = 255;
	textparms.b1 = 0;
	textparms.x = -1.0;
	textparms.y = 0.25;
	textparms.effect = 0;
	textparms.fxTime = 0.0;
	textparms.holdTime = 3.0;
	textparms.fadeinTime = 1.0;
	textparms.fadeoutTime = 1.0;
	textparms.channel = 4;

	UTIL_HudMessageAll(textparms, "The first infection has been started!!");

	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		if (MF_IsPlayerIngame(i))
			CLIENT_COMMAND(g_TypeConversion.id_to_edict(i), "spk scientist/scream1\n");
	}
}

int OnPlayerTakeDamage(Hook* hook, void* pthis, entvars_t* inflictor, entvars_t* attacker, float damage, int damagebits)
{
	int origret;

	SERVER_PRINT(UTIL_VarArgs("OnPlayerTakeDamage(%p, %p, %p, %f, %d)\n", pthis, inflictor, attacker, damage, damagebits));

#if defined(_WIN32)
	origret = reinterpret_cast<int(__fastcall*)(void*, int, entvars_t*, entvars_t*, float, int)>(hook->func)(pthis, 0, inflictor, attacker, damage, damagebits);
#elif defined(__linux__) || defined(__APPLE__)
	origret = reinterpret_cast<int (*)(void*, entvars_t*, entvars_t*, float, int)>(hook->func)(pthis, inflictor, attacker, damage, damagebits);
#endif

	return origret;
}


void OnWeaponTouch(Hook* hook, void* pthis, void* pother)
{
	edict_t* pEdict = g_TypeConversion.cbase_to_edict(pother);

	Player* pPlayer = GetPlayerHandler()->GetPlayer(pEdict);
	if (pPlayer != nullptr)
	{
		if (pPlayer->GetClass()->IsZombie())
		{
			return;
		}
	}

#if defined(_WIN32)
	reinterpret_cast<void(__fastcall*)(void*, int, void*)>(hook->func)(pthis, 0, pother);
#elif defined(__linux__) || defined(__APPLE__)
	reinterpret_cast<void (*)(void*, void*)>(hook->func)(pthis, pother);
#endif
}

int OnKnifeDeploy(Hook* hook, void* pthis)
{
	int origret;

	//SERVER_PRINT(UTIL_VarArgs("OnKnifeDeploy(%d)\n", g_TypeConversion.cbase_to_id(pthis)));

#if defined(_WIN32)
	origret = reinterpret_cast<int(__fastcall*)(void*, int)>(hook->func)(pthis, 0);
#elif defined(__linux__) || defined(__APPLE__)
	origret = reinterpret_cast<int (*)(void*)>(hook->func)(pthis);
#endif

	GET_OFFSET("CBasePlayerItem", m_pPlayer, offsetPlayer);

	edict_t* pKnife = g_TypeConversion.cbase_to_edict(pthis);
	void* pCBase = get_pdata<void*>(pthis, offsetPlayer);
	Player* pPlayer = GetPlayerHandler()->GetPlayer(g_TypeConversion.cbase_to_edict(pCBase));

	// Check if owner is a player
	if (pPlayer != nullptr)
	{
		PlayerClass* pPlayerClass = pPlayer->GetClass();

		// player is zombie
		if (pPlayerClass != nullptr && pPlayerClass->IsZombie())
		{
			// call event
			Zombie* pZombie = dynamic_cast<Zombie*>(pPlayerClass);
			if (pZombie != nullptr)
				pZombie->OnKnifeDeploy(&WrappedEntity(pKnife));
		}
	}
	else
	{
		SERVER_PRINT("Not a player\n");
	}

	return origret;
}

void OnPlayerSpawn(Hook* hook, void* pthis)
{
#if defined(_WIN32)
	reinterpret_cast<void(__fastcall*)(void*, int)>(hook->func)(pthis, 0);
#elif defined(__linux__) || defined(__APPLE__)
	reinterpret_cast<void (*)(void*)>(hook->func)(pthis);
#endif

	int index = g_TypeConversion.cbase_to_id(pthis);
	if (MF_IsPlayerAlive(index))
	{
		Player* pPlayer = GetPlayerHandler()->GetPlayer(index);
		if (pPlayer != nullptr)
		{
			SERVER_PRINT("Player::PlayerClass::Become() called\n");
			pPlayer->GetClass()->Become();
		}
	}
}

void OnPlayerResetMaxspeed(Hook* hook, void* pthis)
{
#if defined(_WIN32)
	reinterpret_cast<void(__fastcall*)(void*, int)>(hook->func)(pthis, 0);
#elif defined(__linux__) || defined(__APPLE__)
	reinterpret_cast<void (*)(void*)>(hook->func)(pthis);
#endif

	int index = g_TypeConversion.cbase_to_id(pthis);
	if (MF_IsPlayerAlive(index))
	{
		Player* pPlayer = GetPlayerHandler()->GetPlayer(index);
		pPlayer->GetClass()->SetMaxspeed();
	}
}

void StartFrame_Post(void)
{
	g_TimerManager.Executes();
	RETURN_META(MRES_IGNORED);
}