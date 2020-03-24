// fakeham.amxx

#include <amxmodx>
#include <cstrike>
#include <hamsandwich>

native FH_PlayerSpawn_Post(id);
native FH_PlayerTakeDamage(id, inclictor, attacker, Float:damage, damageType);

public plugin_init()
{
    register_plugin("Fake HamSandwich", "0.1", "Colgate");

    RegisterHam(Ham_Spawn, "player", "OnPlayerSpawn_Post", 1);
    RegisterHam(Ham_TakeDamage, "player", "OnPlayerTakeDamage");
    RegisterHam(Ham_CS_Player_ResetMaxSpeed, "player", "OnPlayerResetMaxspeed");
}

public OnPlayerSpawn_Post(id)
{
    return FH_PlayerSpawn_Post(id);
}

public OnPlayerTakeDamage(id, inclictor, attacker, Float:damage, damageType)
{
    return FH_PlayerTakeDamage(id, inclictor, attacker, damage, damageType)
}

public OnPlayerResetMaxspeed(id)
{
    return FH_PlayerResetMaxspeed(id);
}

public FH_CallKilled(id, killer, gibs, b)
{
    if (b)
        return ExecuteHamB(Ham_Killed, id, killer, gibs);
    
    return ExecuteHam(Ham_Killed, id, killer, gibs);
}

public FH_CallTakeDamage(id, inclictor, attacker, Float:damage, damageType, b)
{
    if (b)
        return ExecuteHamB(Ham_TakeDamage, id, inclictor, attacker, damage, damageType);
    
    return ExecuteHam(Ham_TakeDamage, id, inclictor, attacker, damage, damageType);
}

public FH_CallResetMaxspeed(id, b)
{
    if (b)
        return ExecuteHamB(Ham_CS_Player_ResetMaxSpeed, id);
    
    return ExecuteHam(Ham_CS_Player_ResetMaxSpeed, id);
}

public FH_SetParamInt(which, value)
{
    return SetHamParamInteger(which, value);
}

public FH_SetParamFloat(which, Float:value)
{
    return SetHamParamFloat(which, value);
}

// Fake Cstrike
public FC_SetPlayerModel(id, const model[], bool:update_index)
{
    return cs_set_user_model(id, model, update_index);
}

public FC_ResetPlayerModel(id)
{
    return cs_reset_user_model(id);
}