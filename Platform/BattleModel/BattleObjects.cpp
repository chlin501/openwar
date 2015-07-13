#include "BattleObjects.h"
#include "BattleSimulator.h"


namespace BattleObjects
{

	Commander::Commander(BattleSimulator* simulator, const char* playerId, int team, CommanderType type)
		:
		_simulator{simulator},
		_playerId{playerId},
		_team{team},
		_type{type}
	{
	}


	const char* Commander::GetPlayerId() const
	{
		return _playerId.c_str();
	}


	int Commander::GetTeam() const
	{
		return _team;
	}


	CommanderType Commander::GetType() const
	{
		return _type;
	}


	int Commander::GetTeamPosition() const
	{
		return _simulator->GetTeamPosition(_team);
	}


	bool Commander::IsIncapacitated() const
	{
		return _isIncapacitated;
	}


	void Commander::SetIncapacitated(bool value)
	{
		_isIncapacitated = value;
	}


	bool Commander::HasAbandonedBattle() const
	{
		return _hasAbandonedBattle;
	}


	void Commander::SetAbandonedBattle(bool value)
	{
		_hasAbandonedBattle = value;
	}

}