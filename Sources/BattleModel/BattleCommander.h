#ifndef BattleCommander_H
#define BattleCommander_H

#include <string>

enum class BattleCommanderType { None, Screen, Script };


class BattleCommander
{
	std::string _id;
	BattleCommanderType _type;
	std::string _configuration;

public:
	BattleCommander(const char* id, BattleCommanderType type, const char* configuration);

	const char* GetId() const { return _id.c_str(); }
	BattleCommanderType GetType() const { return _type; }
	const char* GetConfiguration() const { return _configuration.c_str(); }
};


#endif
