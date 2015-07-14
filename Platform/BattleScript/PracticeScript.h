#ifndef PracticeScript_H
#define PracticeScript_H

#include "BattleScript.h"

class BattleSimulator_v1_0_0;


class PracticeScript : public BattleScript
{
	BattleSimulator_v1_0_0* _simulator{};
	double _commandTimer{};
	int _waveNumber{};

public:
	PracticeScript(BattleSimulator_v1_0_0* simulator);

	void Tick(double secondsSinceLastTick) override;

private:
	void SpawnWave();
	void IssueCommands();
};


#endif
