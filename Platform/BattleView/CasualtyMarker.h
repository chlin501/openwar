// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef CasualtyMarker_H
#define CasualtyMarker_H

#include <cstdlib>
#include "BattleModel/BattleSimulator.h"
#include "BattleModel/SamuraiModule.h"

class ColorBillboardShape;
class BillboardModel;


class CasualtyMarker
{
public:
	struct Casualty
	{
		glm::vec3 position;
		int team;
		SamuraiPlatform platform;
		float time;
		int seed;

		Casualty(glm::vec3 position_, int team_, SamuraiPlatform platform_) :
		position(position_),
		team(team_),
		platform(platform_),
		time(0),
		seed(std::rand() & 0x7fff)
		{ }
	};

	std::vector<Casualty> casualties;
	BattleSimulator* _battleSimulator;

public:
	CasualtyMarker(BattleSimulator* battleSimulator);
	~CasualtyMarker();

	void AddCasualty(glm::vec3 position, int team, SamuraiPlatform platform);
	bool Animate(float seconds);

	void RenderCasualtyColorBillboards(ColorBillboardShape* renderer);
	void AppendCasualtyBillboards(BillboardModel* billboardModel);
};


#endif
