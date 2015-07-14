// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifdef PHALANX_TARGET_OS_IOS
#import <UIKit/UIKit.h>
#endif

#include "BattleMap/BattleMap.h"
#include "BattleSimulator_v1_0_0.h"
#include "Audio/SoundPlayer.h"
#include "BattleGesture.h"
#include "BattleView.h"
#include "UnitCounter.h"
#include "UnitMovementMarker.h"
#include "UnitTrackingMarker.h"
#include "Surface/Surface.h"
#include "BattleHotspot.h"
#include "TerrainView/TerrainViewport.h"


#define SNAP_TO_UNIT_TRESHOLD 22 // meters
#define KEEP_ORIENTATION_TRESHHOLD 40 // meters
#define MODIFIER_AREA_RADIUS_MIN 5.0f // meters
#define MODIFIER_AREA_RADIUS_MAX 25.0f // meters


BattleGesture::BattleGesture(BattleHotspot* hotspot) :
	_hotspot{hotspot}
{
}


BattleGesture::~BattleGesture()
{
	_hotspot->GetBattleView()->GetSimulator()->RemoveObserver(this);
}


void BattleGesture::TouchWasCaptured(Touch* touch)
{
	_hotspot->GetBattleView()->GetSimulator()->AddObserver(this);
}


void BattleGesture::TouchWillBeReleased(Touch* touch)
{
}


void BattleGesture::TouchBegan(Touch* touch)
{
	if (touch->IsCaptured() || _hotspot->HasCapturedTouch())
		return;

	bounds2f viewportBounds = (bounds2f)_hotspot->GetBattleView()->GetTerrainViewport().GetViewportBounds();
	if (!viewportBounds.contains(touch->GetCurrentPosition()))
		return;

	glm::vec2 screenPosition = touch->GetCurrentPosition();
	glm::vec2 terrainPosition = _hotspot->GetBattleView()->GetTerrainPosition3(screenPosition).xy();
	BattleObjects::Unit* unit = FindCommandableUnit(screenPosition, terrainPosition);

	if (_trackingTouch == nullptr)
	{
		if (unit == nullptr)
			return;

		if (unit && _hotspot->GetBattleView()->GetTrackingMarker(unit) == nullptr)
		{
			const BattleObjects::UnitCommand command = unit->GetIssuedCommand();

			_allowTargetEnemyUnit = unit->GetMissileWeaponRange().maximumRange > 0;
			_trackingMarker = _hotspot->GetBattleView()->AddTrackingMarker(unit);

			float distanceToUnitCenter = glm::distance(GetUnitCurrentBounds(unit).mid(), screenPosition);
			float distanceToDestination = glm::distance(GetUnitFutureBounds(unit).mid(), screenPosition);
			float distanceToModifierArea = glm::distance(GetUnitModifierBounds(unit).mid(), screenPosition);
			float distanceMinimum = glm::min(distanceToUnitCenter, glm::min(distanceToDestination, distanceToModifierArea));

			_tappedUnitCenter = distanceToUnitCenter == distanceMinimum;
			_tappedDestination = distanceToDestination == distanceMinimum && !_tappedUnitCenter;
			_tappedModiferArea = distanceToModifierArea == distanceMinimum && !_tappedUnitCenter && !_tappedDestination;

			if (_tappedDestination || _tappedModiferArea)
			{
				_offsetToMarker = 0;//(_boardView->ContentToScreen(vector3(unit->movement.GetFinalDestination(), 0)).y - _boardView->ContentToScreen(vector3(terrainPosition, 0)).y) * GetFlipSign();
				if (_offsetToMarker < 0)
					_offsetToMarker = 0;

				std::vector<glm::vec2>& path = _trackingMarker->_path;
				path.clear();
				path.insert(path.begin(), command.path.begin(), command.path.end());

				glm::vec2 orientation = command.GetDestination() + 18.0f * vector2_from_angle(command.bearing);
				_trackingMarker->SetOrientation(&orientation);
			}
			else
			{
				_offsetToMarker = 0;//(_boardView->ContentToScreen(vector3(unit->state.center, 0)).y - _boardView->ContentToScreen(vector3(terrainPosition, 0)).y) * GetFlipSign();
				if (_offsetToMarker < 0)
					_offsetToMarker = 0;

				glm::vec2 orientation = unit->GetCenter() + 18.0f * vector2_from_angle(unit->GetBearing());
				_trackingMarker->SetOrientation(&orientation);
			}

			if (touch->GetTapCount() > 1 && _tappedUnitCenter && !_tappedDestination)
			{
				BattleObjects::UnitCommand command;

				command.ClearPathAndSetDestination(unit->GetCenter());

				_hotspot->GetBattleView()->GetSimulator()->SetUnitCommand(unit, command, _hotspot->GetBattleView()->GetSimulator()->GetTimerDelay());
			}

			_trackingMarker->SetRunning(touch->GetTapCount() > 1 || (!_tappedUnitCenter && command.running));

			_hotspot->TryCaptureTouch(touch);
			_trackingTouch = touch;
		}
		else if (_modifierTouch == nullptr)
		{
			_hotspot->TryCaptureTouch(touch);
			_modifierTouch = touch;
		}
		else
		{
			_hotspot->TryCaptureTouch(touch);
			_trackingTouch = touch;
		}
	}
	else if (_modifierTouch == nullptr)
	{
		if (unit)
			return;

		if (_gestures)
		{
			for (Gesture* g : *_gestures)
			{
				BattleGesture* gesture = dynamic_cast<BattleGesture*>(g);
				if (gesture && gesture != this && gesture->_trackingTouch)
				{
					if (glm::distance(_trackingTouch->GetCurrentPosition(), touch->GetCurrentPosition()) > glm::distance(gesture->_trackingTouch->GetCurrentPosition(), touch->GetCurrentPosition()))
						return;
				}
			}
		}

		_hotspot->TryCaptureTouch(touch);
		_modifierTouch = touch;
	}
}


void BattleGesture::TouchMoved(Touch* touch)
{
	if (!_hotspot->HasCapturedTouch())
		return;

	if (_trackingMarker)
	{
		int icon_size = 0;

#ifdef PHALANX_TARGET_OS_IOS
		static int* _icon_size = nullptr;
		if (_icon_size == nullptr)
			_icon_size = new int([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone ? 57 : 72);
		icon_size = *_icon_size;
#endif
#ifdef __ANDROID__
		icon_size = 96;
#endif

		glm::vec2 oldPosition = _trackingTouch->GetPreviousPosition();//_boardView->GetTerrainPosition(_trackingTouch->_previous).xy();
		glm::vec2 newPosition = _trackingTouch->GetCurrentPosition();//_boardView->GetTerrainPosition(_trackingTouch->_position).xy();

		float diff = (newPosition.y - oldPosition.y) * GetFlipSign();
		if (diff < 0)
			_offsetToMarker -= diff / 2;
		else
			_offsetToMarker += diff;

		if (_offsetToMarker > icon_size / 2)
			_offsetToMarker = icon_size / 2;

		if (_trackingTouch->GetCurrentButtons() != _trackingTouch->GetPreviousButtons())
			_trackingTouch->ResetHasMoved();

		if (_trackingTouch->HasMoved())
		{
			UpdateTrackingMarker();
		}
	}
}


void BattleGesture::TouchEnded(Touch* touch)
{
	if (!_hotspot->HasCapturedTouch(touch))
		return;

	if (touch == _trackingTouch)
	{
		if (_trackingMarker)
		{
			BattleObjects::Unit* unit = _trackingMarker->GetUnit();
			BattleObjects::UnitCommand command;

			command.path = _trackingMarker->_path;
			command.running = _trackingMarker->GetRunning();
			command.meleeTarget = _trackingMarker->GetMeleeTarget();

			BattleObjects::Unit* missileTarget = _trackingMarker->GetMissileTarget();
			glm::vec2* orientation = _trackingMarker->GetOrientationX();

			if (missileTarget)
			{
				command.missileTarget = missileTarget;
				command.missileTargetLocked = true;
				if (missileTarget != unit)
					command.bearing = angle(missileTarget->GetCenter() - command.GetDestination());
			}
			else if (orientation)
			{
				command.bearing = angle(*orientation - command.GetDestination());
			}

			if (!touch->HasMoved())
			{
				if (_tappedUnitCenter && touch->GetTapCount() > 1)
				{
					command.meleeTarget = nullptr;
					command.ClearPathAndSetDestination(unit->GetCenter());
					command.missileTarget = nullptr;
					command.missileTargetLocked = false;
				}
				else if (_tappedDestination && !_tappedUnitCenter)
				{
					if (!command.running)
					{
						command.running = true;
					}
				}
				else if (_tappedUnitCenter && !_tappedDestination)
				{
					if (command.running)
					{
						command.running = false;
					}
				}
			}

			_hotspot->GetBattleView()->RemoveTrackingMarker(_trackingMarker);
			_trackingMarker = nullptr;

			_hotspot->GetBattleView()->GetSimulator()->IssueUnitCommand(unit, command, _hotspot->GetBattleView()->GetSimulator()->GetTimerDelay());

			if (touch->GetTapCount() == 1)
				SoundPlayer::GetSingleton()->PlayUserInterfaceSound(SoundSampleID::CommandAck);

		}
	}


	if (touch == _modifierTouch && _trackingTouch)
	{
		_trackingTouch->ResetHasMoved();
	}


	if (_trackingTouch && _modifierTouch)
	{
		_trackingTouch->ResetVelocity();
		_modifierTouch->ResetVelocity();
	}


	/*if (_trackingTouch == nullptr && _modifierTouch)
	{
		if (!ViewState::singleton->showTitleScreen)
		{
			vector2 velocity = touch->GetVelocity();
			float speed = norm(velocity) - 1;
			if (speed < 0)
				speed = 0;
			else if (speed > 10)
				speed = 10;
			velocity = vector2::from_polar(velocity.angle(), speed);

			float k = SamuraiSurface::singleton->renderLayer.actualWidth / 2;
			if (SamuraiSurface::singleton->renderLayer.flip)
				k = -k;

			BoardGesture::scrollVelocity = velocity * k;
		}
	}*/


	if (touch == _trackingTouch)
	{
		_trackingTouch = nullptr;
	}
	else if (touch == _modifierTouch)
	{
		_modifierTouch = nullptr;
	}
}


/*void BattleGesture::TouchWasCancelled(Touch* touch)
{
	if (_trackingMarker)
	{
		_hotspot->GetBattleView()->RemoveTrackingMarker(_trackingMarker);
		_trackingMarker = nullptr;
	}
}*/


/***/



void BattleGesture::UpdateTrackingMarker()
{
	BattleObjects::Unit* unit = _trackingMarker->GetUnit();

	glm::vec2 screenTouchPosition = _trackingTouch->GetCurrentPosition();
	glm::vec2 screenMarkerPosition = screenTouchPosition + glm::vec2(0, 1) * (_offsetToMarker * GetFlipSign());
	glm::vec2 touchPosition = _hotspot->GetBattleView()->GetTerrainPosition3(screenTouchPosition).xy();
	glm::vec2 markerPosition = _hotspot->GetBattleView()->GetTerrainPosition3(screenMarkerPosition).xy();

	BattleObjects::Unit* enemyUnit = FindEnemyUnit(touchPosition, markerPosition);
	glm::vec2 unitCenter = unit->GetCenter();

	bool isModifierMode = _tappedModiferArea || _modifierTouch || _trackingTouch->GetCurrentButtons().right;
	_trackingMarker->SetRenderOrientation(isModifierMode);

	if (!isModifierMode)
	{
		std::vector<glm::vec2>& path = _trackingMarker->_path;

		glm::vec2 currentDestination = path.size() != 0 ? *(path.end() - 1) : unit->GetCenter();

		bounds2f contentBounds = _hotspot->GetBattleView()->GetTerrainViewport().GetTerrainBounds();
		glm::vec2 contentCenter = contentBounds.mid();
		float contentRadius = contentBounds.x().size() / 2;
		float maximumRadius = contentRadius - 25.0f;

		glm::vec2 markerOffsetFromCenter = markerPosition - contentCenter;
		if (glm::length(markerOffsetFromCenter) > maximumRadius)
			markerPosition = contentCenter + glm::normalize(markerOffsetFromCenter) * maximumRadius;

		float movementLimit = -1;
		float delta = 1.0f / glm::max(1.0f, glm::distance(currentDestination, markerPosition));
		for (float k = delta; k < 1; k += delta)
		{
			const GroundMap* groundMap = _hotspot->GetBattleView()->GetSimulator()->GetBattleMap()->GetGroundMap();
			if (groundMap && groundMap->IsImpassable(glm::mix(currentDestination, markerPosition, k)))
			{
				movementLimit = k;
				break;
			}
		}

		if (movementLimit >= 0)
		{
			glm::vec2 diff = markerPosition - currentDestination;
			markerPosition = currentDestination + diff * movementLimit;
			markerPosition -= glm::normalize(diff) * 10.0f;

			enemyUnit = nullptr;
		}

		if (enemyUnit && !_trackingMarker->GetMeleeTarget())
			SoundPlayer::GetSingleton()->PlayUserInterfaceSound(SoundSampleID::CommandMod);

		if (!unit->deployed)
		{
			BattleSimulator_v1_0_0* simulator = _hotspot->GetBattleView()->GetSimulator();
			int team = unit->commander->GetTeam();

			if (simulator->IsDeploymentZone(team, markerPosition))
			{
				unitCenter = simulator->ConstrainDeploymentZone(team, markerPosition, 10);
				_trackingMarker->_path.clear();
			}
			else
			{
				while (!_trackingMarker->_path.empty() && simulator->IsDeploymentZone(team, _trackingMarker->_path.front()))
					_trackingMarker->_path.erase(_trackingMarker->_path.begin());

				unitCenter = simulator->ConstrainDeploymentZone(team,
					!_trackingMarker->_path.empty() ? _trackingMarker->_path.front() : unitCenter,
					10);

				if (!_trackingMarker->_path.empty())
					BattleObjects::UnitCommand::UpdateMovementPath(_trackingMarker->_path, unitCenter, _trackingMarker->_path.back());
			}

			simulator->Deploy(unit, unitCenter);
			unitCenter = unit->GetCenter();
		}

		_trackingMarker->SetMeleeTarget(enemyUnit);
		_trackingMarker->SetDestination(&markerPosition);

		if (enemyUnit)
			BattleObjects::UnitCommand::UpdateMovementPath(_trackingMarker->_path, unitCenter, enemyUnit->GetCenter());
		else
			BattleObjects::UnitCommand::UpdateMovementPath(_trackingMarker->_path, unitCenter, markerPosition);

		if (enemyUnit)
		{
			glm::vec2 destination = enemyUnit->GetCenter();
			glm::vec2 orientation = destination + glm::normalize(destination - unitCenter) * 18.0f;
			_trackingMarker->SetOrientation(&orientation);
		}
		else if (BattleObjects::UnitCommand::MovementPathLength(_trackingMarker->_path) > KEEP_ORIENTATION_TRESHHOLD)
		{
			glm::vec2 dir = glm::normalize(markerPosition - unitCenter);
			if (path.size() >= 2)
				dir = glm::normalize(*(path.end() - 1) - *(path.end() - 2));
			glm::vec2 orientation = markerPosition + dir * 18.0f;
			_trackingMarker->SetOrientation(&orientation);
		}
		else
		{
			glm::vec2 orientation = markerPosition + 18.0f * vector2_from_angle(unit->GetBearing());
			_trackingMarker->SetOrientation(&orientation);
		}
	}
	else
	{
		BattleObjects::UnitCommand::UpdateMovementPathStart(_trackingMarker->_path, unitCenter);

		bool holdFire = false;
		if (_trackingMarker->GetUnit()->IsStanding() && _trackingMarker->GetUnit()->GetMissileWeaponRange().maximumRange > 0)
		{
			bounds2f unitCurrentBounds = GetUnitCurrentBounds(_trackingMarker->GetUnit());
			holdFire = glm::distance(screenMarkerPosition, unitCurrentBounds.mid()) <= unitCurrentBounds.x().radius();
		}

		if (holdFire)
		{
			_trackingMarker->SetMissileTarget(_trackingMarker->GetUnit());
			_trackingMarker->SetOrientation(nullptr);
		}
		else
		{
			//if (!_tappedUnitCenter)
			//	enemyUnit = nullptr;
			if (!_allowTargetEnemyUnit)
				enemyUnit = nullptr;

			if (enemyUnit && _trackingMarker->GetMissileTarget() == nullptr)
				SoundPlayer::GetSingleton()->PlayUserInterfaceSound(SoundSampleID::CommandMod);

			_trackingMarker->SetMissileTarget(enemyUnit);
			_trackingMarker->SetOrientation(&markerPosition);
		}
	}
}


int BattleGesture::GetFlipSign() const
{
	return _hotspot->GetBattleView()->GetTerrainViewport().GetFlip() ? -1 : 1;
}


BattleObjects::Unit* BattleGesture::FindCommandableUnit(glm::vec2 screenPosition, glm::vec2 terrainPosition)
{
	BattleObjects::Unit* unitByPosition = FindCommandableUnitByCurrentPosition(screenPosition, terrainPosition);
	BattleObjects::Unit* unitByDestination = FindCommandableUnitByFuturePosition(screenPosition, terrainPosition);
	BattleObjects::Unit* unitByModifier = FindCommandableUnitByModifierArea(screenPosition, terrainPosition);

	if (unitByPosition && unitByDestination == nullptr)
	{
		return unitByPosition;
	}

	if (unitByPosition == nullptr && unitByDestination)
	{
		return unitByDestination;
	}

	if (unitByPosition && unitByDestination)
	{
		float distanceToPosition = glm::distance(unitByPosition->GetCenter(), screenPosition);
		float distanceToDestination = glm::distance(unitByDestination->GetIssuedCommand().GetDestination(), screenPosition);
		return distanceToPosition < distanceToDestination
				? unitByPosition
				: unitByDestination;
	}

	if (unitByModifier)
		return unitByModifier;

	return nullptr;
}


BattleObjects::Unit* BattleGesture::FindCommandableUnitByCurrentPosition(glm::vec2 screenPosition, glm::vec2 terrainPosition)
{
	BattleObjects::Unit* result = nullptr;
	UnitCounter* unitMarker = _hotspot->GetBattleView()->GetNearestUnitCounter(terrainPosition, 0, _hotspot->GetBattleView()->GetCommander(), false);
	if (unitMarker)
	{
		BattleObjects::Unit* unit = unitMarker->_unit;
		if (!unit->IsRouting() && GetUnitCurrentBounds(unit).contains(screenPosition))
		{
			result = unit;
		}
	}
	return result;
}


BattleObjects::Unit* BattleGesture::FindCommandableUnitByFuturePosition(glm::vec2 screenPosition, glm::vec2 terrainPosition)
{
	BattleObjects::Unit* result = nullptr;
	UnitMovementMarker* movementMarker = _hotspot->GetBattleView()->GetNearestMovementMarker(terrainPosition, _hotspot->GetBattleView()->GetCommander());
	if (movementMarker)
	{
		BattleObjects::Unit* unit = movementMarker->GetUnit();
		if (!unit->IsRouting() && GetUnitFutureBounds(unit).contains(screenPosition))
		{
			result = unit;
		}
	}
	return result;
}


BattleObjects::Unit* BattleGesture::FindCommandableUnitByModifierArea(glm::vec2 screenPosition, glm::vec2 terrainPosition)
{
	BattleObjects::Unit* result = nullptr;
	float distance = 10000;

	for (BattleObjects::Unit* unit : _hotspot->GetBattleView()->GetSimulator()->GetUnits())
	{
		if (unit->IsCommandableBy(_hotspot->GetBattleView()->GetCommander()))
		{
			const BattleObjects::UnitCommand& command = unit->GetIssuedCommand();
			glm::vec2 center = !command.path.empty() ? command.path.back() : unit->GetCenter();
			float d = glm::distance(center, terrainPosition);
			if (d < distance && !unit->IsRouting() && GetUnitModifierBounds(unit).contains(screenPosition))
			{
				result = unit;
				distance = d;
			}
		}
	}

	return result;
}


BattleObjects::Unit* BattleGesture::FindEnemyUnit(glm::vec2 touchPosition, glm::vec2 markerPosition)
{
	int enemyTeam = _trackingMarker->GetUnit()->commander->GetTeam() == 1 ? 2 : 1;

	UnitCounter* enemyMarker = nullptr;

	glm::vec2 p = markerPosition;
	glm::vec2 d = (touchPosition - markerPosition) / 4.0f;
	for (int i = 0; i < 4; ++i)
	{
		UnitCounter* unitMarker = _hotspot->GetBattleView()->GetNearestUnitCounter(p, enemyTeam, nullptr, true);
		if (unitMarker && glm::distance(unitMarker->_unit->GetCenter(), p) <= SNAP_TO_UNIT_TRESHOLD)
		{
			enemyMarker = unitMarker;
			break;
		}
		p += d;
	}

	if (enemyMarker)
		return enemyMarker->_unit;
	else
		return nullptr;
}


bounds2f BattleGesture::GetUnitCurrentBounds(BattleObjects::Unit* unit)
{
	return _hotspot->GetBattleView()->GetUnitCurrentIconViewportBounds(unit).add_radius(12);
}


bounds2f BattleGesture::GetUnitFutureBounds(BattleObjects::Unit* unit)
{
	return _hotspot->GetBattleView()->GetUnitFutureIconViewportBounds(unit).add_radius(12);
}


bounds2f BattleGesture::GetUnitModifierBounds(BattleObjects::Unit* unit)
{
	if (unit->IsStanding())
		return _hotspot->GetBattleView()->GetUnitCurrentFacingMarkerBounds(unit).add_radius(12);

	if (unit->IsMoving())
		return _hotspot->GetBattleView()->GetUnitFutureFacingMarkerBounds(unit).add_radius(12);

	return bounds2f();
}


/* BattleObserver */


void BattleGesture::OnAddUnit(BattleObjects::Unit* unit)
{
}


void BattleGesture::OnRemoveUnit(BattleObjects::Unit* unit)
{
	if (_trackingMarker && _trackingMarker->GetUnit() == unit)
	{
		_hotspot->GetBattleView()->RemoveTrackingMarker(_trackingMarker);
		_trackingMarker = nullptr;
	}
}


void BattleGesture::OnCommand(BattleObjects::Unit* unit, float timer)
{
}


void BattleGesture::OnShooting(const BattleObjects::Shooting& shooting, float timer)
{
}


void BattleGesture::OnRelease(const BattleObjects::Shooting& shooting)
{
}


void BattleGesture::OnCasualty(BattleObjects::Unit* unit, glm::vec2 fighter)
{
}


void BattleGesture::OnRouting(BattleObjects::Unit* unit)
{
}
