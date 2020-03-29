#include "amxxmodule.h"
#include "AmxxApi.h"

#include "Utilities.h"

#include "Player.h"
#include "PlayerHandler.h"
#include "Zombie.h"

int g_fwd_SetPlayerModel;
int g_fwd_ResetPlayerModel;

AMX_NATIVE_INFO ModuleNatives[];

std::stack<META_RES> g_ReturnState;

void RegisterForwards()
{
	const char *path = MF_GetLocalInfo("amxx_pluginsdir", "addons/amxmodx/plugins");

	int script = MF_FindScriptByName(MF_BuildPathname("%s/cppsupport.amxx", path));
	AMX* pAmx = MF_GetScriptAmx(script);

	g_fwd_SetPlayerModel = MF_RegisterSPForwardByName(pAmx, "cpp_SetPlayerModel", FP_CELL, FP_STRING, FP_CELL);
	g_fwd_ResetPlayerModel = MF_RegisterSPForwardByName(pAmx, "cpp_ResetPlayerModel", FP_CELL);
}

void CS_SetPlayerModel(edict_t* pPlayer, const char* pszModelName, bool updateIndex)
{
	cell index = g_TypeConversion.edict_to_id(pPlayer);
	MF_ExecuteForward(g_fwd_SetPlayerModel, index, pszModelName, updateIndex);
}

void CS_ResetPlayerModel(edict_t* pPlayer)
{
	cell index = g_TypeConversion.edict_to_id(pPlayer);

	MF_ExecuteForward(g_fwd_ResetPlayerModel, index);
}

void RegisterNatives()
{
	MF_AddNatives(ModuleNatives);
}

static cell AMX_NATIVE_CALL tig_is_player_zombie(AMX* amx, cell* params)
{
	int playerIndex = params[1];

	if (!GetPlayerHandler()->HasPlayer(playerIndex))
	{
		MF_LogError(amx, AMX_ERR_NATIVE, "player id (%d) is invalid", playerIndex);
		return 0;
	}

	Player* pPlayer = GetPlayerHandler()->GetPlayer(playerIndex);
	if (!pPlayer->HasClass())
	{
		MF_LogError(amx, AMX_ERR_NATIVE, "player (%d) has no class", playerIndex);
		return 0;
	}

	return static_cast<cell>(pPlayer->GetClass()->IsZombie());
}

static cell AMX_NATIVE_CALL tig_get_zombie_class_attr(AMX* amx, cell* params)
{
	int index = params[1];

	if (index < 0 || index >= (int)GetZombieClassVector().size())
	{
		MF_LogError(amx, AMX_ERR_NATIVE, "zombie class id (%d) is invalid", index);
		return 0;
	}

	const std::pair<std::string, ZombieAttributes> &pair = GetZombieClassVector().at(index);

	switch (params[2])
	{
		case 0: // name
		{
			MF_SetAmxString(amx, params[3], pair.second.name.c_str(), params[4]);
			break;
		}
		case 1: // desc
		{
			MF_SetAmxString(amx, params[3], pair.second.desc.c_str(), params[4]);
			break;
		}
		case 2: // hp
		{
			cell* byRef = MF_GetAmxAddr(amx, params[3]);
			*byRef = amx_ftoc(pair.second.health);
			break;
		}
		case 3: // speed
		{
			cell* byRef = MF_GetAmxAddr(amx, params[3]);
			*byRef = amx_ftoc(pair.second.maxspeed);
			break;
		}
		case 4: // gravity
		{
			cell* byRef = MF_GetAmxAddr(amx, params[3]);
			*byRef = amx_ftoc(pair.second.gravity);
			break;
		}
		case 5: // knockback
		{
			cell* byRef = MF_GetAmxAddr(amx, params[3]);
			*byRef = amx_ftoc(pair.second.knockback_scalar);
			break;
		}
	}
	
	return 1;
}

static cell AMX_NATIVE_CALL tig_set_next_zombie_class(AMX* amx, cell* params)
{
	int player = params[1];

	if (!GetPlayerHandler()->HasPlayer(player))
	{
		MF_LogError(amx, AMX_ERR_NATIVE, "player id (%d) is invalid", player);
		return 0;
	}

	int index = params[2];

	if (index < 0 || index >= (int)GetZombieClassVector().size())
	{
		MF_LogError(amx, AMX_ERR_NATIVE, "zombie class id (%d) is invalid", index);
		return 0;
	}

	GetPlayerHandler()->GetPlayer(player)->SetNextZombieClass(index);
	return 1;
}

static cell AMX_NATIVE_CALL tig_get_next_zombie_class(AMX* amx, cell* params)
{
	int player = params[1];

	if (!GetPlayerHandler()->HasPlayer(player))
	{
		MF_LogError(amx, AMX_ERR_NATIVE, "player id (%d) is invalid", player);
		return 0;
	}

	return GetPlayerHandler()->GetPlayer(player)->GetNextZombieClass();
}

static cell AMX_NATIVE_CALL tig_get_zombie_class_num(AMX* amx, cell* params)
{
	return GetZombieClassVector().size();
}

AMX_NATIVE_INFO ModuleNatives[] =
{
	{"tig_is_player_zombie", tig_is_player_zombie},
	{"tig_get_zombie_class_attr", tig_get_zombie_class_attr},
	{"tig_get_zombie_class_num", tig_get_zombie_class_num},
	{"tig_set_next_zombie_class", tig_set_next_zombie_class},
	{"tig_get_next_zombie_class", tig_get_next_zombie_class},
	{nullptr, nullptr}
};