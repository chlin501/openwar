#ifndef BattleObserver_H
#define BattleObserver_H

struct Fighter;
struct Unit;
struct Shooting;


class BattleObserver
{
	friend class BattleSimulator;
public:
	virtual ~BattleObserver();

private:
	virtual void OnAddUnit(Unit* unit) = 0;
	virtual void OnRemoveUnit(Unit* unit) = 0;
	virtual void OnCommand(Unit* unit, float timer) = 0;
	virtual void OnShooting(const Shooting& shooting, float timer) = 0;
	virtual void OnRelease(const Shooting& shooting) = 0;
	virtual void OnCasualty(const Fighter& fighter) = 0;
	virtual void OnRouting(Unit* unit) = 0;
};


#endif
