#ifndef MonkeyScript_H
#define MonkeyScript_H

#include "BattleScript.h"

class BattleSimulator_v1_0_0;


class MonkeyScript : public BattleScript
{
	BattleSimulator_v1_0_0* _simulator{};
	double _commandTimer{};

public:
	MonkeyScript(BattleSimulator_v1_0_0* simulator);

	void Tick(double secondsSinceLastTick) override;

private:
	void IssueCommands();
};


#endif
