#include "InfectionMode.h"
#include "amxxmodule.h"

#include <vector>
#include <random>

#include "Utilities.h"
#include "Player.h"
#include "PlayerHandler.h"

DECLARE_GAME_MODE(InfectionMode); // this

void InfectionMode::StartGameMode()
{
	std::vector<int> players;

	std::random_device rd;
	std::mt19937 generator(rd());

	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		if (MF_IsPlayerIngame(i) && MF_IsPlayerAlive(i))
			players.push_back(i);
	}

	int maxZombies = ceil(players.size() * 0.25);

	for (int i = 0; i < maxZombies; i++)
	{
		if (players.size() < 1)
			break;

		std::uniform_int_distribution<int> unif(0, players.size()-1);
		int x = unif(generator);

		int playerIndex = players[x];
		players.erase(players.begin() + x);

		UTIL_ClientPrintAll(HUD_PRINTTALK, UTIL_VarArgs("%d : playerIndex = %d", i, playerIndex));

		Player* pPlayer = GetPlayerHandler()->GetPlayer(playerIndex);
		if (pPlayer != nullptr)
		{
			pPlayer->ChangeClass("Zombie");
			pPlayer->GetClass()->Become();
			pPlayer->SetHealth(pPlayer->GetHealth() * 2.0);
		}
		else
		{
			UTIL_ClientPrintAll(HUD_PRINTTALK, UTIL_VarArgs("GetPlayerHandler()->GetPlayer(%d) is nullptr", playerIndex));
		}
	}

	hudtextparms_t textparms;

	textparms.r2 = 255;
	textparms.g2 = 255;
	textparms.b2 = 250;
	textparms.r1 = 0;
	textparms.g1 = 255;
	textparms.b1 = 0;
	textparms.x = -1.0;
	textparms.y = 0.25;
	textparms.effect = 0;
	textparms.fxTime = 0.0;
	textparms.holdTime = 3.0;
	textparms.fadeinTime = 1.0;
	textparms.fadeoutTime = 1.0;
	textparms.channel = 4;

	UTIL_HudMessageAll(textparms, "The first infection has been start!");
	
	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		if (MF_IsPlayerIngame(i))
			CLIENT_COMMAND(g_TypeConversion.id_to_edict(i), "spk scientist/scream08\n");
	}
}
