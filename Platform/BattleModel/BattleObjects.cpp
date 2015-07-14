#include "BattleObjects.h"
#include "BattleSimulator.h"


BattleObjects::Commander::Commander(BattleSimulator* simulator, const char* playerId, int team, CommanderType type) :
	_simulator{simulator},
	_playerId{playerId},
	_team{team},
	_type{type}
{
}


const char* BattleObjects::Commander::GetPlayerId() const
{
	return _playerId.c_str();
}


int BattleObjects::Commander::GetTeam() const
{
	return _team;
}


BattleObjects::CommanderType BattleObjects::Commander::GetType() const
{
	return _type;
}


int BattleObjects::Commander::GetTeamPosition() const
{
	return _simulator->GetTeamPosition(_team);
}


bool BattleObjects::Commander::IsIncapacitated() const
{
	return _isIncapacitated;
}


void BattleObjects::Commander::SetIncapacitated(bool value)
{
	_isIncapacitated = value;
}


bool BattleObjects::Commander::HasAbandonedBattle() const
{
	return _hasAbandonedBattle;
}


void BattleObjects::Commander::SetAbandonedBattle(bool value)
{
	_hasAbandonedBattle = value;
}
