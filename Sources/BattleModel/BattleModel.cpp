// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "../Library/Algebra/geometry.h"
#include "BattleModel.h"




UnitCommand::UnitCommand() :
path(),
facing(0),
running(false),
meleeTarget(nullptr),
missileTarget(nullptr),
missileTargetLocked(false),
holdFire(false)
{
}


Projectile::Projectile() :
position1(),
position2(),
delay(0)
{
}


Shooting::Shooting() :
missileType(MissileType::None),
timeToImpact(0),
projectiles()
{
}


FighterState::FighterState() :
position(),
position_z(0),
readyState(ReadyState_Unready),
readyingTimer(0),
strikingTimer(0),
stunnedTimer(0),
opponent(nullptr),
destination(),
velocity(),
direction(0),
meleeTarget(nullptr)
{
}


Fighter::Fighter() :
unit(nullptr),
state(),
terrainForest(false),
terrainImpassable(false),
terrainPosition(),
nextState(),
casualty(false)
{
}


FighterUpdate Fighter::GetFighterUpdate(const UnitUpdate& unitUpdate)
{
	FighterUpdate result;

	result.positionX = (unsigned char)(255.0f * (state.position.x - unitUpdate.minX) / (unitUpdate.maxX - unitUpdate.minX));
	result.positionY = (unsigned char)(255.0f * (state.position.y - unitUpdate.minY) / (unitUpdate.maxY - unitUpdate.minY));

	return result;
}


void Fighter::SetFighterUpdate(const UnitUpdate& unitUpdate, const FighterUpdate& fighterUpdate)
{
	float positionX = unitUpdate.minX + (unitUpdate.maxX - unitUpdate.minX) * (float)fighterUpdate.positionX / 255.0f;
	float positionY = unitUpdate.minY + (unitUpdate.maxY - unitUpdate.minY) * (float)fighterUpdate.positionY / 255.0f;

	if (positionX == 0)
		state.position = glm::vec2(positionX, positionY);
	else
		state.position = glm::vec2(positionX, positionY);
}


UnitState::UnitState() :
loadingTimer(0),
loadingDuration(0),
shootingCounter(0),
morale(1),
unitMode(UnitMode_Initializing),
center(),
direction(0),
influence(0),
recentCasualties(0),
waypoint()
{
}


UnitUpdate::UnitUpdate()
{
}


UnitUpdate Unit::GetUnitUpdate()
{
	UnitUpdate result;

	result.unitId = unitId;
	result.fightersCount = fightersCount;
	result.morale = state.morale;

	result.movementDestination = command.path.empty() ? state.center : command.path.back();
	result.movementDirection = command.facing;
	result.movementTargetUnitId = command.meleeTarget != nullptr ? command.meleeTarget->unitId : 0;
	result.movementRunning = command.running;

	result.minX = 20000;
	result.maxX = -20000;
	result.minY = 20000;
	result.maxY = -20000;

	for (int i = 0; i < fightersCount; ++i)
	{
		glm::vec2 p = fighters[i].state.position;
		result.minX = fminf(result.minX, p.x);
		result.maxX = fmaxf(result.maxX, p.x);
		result.minY = fminf(result.minY, p.y);
		result.maxY = fmaxf(result.maxY, p.y);
	}

	return result;
}


void Unit::SetUnitUpdate(UnitUpdate unitUpdate, BattleModel* battleModel)
{
	fightersCount = unitUpdate.fightersCount;

	state.morale = unitUpdate.morale;

	command.path.clear();
	command.path.push_back(unitUpdate.movementDestination);
	command.facing = unitUpdate.movementDirection;
	command.meleeTarget = battleModel->GetUnit(unitUpdate.movementTargetUnitId);
	command.running = unitUpdate.movementRunning;
}


UnitStats::UnitStats() :
platformType(PlatformType::None),
missileType(MissileType::None),
samuraiPlaform(SamuraiPlatform_Cav),
samuraiWeapon(SamuraiWeapon_Yari),
weaponReach(0),
trainingLevel(0),
strikingDuration(0),
fireRate(25.0f), // rounds per minute
minimumRange(0),
maximumRange(0),
fireAccuracy(0.7f),
walkingSpeed(0),
runningSpeed(0),
fighterSize(glm::vec2(10, 10)),
spacing(glm::vec2(10, 10))
{
}


UnitRange::UnitRange() :
center(),
angleStart(0),
angleLength(0),
minimumRange(0),
maximumRange(0),
actualRanges()
{

}



static float normalize_angle(float a)
{
	static float two_pi = 2.0f * (float)M_PI;
	while (a < 0)
		a += two_pi;
	while (a > two_pi)
		a -= two_pi;
	return a;
}


bool UnitRange::IsWithinRange(glm::vec2 p) const
{
	if (minimumRange > 0 && maximumRange > 0)
	{
		glm::vec2 diff = p - center;
		float angle = glm::atan(diff.y, diff.x);
		float angleDelta = 0.5f * angleLength;

		if (glm::abs(diff_radians(angle, angleStart + angleDelta)) > angleDelta)
			return false;

		float distance = glm::length(diff);
		if (distance < minimumRange)
			return false;

		if (!actualRanges.empty())
		{
			float n = actualRanges.size() - 1;
			float k = n * normalize_angle(angle - angleStart) / angleLength;
			float i = glm::floor(k);

			float a0 = actualRanges[(int)i];
			float a1 = actualRanges[(int)i + 1];
			float actualRange = glm::mix(a0, a1, k - i);

			return distance <= actualRange;
		}
	}

	return false;
}



Unit::Unit() :
unitId(0),
player(Player(1, 1)),
stats(),
fighters(nullptr),
state(),
fightersCount(0),
shootingCounter(0),
formation(),
timeUntilSwapFighters(0),
nextState(),
unitRange(),
command()
{
}


glm::vec2 Unit::CalculateUnitCenter()
{
	if (state.unitMode == UnitMode_Initializing)
		return state.center;

	glm::vec2 p = glm::vec2();
	int count = 0;

	for (Fighter* fighter = fighters, * end = fighter + fightersCount; fighter != end; ++fighter)
	{
		p += fighter->state.position;
		++count;
	}

	return p / (float)count;
}


float Unit::GetSpeed()
{
	if (state.IsRouting())
		return stats.runningSpeed * 1.2f;
	else
		return command.running || (command.meleeTarget != nullptr) ? stats.runningSpeed : stats.walkingSpeed;
}


int Unit::GetFighterRank(Fighter* fighter)
{
	Unit* unit = fighter->unit;
	return (fighter - unit->fighters) % unit->formation.numberOfRanks;
}


int Unit::GetFighterFile(Fighter* fighter)
{
	Unit* unit = fighter->unit;
	return (int)(fighter - unit->fighters) / unit->formation.numberOfRanks;
}


Fighter* Unit::GetFighter(Unit* unit, int rank, int file)
{
	if (0 <= rank && rank < unit->formation.numberOfRanks && file >= 0)
	{
		int index = rank + file * unit->formation.numberOfRanks;
		if (index < unit->fightersCount)
			return unit->fighters + index;
	}
	return 0;
}



BattleModel::BattleModel() :
groundMap(nullptr),
heightMap(nullptr),
lastUnitId(0),
bluePlayer(Player(1, 1)),
winnerTeam(0),
time(0),
timeStep(1.0f / 15.0f)
{
}


BattleModel::~BattleModel()
{
	for (std::map<int, Unit*>::iterator i = units.begin(); i != units.end(); ++i)
	{
		Unit* unit = (*i).second;
		delete[] unit->fighters;
		delete unit;
	}
}



bool BattleModel::IsMelee() const
{
	for (std::map<int, Unit*>::const_iterator i = units.begin(); i != units.end(); ++i)
	{
		const Unit* unit = (*i).second;
		for (Fighter* fighter = unit->fighters, * end = fighter + unit->fightersCount; fighter != end; ++fighter)
		{
			if (fighter->state.opponent != nullptr)
			{
				return true;
			}
		}
	}
	return false;
}


Unit* BattleModel::AddUnit(Player player, int numberOfFighters, UnitStats stats, glm::vec2 position)
{
	Unit* unit = new Unit();

	unit->unitId = ++lastUnitId;
	unit->player = player;
	unit->stats = stats;

	unit->fightersCount = numberOfFighters;
	unit->fighters = new Fighter[numberOfFighters];

	for (Fighter* i = unit->fighters, * end = i + numberOfFighters; i != end; ++i)
		i->unit = unit;

	unit->command.facing = player.team == 1 ? (float)M_PI_2 : (float)M_PI_2 * 3;
	//unit->command.waypoint = position;

	unit->state.unitMode = UnitMode_Initializing;
	unit->state.center = position;
	unit->state.direction = unit->command.facing;

	unit->command.missileTarget = nullptr;

	unit->formation.rankDistance = stats.fighterSize.y + stats.spacing.y;
	unit->formation.fileDistance = stats.fighterSize.x + stats.spacing.x;
	unit->formation.numberOfRanks = (int)fminf(6, unit->fightersCount);
	unit->formation.numberOfFiles = (int)ceilf((float)unit->fightersCount / unit->formation.numberOfRanks);

	units[unit->unitId] = unit;

	return unit;
}


UnitStats SamuraiBattleModel::GetDefaultUnitStats(SamuraiPlatform platform, SamuraiWeapon weapon)
{
	UnitStats result;

	result.platformType = platform == SamuraiPlatform_Cav || platform == SamuraiPlatform_Gen ? PlatformType::Cavalry : PlatformType::Infantry;
	result.samuraiPlaform = platform;
	result.samuraiWeapon = weapon;

	if (platform == SamuraiPlatform_Cav || platform == SamuraiPlatform_Gen)
	{
		result.fighterSize = glm::vec2(1.1f, 2.3f);
		result.spacing = glm::vec2(1.1f, 1.7f);
		result.walkingSpeed = 7;
		result.runningSpeed = 14;
	}
	else
	{
		result.fighterSize = glm::vec2(0.7f, 0.3f);
		result.spacing = glm::vec2(1.1f, 0.9f);
		result.walkingSpeed = 4;
		result.runningSpeed = 8;
	}

	result.readyingDuration = 1.0f;

	switch (weapon)
	{
		case SamuraiWeapon_Bow:
			result.missileType = MissileType::Bow;
			result.minimumRange = 20;
			result.maximumRange = 150;
			result.runningSpeed += 2; // increased speed
			result.strikingDuration = 3.0f;
			break;

		case SamuraiWeapon_Arq:
			result.missileType = MissileType::Arq;
			result.minimumRange = 20;
			result.maximumRange = 110;
			result.walkingSpeed = 5;
			result.runningSpeed = 9;
			result.strikingDuration = 3.0f;
			break;

		case SamuraiWeapon_Yari:
			result.weaponReach = 5.0f;
			result.strikingDuration = 2.0f;
			break;

		case SamuraiWeapon_Nagi:
			result.weaponReach = 2.4f;
			result.strikingDuration = 1.9f;
			break;

		case SamuraiWeapon_Kata:
			result.weaponReach = 1.0f;
			result.strikingDuration = 1.8f;
			break;
	}

	switch (result.samuraiPlaform)
	{
		case SamuraiPlatform_Ash:
			result.trainingLevel = 0.5f;
			break;
		case SamuraiPlatform_Gen:
			result.trainingLevel = 0.9f;
			break;
		default:
			result.trainingLevel = 0.8f;
			break;
	}

	return result;
}
