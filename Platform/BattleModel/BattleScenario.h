#ifndef BattleScenario_H
#define BattleScenario_H

class BattleSimulator;


class BattleScenario
{
	BattleSimulator* _battleSimulator;

public:
	BattleScenario(BattleSimulator* battleSimulator);
	virtual ~BattleScenario();

	BattleSimulator* GetBattleSimulator() const { return _battleSimulator; }

	void Tick(float secondsSinceLastTick);
};


#endif
