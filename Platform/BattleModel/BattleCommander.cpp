#include "BattleCommander.h"
#include "BattleObjects.h"


BattleCommander::BattleCommander(BattleObjects* simulator, const char* playerId, int team, BattleCommanderType type) :
	_simulator{simulator},
	_playerId{playerId},
	_team{team},
	_type{type}
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
	return _simulator->GetTeamPosition(_team);
}


bool BattleCommander::IsIncapacitated() const
{
	return _isIncapacitated;
}


void BattleCommander::SetIncapacitated(bool value)
{
	_isIncapacitated = value;
}


bool BattleCommander::HasAbandonedBattle() const
{
	return _hasAbandonedBattle;
}


void BattleCommander::SetAbandonedBattle(bool value)
{
	_hasAbandonedBattle = value;
}
