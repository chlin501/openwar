#ifndef BattleScenario_H
#define BattleScenario_H

#include <glm/glm.hpp>

#include "BattleCommander.h"
#include "BattleObjects.h"

class BattleSimulator;
class BattleScript;


class BattleScenario
{
	BattleSimulator* _battleSimulator{};
	BattleScript* _battleScript{};

	std::vector<BattleCommander*> _commanders{};
	BattleCommander* _dummyCommander{};
	int _teamPosition1{};
	int _teamPosition2{};

	std::pair<glm::vec2, float> _deploymentZones[2]{};
	float _executionTime{};
	bool _deploymentEnabled{};

	bool _practice{};
	int _winnerTeam{};

public:
	BattleScenario(BattleSimulator* battleSimulator, float executionTime);
	virtual ~BattleScenario();

	BattleSimulator* GetBattleSimulator() const { return _battleSimulator; }

	void SetBattleScript(BattleScript* battleScript);

	void Tick(float secondsSinceLastTick);

	BattleCommander* AddCommander(const char* playerId, int team, BattleCommanderType type);
	BattleCommander* GetCommander(const char* playerId) const;
	const std::vector<BattleCommander*>& GetCommanders() const { return _commanders; }
	BattleCommander* GetDummyCommander() const;

	BattleCommander* GetCommanderForUnit(const BattleObjects::Unit* unit) const { return unit->commander; }
	bool IsFriendlyCommander(const BattleObjects::Unit* unit, BattleCommander* battleCommander) const;
	bool IsCommandableBy(const BattleObjects::Unit* unit, BattleCommander* battleCommander) const;

	void SetTeamPosition(int team, int position);
	int GetTeamPosition(int team) const;

	void EnableDeploymentZones();
	std::pair<glm::vec2, float> GetDeploymentZone(int team) const;
	bool IsDeploymentZone(int team, glm::vec2 position) const;
	glm::vec2 ConstrainDeploymentZone(int team, glm::vec2 position, float inset) const;

	void SetPractice(bool value) { _practice = value; }
	int GetWinnerTeam() const { return _winnerTeam; }

private:
	void UpdateDeploymentZones(float secondsSinceLastTick);
	void SetDeploymentZone(int team, glm::vec2 center, float radius);
	bool HasCompletedDeployment(int team) const;
};


#endif
