#include "Utilities.h"
#include "extdll.h"
#include "meta_api.h"

HLTypeConversion g_TypeConversion;

void UTIL_HudMessage(edict_t* pEntity, const hudtextparms_t& textparms, const char* pMessage)
{
	if (!pEntity)
		return;

	MESSAGE_BEGIN(MSG_ONE, SVC_TEMPENTITY, NULL, pEntity);
	WRITE_BYTE(TE_TEXTMESSAGE);
	WRITE_BYTE(textparms.channel & 0xFF);

	WRITE_SHORT(FixedSigned16(textparms.x, 1 << 13));
	WRITE_SHORT(FixedSigned16(textparms.y, 1 << 13));
	WRITE_BYTE(textparms.effect);

	WRITE_BYTE(textparms.r1);
	WRITE_BYTE(textparms.g1);
	WRITE_BYTE(textparms.b1);
	WRITE_BYTE(textparms.a1);

	WRITE_BYTE(textparms.r2);
	WRITE_BYTE(textparms.g2);
	WRITE_BYTE(textparms.b2);
	WRITE_BYTE(textparms.a2);

	WRITE_SHORT(FixedUnsigned16(textparms.fadeinTime, 1 << 8));
	WRITE_SHORT(FixedUnsigned16(textparms.fadeoutTime, 1 << 8));
	WRITE_SHORT(FixedUnsigned16(textparms.holdTime, 1 << 8));

	if (textparms.effect == 2)
		WRITE_SHORT(FixedUnsigned16(textparms.fxTime, 1 << 8));

	if (strlen(pMessage) < 512)
	{
		WRITE_STRING(pMessage);
	}
	else
	{
		char tmp[512];
		strncpy(tmp, pMessage, 511);
		tmp[511] = 0;
		WRITE_STRING(tmp);
	}
	MESSAGE_END();
}

void UTIL_HudMessageAll(const hudtextparms_t& textparms, const char* pMessage)
{
	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		edict_t* pPlayer = INDEXENT(i);
		if (pPlayer)
			UTIL_HudMessage(pPlayer, textparms, pMessage);
	}
}

void UTIL_IntToString(int value, char* output)
{
	static const char* words[] =
	{ "zero ","one ","two ","three ","four ",
	"five ", "six ","seven ","eight ","nine ","ten ",
	"eleven ","twelve ","thirteen ","fourteen ","fifteen ",
	"sixteen ","seventeen ","eighteen ","nineteen ",
	"twenty ","thirty ","fourty ", "fifty ","sixty ",
	"seventy ","eighty ","ninety ",
	"hundred ","thousand " };

	*output = 0;
	if (value < 0) value = -value;
	int tho = value / 1000;
	int aaa = 0;

	if (tho)
	{
		aaa += sprintf(&output[aaa], "%s", words[tho]);
		aaa += sprintf(&output[aaa], "%s", words[29]);
		value = value % 1000;
	}

	int hun = value / 100;

	if (hun)
	{
		aaa += sprintf(&output[aaa], "%s", words[hun]);
		aaa += sprintf(&output[aaa], "%s", words[28]);
		value = value % 100;
	}

	int ten = value / 10;
	int unit = value % 10;

	if (ten)
		aaa += sprintf(&output[aaa], "%s", words[(ten > 1) ? (ten + 18) : (unit + 10)]);

	if (ten != 1 && (unit || (!value && !hun && !tho)))
		sprintf(&output[aaa], "%s", words[unit]);
}

int UTIL_ReadFlags(const char* c)
{
	int flags = 0;

	while (*c)
		flags |= (1 << (*c++ - 'a'));

	return flags;
}

char* UTIL_VarArgs(const char* format, ...)
{
	va_list argptr;
	static char string[1024];

	va_start(argptr, format);
	vsprintf(string, format, argptr);
	va_end(argptr);

	return string;
}

void UTIL_StripUserWeapons(edict_t* pPlayer)
{
	const auto pEntity = CREATE_NAMED_ENTITY(MAKE_STRING("player_weaponstrip"));
	
	if (FNullEnt(pEntity))
	{
		return;
	}

	MDLL_Spawn(pEntity);
	MDLL_Use(pEntity, pPlayer);
	REMOVE_ENTITY(pEntity);
}

void UTIL_GiveItem(edict_t* pPlayer, const char* pszItemName)
{
	auto pEntity = CREATE_NAMED_ENTITY(ALLOC_STRING(pszItemName));

	if (FNullEnt(pEntity))
		return;

	pEntity->v.origin = pPlayer->v.origin;
	pEntity->v.spawnflags |= SF_NORESPAWN;

	MDLL_Spawn(pEntity);

	const auto oldSolid = pEntity->v.solid;

	MDLL_Touch(pEntity, pPlayer);

	if (pEntity->v.solid == oldSolid)
	{
		REMOVE_ENTITY(pEntity); // The function did not fail - we're just deleting the item
	}
}

void UTIL_ClientPrintAll(int msg_dest, const char* msg_name, const char* param1, const char* param2, const char* param3, const char* param4)
{
	static int msgTextMsg = 0;
	if (msgTextMsg == 0)
		msgTextMsg = GET_USER_MSG_ID(PLID, "TextMsg", nullptr);

	MESSAGE_BEGIN(MSG_ALL, msgTextMsg);
	WRITE_BYTE(msg_dest);
	WRITE_STRING(msg_name);

	if (param1)
		WRITE_STRING(param1);
	if (param2)
		WRITE_STRING(param2);
	if (param3)
		WRITE_STRING(param3);
	if (param4)
		WRITE_STRING(param4);

	MESSAGE_END();
}


edict_t* UTIL_FindEntityByString(edict_t* pStartEntity, const char* szKeyword, const char* szValue)
{
	edict_t* pentEntity;

	if (pStartEntity)
		pentEntity = pStartEntity;
	else
		pentEntity = NULL;

	pentEntity = FIND_ENTITY_BY_STRING(pentEntity, szKeyword, szValue);

	if (!FNullEnt(pentEntity))
		return pentEntity;

	return NULL;
}

void UTIL_SendAudio(edict_t* pEnt, int sender, const char* pszAudio, int pitch)
{
	static int msgSendAudio = 0;
	if (msgSendAudio == 0)
		msgSendAudio = GET_USER_MSG_ID(PLID, "SendAudio", 0);

	MESSAGE_BEGIN((pEnt == nullptr) ? MSG_BROADCAST : MSG_ONE_UNRELIABLE, msgSendAudio, nullptr, pEnt);
	WRITE_BYTE(sender);
	WRITE_STRING(pszAudio);
	WRITE_SHORT(pitch);
	MESSAGE_END();
}

edict_t* UTIL_FindEntityByClassname(edict_t* pStartEntity, const char* szName)
{
	return UTIL_FindEntityByString(pStartEntity, "classname", szName);
}
/*
void UTIL_LogPrintf(const char* fmt, ...)
{
	va_list			argptr;
	static char		string[1024];

	va_start(argptr, fmt);
	vsprintf(string, fmt, argptr);
	va_end(argptr);

	// Print to server console
	ALERT(at_logged, "%s", string);
}*/