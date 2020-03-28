#include "amxxmodule.h"
#include "AmxxApi.h"

#include "Utilities.h"

#include "Player.h"
#include "PlayerHandler.h"

int g_fwd_SetPlayerModel;
int g_fwd_ResetPlayerModel;

AMX_NATIVE_INFO ModuleNatives[];

std::stack<int> g_ReturnState;
std::stack<std::vector<std::any>> g_Parameters;

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
	if (pPlayer->HasClass())
	{
		MF_LogError(amx, AMX_ERR_NATIVE, "player (%d) has no class", playerIndex);
		return 0;
	}

	return static_cast<cell>(pPlayer->GetClass()->IsZombie());
}

AMX_NATIVE_INFO ModuleNatives[] =
{
	{"tig_is_player_zombie", tig_is_player_zombie},
	{nullptr, nullptr}
};