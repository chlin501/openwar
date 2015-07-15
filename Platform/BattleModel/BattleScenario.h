#ifndef BattleScenario_H
#define BattleScenario_H

#include <glm/glm.hpp>

class BattleSimulator;


class BattleScenario
{
	BattleSimulator* _battleSimulator;

	std::pair<glm::vec2, float> _deploymentZones[2]{};
	float _deploymentTimer{};
	bool _deploymentEnabled{};

public:
	BattleScenario(BattleSimulator* battleSimulator);
	virtual ~BattleScenario();

	BattleSimulator* GetBattleSimulator() const { return _battleSimulator; }

	void Tick(float secondsSinceLastTick);

	void EnableDeploymentZones(float deploymentTimer);
	std::pair<glm::vec2, float> GetDeploymentZone(int team) const;
	bool IsDeploymentZone(int team, glm::vec2 position) const;
	glm::vec2 ConstrainDeploymentZone(int team, glm::vec2 position, float inset) const;

private:
	void UpdateDeploymentZones(double secondsSinceLastTick);
	void SetDeploymentZone(int team, glm::vec2 center, float radius);
	bool HasCompletedDeployment(int team) const;
};


#endif
