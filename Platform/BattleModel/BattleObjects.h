#ifndef BattleObjects_H
#define BattleObjects_H

#include <string>
#include <glm/glm.hpp>

class BattleSimulator;


namespace BattleObjects
{

	class Unit;


	enum class CommanderType { None = 0, Player = 1, Script = 2 };


	class Commander
	{
		BattleSimulator* _simulator{};
		std::string _playerId{};
		int _team{};
		CommanderType _type{};
		bool _isIncapacitated{};
		bool _hasAbandonedBattle{};

	public:
		Commander(BattleSimulator* simulator, const char* playerId, int team, CommanderType type);

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


	struct Fighter
	{
		// static attributes
		Unit* unit{};
	};


	struct Unit
	{
		// static attributes
		Commander* commander{};
	};


} // namespace BattleObjects

#endif
