// Copyright (C) 2015 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef BattleObjects_v1_H
#define BattleObjects_v1_H

#include "BattleObjects.h"

class BattleMap;
class BattleScript;
class BattleSimulator_v1_0_0;
class GroundMap;
class HeightMap;


class BattleObjects_v1 : public virtual BattleObjects
{
public:
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


	struct Shooting
	{
		Unit* unit{};
		MissileType missileType{};
		float timeToImpact{};
		glm::vec2 target{};
		bool released{};
		std::vector<BattleObjects::Projectile> projectiles{};
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

		Unit* GetUnit() const { return unit; }
		int GetRank() const;
		int GetFile() const;
	};


	struct UnitStats
	{
		PlatformType platformType{};
		MissileType missileType{};

		float weaponReach{};
		float trainingLevel{};
		float strikingDuration{};
		float readyingDuration{};
		float minimumRange{}; // minimum fire range in meters
		float maximumRange{}; // maximum fire range in meters
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


	struct Unit : public BattleObjects::Unit
	{
		// static attributes
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

		glm::vec2 CalculateUnitCenter();
		float GetSpeed();

		const UnitCommand& GetCommand() const { return nextCommandTimer > 0 ? nextCommand : command; }

		bool IsInMelee() const;

		Fighter* GetFighter(int rank, int file) const;

	public: // BattleObjects::Unit overrides
		glm::vec2 GetCenter() const override { return state.center; }
		void SetCenter(glm::vec2 value) { state.center = value; }

		float GetBearing() const override { return state.bearing; }
		void SetBearing(float value) {state.bearing = value; }

		float GetMorale() const override { return state.morale; }
		void SetMorale(float value) { state.morale = value; }
		bool IsRouting() const { return state.IsRouting(); }
	};

protected:
	std::vector<BattleObjects_v1::Unit*> _units{};

public:
	virtual const std::vector<BattleObjects_v1::Unit*>& GetUnits() { return _units; }


}; // class BattleObjects_v1


#endif
