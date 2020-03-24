#include "amxxmodule.h"
#include "HLTypeConversion.h"

extern HLTypeConversion g_TypeConversion;

int g_fwd_SetPlayerModel;
int g_fwd_ResetPlayerModel;

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