#pragma once

#include "ham_const.h"

void* GetFunction(void* pthis, int id, bool& istramp);
void* GetHamFunction(void* pthis, int id);

int RegisterHam(int func, const char* classname, void* targetfunc);
int RegisterGameRules(int func, void* targetfunc);