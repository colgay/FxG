#pragma once

#include "HLTypeConversion.h"

extern HLTypeConversion g_TypeConversion;

#define DMG_BURN (1 << 3)	// heat burned
#define	SF_NORESPAWN (1 << 30)

void UTIL_SendAudio(edict_t* pEnt, int sender, const char* pszAudio, int pitch);

void UTIL_IntToString(int value, char* output);

int UTIL_ReadFlags(const char* c);

char* UTIL_VarArgs(const char* format, ...);

void UTIL_StripUserWeapons(edict_t* Player);

void UTIL_GiveItem(edict_t* pPlayer, const char* pszItemName);

void UTIL_ClientPrintAll(int msg_dest, const char* msg_name, const char* param1, const char* param2, const char* param3, const char* param4);

void UTIL_HudMessage(edict_t* pEntity, const hudtextparms_t& textparms, const char* pMessage);

void UTIL_HudMessageAll(const hudtextparms_t& textparms, const char* pMessage);

edict_t* UTIL_FindEntityByString(edict_t* pStartEntity, const char* szKeyword, const char* szValue);

edict_t* UTIL_FindEntityByClassname(edict_t* pStartEntity, const char* szName);

//void UTIL_LogPrintf(const char* fmt, ...);