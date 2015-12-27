#ifndef PracticeScript_H
#define PracticeScript_H

#include "BattleScript.h"

class BattleScenario;
class BattleSimulator;


class PracticeScript : public BattleScript
{
	BattleScenario* _battleScenario{};
	BattleSimulator* _battleSimulator{};
	double _commandTimer{};
	int _waveNumber{};

public:
	PracticeScript(BattleScenario* battleScenario);

	void Tick(double secondsSinceLastTick) override;

private:
	void SpawnWave();
	void IssueCommands();
};


#endif