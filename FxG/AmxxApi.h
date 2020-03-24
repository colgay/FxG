#pragma once

#include "extdll.h"

void RegisterForwards();

void CS_SetPlayerModel(edict_t* pPlayer, const char* pszModelName, bool updateIndex);
void CS_ResetPlayerModel(edict_t* pPlayer);