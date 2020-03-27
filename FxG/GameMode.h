#pragma once

class GameMode
{
public:
	GameMode() {}

	virtual ~GameMode() {}

	virtual void StartGameMode() {}
	virtual void OnRoundEnd() {}
	virtual void OnCheckWinConditions() {}
};

#define DECLARE_GAME_MODE(game_mode) \
	static __GameModeHelper<game_mode> __s_GameModeHelper(#game_mode);

class BaseGameModeHelper
{
public:
	BaseGameModeHelper(const char* pszName);

	virtual ~BaseGameModeHelper() = default;
	virtual GameMode* Instantiate() = 0;
};

template<class T>
class __GameModeHelper : public BaseGameModeHelper
{
public:
	__GameModeHelper<T>(const char* pszName) : BaseGameModeHelper(pszName) {}

	virtual GameMode* Instantiate() override
	{
		return new T();
	}
};

BaseGameModeHelper* GetGameModeHelperByName(const char* pszName);