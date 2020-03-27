//#include "amxxmodule.h"
#include "Player.h"

#include <vector>
#include <string>
#include <map>

//#include "Utilities.h"

static std::map<std::string, BasePlayerClassHelper*> &PlayerClassMap()
{
    static std::map<std::string, BasePlayerClassHelper*> m;
    return m;
}

BasePlayerClassHelper* GetPlayerClassHelperByName(const char* pszName)
{
    /*
    static bool initialized = false;

    if (!initialized)
    {
        for (size_t i = 0; i < g_PlayerClassVector.size(); i++)
        {
            s_PlayerClassHelpers[g_PlayerClassVector[i].first] = g_PlayerClassVector[i].second;
        }

        initialized = true;
        MF_Log(UTIL_VarArgs("initialized PlayerClass with size (%d).\n", g_PlayerClassVector.size()));
        //g_PlayerClassVector.clear();
    }*/

    auto it = PlayerClassMap().find(pszName);
    if (it == PlayerClassMap().end())
        return nullptr;

    return it->second;
}

BasePlayerClassHelper::BasePlayerClassHelper(const char *pszName)
{
    PlayerClassMap().insert_or_assign(pszName, this);
}