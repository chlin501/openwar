#ifndef BattleCommander_H
#define BattleCommander_H

#include <string>

enum class BattleCommanderType { None, Screen, Script };


class BattleCommander
{
	std::string _id;
	BattleCommanderType _type;

public:
	BattleCommander(const char* id, BattleCommanderType type);
	~BattleCommander();

	const char* GetId() const { return _id.c_str(); }
	BattleCommanderType GetType() const { return _type; }
};



#endif
