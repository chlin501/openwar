// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef BATTLEGESTURE_H
#define BATTLEGESTURE_H

#include "BattleModel/BattleSimulator.h"
#include "Algebra/bounds.h"
#include "Surface/Gesture.h"
#include "Surface/Touch.h"

class BattleHotspot;
class BattleView;
class UnitTrackingMarker;
class Unit;
class UnitCounter;


class BattleGesture : public Gesture
{
	BattleHotspot* _hotspot{};

	bool _tappedUnitCenter{};
	bool _tappedDestination{};
	bool _tappedModiferArea{};

	UnitTrackingMarker* _trackingMarker{};

	Touch* _trackingTouch{};
	Touch* _modifierTouch{};

	float _offsetToMarker{};
	bool _allowTargetEnemyUnit{};

public:
	BattleGesture(BattleHotspot* hotspot);
	virtual ~BattleGesture();

	virtual void TouchWasCaptured(Touch* touch);
	virtual void TouchWillBeReleased(Touch* touch);

	virtual void TouchBegan(Touch* touch);
	virtual void TouchMoved(Touch* touch);
	virtual void TouchEnded(Touch* touch);

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
};


#endif
