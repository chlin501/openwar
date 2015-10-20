// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef SmokeMarker_H
#define SmokeMarker_H

#include "BattleModel/BattleSimulator_v1_0_0.h"

class BillboardModel;


class SmokeCounter
{
public:
	struct Particle
	{
		glm::vec3 position{};
		glm::vec3 velocity{};
		float time{};
	};

	BattleObjects::MissileType _missileType{};
	std::vector<Particle> particles{};
	int _soundCookie{};
	bool _impacted{};

public:
	SmokeCounter(BattleObjects::MissileType missileType);
	~SmokeCounter();

	bool Animate(float seconds);
	void AddParticle(glm::vec3 position1, glm::vec3 position2, float delay);
	void AppendSmokeBillboards(BillboardModel* billboardModel);
};


#endif
