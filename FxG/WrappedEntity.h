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

	bool operator==(const WrappedEntity& rhs) const
	{
		return this->IsEqualTo(&rhs);
	}

	bool IsEqualTo(const WrappedEntity* pRhs) const
	{
		return (this->GetPev() == pRhs->GetPev());
	}

protected:
	edict_t* m_pEdict;
};

class ModifiableWrappedEntity : public WrappedEntity
{
public:
	ModifiableWrappedEntity(edict_t* pEdict) : WrappedEntity(pEdict) {}

	void SetEdict(edict_t* pEdict) { m_pEdict = pEdict; }
	//void SetPev(entvars_t* pev) { m_pEdict = g_TypeConversion.entvar_to_edict(pev); }
};