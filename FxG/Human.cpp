#include "Human.h"
#include "Utilities.h"
#include "CstrikeDatas.h"
#include "AmxxApi.h"

DECLARE_PLAYER_CLASS(Human);

Human::Human(Player* pPlayer) : PlayerClass(pPlayer)
{

}

void Human::SetMaxspeed()
{
	//m_pPlayer->SetMaxSpeed(m_pPlayer->GetMaxSpeed() * 1.0);
}

void Human::Become()
{
	m_pPlayer->SetGravity(1.0);
	m_pPlayer->SetHealth(100);
	m_pPlayer->SetMaxHealth(m_pPlayer->GetHealth());
	m_pPlayer->SetArmorValue(0.0f);
	m_pPlayer->ChangeTeam(TEAM_CT, CS_DONTCHANGE);
	m_pPlayer->ResetModel();
	m_pPlayer->ResetMaxspeed();
}
