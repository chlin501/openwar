// Copyright (C) 2015 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef BattleObjects_v1_H
#define BattleObjects_v1_H

#include "BattleObjects.h"

class BattleMap;
class BattleScript;
class GroundMap;
class HeightMap;


class BattleObjects_v1 : public virtual BattleObjects
{
public:
	struct Fighter;
	struct Unit;


	enum SamuraiPlatform
	{
		SamuraiPlatform_Cav = 0, // Cavalry
		SamuraiPlatform_Gen = 1, // General
		SamuraiPlatform_Ash = 2, // Ashigaru
		SamuraiPlatform_Sam = 3  // Samurai
	};


	enum SamuraiWeapon
	{
		SamuraiWeapon_Yari = 0,
		SamuraiWeapon_Kata = 1,
		SamuraiWeapon_Nagi = 2,
		SamuraiWeapon_Bow = 3,
		SamuraiWeapon_Arq = 4
	};

	enum UnitMode
	{
		UnitMode_Initializing,
		UnitMode_Standing,
		UnitMode_Moving,
		UnitMode_Turning
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
	};


	struct Unit : public BattleObjects::Unit
	{
		// static attributes
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
		UnitCommand command{};
		UnitCommand nextCommand{};
		float nextCommandTimer{};

		glm::vec2 CalculateUnitCenter();
		float GetSpeed();

		Fighter* GetFighter(int rank, int file) const;

	public: // BattleObjects::Unit overrides
		glm::vec2 GetCenter() const override { return state.center; }
		void SetCenter(glm::vec2 value) { state.center = value; }

		float GetBearing() const override { return state.bearing; }
		void SetBearing(float value) {state.bearing = value; }

		float GetIntrinsicMorale() const override { return state.morale; }
		float GetEffectiveMorale() const override { return state.morale + state.influence; }
		void SetIntrinsicMorale(float value) { state.morale = value; }

		bool IsRouting() const override { return state.IsRouting(); }
		bool IsStanding() const override { return state.unitMode == BattleObjects_v1::UnitMode_Standing; }
		bool IsMoving() const override { return state.unitMode == BattleObjects_v1::UnitMode_Moving; }
		bool IsInMelee() const override;

		const UnitCommand& GetCurrentCommand() const { return command; }
		const UnitCommand& GetIssuedCommand() const { return nextCommandTimer > 0 ? nextCommand : command; }

		PlatformType GetPlatformType() const override { return stats.platformType; }

		std::pair<bool, float> GetLoadingProgress() const override
		{
			return state.loadingDuration != 0
				? std::make_pair(true, state.loadingTimer / state.loadingDuration)
				: std::make_pair(false, 0.0f);
		}

		float GetWeaponReach() const override { return stats.weaponReach; }
		const UnitRange& GetMissileWeaponRange() const override { return unitRange; }
		const Formation& GetFormation() const override { return formation; }

		int GetFighterCount() const override { return fightersCount; }
		void SetFighterCount(int value) override { fightersCount = value; }

		FighterPosition GetFighterPosition(int index) const override
		{
			const FighterState& state = fighters[index].state;
			return {state.position, state.bearing};
		}

		void SetFighterPosition(int index, glm::vec3 value) override
		{
			FighterState& state = fighters[index].state;
			state.position = value.xy();
			state.position_z = value.z;
			state.readyState = BattleObjects_v1::ReadyState_Unready;
			state.readyingTimer = 0;
			state.strikingTimer = 0;
			state.stunnedTimer = 0;
			state.opponent = nullptr;
			fighters[index].casualty = false;
			timeUntilSwapFighters = 0.2f;
		}

		FighterAssignment GetFighterAssignment(int index) const override
		{
			return {index % formation.numberOfRanks, index / formation.numberOfRanks};
		}

		void SetFighterCasualty(glm::vec2 position) override;
	};

protected:
	std::vector<BattleObjects::Unit*> _units_base{};
	std::vector<BattleObjects_v1::Unit*> _units{};

public:
	const std::vector<BattleObjects::Unit*>& GetUnits() const override { return _units_base; }

	static SamuraiPlatform GetSamuraiPlatform(const char* unitClass);
	static SamuraiWeapon GetSamuraiWeapon(const char* unitClass);

	static BattleObjects_v1::UnitStats GetDefaultUnitStats(const char* unitClass);
	static BattleObjects_v1::UnitStats GetDefaultUnitStats(SamuraiPlatform platform, SamuraiWeapon weapon);

}; // class BattleObjects_v1


#endif
