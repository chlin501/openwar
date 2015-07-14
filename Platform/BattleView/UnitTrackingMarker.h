// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef UnitTrackingMarker_H
#define UnitTrackingMarker_H

#include "UnitMarker.h"
#include "BattleSimulator_v1_0_0.h"
#include "Shapes/VertexShape.h"

class BattleView;
class BillboardTextureShape;


class UnitTrackingMarker : public UnitMarker
{
	BattleObjects_v1::Unit* _meleeTarget{};
	glm::vec2 _destination{};
	bool _hasDestination{};

	BattleObjects_v1::Unit* _missileTarget{};
	glm::vec2 _orientation{};
	bool _hasOrientation{};
	bool _renderOrientation{};

	bool _running{};

public:
	std::vector<glm::vec2> _path{};

public:
	UnitTrackingMarker(BattleView* battleView, BattleObjects_v1::Unit* unit);
	~UnitTrackingMarker();

	void SetRunning(bool value) { _running = value; }
	bool GetRunning() const { return _running; }

	void SetMeleeTarget(BattleObjects_v1::Unit* value) { _meleeTarget = value; }
	BattleObjects_v1::Unit* GetMeleeTarget() const { return _meleeTarget; }

	void SetMissileTarget(BattleObjects_v1::Unit* value) { _missileTarget = value; }
	BattleObjects_v1::Unit* GetMissileTarget() const { return _missileTarget; }

	/***/


	void SetDestination(glm::vec2* value)
	{
		if (value) _destination = *value;
		_hasDestination = value !=  nullptr;
	}

	void SetOrientation(glm::vec2* value)
	{
		if (value) _orientation = *value;
		_hasOrientation = value;
	}

	void SetRenderOrientation(bool value) { _renderOrientation = value; }

	glm::vec2* GetOrientationX()
	{
		if (_missileTarget) return &_missileTarget->state.center;
		else if (_hasOrientation) return &_orientation;
		else return nullptr;
	}

	glm::vec2 DestinationXXX() const
	{
		return GetMeleeTarget() ? GetMeleeTarget()->GetCenter()
			: _path.size() != 0 ? *(_path.end() - 1)
			: _hasDestination ? _destination
			: GetUnit()->GetCenter();
	}


	float GetFacing() const;

	void RenderTrackingFighters(VertexShape_3f_4f_1f* vertices);
	void RenderTrackingMarker(BillboardTextureShape* renderer);
	void AppendFacingMarker(VertexShape_2f_2f* vertices, BattleView* battleView);
	void RenderTrackingShadow(BillboardTextureShape* renderer);
	void RenderTrackingPath(VertexShape_3f_4f* vertices);
	void RenderOrientation(VertexShape_3f_4f* vertices);
};


#endif
