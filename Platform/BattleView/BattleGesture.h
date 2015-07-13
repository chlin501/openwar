// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef BATTLEGESTURE_H
#define BATTLEGESTURE_H

#include "BattleModel/BattleSimulator.h"
#include "BattleModel/BattleObserver.h"
#include "Algebra/bounds.h"
#include "Surface/Gesture.h"
#include "Surface/Touch.h"

class BattleHotspot;
class BattleView;
class UnitTrackingMarker;
class UnitCounter;


class BattleGesture : public Gesture, BattleObserver
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

	void TouchWasCaptured(Touch* touch) override;
	void TouchWillBeReleased(Touch* touch) override;

	void TouchBegan(Touch* touch) override;
	void TouchMoved(Touch* touch) override;
	void TouchEnded(Touch* touch) override;

private:
	void UpdateTrackingMarker();

	int GetFlipSign() const;

	BattleObjects_v1::Unit* FindCommandableUnit(glm::vec2 screenPosition, glm::vec2 terrainPosition);
	BattleObjects_v1::Unit* FindCommandableUnitByCurrentPosition(glm::vec2 screenPosition, glm::vec2 terrainPosition);
	BattleObjects_v1::Unit* FindCommandableUnitByFuturePosition(glm::vec2 screenPosition, glm::vec2 terrainPosition);
	BattleObjects_v1::Unit* FindCommandableUnitByModifierArea(glm::vec2 screenPosition, glm::vec2 terrainPosition);

	BattleObjects_v1::Unit* FindEnemyUnit(glm::vec2 touchPosition, glm::vec2 markerPosition);

	bounds2f GetUnitCurrentBounds(BattleObjects_v1::Unit* unit);
	bounds2f GetUnitFutureBounds(BattleObjects_v1::Unit* unit);
	bounds2f GetUnitModifierBounds(BattleObjects_v1::Unit* unit);

private: // BattleObserver
	void OnAddUnit(BattleObjects_v1::Unit* unit) override;
	void OnRemoveUnit(BattleObjects_v1::Unit* unit) override;
	void OnCommand(BattleObjects_v1::Unit* unit, float timer) override;
	void OnShooting(const BattleObjects_v1::Shooting& shooting, float timer) override;
	void OnRelease(const BattleObjects_v1::Shooting& shooting) override;
	void OnCasualty(const BattleObjects_v1::Fighter& fighter) override;
	void OnRouting(BattleObjects_v1::Unit* unit) override;
};


#endif
