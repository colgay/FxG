#pragma once

#include <string>

class GameMode
{
public:
	GameMode(const char *pszName) : m_name(pszName)
	{}

	virtual ~GameMode() {}

	virtual void OnRoundStart() {}
	virtual void OnRoundEnd() {}

protected:
	std::string m_name;
};