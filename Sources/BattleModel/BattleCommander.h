#ifndef BattleCommander_H
#define BattleCommander_H

#include <string>

enum class BattleCommanderType { None = 0, Player = 1, Script = 2 };


class BattleCommander
{
	std::string _id;
	int _team;
	BattleCommanderType _type;
	bool _isActive;

public:
	BattleCommander(const char* id, int team, BattleCommanderType type);

	const char* GetId() const { return _id.c_str(); }
	int GetTeam() const { return _team; }
	BattleCommanderType GetType() const { return _type; }

	bool IsActive() const { return _isActive; }
	void SetActive(bool value) { _isActive = value; }
};


#endif
