#include "BattleObjects.h"
#include "BattleSimulator_v1_0_0.h"


BattleObjects::Commander::Commander(BattleObjects* simulator, const char* playerId, int team, CommanderType type) :
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


BattleObjects::Commander* BattleObjects::AddCommander(const char* playerId, int team, BattleObjects::CommanderType type)
{
	Commander* commander = new Commander(this, playerId, team, type);
	_commanders.push_back(commander);
	return commander;
}


BattleObjects::Commander* BattleObjects::GetCommander(const char* playerId) const
{
	for (Commander* commander : _commanders)
		if (std::strcmp(commander->GetPlayerId(), playerId) == 0)
			return commander;

	return nullptr;
}


BattleObjects::Commander* BattleObjects::GetDummyCommander() const
{
	return _dummyCommander;
}



void BattleObjects::SetTeamPosition(int team, int position)
{
	if (team == 1)
		_teamPosition1 = position;
	if (team == 2)
		_teamPosition2 = position;
}


int BattleObjects::GetTeamPosition(int team) const
{
	if (team == 1)
		return _teamPosition1;
	if (team == 2)
		return _teamPosition2;
	return 0;
}

