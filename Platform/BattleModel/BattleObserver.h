#ifndef BattleObserver_H
#define BattleObserver_H

#include "BattleObjects_v1.h"


class BattleObserver
{
	friend class BattleSimulator_v1_0_0;
public:
	virtual ~BattleObserver();

private:
	virtual void OnAddUnit(BattleObjects_v1::Unit* unit) = 0;
	virtual void OnRemoveUnit(BattleObjects_v1::Unit* unit) = 0;
	virtual void OnCommand(BattleObjects_v1::Unit* unit, float timer) = 0;
	virtual void OnShooting(const BattleObjects_v1::Shooting& shooting, float timer) = 0;
	virtual void OnRelease(const BattleObjects_v1::Shooting& shooting) = 0;
	virtual void OnCasualty(const BattleObjects_v1::Fighter& fighter) = 0;
	virtual void OnRouting(BattleObjects_v1::Unit* unit) = 0;
};


#endif
