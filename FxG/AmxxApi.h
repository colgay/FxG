#pragma once

enum ForwardState
{
	FSTATE_ACTIVE,
	FSTATE_STOP
};

void RegisterForwards();
void RegisterNatives();

void CS_SetPlayerModel(edict_t* pPlayer, const char* pszModelName, bool updateIndex);
void CS_ResetPlayerModel(edict_t* pPlayer);