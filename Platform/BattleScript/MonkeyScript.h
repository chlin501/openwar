#ifndef MonkeyScript_H
#define MonkeyScript_H

#include "BattleScript.h"

class BattleSimulator;


class MonkeyScript : public BattleScript
{
	BattleSimulator* _simulator{};
	double _commandTimer{};

public:
	MonkeyScript(BattleSimulator* simulator);

	void Tick(double secondsSinceLastTick) override;

private:
	void IssueCommands();
};


#endif
