#pragma once

class WrappedEntity;
class ModifiableWrappedEntity;
class Player;

class PlayerClass
{
public:
	PlayerClass(Player* pPlayer)
	{
		m_pPlayer = pPlayer;
	}

	virtual ~PlayerClass() {}

	Player* GetPlayer() const { return m_pPlayer; }

	virtual bool IsZombie() const { return false; }
	virtual bool IsBoss() const { return false; }
	virtual bool IsSubBoss() const { return false; }

	virtual bool IsLeader() const { return false; }

	virtual void Become() {}
	virtual void SetMaxspeed() {}

	virtual void OnSpawn() {}
	virtual void OnThink() {}
	virtual int OnTakeDamage(ModifiableWrappedEntity* pInflictor, ModifiableWrappedEntity* pAttacker, float& damage, int& damageType) { return 0; }
	virtual void OnKilled(ModifiableWrappedEntity* pKiller, int& shouldgib) {}

protected:
	Player* m_pPlayer;
};


#define DEFINE_PLAYER_CLASS(player_class) \
	player_class(Player *pPlayer) : m_pPlayer(pPlayer) {}

#define DECLARE_PLAYER_CLASS(player_class) \
	static __PlayerClassHelper<player_class> __s_PlayerClassHelper(#player_class);

class BasePlayerClassHelper
{
public:
	BasePlayerClassHelper(const char* pszName);

	virtual ~BasePlayerClassHelper() = default;
	virtual PlayerClass* Instantiate(Player* pPlayer) = 0;
};

template<class T>
class __PlayerClassHelper : public BasePlayerClassHelper
{
public:
	__PlayerClassHelper<T>(const char* pszName) : BasePlayerClassHelper(pszName) {}

	virtual PlayerClass* Instantiate(Player* pPlayer) override
	{
		return new T(pPlayer);
	}
};

BasePlayerClassHelper* GetPlayerClassHelperByName(const char* pszName);