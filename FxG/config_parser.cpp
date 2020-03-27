// vim: set ts=4 sw=4 tw=99 noet:
//
// AMX Mod X, based on AMX Mod by Aleksander Naszko ("OLO").
// Copyright (C) The AMX Mod X Development Team.
//
// This software is licensed under the GNU General Public License, version 3 or higher.
// Additional exceptions apply. For full license details, see LICENSE.txt or visit:
//     https://alliedmods.net/amxmodx-license

//
// Ham Sandwich Module
//

#include "amxxmodule.h"
#include "Hacks.h"
#include "ham_const.h"
#include "hooklist.h"
#include "offsets.h"
#include "GameRules.h"
#include <amtl/am-platform.h>

IGameConfig* g_pCommonConfig;
IGameConfig* g_pMainConfig;
IGameConfigManager* g_pConfigManager;

int ReadConfig(void)
{
	SERVER_PRINT("read config\n");

	g_pConfigManager = MF_GetConfigManager();

	char error[256] = "";

	if (!g_pConfigManager->LoadGameConfigFile("modules.games", &g_pMainConfig, error, sizeof(error)) && *error)
	{
		MF_Log("Could not read module.games gamedata: %s", error);
		return -1;
	}

	InitializeHacks();

	if (!g_pConfigManager->LoadGameConfigFile("common.games", &g_pCommonConfig, error, sizeof error))
	{
		MF_Log("common.games gamedata could not be read: %s", error);
		return -1;
	}

	TypeDescription value;

	if (g_pCommonConfig->GetOffset("pev", &value))
	{
		Offsets.SetPev(value.fieldOffset);
	}

	if (g_pCommonConfig->GetOffset("base", &value))
	{
		Offsets.SetBase(value.fieldOffset);
	}

	void* address = nullptr;

	if (!g_pCommonConfig->GetAddress("g_pGameRules", &address) || !address)
	{
		MF_Log("g_pGameRules address could not be found.");
		return -1;
	}

#if defined(KE_WINDOWS)
	g_GameRules.Init(*reinterpret_cast<void***>(address));
#else
	g_GameRules.Init(reinterpret_cast<void**>(address));
#endif

	for (auto index = 0; index < HAM_LAST_ENTRY_DONT_USE_ME_LOL; ++index)
	{
		if (g_pCommonConfig->GetOffset(hooklist[index].name, &value))
		{
			hooklist[index].isset = 1;
			hooklist[index].vtid = value.fieldOffset;

			//SERVER_PRINT(UTIL_VarArgs("vtid = %d,", hooklist[index].vtid));
		}
	}

	return 1;
}

void CloseConfigFiles()
{
	g_pConfigManager->CloseGameConfigFile(g_pMainConfig);
	g_pConfigManager->CloseGameConfigFile(g_pCommonConfig);

	ShutdownHacks();
}