#include "BattleCommander.h"


BattleCommander::BattleCommander(const char* id, int team, BattleCommanderType type, const char* configuration) :
_id(id),
_team(team),
_type(type),
_configuration(configuration),
_isActive(false)
{
}
