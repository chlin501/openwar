// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include <cstring>
#include <glm/gtc/constants.hpp>
#include "BattleScript.h"
#include "HeightMap.h"
#include "SamuraiModule.h"
#include "SmoothGroundMap.h"
#include "TiledGroundMap.h"
#include "BattleScenario.h"


std::map<lua_State*, BattleScript*> BattleScript::_scripts;


static void print_log(const char* operation, const char* message)
{
#ifdef OPENWAR_USE_NSBUNDLE_RESOURCES
	NSLog(@"BattleScript (%s):\n%s", operation, message);
#endif
}



BattleScript::BattleScript(BattleScenario* scenario) :
_scenario(scenario),
_simulator(scenario->GetSimulator()),
_state(nullptr),
_nextUnitId(1)
{
	_state = luaL_newstate();

	luaL_openlibs(_state);

	lua_pushcfunction(_state, openwar_init_groundmap);
	lua_setglobal(_state, "openwar_init_groundmap");


	lua_pushcfunction(_state, battle_message);
	lua_setglobal(_state, "battle_message");

	lua_pushcfunction(_state, battle_get_time);
	lua_setglobal(_state, "battle_get_time");

	lua_pushcfunction(_state, battle_new_unit);
	lua_setglobal(_state, "battle_new_unit");

	lua_pushcfunction(_state, battle_set_unit_movement);
	lua_setglobal(_state, "battle_set_unit_movement");

	lua_pushcfunction(_state, battle_get_unit_status);
	lua_setglobal(_state, "battle_get_unit_status");

	lua_pushcfunction(_state, battle_set_terrain_tile);
	lua_setglobal(_state, "battle_set_terrain_tile");

	lua_pushcfunction(_state, battle_set_terrain_height);
	lua_setglobal(_state, "battle_set_terrain_height");

	lua_pushcfunction(_state, battle_add_terrain_tree);
	lua_setglobal(_state, "battle_add_terrain_tree");

	_scripts[_state] = this;
}


BattleScript::~BattleScript()
{
	lua_close(_state);
	_scripts.erase(_state);
}


void BattleScript::AddPackagePath(const char* path)
{
	lua_getglobal(_state, "package");
	if (lua_isnil(_state, -1))
	{
		lua_pop(_state, 1);
		return;
	}

	lua_pushstring(_state, "path");
	lua_gettable(_state, -2);
	if (lua_isnil(_state, -1))
	{
		lua_pop(_state, 2); // package & path
		return;
	}

	std::string s(lua_tostring(_state, -1));
	lua_pop(_state, 1);

	s.append(";");
	s.append(path);

	lua_pushstring(_state, "path");
	lua_pushstring(_state, s.c_str());
	lua_settable(_state, -3);

	lua_pop(_state, 1); // package
}


void BattleScript::Execute(const char* script, size_t length)
{
	if (script != nullptr)
	{
		int error = luaL_loadbuffer(_state, script, length, "line");
		if (!error)
            error = lua_pcall(_state, 0, 0, 0);

		if (error)
		{
			print_log("ERROR", lua_tostring(_state, -1));
			lua_pop(_state, 1);  /* pop error message from the stack */
		}
	}
}


void BattleScript::Tick(double secondsSinceLastUpdate)
{
	lua_getglobal(_state, "openwar_battle_tick");

	if (lua_isnil(_state, -1))
	{
		lua_pop(_state, 1);
	}
	else
	{
		lua_pushnumber(_state, secondsSinceLastUpdate);
		int error = lua_pcall(_state, 1, 0, 0);
		if (error)
		{
			print_log("ERROR", lua_tostring(_state, -1));
			lua_pop(_state, 1);  /* pop error message from the stack */
		}
	}

	if (_simulator != nullptr)
	{
		_simulator->AdvanceTime((float)secondsSinceLastUpdate);
	}
}


/* BattleObserver */


void BattleScript::OnSetGroundMap(GroundMap* groundMap)
{

}


void BattleScript::OnAddUnit(Unit* unit)
{
	int unitId = _nextUnitId++;
	_units[unitId] = unit;
	_unitId[unit] = unitId;
}


void BattleScript::OnRemoveUnit(Unit* unit)
{
	int unitId = _unitId[unit];
	_units.erase(unitId);
	_unitId.erase(unit);
}


void BattleScript::OnShooting(const Shooting &shooting)
{

}


void BattleScript::OnCasualty(const Fighter& fighter)
{

}


/***/



int BattleScript::NewUnit(int player, int team, const char* unitClass, int strength, glm::vec2 position, float bearing)
{
	UnitStats unitStats = SamuraiModule::GetDefaultUnitStats(unitClass);

	Unit* unit = _simulator->AddUnit(player, team, unitClass, strength, unitStats, position);
	unit->command.bearing = glm::radians(90 - bearing);

	return _unitId[unit];
}


void BattleScript::SetUnitMovement(int unitId, bool running, std::vector<glm::vec2> path, int chargeId, float heading)
{
	Unit* unit = _units[unitId];
	if (unit != nullptr)
	{
		unit->command.path = path;
		unit->command.bearing = heading;
		unit->command.meleeTarget = _units[chargeId];
		unit->command.running = running;
	}
}


/***/



int BattleScript::openwar_init_groundmap(lua_State* L)
{
	BattleScript* script = _scripts[L];

	int n = lua_gettop(L);
	const char* s = n < 1 ? nullptr : lua_tostring(L, 1);

	if (s != nullptr && std::strcmp(s, "smooth") == 0)
	{
		const char* name = n < 2 ? nullptr : lua_tostring(L, 2);
		float size = n < 3 ? 1024 : (float)lua_tonumber(L, 3);

		script->_scenario->SetSmoothMap(name, size);
	}
	else if (s != nullptr && std::strcmp(s, "tiled") == 0)
	{
		int nx = n < 2 ? 0 : (int)lua_tonumber(L, 2);
		int ny = n < 3 ? 0 : (int)lua_tonumber(L, 3);

		script->_scenario->SetTiledMap(nx, ny);
	}

	return 0;
}


int BattleScript::battle_message(lua_State* L)
{
	int n = lua_gettop(L);

	const char* s = n < 1 ? nullptr : lua_tostring(L, 1);
	if (s != nullptr)
		print_log("MESSAGE", s);

	return 0;
}


int BattleScript::battle_get_time(lua_State* L)
{
	lua_pushnumber(L, 47.62);
	return 1;
}


int BattleScript::battle_new_unit(lua_State* L)
{
	BattleScript* script = _scripts[L];

	int n = lua_gettop(L);
	int player = n < 1 ? 0 : (int)lua_tonumber(L, 1);
	const char* platform = n < 2 ? "" : lua_tostring(L, 2);
	const char* weapon = n < 3 ? "" : lua_tostring(L, 3);
	int strength = n < 4 ? 40 : (int)lua_tonumber(L, 4);
	float x = n < 5 ? 512 : (float)lua_tonumber(L, 5);
	float y = n < 6 ? 512 : (float)lua_tonumber(L, 6);
	float b = n < 7 ? 0 :  (float)lua_tonumber(L, 7);

	std::string unitClass = std::string(platform) + "-" + weapon;

	int unitId = script->NewUnit(player, player, unitClass.c_str(), strength, glm::vec2(x, y), b);

	lua_pushnumber(L, unitId);

	return 1;
}


int BattleScript::battle_set_unit_movement(lua_State* L)
{
	BattleScript* script = _scripts[L];

	int n = lua_gettop(L);
	int unitId = n < 1 ? 0 : (int)lua_tonumber(L, 1);
	bool running = n < 2 ? false : lua_toboolean(L, 2);
	std::vector<glm::vec2> path;
	if (n >= 3) ToPath(path, L, 3);
	int chargeId = n < 4 ? 0 : (int)lua_tonumber(L, 4);
	float heading = n < 5 ? 0 : (float)lua_tonumber(L, 5);

	script->SetUnitMovement(unitId, running, path, chargeId, heading);

	return 0;
}


int BattleScript::battle_get_unit_status(lua_State* L)
{
	BattleScript* script = _scripts[L];

	int n = lua_gettop(L);
	int unitId = n < 1 ? 0 : (int)lua_tonumber(L, 1);

	Unit* unit = script->_units[unitId];
	if (unit != nullptr)
	{
		UnitStatus status(unit);
		lua_pushnumber(L, status.position.x);
		lua_pushnumber(L, status.position.y);
		lua_pushnumber(L, status.heading);
		lua_pushnumber(L, status.state);
		lua_pushnumber(L, status.strength);
		lua_pushnumber(L, status.morale);
		return 6;
	}

	return 0;
}


int BattleScript::battle_set_terrain_tile(lua_State* L)
{
	BattleScript* script = _scripts[L];

	TiledGroundMap* tiledGroundMap = dynamic_cast<TiledGroundMap*>(script->_simulator->GetGroundMap());
	if (tiledGroundMap != nullptr)
	{
		int n = lua_gettop(L);
		int x = n < 1 ? 0 : (int)lua_tonumber(L, 1);
		int y = n < 2 ? 0 : (int)lua_tonumber(L, 2);
		const char* texture = n < 3 ? nullptr : lua_tostring(L, 3);
		int rotate = n < 4 ? 0 : (int)lua_tonumber(L, 4);
		bool mirror = n < 5 ? 0 : lua_toboolean(L, 5);

		tiledGroundMap->SetTile(x, y, std::string(texture), rotate, mirror);
	}

	return 0;
}


int BattleScript::battle_set_terrain_height(lua_State* L)
{
	BattleScript* script = _scripts[L];

	TiledGroundMap* tiledGroundMap = dynamic_cast<TiledGroundMap*>(script->_simulator->GetGroundMap());
	if (tiledGroundMap != nullptr)
	{
		int n = lua_gettop(L);
		int x = n < 1 ? 0 : (int)lua_tonumber(L, 1);
		int y = n < 2 ? 0 : (int)lua_tonumber(L, 2);
		float h = n < 3 ? 0 : (float)lua_tonumber(L, 3);

		tiledGroundMap->SetHeight(x, y, h);
	}

	return 0;
}


int BattleScript::battle_add_terrain_tree(lua_State* L)
{
	/*
	int n = lua_gettop(L);
	float x = n < 1 ? 0 : (float)lua_tonumber(L, 1);
	float y = n < 2 ? 0 : (float)lua_tonumber(L, 2);

	_battlescript->_battleSimulator->terrainForest->AddTree(glm::vec2(x, y));
	*/

	return 0;
}


/***/



void BattleScript::ToPath(std::vector<glm::vec2>& result, lua_State* L, int index)
{
	int key = 1;
	while (true)
	{
		lua_rawgeti(L, index, key);
		if (!lua_istable(L, -1))
			break;

		lua_pushstring(L, "x");
		lua_gettable(L, -2);
		float x = (float)lua_tonumber(L, -1);
		lua_pop(L, 1);

		lua_pushstring(L, "y");
		lua_gettable(L, -2);
		float y = (float)lua_tonumber(L, -1);
		lua_pop(L, 1);

		result.push_back(glm::vec2(x, y));

		lua_pop(L, 1);

		++key;
	}
}


/***/



BattleScript::UnitStatus::UnitStatus(Unit* unit)
{
	position = unit->state.center;
	heading = 0;
	state = 0;
	strength = unit->fightersCount;
	morale = unit->state.morale;

	if (unit->state.IsRouting())
	{
		state = 3;
	}
	else if (unit->command.meleeTarget != nullptr)
	{
		state = 4;
	}
	else if (!unit->command.path.empty())
	{
		if (unit->command.running)
			state = 2;
		else
			state = 1;
	}

	// 0 = STANDING
	// 1 = WALKING
	// 2 = RUNNING
	// 3 = ROUTING
	// 4 = CHARGING
	// 5 = FIGHTING
	// 6 = SHOOTING
}
