#include "BattleSimulator.h"
#include "BattleObserver.h"


BattleSimulator::BattleSimulator()
{
}


BattleSimulator::~BattleSimulator()
{
}


void BattleSimulator::AddObserver(BattleObserver* observer)
{
	_observers.insert(observer);

	for (BattleObjects::Unit* unit : GetUnits())
	{
		observer->OnAddUnit(unit);
		observer->OnCommand(unit, 0);
	}
}


void BattleSimulator::RemoveObserver(BattleObserver* observer)
{
	_observers.erase(observer);
}


void BattleSimulator::NotifyAddUnit(BattleObjects::Unit* unit)
{
	for (BattleObserver* observer : _observers)
		observer->OnAddUnit(unit);
}


void BattleSimulator::NotifyRemoveUnit(BattleObjects::Unit* unit)
{
	for (BattleObserver* observer : _observers)
		observer->OnRemoveUnit(unit);
}


void BattleSimulator::NotifyCommand(BattleObjects::Unit* unit, float timer)
{
	for (BattleObserver* observer : _observers)
		observer->OnCommand(unit, timer);
}


void BattleSimulator::NotifyShooting(const BattleObjects::Shooting& shooting, float timer)
{
	for (BattleObserver* observer : _observers)
		observer->OnShooting(shooting, timer);
}


void BattleSimulator::NotifyRelease(const BattleObjects::Shooting& shooting)
{
	for (BattleObserver* observer : _observers)
		observer->OnRelease(shooting);
}


void BattleSimulator::NotifyCasualty(BattleObjects::Unit* unit, glm::vec2 fighter)
{
	for (BattleObserver* observer : _observers)
		observer->OnCasualty(unit, fighter);
}


void BattleSimulator::NotifyRouting(BattleObjects::Unit* unit)
{
	for (BattleObserver* observer : _observers)
		observer->OnRouting(unit);
}
