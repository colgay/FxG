#include "GameMode.h"

//#include "amxxmodule.h"

#include <vector>
#include <string>
#include <map>

static std::map<std::string, BaseGameModeHelper*> &GameModeMap()
{
    static std::map<std::string, BaseGameModeHelper*> m;
    return m;
}

BaseGameModeHelper* GetGameModeHelperByName(const char* pszName)
{
    /*static bool initialized = false;

    if (!initialized)
    {
        for (size_t i = 0; i < g_GameModeVector.size(); i++)
        {
            s_GameModeHelpers[g_GameModeVector[i].first] = g_GameModeVector[i].second;
        }

        initialized = true;
        MF_Log(UTIL_VarArgs("initialized GameMode with size (%d).\n", g_GameModeVector.size()));
        
        //g_GameModeVector.clear();
    }*/

    auto it = GameModeMap().find(pszName);
    if (it == GameModeMap().end())
        return nullptr;

    return it->second;
}

BaseGameModeHelper::BaseGameModeHelper(const char* pszName)
{
    //g_GameModeVector.push_back(std::make_pair(pszName, this)); // mo push_back, size is 0
    GameModeMap().insert_or_assign(pszName, this);
}