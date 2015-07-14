#ifndef BattleObjects_H
#define BattleObjects_H

#include <string>
#include <glm/glm.hpp>

class BattleMap;


class BattleObjects
{
public:
	class Unit;

	enum class CommanderType { None = 0, Player = 1, Script = 2 };


	class Commander
	{
		BattleObjects* _simulator{};
		std::string _playerId{};
		int _team{};
		CommanderType _type{};
		bool _isIncapacitated{};
		bool _hasAbandonedBattle{};

	public:
		Commander(BattleObjects* simulator, const char* playerId, int team, CommanderType type);

		const char* GetPlayerId() const;
		int GetTeam() const;
		CommanderType GetType() const;

		int GetTeamPosition() const;

		bool IsIncapacitated() const;
		void SetIncapacitated(bool value);

		bool HasAbandonedBattle() const;
		void SetAbandonedBattle(bool value);
	};


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
			UpdateMovementPath(path, curr, dest);
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

		static void UpdateMovementPathStart(std::vector<glm::vec2>& path, glm::vec2 startPosition);
		static void UpdateMovementPath(std::vector<glm::vec2>& path, glm::vec2 startPosition, glm::vec2 endPosition);
		static float MovementPathLength(const std::vector<glm::vec2>& path);
		static bool IsForwardMotion(const std::vector<glm::vec2>& path, glm::vec2 position);
	};


	struct FighterPosition
	{
		glm::vec2 position;
		float bearing;
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

		glm::vec2 GetFrontLeft(glm::vec2 center) const;
		void SetDirection(float direction);
	};


	struct FighterAssignment
	{
		int rank;
		int file;
	};


	class Unit
	{
		bool _ownedBySimulator{};

	public:
		Commander* commander{};
		std::string unitClass{};
		bool deployed{};

		virtual ~Unit();

		bool IsOwnedBySimulator() const;
		void SetOwnedBySimulator(bool value);

		bool IsFriendlyCommander(Commander* battleCommander) const;
		bool IsCommandableBy(Commander* battleCommander) const;

		virtual glm::vec2 GetCenter() const = 0;
		virtual void SetCenter(glm::vec2 value) = 0;

		virtual float GetBearing() const = 0;
		virtual void SetBearing(float value) = 0;

		virtual float GetIntrinsicMorale() const = 0;
		virtual float GetEffectiveMorale() const = 0;
		virtual void SetIntrinsicMorale(float value) = 0;

		virtual bool IsRouting() const = 0;
		virtual bool IsStanding() const = 0;
		virtual bool IsMoving() const = 0;

		virtual const UnitCommand& GetCurrentCommand() const = 0;
		virtual const UnitCommand& GetIssuedCommand() const = 0;

		virtual PlatformType GetPlatformType() const = 0;
		virtual std::pair<bool, float> GetLoadingProgress() const = 0;
		virtual float GetWeaponReach() const = 0;
		virtual const UnitRange& GetMissileWeaponRange() const = 0;
		virtual const Formation& GetFormation() const = 0;

		virtual int GetFighterCount() const = 0;
		virtual void SetFighterCount(int value) = 0;

		virtual FighterPosition GetFighterPosition(int index) const = 0;
		virtual void SetFighterPosition(int index, glm::vec3 value) = 0;

		virtual FighterAssignment GetFighterAssignment(int index) const = 0;
		virtual void SetFighterCasualty(glm::vec2 position) = 0;
	};

protected:
	std::vector<Commander*> _commanders{};
	Commander* _dummyCommander{};
	int _teamPosition1{};
	int _teamPosition2{};
	BattleMap* _battleMap{};

public:
	BattleObjects();
	virtual ~BattleObjects();

	Commander* AddCommander(const char* playerId, int team, CommanderType type);
	Commander* GetCommander(const char* playerId) const;
	const std::vector<Commander*>& GetCommanders() const { return _commanders; }
	Commander* GetDummyCommander() const;

	void SetTeamPosition(int team, int position);
	int GetTeamPosition(int team) const;
	bool TeamHasAbandondedBattle(int team) const;

	BattleMap* GetBattleMap() const { return _battleMap; }

	virtual const std::vector<Unit*>& GetUnits() const = 0;

}; // class BattleObjects


#endif
