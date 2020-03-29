#pragma once

#include "ham_const.h"
#include "hook.h"
#include "amtl/am-vector.h"

extern ke::Vector<Hook*> g_Hooks[HAM_LAST_ENTRY_DONT_USE_ME_LOL];

void* GetFunction(void* pthis, int id, bool& istramp);
void* GetHamFunction(void* pthis, int id);

int RegisterHam(int func, const char* classname, void* targetfunc);
int RegisterGameRules(int func, void* targetfunc);