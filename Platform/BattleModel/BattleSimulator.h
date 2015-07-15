#ifndef BattleSimulator_H
#define BattleSimulator_H

#include "BattleObjects.h"

class BattleObserver;


class BattleSimulator : public virtual BattleObjects
{
protected:
	std::set<BattleObserver*> _observers{};

public:
	BattleSimulator();
	virtual ~BattleSimulator();

	void AddObserver(BattleObserver* observer);
	void RemoveObserver(BattleObserver* observer);

	float GetTimerDelay() const { return 0.25f; }

	virtual void AdvanceTime(float secondsSinceLastTime) = 0;

protected:
	void NotifyAddUnit(BattleObjects::Unit* unit);
	void NotifyRemoveUnit(BattleObjects::Unit* unit);
	void NotifyCommand(BattleObjects::Unit* unit, float timer);
	void NotifyShooting(const BattleObjects::Shooting& shooting, float timer);
	void NotifyRelease(const BattleObjects::Shooting& shooting);
	void NotifyCasualty(BattleObjects::Unit* unit, glm::vec2 fighter);
	void NotifyRouting(BattleObjects::Unit* unit);
};


#endif
