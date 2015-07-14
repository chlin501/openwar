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


/***/


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


bool BattleObjects::Unit::IsFriendlyCommander(BattleObjects::Commander* battleCommander) const
{
	if (battleCommander == nullptr)
		return false;

	if (commander == battleCommander)
		return true;

	if (battleCommander->GetType() == BattleObjects::CommanderType::None)
		return false;

	if (commander->GetTeam() != battleCommander->GetTeam())
		return false;

	return true;
}


bool BattleObjects::Unit::IsCommandableBy(BattleObjects::Commander* battleCommander) const
{
	if (battleCommander == nullptr)
		return false;

	if (commander == battleCommander)
		return true;

	if (battleCommander->GetType() == BattleObjects::CommanderType::None)
		return false;

	if (commander->IsIncapacitated() && commander->GetTeam() == battleCommander->GetTeam())
		return true;

	return false;
}


/***/


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
