#ifndef BattleCommander_H
#define BattleCommander_H

#include <string>

enum class BattleCommanderType { None = 0, Online = 1, Practice = 2 };


class BattleCommander
{
	std::string _id;
	int _team;
	BattleCommanderType _type;
	std::string _configuration;
	bool _isActive;

public:
	BattleCommander(const char* id, int team, BattleCommanderType type, const char* configuration);

	const char* GetId() const { return _id.c_str(); }
	int GetTeam() const { return _team; }
	BattleCommanderType GetType() const { return _type; }
	const char* GetConfiguration() const { return _configuration.c_str(); }

	bool IsActive() const { return _isActive; }
	void SetActive(bool value) { _isActive = value; }
};


#endif
