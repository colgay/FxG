#include "Player.h"
#include "PlayerHandler.h"
#include "Utilities.h"

static PlayerHandler s_PlayerHandler;

PlayerHandler *GetPlayerHandler()
{
	static bool initialized = false;
	if (!initialized)
	{
		s_PlayerHandler.Init(gpGlobals->maxClients);
		initialized = true;
	}

	return &s_PlayerHandler;
}

PlayerHandler::~PlayerHandler()
{
	Release();
}

void PlayerHandler::Init(int maxplayers)
{
	if (maxplayers > MAX_PLAYERS)
		maxplayers = MAX_PLAYERS;

	for (m_maxplayers = 0; m_maxplayers < maxplayers; m_maxplayers++)
	{
		m_pPlayers[m_maxplayers] = new Player();
		m_pPlayers[m_maxplayers]->SetIndex(m_maxplayers + 1);
	}

	m_maxplayers--;
}

void PlayerHandler::Release()
{
	for (; m_maxplayers >= 0; --m_maxplayers)
	{
		delete m_pPlayers[m_maxplayers];
	}
}

void PlayerHandler::PutInServer(edict_t* pEntity)
{
	int index = g_TypeConversion.edict_to_id(pEntity);
	if (IsPlayer(index))
	{
		m_pPlayers[index - 1]->PutInServer(index);
		SERVER_PRINT("Player Found PutInServer\n");
	}
}

void PlayerHandler::Disconnect(edict_t* pEntity)
{
	int index = g_TypeConversion.edict_to_id(pEntity);
	if (IsPlayer(index))
	{
		m_pPlayers[index - 1]->Disconnect();
	}
}

Player* PlayerHandler::GetPlayer(edict_t* pEnt)
{
	int index = g_TypeConversion.edict_to_id(pEnt);
	return GetPlayer(index);
}

Player* PlayerHandler::GetPlayer(int index)
{
	if (IsPlayer(index))
		return m_pPlayers[index - 1];

	return nullptr;
}