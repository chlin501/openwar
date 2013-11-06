// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef BattleScript_H
#define BattleScript_H

#include <map>
#include "BattleSimulator.h"
#include "lua.hpp"


class BattleScript : public BattleObserver
{
	struct UnitStatus
	{
		glm::vec2 position;
		float heading;
		int state;
		int strength;
		float morale;

		UnitStatus(Unit* unit);
	};

	BattleSimulator* _simulator;
	std::map<int, Unit*> _units;
	std::map<Unit*, int> _unitId;
	lua_State* _state;
	int _nextUnitId;

public:
	BattleScript(BattleSimulator* simulator);
	virtual ~BattleScript();

	void SetGlobalNumber(const char* name, double value);
	void SetGlobalString(const char* name, const char* value);
	void AddStandardPath();
	void AddPackagePath(const char* path);
	void Execute(const char* script, size_t length);

	BattleSimulator* GetSimulator() const { return _simulator; }

	void CreateSimulator();

	void Tick(double secondsSinceLastTick);

	// BattleObserver
	virtual void OnAddUnit(Unit* unit);
	virtual void OnRemoveUnit(Unit* unit);
	virtual void OnShooting(const Shooting& shooting);
	virtual void OnCasualty(const Fighter& fighter);

private:
	int NewUnit(int player, int team, const char* unitClass, int strength, glm::vec2 position, float bearing);
	void SetUnitMovement(int unitId, bool running, std::vector<glm::vec2> path, int chargeId, float heading);

	static int openwar_terrain_init(lua_State* L);
	static int openwar_simulator_init(lua_State* L);

	static int battle_message(lua_State* L);
	static int battle_get_time(lua_State* L);
	static int battle_new_unit(lua_State* L);
	static int battle_set_unit_movement(lua_State* L);
	static int battle_get_unit_status(lua_State* L);

	static int battle_set_terrain_tile(lua_State* L);
	static int battle_set_terrain_height(lua_State* L);
	static int battle_add_terrain_tree(lua_State* L);

	static void ToPath(std::vector<glm::vec2>& result, lua_State* L, int index);
};


#endif
