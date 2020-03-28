#pragma once

#include "HLTypeConversion.h"

extern HLTypeConversion g_TypeConversion;

#define DMG_GENERIC		0		// generic damage was done
#define DMG_CRUSH		(1<<0)		// crushed by falling or moving object
#define DMG_BULLET		(1<<1)		// shot
#define DMG_SLASH		(1<<2)		// cut, clawed, stabbed
#define DMG_BURN		(1<<3)		// heat burned
#define DMG_FREEZE		(1<<4)		// frozen
#define DMG_FALL		(1<<5)		// fell too far
#define DMG_BLAST		(1<<6)		// explosive blast damage
#define DMG_CLUB		(1<<7)		// crowbar, punch, headbutt
#define DMG_SHOCK		(1<<8)		// electric shock
#define DMG_SONIC		(1<<9)		// sound pulse shockwave
#define DMG_ENERGYBEAM		(1<<10)		// laser or other high energy beam
#define DMG_NEVERGIB		(1<<12)		// with this bit OR'd in, no damage type will be able to gib victims upon death
#define DMG_ALWAYSGIB		(1<<13)		// with this bit OR'd in, any damage type can be made to gib victims upon death
#define DMG_DROWN		(1<<14)		// Drowning

// time-based damage
#define DMG_TIMEBASED		(~(0x3FFF))	// mask for time-based damage

#define DMG_PARALYZE		(1<<15)		// slows affected creature down
#define DMG_NERVEGAS		(1<<16)		// nerve toxins, very bad
#define DMG_POISON		(1<<17)		// blood poisioning
#define DMG_RADIATION		(1<<18)		// radiation exposure
#define DMG_DROWNRECOVER	(1<<19)		// drowning recovery
#define DMG_ACID		(1<<20)		// toxic chemicals or acid burns
#define DMG_SLOWBURN		(1<<21)		// in an oven
#define DMG_SLOWFREEZE		(1<<22)		// in a subzero freezer
#define DMG_MORTAR		(1<<23)		// Hit by air raid (done to distinguish grenade from mortar)
#define DMG_EXPLOSION		(1<<24)

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

void UTIL_Damage(edict_t *pEnt, int damageSave, int damageTake, int damageType, const Vector &coord);

//void UTIL_LogPrintf(const char* fmt, ...);