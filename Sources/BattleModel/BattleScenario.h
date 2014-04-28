#ifndef BattleScenario_H
#define BattleScenario_H

#include <string>
#include <vector>
#include "../Library/Algebra/image.h"
#include "BattleCommander.h"

class BattleScript;
class BattleSimulator;


enum class BattleOutcome
{
	None,
	Won,
	Lost,
	Draw
};


class BattleScenario
{
	BattleSimulator* _simulator;
	BattleScript* _script;
	std::vector<BattleCommander*> _commanders;
	std::string _smoothMapHash;
	image* _smoothMap;
	int _teamStartPosition1;
	int _teamStartPosition2;

public:
	BattleScenario();
	~BattleScenario();

	BattleSimulator* GetSimulator() const { return _simulator; }

	void SetScript(BattleScript* value);
	BattleScript* GetScript() const;

	void SetTeamStartPosition(int team, int position);
	int GetTeamStartPosition(int team) const;

	BattleCommander* AddCommander(const char* playerId, int team, BattleCommanderType type);
	BattleCommander* GetCommander(const char* playerId) const;
	const std::vector<BattleCommander*>& GetCommanders() const { return _commanders; }


	void StartScript(bool master);

	BattleOutcome GetOutcome(int team) const;

	void SetSmoothMap(const char* path, const char* hash, float size);
	void SetTiledMap(int x, int y);

	void Tick(double secondsSinceLastTick);
};



#endif
