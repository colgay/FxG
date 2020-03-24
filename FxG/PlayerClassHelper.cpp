#include "PlayerClass.h"

#include "meta_api.h"
#include "Utilities.h"

#include <vector>
#include <string>
#include <unordered_map>

static std::unordered_map<std::string, BasePlayerClassHelper*> s_PlayerClassHelpers;
extern std::vector<std::pair<std::string, BasePlayerClassHelper*>> g_PlayerClassVector;

BasePlayerClassHelper* GetPlayerClassHelperByName(const char* pszName)
{
    static bool initialized = false;

    if (!initialized)
    {
        for (size_t i = 0; i < g_PlayerClassVector.size(); i++)
        {
            s_PlayerClassHelpers[g_PlayerClassVector[i].first] = g_PlayerClassVector[i].second;
        }

        initialized = true;
        SERVER_PRINT(UTIL_VarArgs("initialized with size (%d).\n", g_PlayerClassVector.size()));
    }

    auto it = s_PlayerClassHelpers.find(pszName);
    if (it == s_PlayerClassHelpers.end())
        return nullptr;

    return it->second;
}

BasePlayerClassHelper::BasePlayerClassHelper(const char *pszName)
{
    g_PlayerClassVector.push_back(std::make_pair(pszName, this));
}