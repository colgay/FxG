#pragma once

#include <stack>
#include <vector>
#include <any>
#include "ham_const.h"

extern std::stack<META_RES> g_ReturnState;

#define _FUNC_BEGIN(res) \
	g_ReturnState.push(res);

#define _FUNC_BEGIN_V(type, value, res) \
	type __ret = value, __origret = value, __tmpret = value; \
	g_ReturnState.push(res);

#define _FUNC_STATE \
	g_ReturnState.top()

#define _FUNC_ORIG_RET \
	 __origret

#define _RETURN_STATE(state) \
	if (state > _FUNC_STATE) {	\
		_FUNC_STATE = state;		\
		return;						\
	}

#define _RETURN_STATE_V(state, value) \
	if (state > _FUNC_STATE) { \
		_FUNC_STATE = state;	  \
		return value;			  \
	}

#define _CALL_FUNC(func) \
	__tmpret = func; \
	if (_FUNC_STATE >= MRES_OVERRIDE) { \
		__ret = __tmpret; \
	}

#define _CHECK_RESULT() \
	if (_FUNC_STATE < MRES_OVERRIDE) { \
		__ret = __origret;	\
	}

#define _RETURN_FUNC_END() \
	g_ReturnState.pop(); \
	return;

#define _RETURN_FUNC_END_V() \
	_CHECK_RESULT(); \
	g_ReturnState.pop(); \
	return __ret;

#define _RETURN_META_END() \
	META_RES __fuckyouon9jai__ = _FUNC_STATE; \
	g_ReturnState.pop(); \
	RETURN_META(__fuckyouon9jai__);

#define _RETURN_META_END_V() \
	META_RES __fuckyouon9jai__ = _FUNC_STATE; \
	_CHECK_RESULT(); \
	g_ReturnState.pop(); \
	RETURN_META_VALUE(__fuckyouon9jai__, __ret);
	
#define _FUNC_CHECK_STATE() if (_FUNC_STATE >= MRES_SUPERCEDE)
	
enum ForwardState
{
	FSTATE_ACTIVE,
	FSTATE_STOP
};

void RegisterForwards();
void RegisterNatives();

void CS_SetPlayerModel(edict_t* pPlayer, const char* pszModelName, bool updateIndex);
void CS_ResetPlayerModel(edict_t* pPlayer);