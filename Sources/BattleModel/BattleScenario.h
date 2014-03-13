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
	std::string _smoothMapName;
	image* _smoothMap;

public:
	BattleScenario();
	~BattleScenario();

	BattleSimulator* GetSimulator() const { return _simulator; }

	void SetScript(BattleScript* value);
	BattleScript* GetScript() const;

	BattleCommander* AddCommander(const char* id, int team, BattleCommanderType type, const char* configuration);
	BattleCommander* GetCommander(const char* id) const;
	const std::vector<BattleCommander*>& GetCommanders() const { return _commanders; }


	void StartScript(bool master);

	BattleOutcome GetOutcome(int team) const;

	void SetSmoothMap(const char* name, float size);
	void SetTiledMap(int x, int y);

	void Tick(double secondsSinceLastTick);
};



#endif
