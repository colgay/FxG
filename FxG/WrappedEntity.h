#pragma once

#include "PlayerHandler.h"

//extern PlayerHandler* g_pPlayers;

class WrappedEntity
{
public:
	WrappedEntity(edict_t* pEdict) : m_pEdict(pEdict)
	{
	}

	bool IsPlayer() const { return ToPlayer() != nullptr; }

	Player* ToPlayer() { return GetPlayerHandler()->GetPlayer(m_pEdict); }
	const Player* ToPlayer() const { return GetPlayerHandler()->GetPlayer(m_pEdict); }

	entvars_t* GetPev() { return &m_pEdict->v; }
	const entvars_t* GetPev() const { return &m_pEdict->v; }

	edict_t* GetEdict() { return m_pEdict; }
	const edict_t* GetEdict() const { return m_pEdict; }

private:
	edict_t* m_pEdict;
};