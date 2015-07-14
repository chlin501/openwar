#ifndef BattleObjects_H
#define BattleObjects_H

#include <string>
#include <glm/glm.hpp>


class BattleObjects
{
public:
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


	class Unit
	{
	public:
		virtual ~Unit();

		virtual glm::vec2 GetCenter() const = 0;
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
