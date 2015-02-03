#ifndef MonkeyScript_H
#define MonkeyScript_H

#include "BattleScript.h"


class MonkeyScript : public BattleScript
{
	double _commandTimer{};

public:
	MonkeyScript(BattleScenario* scenario);
	virtual ~MonkeyScript();

	virtual void Tick(double secondsSinceLastTick);

private:
	void IssueCommands();
};


#endif
