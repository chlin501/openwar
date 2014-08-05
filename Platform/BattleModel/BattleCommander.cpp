#include "BattleCommander.h"
#import "BattleScenario.h"


BattleCommander::BattleCommander(BattleScenario* scenario, const char* playerId, int team, BattleCommanderType type) :
	_scenario(scenario),
	_playerId(playerId),
	_team(team),
	_type(type),
	_isSharingCommand(false)
{
}


const char* BattleCommander::GetPlayerId() const
{
	return _playerId.c_str();
}


int BattleCommander::GetTeam() const
{
	return _team;
}


BattleCommanderType BattleCommander::GetType() const
{
	return _type;
}


int BattleCommander::GetTeamPosition() const
{
	return _scenario->GetTeamPosition(_team);
}


bool BattleCommander::IsSharingCommand() const
{
	return _isSharingCommand;
}


void BattleCommander::SetSharingCommand(bool value)
{
	_isSharingCommand = value;
}
