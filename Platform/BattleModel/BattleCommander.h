#ifndef BattleCommander_H
#define BattleCommander_H

#include <string>

class BattleScenario;

enum class BattleCommanderType { None = 0, Player = 1, Script = 2 };


class BattleCommander
{
	BattleScenario* _scenario;
	std::string _playerId;
	int _team;
	BattleCommanderType _type;
	bool _isSharingCommand;

public:
	BattleCommander(BattleScenario* scenario, const char* playerId, int team, BattleCommanderType type);

	const char* GetPlayerId() const;
	int GetTeam() const;
	BattleCommanderType GetType() const;

	int GetTeamPosition() const;

	bool IsSharingCommand() const;
	void SetSharingCommand(bool value);
};


#endif
