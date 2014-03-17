#include "BattleCommander.h"


BattleCommander::BattleCommander(const char* id, int team, BattleCommanderType type) :
_id(id),
_team(team),
_type(type),
_isActive(false)
{
}
