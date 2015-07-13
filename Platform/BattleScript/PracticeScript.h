#ifndef PracticeScript_H
#define PracticeScript_H

#include "BattleScript.h"

class BattleSimulator;


class PracticeScript : public BattleScript
{
	BattleSimulator* _simulator{};
	double _commandTimer{};
	int _waveNumber{};

public:
	PracticeScript(BattleSimulator* simulator);

	void Tick(double secondsSinceLastTick) override;

private:
	void SpawnWave();
	void IssueCommands();
};


#endif
