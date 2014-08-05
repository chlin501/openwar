// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef BATTLEGESTURE_H
#define BATTLEGESTURE_H

#include "BattleModel/BattleSimulator.h"
#include "Algebra/bounds.h"
#include "Surface/Gesture.h"
#include "Surface/Touch.h"

class BattleView;
class UnitTrackingMarker;
class Unit;
class UnitCounter;


class BattleGesture : public Gesture
{
	BattleView* _battleView;

	bool _tappedUnitCenter;
	bool _tappedDestination;
	bool _tappedModiferArea;

	UnitTrackingMarker* _trackingMarker;

	Touch* _trackingTouch;
	Touch* _modifierTouch;

	float _offsetToMarker;
	bool _allowTargetEnemyUnit;

public:
	static bool disableUnitTracking;

	BattleGesture(BattleView* battleView);
	virtual ~BattleGesture();

	BattleView* GetBattleView() const { return _battleView; }

	virtual void Update(Surface* surface, double secondsSinceLastUpdate);
	virtual void RenderHints();

	virtual void TouchBegan(Touch* touch);
	virtual void TouchMoved();
	virtual void TouchEnded(Touch* touch);

	virtual void TouchWasCancelled(Touch* touch);

	//void UpdateTouchMarkers();

private:
	void UpdateTrackingMarker();

	int GetFlipSign() const;

	Unit* FindCommandableUnit(glm::vec2 screenPosition, glm::vec2 terrainPosition);
	Unit* FindCommandableUnitByCurrentPosition(glm::vec2 screenPosition, glm::vec2 terrainPosition);
	Unit* FindCommandableUnitByFuturePosition(glm::vec2 screenPosition, glm::vec2 terrainPosition);
	Unit* FindCommandableUnitByModifierArea(glm::vec2 screenPosition, glm::vec2 terrainPosition);

	Unit* FindEnemyUnit(glm::vec2 touchPosition, glm::vec2 markerPosition);

	bounds2f GetUnitCurrentBounds(Unit* unit);
	bounds2f GetUnitFutureBounds(Unit* unit);
	bounds2f GetUnitModifierBounds(Unit* unit);

	bool IsInsideUnitModifierArea(Unit* unit, glm::vec2 position);
};


#endif
