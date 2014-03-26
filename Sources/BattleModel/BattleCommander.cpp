#include "BattleCommander.h"


BattleCommander::BattleCommander(const char* playerId, int team, BattleCommanderType type) :
_playerId(playerId),
_team(team),
_type(type),
_isActive(false)
{
}
