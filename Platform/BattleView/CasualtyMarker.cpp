// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "CasualtyMarker.h"
#include "Shapes/BillboardColorShader.h"
#include "Shapes/BillboardTextureShape.h"
#include "BattleMap/BattleMap.h"
#include "BattleMap/SmoothGroundMap.h"



CasualtyMarker::CasualtyMarker(BattleSimulator_v1_0_0* battleSimulator) :
	_battleSimulator{battleSimulator}
{
}


CasualtyMarker::~CasualtyMarker()
{
}


void CasualtyMarker::AddCasualty(glm::vec3 position, int team, SamuraiPlatform platform)
{
	casualties.push_back(Casualty(position, team, platform));
}


bool CasualtyMarker::Animate(float seconds)
{
	for (Casualty& casualty : casualties)
		casualty.time += seconds;

	return true;
}



void CasualtyMarker::RenderCasualtyColorBillboards(VertexShape_3f_4f_1f* vertices)
{
	if (casualties.empty())
		return;

	glm::vec4 c1 = glm::vec4(1, 1, 1, 0.8);
	glm::vec4 cr = glm::vec4(1, 0, 0, 0);
	glm::vec4 cb = glm::vec4(0, 0, 1, 0);

	for (const CasualtyMarker::Casualty& casualty : casualties)
	{
		if (casualty.time <= 1)
		{
			glm::vec4 c = glm::mix(c1, casualty.team == 1 ? cb : cr, casualty.time);
			vertices->AddVertex(Vertex_3f_4f_1f(casualty.position, c, 6.0));
		}
	}
}


void CasualtyMarker::AppendCasualtyBillboards(BillboardModel* billboardModel)
{
	for (const CasualtyMarker::Casualty& casualty : casualties)
	{
		int shape = 0;
		float height = 0;
		//int j = 0, i = 0;
		switch (casualty.platform)
		{
			case SamuraiPlatform_Ash:
				shape = billboardModel->_billboardShapeCasualtyAsh[casualty.seed & 7];
				height = 2.5f;
				//i = 3;
				//j = casualty.seed & 3;
				break;
			case SamuraiPlatform_Sam:
				shape = billboardModel->_billboardShapeCasualtySam[casualty.seed & 7];
				height = 2.5f;
				//i = 3;
				//j = 4 + (casualty.seed & 3);
				break;
			case SamuraiPlatform_Cav:
			case SamuraiPlatform_Gen:
				shape = billboardModel->_billboardShapeCasualtySam[casualty.seed & 15];
				height = 3.0f;
				//i = 4;
				//j = casualty.seed & 7;
				break;
		}

		const float adjust = 0.5 - 2.0 / 64.0; // place texture 2 texels below ground
		glm::vec3 p = _battleSimulator->GetBattleMap()->GetHeightMap()->GetPosition(casualty.position.xy(), adjust * height);
		billboardModel->dynamicBillboards.push_back(Billboard(p, 0, height, shape));

	}
}
