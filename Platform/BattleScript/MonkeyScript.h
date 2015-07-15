#ifndef MonkeyScript_H
#define MonkeyScript_H

#include "BattleScript.h"

class BattleScenario;
class BattleSimulator;


class MonkeyScript : public BattleScript
{
	BattleScenario* _battleScenario{};
	BattleSimulator* _battleSimulator{};
	double _commandTimer{};

public:
	MonkeyScript(BattleScenario* battleSimulator);

	void Tick(double secondsSinceLastTick) override;

private:
	void IssueCommands();
};


#endif
