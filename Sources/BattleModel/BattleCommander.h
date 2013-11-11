#ifndef BattleCommander_H
#define BattleCommander_H

#include <string>

enum class BattleCommanderType { None = 0, Screen = 1, Script = 2 };


class BattleCommander
{
	int _team;
	BattleCommanderType _type;
	std::string _configuration;

public:
	BattleCommander(int team, BattleCommanderType type, const char* configuration);

	int GetTeam() const { return _team; }
	BattleCommanderType GetType() const { return _type; }
	const char* GetConfiguration() const { return _configuration.c_str(); }
};


#endif
