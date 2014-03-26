#ifndef BattleCommander_H
#define BattleCommander_H

#include <string>

enum class BattleCommanderType { None = 0, Player = 1, Script = 2 };


class BattleCommander
{
	std::string _playerId;
	int _team;
	BattleCommanderType _type;
	bool _isActive;

public:
	BattleCommander(const char* playerId, int team, BattleCommanderType type);

	const char* GetPlayerId() const { return _playerId.c_str(); }
	int GetTeam() const { return _team; }
	BattleCommanderType GetType() const { return _type; }

	bool IsActive() const { return _isActive; }
	void SetActive(bool value) { _isActive = value; }
};


#endif
