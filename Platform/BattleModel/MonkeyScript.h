#ifndef MonkeyScript_H
#define MonkeyScript_H

#include "BattleScript.h"


class MonkeyScript : public BattleScript
{
	double _commandTimer{};

public:
	MonkeyScript(BattleSimulator* simulator);
	virtual ~MonkeyScript();

	virtual void Tick(double secondsSinceLastTick);

private:
	void IssueCommands();
};


#endif
