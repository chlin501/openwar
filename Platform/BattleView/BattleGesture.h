// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef BATTLEGESTURE_H
#define BATTLEGESTURE_H

#include "BattleModel/BattleSimulator_v1_0_0.h"
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

	BattleObjects::Unit* FindCommandableUnit(glm::vec2 screenPosition, glm::vec2 terrainPosition);
	BattleObjects::Unit* FindCommandableUnitByCurrentPosition(glm::vec2 screenPosition, glm::vec2 terrainPosition);
	BattleObjects::Unit* FindCommandableUnitByFuturePosition(glm::vec2 screenPosition, glm::vec2 terrainPosition);
	BattleObjects::Unit* FindCommandableUnitByModifierArea(glm::vec2 screenPosition, glm::vec2 terrainPosition);

	BattleObjects::Unit* FindEnemyUnit(glm::vec2 touchPosition, glm::vec2 markerPosition);

	bounds2f GetUnitCurrentBounds(BattleObjects::Unit* unit);
	bounds2f GetUnitFutureBounds(BattleObjects::Unit* unit);
	bounds2f GetUnitModifierBounds(BattleObjects::Unit* unit);

private: // BattleObserver
	void OnAddUnit(BattleObjects::Unit* unit) override;
	void OnRemoveUnit(BattleObjects::Unit* unit) override;
	void OnCommand(BattleObjects::Unit* unit, float timer) override;
	void OnShooting(const BattleObjects::Shooting& shooting, float timer) override;
	void OnRelease(const BattleObjects::Shooting& shooting) override;
	void OnCasualty(BattleObjects::Unit* unit, glm::vec2 fighter) override;
	void OnRouting(BattleObjects::Unit* unit) override;
};


#endif
