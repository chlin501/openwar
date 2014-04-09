#include "BattleCommander.h"
#import "BattleScenario.h"


BattleCommander::BattleCommander(BattleScenario* scenario, const char* playerId, int team, BattleCommanderType type) :
_scenario(scenario),
_playerId(playerId),
_team(team),
_type(type),
_isActive(false)
{
}


int BattleCommander::GetTeamStartPosition() const
{
	return _scenario->GetTeamStartPosition(_team);
}
