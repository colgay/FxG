#pragma once

#include "extdll.h"
#include <vector>
#include "PlayerClass.h"

class Player
{
public:
	Player() : m_index(0), m_pEdict(NULL), m_pPlayerClass(nullptr), m_bIsConnected(false)
	{}

	~Player();

	void PutInServer(int index);
	bool IsAlive() const;
	void Disconnect();

	bool IsConnected(void) const { return m_bIsConnected; }

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

	int GetButton() const { return GetPev()->button; }
	void SetButton(int button) { GetPev()->button = button; }

	int GetOldButton() const { return GetPev()->oldbuttons; }
	void SetOldButton(int button) { GetPev()->oldbuttons = button; }

	int GetFlags() const { return GetPev()->flags; }
	void SetFlags(int flags) { GetPev()->flags = flags; }

	Vector GetVelocity() const { return GetPev()->velocity; }
	void SetVelocity(const Vector& vel) { GetPev()->velocity = vel; }

	Vector GetViewAngle() const { return GetPev()->v_angle; }

	Vector GetOrigin() const { return GetPev()->origin; }

	int GetTeam() const;
	void ChangeTeam(int team, int model, bool send_teaminfo = true);

	void SetModel(const char* model, bool updateIndex);
	void ResetModel();

	void ResetMaxspeed();
	
	bool HasClass() const { return (GetClass() != nullptr); }
	PlayerClass* GetClass() const { return m_pPlayerClass; }
	PlayerClass* ChangeClass(const char* pszClassName);

	void CleanRecyleBinOfClasses();

private:
	entvars_t* GetPev() { return &m_pEdict->v; }
	const entvars_t* GetPev() const { return &m_pEdict->v; }

	edict_t* m_pEdict;
	int m_index;

	PlayerClass* m_pPlayerClass;

	bool m_bIsConnected;

	std::vector<PlayerClass*> m_RecycleBinOfClasses;
};