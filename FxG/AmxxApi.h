#pragma once

#include <stack>
#include <vector>
#include <any>
#include "ham_const.h"

extern std::stack<int> g_ReturnState;

// colgate

#if 0

inline void __FUNC_BEGIN()
{
	g_ReturnState.push(HAM_IGNORED);
}

inline int __RESULT() { return g_ReturnState.top(); }

inline void __SET_HAM_STATE(int state)
{
	if (state > __RESULT())
	{
		__RESULT() = state;
		//return;
	}
}

/*inline std::any __RETURN_VALUE(int state, std::any &ret)
{
	if (state > std::any_cast<int>(__RETURN_RESULT))
	{
		__RETURN_RESULT() = state;
		return ret;
	}
}*/

inline void __CALL_FUNC(func)
{
	if (std::any_cast<int>(__RETURN_RESULT) >= HAM_OVERRIDE) {
		\
			ret = func;								\
	}
	else {
		\
			func;									\
	}
}

#define __POP() \
	g_Parameters.pop();

#define __CHECK_SUPERCEDE()		\
	if (std::any_cast<int>(__RETURN_RESULT) >= HAM_SUPERCEDE) { \
		__POP();								\
		return;									\
	}

#define __CHECK_SUPERCEDE_VALUE()		\
	if (std::any_cast<int>(__RETURN_RESULT) >= HAM_SUPERCEDE) { \
		__POP();								\
		return ret;								\
	}

#define __END_FUNC()	\
	__POP();			\
	return;

#define __END_FUNC_VALUE(ret, origret)	\
	if (std::any_cast<int>(__RETURN_RESULT) >= HAM_OVERRIDE) {	\
		__POP();								\
		return ret;								\
	} else {									\
		__POP();								\
		return origret;							\
	}

// original

#else

#define _HAM_FUNC_BEGIN() \
	g_ReturnState.push(HAM_UNSET); \

#define _HAM_GET_STATE() \
	g_ReturnState.top()

#define _RETURN_HAM(state) \
	if (state > _HAM_GET_STATE()) { \
		_HAM_GET_STATE() = state;	\
		return;	\
	}

#define _RETURN_HAM_V(state) \
	if (state > _HAM_GET_STATE()) { \
		_HAM_GET_STATE() = state;	\
		return ret\
	}

#define _HAM_CALL_FUNC(func) \
	if (_HAM_GET_STATE() >= HAM_OVERRIDE) {	\
		ret = func;								\
	} else { \
		func; \
	}

#define _HAM_CHECK()		\
	if (_HAM_GET_STATE() >= HAM_SUPERCEDE) { \
		g_ReturnState.pop();					\
		return;									\
	}

#define _HAM_CHECK_V()		\
	if (_HAM_GET_STATE() >= HAM_SUPERCEDE) { \
		g_ReturnState.pop();						\
		return ret;								\
	}

#define _HAM_FUNC_END()	\
	g_ReturnState.pop(); \
	return;

#define _HAM_FUNC_END_V()	\
	if (_HAM_GET_STATE() >= HAM_OVERRIDE) {	\
		g_ReturnState.pop();								\
		return ret;								\
	} else {									\
		g_ReturnState.pop();								\
		return origret;							\
	}
	
#endif
	
enum ForwardState
{
	FSTATE_ACTIVE,
	FSTATE_STOP
};

void RegisterForwards();
void RegisterNatives();

void CS_SetPlayerModel(edict_t* pPlayer, const char* pszModelName, bool updateIndex);
void CS_ResetPlayerModel(edict_t* pPlayer);