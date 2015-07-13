// Copyright (C) 2015 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef BattleObjects_v1_H
#define BattleObjects_v1_H

#include <string>
#include <glm/glm.hpp>

#include "BattleObjects.h"

class BattleMap;
class BattleScript;
class BattleSimulator;
class GroundMap;
class HeightMap;


namespace BattleObjects_v1
{

	struct Fighter;
	struct Unit;


	enum class PlatformType
	{
		None,
		Infantry,
		Cavalry
	};


	enum class MissileType
	{
		None,
		Bow,
		Arq
	};


	enum UnitMode
	{
		UnitMode_Initializing,
		UnitMode_Standing,
		UnitMode_Moving,
		UnitMode_Turning
	};


	struct Projectile
	{
		glm::vec2 position1{};
		glm::vec2 position2{};
		float delay{};
	};


	struct Shooting
	{
		Unit* unit{};
		MissileType missileType{};
		float timeToImpact{};
		glm::vec2 target{};
		bool released{};
		std::vector<Projectile> projectiles{};
	};


	enum ReadyState
	{
		ReadyState_Unready,
		ReadyState_Readying,
		ReadyState_Prepared,
		ReadyState_Striking,
		ReadyState_Stunned
	};


	struct FighterState
	{
		// dynamic attributes
		glm::vec2 position{};
		float position_z{};
		ReadyState readyState{};
		float readyingTimer{};
		float strikingTimer{};
		float stunnedTimer{};
		Fighter* opponent{};

		// intermediate attributes
		glm::vec2 destination{};
		glm::vec2 velocity{};
		float bearing{};
		Fighter* meleeTarget{};
	};


	struct Fighter
	{
		// static attributes
		Unit* unit{};

		// dynamic attributes
		FighterState state{};

		// optimization attributes
		bool terrainForest{};
		bool terrainImpassable{};
		glm::vec2 terrainPosition{};

		// intermediate attributes
		FighterState nextState{};
		bool casualty{};
	};


	struct UnitStats
	{
		PlatformType platformType{};
		MissileType missileType{};

		float weaponReach{};
		float trainingLevel{};
		float strikingDuration{};
		float readyingDuration{};
		float fireRate{25.0f}; // shots per minute
		float minimumRange{}; // minimum fire range in meters
		float maximumRange{}; // maximum fire range in meters
		float fireAccuracy{0.7f}; // percentage of hit at half range
		float walkingSpeed{}; // meters per second
		float runningSpeed{}; // meters per second
		glm::vec2 fighterSize{10.0f, 10.0f}; // x = side-to-side, y = front-to-back
		glm::vec2 spacing{10.0f, 10.0f};  // x = side-to-side, y = front-to-back
	};


	struct UnitRange
	{
		glm::vec2 center{};
		float angleStart{};
		float angleLength{};
		float minimumRange{};
		float maximumRange{};
		std::vector<float> actualRanges{};

		bool IsWithinRange(glm::vec2 p) const;
	};


	struct UnitState
	{
		// dynamic attributes
		float loadingTimer{};
		float loadingDuration{};
		int shootingCounter{};
		float morale{1.0f};

		// optimization attributes
		UnitMode unitMode{};
		glm::vec2 center{};
		float bearing{};
		float influence{};

		// intermediate attributes
		int recentCasualties{};
		glm::vec2 waypoint{};


		bool IsRouting() const
		{
			float x = morale + influence;
			return x <= 0;
		}


		float GetRoutingBlinkTime() const
		{
			float x = morale + influence;
			return 0 <= x && x < 0.33f ? 0.1f + x * 3 : 0;
		}
	};


	struct MovementPath
	{
		static void UpdateMovementPathStart(std::vector<glm::vec2>& path, glm::vec2 startPosition);

		static void UpdateMovementPath(std::vector<glm::vec2>& path, glm::vec2 startPosition, glm::vec2 endPosition);
		static float Length(const std::vector<glm::vec2>& path);
	};


	struct UnitCommand
	{
		std::vector<glm::vec2> path{};
		bool running{};
		float bearing{};
		Unit* meleeTarget{};
		Unit* missileTarget{}; // set to self to hold fire
		bool missileTargetLocked{};


		void UpdatePath(glm::vec2 curr, glm::vec2 dest)
		{
			MovementPath::UpdateMovementPath(path, curr, dest);
		}


		void ClearPathAndSetDestination(glm::vec2 p)
		{
			path.clear();
			path.push_back(p);
		}


		glm::vec2 GetDestination() const
		{
			return !path.empty() ? path.back() : glm::vec2(512, 512);
		}
	};


	struct Formation
	{
		float rankDistance{};
		float fileDistance{};
		int numberOfRanks{}; // updated by UpdateFormation() and AddUnit()
		int numberOfFiles{}; // updated by UpdateFormation() and AddUnit()
		float _direction{};
		glm::vec2 towardRight{};
		glm::vec2 towardBack{};

		glm::vec2 GetFrontLeft(glm::vec2 center);
		void SetDirection(float direction);
	};


	struct Unit
	{
		// static attributes
		BattleObjects::Commander* commander{};
		std::string unitClass{};
		UnitStats stats{};
		Fighter* fighters{};

		// dynamic attributes
		UnitState state{}; // updated by AssignNextState()
		int fightersCount{}; // updated by RemoveCasualties()
		int shootingCounter{}; // updated by ResolveMissileCombat()
		Formation formation{}; // updated by UpdateFormation()
		float timeUntilSwapFighters{};

		// intermediate attributes
		UnitState nextState{}; // updated by ComputeNextState()
		UnitRange unitRange{};

		// control attributes
		bool deployed{};
		UnitCommand command{};
		UnitCommand nextCommand{};
		float nextCommandTimer{};
		bool _ownedBySimulator{};


		glm::vec2 CalculateUnitCenter();

		float GetSpeed();


		const UnitCommand& GetCommand() const { return nextCommandTimer > 0 ? nextCommand : command; }


		bool IsOwnedBySimulator() const;
		void SetOwnedBySimulator(bool value);

		bool IsFriendlyCommander(BattleObjects::Commander* battleCommander) const;
		bool IsCommandableBy(BattleObjects::Commander* battleCommander) const;

		bool IsInMelee() const;

		static int GetFighterRank(Fighter* fighter);
		static int GetFighterFile(Fighter* fighter);
		static Fighter* GetFighter(Unit* unit, int rank, int file);
	};

} // namespace BattleObjects_v1

#endif
