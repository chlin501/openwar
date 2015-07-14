#ifndef BattleObjects_H
#define BattleObjects_H

#include <string>
#include <glm/glm.hpp>


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


	struct Projectile
	{
		glm::vec2 position1{};
		glm::vec2 position2{};
		float delay{};
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


	struct FighterAssignment
	{
		int rank;
		int file;
	};


	class Unit
	{
		bool _ownedBySimulator{};

	public:
		BattleObjects::Commander* commander{};

		virtual ~Unit();

		bool IsOwnedBySimulator() const;
		void SetOwnedBySimulator(bool value);

		bool IsFriendlyCommander(BattleObjects::Commander* battleCommander) const;
		bool IsCommandableBy(BattleObjects::Commander* battleCommander) const;

		virtual glm::vec2 GetCenter() const = 0;
		virtual void SetCenter(glm::vec2 value) = 0;

		virtual float GetBearing() const = 0;
		virtual void SetBearing(float value) = 0;

		virtual float GetMorale() const = 0;
		virtual void SetMorale(float value) = 0;
		virtual bool IsRouting() const = 0;

		virtual const UnitCommand& GetCurrentCommand() const = 0;
		virtual const UnitCommand& GetIssuedCommand() const = 0;

		virtual int GetFighterCount() const = 0;
		virtual void SetFighterCount(int value) = 0;

		virtual FighterPosition GetFighterPosition(int index) const = 0;
		virtual void SetFighterPosition(int index, glm::vec3 value) = 0;

		virtual FighterAssignment GetFighterAssignment(int index) const = 0;
		virtual void SetFighterCasualty(glm::vec2 position) = 0;
	};


protected:
	std::vector<BattleObjects::Commander*> _commanders{};
	BattleObjects::Commander* _dummyCommander{};
	int _teamPosition1{};
	int _teamPosition2{};
	std::vector<BattleObjects::Unit*> _units{};

public:
	BattleObjects::Commander* AddCommander(const char* playerId, int team, BattleObjects::CommanderType type);
	BattleObjects::Commander* GetCommander(const char* playerId) const;
	const std::vector<BattleObjects::Commander*>& GetCommanders() const { return _commanders; }
	BattleObjects::Commander* GetDummyCommander() const;

	void SetTeamPosition(int team, int position);
	int GetTeamPosition(int team) const;

}; // class BattleObjects


#endif
