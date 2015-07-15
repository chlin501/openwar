#include "BattleObjects.h"
#include "BattleSimulator_v1_0_0.h"


void BattleObjects::UnitCommand::UpdateMovementPathStart(std::vector<glm::vec2>& path, glm::vec2 startPosition)
{
	const float spacing = 10;

	while (!path.empty() && glm::distance(path.front(), startPosition) < spacing)
		path.erase(path.begin());

	path.insert(path.begin(), startPosition);
}


void BattleObjects::UnitCommand::UpdateMovementPath(std::vector<glm::vec2>& path, glm::vec2 startPosition, glm::vec2 endPosition)
{
	const float spacing = 10;

	while (!path.empty() && glm::distance(path.front(), startPosition) < spacing)
		path.erase(path.begin());

	while (!path.empty() && glm::distance(path.back(), endPosition) < spacing)
		path.erase(path.end() - 1);

	while (!IsForwardMotion(path, endPosition))
		path.pop_back();

	path.insert(path.begin(), startPosition);

	int n = 20;
	glm::vec2 p = path.back();
	while (n-- != 0 && glm::length(p - endPosition) > 2 * spacing)
	{
		p += spacing * glm::normalize(endPosition - p);
		path.push_back(p);
	}
	if (glm::length(p - endPosition) > spacing)
	{
		path.push_back(0.5f * (p + endPosition));
	}

	path.push_back(endPosition);
}


float BattleObjects::UnitCommand::MovementPathLength(const std::vector<glm::vec2>& path)
{
	float result = 0;

	for (std::vector<glm::vec2>::const_iterator i = path.begin() + 1; i < path.end(); ++i)
		result += glm::distance(*(i - 1), *i);

	return result;
}


bool BattleObjects::UnitCommand::IsForwardMotion(const std::vector<glm::vec2>& path, glm::vec2 position)
{
	if (path.size() < 2)
		return true;

	glm::vec2 last = *(path.end() - 1);
	glm::vec2 prev = *(path.end() - 2);
	glm::vec2 next = last + (last - prev);

	return glm::length(position - next) < glm::length(position - prev);
}


/***/


BattleObjects::Unit::~Unit()
{
}


bool BattleObjects::Unit::IsOwnedBySimulator() const
{
	return _ownedBySimulator;
}


void BattleObjects::Unit::SetOwnedBySimulator(bool value)
{
	_ownedBySimulator = value;
}


bool BattleObjects::Unit::IsFriendlyCommander(BattleCommander* battleCommander) const
{
	if (battleCommander == nullptr)
		return false;

	if (commander == battleCommander)
		return true;

	if (battleCommander->GetType() == BattleCommanderType::None)
		return false;

	if (commander->GetTeam() != battleCommander->GetTeam())
		return false;

	return true;
}


bool BattleObjects::Unit::IsCommandableBy(BattleCommander* battleCommander) const
{
	if (battleCommander == nullptr)
		return false;

	if (commander == battleCommander)
		return true;

	if (battleCommander->GetType() == BattleCommanderType::None)
		return false;

	if (commander->IsIncapacitated() && commander->GetTeam() == battleCommander->GetTeam())
		return true;

	return false;
}


/***/


BattleObjects::BattleObjects()
{
	_dummyCommander = new BattleCommander(this, "", 1, BattleCommanderType::None);

}


BattleObjects::~BattleObjects()
{
	for (BattleCommander* commander : _commanders)
		delete commander;
	delete _dummyCommander;
}


BattleCommander* BattleObjects::AddCommander(const char* playerId, int team, BattleCommanderType type)
{
	BattleCommander* commander = new BattleCommander(this, playerId, team, type);
	_commanders.push_back(commander);
	return commander;
}


BattleCommander* BattleObjects::GetCommander(const char* playerId) const
{
	for (BattleCommander* commander : _commanders)
		if (std::strcmp(commander->GetPlayerId(), playerId) == 0)
			return commander;

	return nullptr;
}


BattleCommander* BattleObjects::GetDummyCommander() const
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


bool BattleObjects::TeamHasAbandondedBattle(int team) const
{
	for (BattleObjects::Unit* unit : GetUnits())
		if (unit->commander->GetTeam() == team && !unit->commander->HasAbandonedBattle())
			return false;

	return true;
}


int BattleObjects::CountCavalryInMelee() const
{
	int result = 0;

	for (const Unit* unit : GetUnits())
		if (unit->GetPlatformType() == BattleObjects::PlatformType::Cavalry && unit->IsInMelee())
			++result;

	return result;
}


int BattleObjects::CountInfantryInMelee() const
{
	int result = 0;

	for (const Unit* unit : GetUnits())
		if (unit->GetPlatformType() == BattleObjects::PlatformType::Infantry && unit->IsInMelee())
			++result;

	return result;
}
