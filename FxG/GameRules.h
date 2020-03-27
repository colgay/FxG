#pragma once

class GameMode;

enum
{
	WINSTATUS_NONE = 0,
	WINSTATUS_CTS,
	WINSTATUS_TERRORISTS,
	WINSTATUS_DRAW,
};

enum ScenarioEventEndRound
{
	ROUND_NONE,
	ROUND_TARGET_BOMB,
	ROUND_VIP_ESCAPED,
	ROUND_VIP_ASSASSINATED,
	ROUND_TERRORISTS_ESCAPED,
	ROUND_CTS_PREVENT_ESCAPE,
	ROUND_ESCAPING_TERRORISTS_NEUTRALIZED,
	ROUND_BOMB_DEFUSED,
	ROUND_CTS_WIN,
	ROUND_TERRORISTS_WIN,
	ROUND_END_DRAW,
	ROUND_ALL_HOSTAGES_RESCUED,
	ROUND_TARGET_SAVED,
	ROUND_HOSTAGE_NOT_RESCUED,
	ROUND_TERRORISTS_NOT_ESCAPED,
	ROUND_VIP_NOT_ESCAPED,
	ROUND_GAME_COMMENCE,
	ROUND_GAME_RESTART,
	ROUND_GAME_OVER
};

class Timer;

class GameRules
{
public:
	GameRules()
		: m_pCurrentMode(nullptr), m_pAddress(nullptr), m_NeededPlayers(false),
		m_IsActivated(false), m_IsGameModeStarted(false)/*, m_RoundStartTime(0)*/
	{
		// ...
	}

	~GameRules();
	
	void PlayerDeathThink(void* pPlayer);
	void OnServerActivated() { m_IsActivated = true; }
	void Init(void** pAddr) { m_pAddress = pAddr; }
	int OnEntitySpawn(edict_t* pEntity);
	void OnCheckWinConditions(void *pGameRules);
	void OnThink(void* pGameRules);
	void OnRoundStart();
	void OnRoundRestart();
	void OnGameCountDown(Timer *pTimer);
	void TerminateRound(int status, const char* pszMsg, ScenarioEventEndRound event, float tmDelay);
	void EndRoundMessage(const char* sentence, ScenarioEventEndRound event);
	void InitializePlayerCounts();
	void UpdateTeamScores();
	int CountHumans(bool alive) const;
	float GetRoundRemainingTime() const;
	bool IsFreezePeriod() const;

	GameMode* StartGameMode(const char *pszMode);

	void** GetAddress() const { return m_pAddress; }

private:
	GameMode *m_pCurrentMode; 
	void** m_pAddress;
	bool m_NeededPlayers;
	bool m_IsActivated;
	bool m_IsGameModeStarted;
	//float m_RoundStartTime;
};

extern GameRules g_GameRules;