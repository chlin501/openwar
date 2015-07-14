#ifndef BattleObserver_H
#define BattleObserver_H

#include "BattleObjects_v1.h"


class BattleObserver
{
	friend class BattleSimulator_v1_0_0;
public:
	virtual ~BattleObserver();

private:
	virtual void OnAddUnit(BattleObjects::Unit* unit) = 0;
	virtual void OnRemoveUnit(BattleObjects::Unit* unit) = 0;
	virtual void OnCommand(BattleObjects::Unit* unit, float timer) = 0;
	virtual void OnShooting(const BattleObjects::Shooting& shooting, float timer) = 0;
	virtual void OnRelease(const BattleObjects::Shooting& shooting) = 0;
	virtual void OnCasualty(const BattleObjects_v1::Fighter& fighter) = 0;
	virtual void OnRouting(BattleObjects::Unit* unit) = 0;
};


#endif
