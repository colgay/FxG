#pragma once

#include "extdll.h"
#include "PlayerClass.h"

class Player
{
public:
	Player() : m_index(0), m_pEdict(NULL), m_pPlayerClass(nullptr)
	{}

	~Player();

	void PutInServer(int index);
	bool IsAlive() const;
	void Disconnect();

	int GetCurrentWeapon() const;

	int GetIndex() const { return m_index; }
	edict_t* GetEdict() const { return m_pEdict; }

	void SetIndex(int index) { m_index = index; }
	void SetEdict(edict_t* pEdict) { m_pEdict = pEdict; }

	float GetHealth() const { return GetPev()->health; }
	void SetHealth(float health) { GetPev()->health = health; }

	float GetMaxHealth() const { return GetPev()->max_health; }
	void SetMaxHealth(float health) { GetPev()->max_health = health; }

	float GetArmorValue() const { return GetPev()->armorvalue; }
	void SetArmorValue(float value) { GetPev()->armorvalue = value; }

	float GetGravity() const { return GetPev()->gravity; }
	void SetGravity(float gravity) { GetPev()->gravity = gravity; }

	float GetMaxSpeed() const { return GetPev()->maxspeed; }
	void SetMaxSpeed(float maxspeed) { GetPev()->maxspeed = maxspeed; }

	int GetTeam() const;
	void ChangeTeam(int team, int model, bool send_teaminfo = true);

	void SetModel(const char* model, bool updateIndex);
	void ResetModel();

	void ResetMaxspeed();
	
	bool HasClass() const { return (GetClass() != nullptr); }
	PlayerClass* GetClass() const { return m_pPlayerClass; }
	PlayerClass* ChangeClass(const char* pszClassName);

private:
	entvars_t* GetPev() { return &m_pEdict->v; }
	const entvars_t* GetPev() const { return &m_pEdict->v; }

	edict_t* m_pEdict;
	int m_index;

	PlayerClass* m_pPlayerClass;
};