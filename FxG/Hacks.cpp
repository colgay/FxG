#include "Hacks.h"

#include "extdll.h"
#include "meta_api.h"

#include "GameRules.h"
#include "Utilities.h"

#include <IGameConfigs.h>
#include <CDetour/detours.h>

extern IGameConfig* g_pMainConfig;
CDetour* PlayerDeathThinkDetour;

void DestroyDetour(CDetour*& detour)
{
	if (detour)
	{
		detour->Destroy();
		detour = nullptr;
	}
}

void ToggleDetour(CDetour* detour, bool enable)
{
	if (detour)
	{
		(enable) ? detour->EnableDetour() : detour->DisableDetour();
	}
}

DETOUR_DECL_MEMBER0(PlayerDeathThink, void)
{
	g_GameRules.PlayerDeathThink(this);

	return DETOUR_MEMBER_CALL(PlayerDeathThink)();
}

void CtrlDetours(bool set)
{
	if (set)
	{
		void* address = nullptr;

		if (g_pMainConfig->GetMemSig("PlayerDeathThink", &address)) // CBasePlayer::PlayerDeathThink()
		{
			SERVER_PRINT(UTIL_VarArgs("PlayerDeathThink address is %p\n", address));
			PlayerDeathThinkDetour = DETOUR_CREATE_MEMBER_FIXED(PlayerDeathThink, address);
		}
	}
	else
	{
		DestroyDetour(PlayerDeathThinkDetour);
	}
}

void InitializeHacks()
{
	CtrlDetours(true);
}

void ShutdownHacks()
{
	CtrlDetours(false);
}

void ToggleHooks(bool enable)
{
	ToggleDetour(PlayerDeathThinkDetour, enable);
}