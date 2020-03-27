#include "amxxmodule.h"
#include "GameRules.h"

#include <set>
#include <string>

#include "GameMode.h"
#include "CstrikeDatas.h"
#include "offsets.h"
#include "Timer.h"
#include "Utilities.h"
#include "Player.h"
#include "PlayerHandler.h"

GameRules g_GameRules;

//void OnGameCountDown(Timer* pTimer);

GameRules::~GameRules()
{
	if (m_pCurrentMode != nullptr)
	{
		delete m_pCurrentMode;
		m_pCurrentMode = nullptr;
	}
}

void GameRules::PlayerDeathThink(void* pPlayer)
{
	if (m_IsGameModeStarted)
	{
		GET_OFFSET("CHalfLifeMultiplay", m_iRoundWinStatus, offsetWinStatus);

		if (get_pdata<int32>(*this->GetAddress(), offsetWinStatus) == WINSTATUS_NONE)
		{
			GET_OFFSET("CBasePlayer", m_iJoiningState, offsetJoinState);
			GET_OFFSET("CBasePlayer", m_fDeadTime, offsetDeadTime);

			edict_t* pPlayerEnt = g_TypeConversion.cbase_to_edict(pPlayer);

			if (get_pdata<int32>(pPlayerEnt, offsetJoinState) == JOINED && pPlayerEnt->v.deadflag == DEAD_DEAD)
			{
				if (gpGlobals->time >= get_pdata<float>(pPlayerEnt, offsetDeadTime) + 2.0)
				{
					GetPlayerHandler()->GetPlayer(pPlayerEnt)->ChangeClass("Zombie");
					pPlayerEnt->v.deadflag = DEAD_RESPAWNABLE;
				}
			}
		}
	}
}

int GameRules::OnEntitySpawn(edict_t* pEntity)
{
	if (m_IsActivated || FNullEnt(pEntity))
		RETURN_META_VALUE(MRES_IGNORED, 0);

	static const std::set<std::string> entities{
		"func_bomb_target",
		"info_bomb_target",
		"info_vip_start",
		"func_vip_safetyzone",
		"func_escapezone",
		"hostage_entity",
		"monster_scientist",
		"func_hostage_rescue",
		"info_hostage_rescue",
		"func_buyzone" };

	auto search = entities.find(STRING(pEntity->v.classname));
	if (search != entities.end())
	{
		SERVER_PRINT(UTIL_VarArgs("removed entity \"%s\"\n", STRING(pEntity->v.classname)));
		REMOVE_ENTITY(pEntity);
		RETURN_META_VALUE(MRES_SUPERCEDE, -1);
	}

	RETURN_META_VALUE(MRES_IGNORED, 0);
}

void GameRules::OnCheckWinConditions(void *pGameRules)
{
	GET_OFFSET("CHalfLifeMultiplay", m_iRoundWinStatus, offsetWinStatus);

	if (get_pdata<int32>(pGameRules, offsetWinStatus) != WINSTATUS_NONE)
		return;

	GET_OFFSET("CHalfLifeMultiplay", m_bFirstConnected, offsetConnected);

	SERVER_PRINT(UTIL_VarArgs("m_iRoundWinStatus = %d, m_bFirstConnected = %s",
		get_pdata<int32>(pGameRules, offsetWinStatus),
		get_pdata<bool>(pGameRules, offsetConnected) ? "true" : "false"));

	if (get_pdata<bool>(pGameRules, offsetConnected) && get_pdata<int32>(pGameRules, offsetWinStatus) != WINSTATUS_NONE)
		return;

	InitializePlayerCounts();

	GET_OFFSET("CHalfLifeMultiplay", m_iNumTerrorist, offsetNumT);
	GET_OFFSET("CHalfLifeMultiplay", m_iNumCT, offsetNumCt);
	GET_OFFSET("CHalfLifeMultiplay", m_iNumSpawnableTerrorist, offsetSpawnableT);
	GET_OFFSET("CHalfLifeMultiplay", m_iNumSpawnableCT, offsetSpawnableCt);

	int numSpawnableT = get_pdata<int32>(pGameRules, offsetSpawnableT);
	int numSpawnableCt = get_pdata<int32>(pGameRules, offsetSpawnableCt);

	SERVER_PRINT(UTIL_VarArgs("m_iNumSpawnableTerrorist = %d, m_iNumSpawnableCT = %d", numSpawnableT, numSpawnableCt));

	m_NeededPlayers = false;
	if (numSpawnableT + numSpawnableCt < 3)
	{
		UTIL_ClientPrintAll(HUD_PRINTCONSOLE, "#Game_scoring");
		m_NeededPlayers = true;
		set_pdata<bool>(pGameRules, offsetConnected, false);
	}

	if (!get_pdata<bool>(pGameRules, offsetConnected) && numSpawnableT + numSpawnableCt >= 3)
	{
		GET_OFFSET("CGameRules", m_bFreezePeriod, offsetFreeze);
		GET_OFFSET("CHalfLifeMultiPlay", m_bCompleteReset, offsetReset);

		UTIL_LogPrintf("World triggered \"Game_Commencing\"\n");

		set_pdata<bool>(pGameRules, offsetFreeze, false);
		set_pdata<bool>(pGameRules, offsetReset, true);

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
		textparms.channel = 1;

		UTIL_HudMessageAll(textparms, "Game Commencing...");

		TerminateRound(WINSTATUS_DRAW, "#Game_Commencing", ROUND_GAME_COMMENCE, 3.0);

		set_pdata<bool>(pGameRules, offsetConnected, true);
		return;
	}

	if (m_IsGameModeStarted)
	{
		if (!CountHumans(true))
		{
			UTIL_SendAudio(nullptr, 0, "%!MRAD_terwin", 90);

			hudtextparms_t textparms;

			textparms.r2 = 255;
			textparms.g2 = 255;
			textparms.b2 = 250;
			textparms.r1 = 255;
			textparms.g1 = 0;
			textparms.b1 = 0;
			textparms.x = -1.0;
			textparms.y = 0.25;
			textparms.effect = 0;
			textparms.fxTime = 0.0;
			textparms.holdTime = 5.5;
			textparms.fadeinTime = 1.0;
			textparms.fadeoutTime = 1.0;
			textparms.channel = 1;

			UTIL_HudMessageAll(textparms, "Zombies Win...");

			TerminateRound(WINSTATUS_TERRORISTS, "#Terrorists_Win", ROUND_TERRORISTS_WIN, 7.5);
		}
	}
	else if (!CountHumans(true))
	{
		UTIL_SendAudio(nullptr, 0, "%!MRAD_rounddraw", 100);

		hudtextparms_t textparms;

		textparms.r2 = 255;
		textparms.g2 = 255;
		textparms.b2 = 250;
		textparms.r1 = 150;
		textparms.g1 = 150;
		textparms.b1 = 150;
		textparms.x = -1.0;
		textparms.y = 0.25;
		textparms.effect = 0;
		textparms.fxTime = 0.0;
		textparms.holdTime = 3.0;
		textparms.fadeinTime = 1.0;
		textparms.fadeoutTime = 1.0;
		textparms.channel = 1;

		UTIL_HudMessageAll(textparms, "Round Draw...");

		TerminateRound(WINSTATUS_TERRORISTS, "#Round_Draw", ROUND_END_DRAW, 5.0);
	}
}

void GameRules::OnThink(void* pGameRules)
{
	//SERVER_PRINT("GameRules::Think() called");

	GET_OFFSET("CHalfLifeMultiplay", m_iRoundWinStatus, offsetWinStatus);

	if (!IsFreezePeriod() && get_pdata<int32>(pGameRules, offsetWinStatus) == WINSTATUS_NONE && GetRoundRemainingTime() <= 0)
	{
		GET_OFFSET("CHalfLifeMultiplay", m_bFirstConnected, offsetConnected);
		
		if (!get_pdata<bool>(pGameRules, offsetConnected))
		{
			UTIL_SendAudio(nullptr, 0, "%!MRAD_rounddraw", 100);

			hudtextparms_t textparms;

			textparms.r2 = 255;
			textparms.g2 = 255;
			textparms.b2 = 250;
			textparms.r1 = 150;
			textparms.g1 = 150;
			textparms.b1 = 150;
			textparms.x = -1.0;
			textparms.y = 0.25;
			textparms.effect = 0;
			textparms.fxTime = 0.0;
			textparms.holdTime = 3.0;
			textparms.fadeinTime = 1.0;
			textparms.fadeoutTime = 1.0;
			textparms.channel = 1;

			UTIL_HudMessageAll(textparms, "Round Draw...");

			TerminateRound(WINSTATUS_TERRORISTS, "#Round_Draw", ROUND_END_DRAW, 5.0);
		}
		else if (m_IsGameModeStarted)
		{
			UTIL_SendAudio(nullptr, 0, "%!MRAD_ctwin", 100);

			hudtextparms_t textparms;

			textparms.r2 = 255;
			textparms.g2 = 255;
			textparms.b2 = 250;
			textparms.r1 = 0;
			textparms.g1 = 100;
			textparms.b1 = 255;
			textparms.x = -1.0;
			textparms.y = 0.25;
			textparms.effect = 0;
			textparms.fxTime = 0.0;
			textparms.holdTime = 3.0;
			textparms.fadeinTime = 1.0;
			textparms.fadeoutTime = 1.0;
			textparms.channel = 1;

			UTIL_HudMessageAll(textparms, "Survivors have defeat the plague...");

			TerminateRound(WINSTATUS_CTS, "#CTs_Win", ROUND_CTS_WIN, 7.5);
		}
	}
}

void GameRules::OnRoundStart()
{
	//m_RoundStartTime = gpGlobals->time;
}

void GameRules::OnRoundRestart()
{
	m_IsGameModeStarted = false;

	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		if (!MF_IsPlayerIngame(i))
			continue;

		Player* pPlayer = GetPlayerHandler()->GetPlayer(i);
		if (pPlayer != nullptr)
		{
			if (pPlayer->HasClass() && pPlayer->GetClass()->IsZombie())
			{
				pPlayer->ChangeClass("Human");
			}
		}
	}

	g_TimerManager.Remove(0);

	TimerParam_t params;
	params.push_back(20);

	//g_TimerManager.Create(1.0, 0, &this->OnGameCountDown, params, true);
	g_TimerManager.Create(1.0, 0, [](Timer* pTimer) { g_GameRules.OnGameCountDown(pTimer); }, params, true);// this is ok?

	//m_RoundStartTime = gpGlobals->time;
}

void GameRules::OnGameCountDown(Timer* pTimer)
{
	TimerParam_t& params = pTimer->GetParams();

	//GameRules* pGameRules = std::any_cast<GameRules*>(params[0]);

	int& count = std::any_cast<int&>(params[0]);
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
		textparms.channel = 1;

		UTIL_HudMessageAll(textparms, UTIL_VarArgs("Covid-19 coronavirus is spreading (%d)...", count));
	}
	else
	{
		this->StartGameMode("InfectionMode");
		pTimer->MarkAsDeleted(true);
	}
}

void GameRules::TerminateRound(int status, const char *pszMsg, ScenarioEventEndRound event, float tmDelay)
{
	if (!m_NeededPlayers && status != WINSTATUS_DRAW)
	{
		if (status == WINSTATUS_TERRORISTS)
		{
			GET_OFFSET("CHalfLifeMultiplay", m_iNumTerroristWins, offsetTerroristWins);
			set_pdata<int32>(*this->GetAddress(), offsetTerroristWins, get_pdata<int32>(*this->GetAddress(), offsetTerroristWins) + 1);
		}
		else if (status == WINSTATUS_CTS)
		{
			GET_OFFSET("CHalfLifeMultiplay", m_iNumCTWins, offsetCtWins);
			set_pdata<int32>(*this->GetAddress(), offsetCtWins, get_pdata<int32>(*this->GetAddress(), offsetCtWins) + 1);
		}

		this->UpdateTeamScores();
	}

	this->EndRoundMessage(pszMsg, event);

	GET_OFFSET("CHalfLifeMultiplay", m_iRoundWinStatus, offsetStatus);
	GET_OFFSET("CHalfLifeMultiplay", m_fTeamCount, offsetCount);
	GET_OFFSET("CHalfLifeMultiplay", m_bRoundTerminating, offsetTerminating);

	set_pdata<int32>(*this->GetAddress(), offsetStatus, status);
	set_pdata<float>(*this->GetAddress(), offsetCount, gpGlobals->time + tmDelay);
	set_pdata<bool>(*this->GetAddress(), offsetTerminating, true);
}

void GameRules::EndRoundMessage(const char* sentence, ScenarioEventEndRound event)
{
	const char* team = nullptr;
	const char* message = sentence;
	bool bTeamTriggered = true;

	if (sentence[0] == '#')
		message = sentence + 1;

	if (sentence[0])
	{
		//UTIL_ClientPrintAll(HUD_PRINTCENTER, sentence);

		switch (event)
		{
		case ROUND_TARGET_BOMB:
		case ROUND_VIP_ASSASSINATED:
		case ROUND_TERRORISTS_ESCAPED:
		case ROUND_TERRORISTS_WIN:
		case ROUND_HOSTAGE_NOT_RESCUED:
		case ROUND_VIP_NOT_ESCAPED:
			team = "ZOMBIE";
			break;
		case ROUND_VIP_ESCAPED:
		case ROUND_CTS_PREVENT_ESCAPE:
		case ROUND_ESCAPING_TERRORISTS_NEUTRALIZED:
		case ROUND_BOMB_DEFUSED:
		case ROUND_CTS_WIN:
		case ROUND_ALL_HOSTAGES_RESCUED:
		case ROUND_TARGET_SAVED:
		case ROUND_TERRORISTS_NOT_ESCAPED:
			team = "SURVIVOR";
			break;
		default:
			bTeamTriggered = false;
			break;
		}

		GET_OFFSET("CHalfLifeMultiplay", m_iNumCTWins, offsetCtWins);
		GET_OFFSET("CHalfLifeMultiplay", m_iNumTerroristWins, offsetTerroristWins);

		if (bTeamTriggered)
		{
			UTIL_LogPrintf("Team \"%s\" triggered \"%s\" (SURVIVOR \"%i\") (ZOMBIE \"%i\")\n", team, message, 
				get_pdata<int32>(*this->GetAddress(), offsetCtWins), get_pdata<int32>(*this->GetAddress(), offsetTerroristWins));
		}
		else
		{
			UTIL_LogPrintf("World triggered \"%s\" (SURVIVOR \"%i\") (ZOMBIE \"%i\")\n", message, 
				get_pdata<int32>(*this->GetAddress(), offsetCtWins), get_pdata<int32>(*this->GetAddress(), offsetTerroristWins));
		}
	}

	UTIL_LogPrintf("World triggered \"Round_End\"\n");
}

void GameRules::InitializePlayerCounts()
{
	GET_OFFSET("CBasePlayer", m_iTeam, offsetTeam);
	GET_OFFSET("CBasePlayer", m_iMenu, offsetMenu);
	GET_OFFSET("CHalfLifeMultiplay", m_iNumTerrorist, offsetNumT);
	GET_OFFSET("CHalfLifeMultiplay", m_iNumCT, offsetNumCt);
	GET_OFFSET("CHalfLifeMultiplay", m_iNumSpawnableTerrorist, offsetSpawnableT);
	GET_OFFSET("CHalfLifeMultiplay", m_iNumSpawnableCT, offsetSpawnableCt);

	int numT = 0, numCt = 0, numSpawnableT = 0, numSpawnableCt = 0;

	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		if (!MF_IsPlayerIngame(i))
			continue;

		edict_t* pPlayer = g_TypeConversion.id_to_edict(i);

		switch (get_pdata<int32>(pPlayer, offsetTeam))
		{
			case TEAM_T:
			{
				numT++;

				if (get_pdata<int32>(pPlayer, offsetMenu) != Menu_ChooseAppearance)
					numSpawnableT++;

				break;
			}
			case TEAM_CT:
			{
				numCt++;

				if (get_pdata<int32>(pPlayer, offsetMenu) != Menu_ChooseAppearance)
					numSpawnableCt++;

				break;
			}
		}
	}

	set_pdata<int32>(*this->GetAddress(), offsetNumT, numT);
	set_pdata<int32>(*this->GetAddress(), offsetNumCt, numCt);
	set_pdata<int32>(*this->GetAddress(), offsetSpawnableT, numSpawnableT);
	set_pdata<int32>(*this->GetAddress(), offsetSpawnableCt, numSpawnableCt);
}

void GameRules::UpdateTeamScores()
{
	static int msgTeamScore = 0;
	if (msgTeamScore == 0)
	{
		msgTeamScore = GET_USER_MSG_ID(PLID, "TeamScore", 0);
	}

	GET_OFFSET("CHalfLifeMultiplay", m_iNumCTWins, offsetCtWins);
	GET_OFFSET("CHalfLifeMultiplay", m_iNumTerroristWins, offsetTerroristWins);

	MESSAGE_BEGIN(MSG_BROADCAST, msgTeamScore);
	WRITE_STRING("CT");
	WRITE_SHORT(get_pdata<int32>(*this->GetAddress(), offsetCtWins));
	MESSAGE_END();

	MESSAGE_BEGIN(MSG_BROADCAST, msgTeamScore);
	WRITE_STRING("TERRORIST");
	WRITE_SHORT(get_pdata<int32>(*this->GetAddress(), offsetTerroristWins));
	MESSAGE_END();
}

int GameRules::CountHumans(bool alive) const
{
	int count = 0;

	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		if (!MF_IsPlayerIngame(i))
			continue;

		if (alive && !MF_IsPlayerAlive(i))
			continue;

		Player* pPlayer = GetPlayerHandler()->GetPlayer(i);
		if (pPlayer->HasClass() && !pPlayer->GetClass()->IsZombie())
			count++;
	}

	return count;
}

float GameRules::GetRoundRemainingTime() const
{
	GET_OFFSET("CHalfLifeMultiplay", m_iRoundTimeSecs, offsetTimeSecs);
	GET_OFFSET("CHalfLifeMultiplay", m_fRoundCount, offsetRoundCount);

	return static_cast<float>(get_pdata<int32>(*this->GetAddress(), offsetTimeSecs)) - gpGlobals->time + get_pdata<float>(*this->GetAddress(), offsetRoundCount);
}

bool GameRules::IsFreezePeriod() const
{
	GET_OFFSET("CGameRules", m_bFreezePeriod, offsetFreeze);

	return get_pdata<bool>(*this->GetAddress(), offsetFreeze);
}

GameMode* GameRules::StartGameMode(const char* pszMode)
{
	BaseGameModeHelper* pHelper = GetGameModeHelperByName(pszMode);
	if (pHelper == nullptr)
	{
		MF_Log("GameRules::StartGameMode(): GameMode %s is not found\n", pszMode);
		return nullptr;
	}

	if (m_pCurrentMode != nullptr)
	{
		delete m_pCurrentMode;
		m_pCurrentMode = nullptr;
	}

	m_pCurrentMode = pHelper->Instantiate();
	m_pCurrentMode->StartGameMode();
	m_IsGameModeStarted = true;

	return m_pCurrentMode;
}