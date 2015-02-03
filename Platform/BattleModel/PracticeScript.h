#ifndef PracticeScript_H
#define PracticeScript_H

#include "BattleScript.h"


class PracticeScript : public BattleScript
{
	double _commandTimer{};
	int _waveNumber{};

public:
	PracticeScript(BattleScenario* scenario);
	virtual ~PracticeScript();

	virtual void Execute();
	virtual void Tick(double secondsSinceLastTick);

	void SpawnWave();
	void IssueCommands();
};


#endif
