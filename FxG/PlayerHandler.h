#pragma once

#ifndef MAX_PLAYERS
#define MAX_PLAYERS 32
#endif

class Player;

class PlayerHandler
{
public:
	PlayerHandler() : m_maxplayers(0), m_pPlayers{ 0 } { /* or... ::memset(&m_pPlayers, 0, sizeof(m_pPlayers)) */ }
	~PlayerHandler();

	void Init(int maxplayers);
	void Release();

	void PutInServer(edict_t* pEntity);
	void Disconnect(edict_t* pEntity);

	bool IsPlayer(int index)
	{
		if (index >= 1 && index <= m_maxplayers)
			return true;

		return false;
	}

	template<typename T>
	bool HasPlayer(T key) { return GetPlayer(key) != nullptr; }
	Player* GetPlayer(edict_t* pEnt);
	Player* GetPlayer(int index);

private:
	Player* m_pPlayers[MAX_PLAYERS];
	int m_maxplayers;
};

PlayerHandler *GetPlayerHandler();