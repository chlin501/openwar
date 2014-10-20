// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#if TARGET_OS_IPHONE
#import <UIKit/UIKit.h>
#endif

#include "BattleModel/BattleCommander.h"
#include "BattleModel/BattleSimulator.h"
#include "Audio/SoundPlayer.h"
#include "Shapes/PlainShape3.h"
#include "BattleGesture.h"
#include "BattleView.h"
#include "UnitCounter.h"
#include "UnitMovementMarker.h"
#include "UnitTrackingMarker.h"


#define SNAP_TO_UNIT_TRESHOLD 22 // meters
#define KEEP_ORIENTATION_TRESHHOLD 40 // meters
#define MODIFIER_AREA_RADIUS_MIN 5.0f // meters
#define MODIFIER_AREA_RADIUS_MAX 25.0f // meters


bool BattleGesture::disableUnitTracking = false;


BattleGesture::BattleGesture(BattleView* battleView) :
_battleView(battleView),
_tappedUnitCenter(false),
_tappedDestination(false),
_tappedModiferArea(false),
_trackingMarker(0),
_trackingTouch(0),
_modifierTouch(0),
_offsetToMarker(0),
_allowTargetEnemyUnit(false)
{
}


BattleGesture::~BattleGesture()
{
}


void BattleGesture::Update(Surface* surface, double secondsSinceLastUpdate)
{

}


void BattleGesture::RenderHints()
{
	glm::mat4 transform = ViewportTransform(_battleView->GetFrame());

	VertexBuffer_2f vertices;
	vertices._mode = GL_LINES;

	for (UnitCounter* unitMarker : _battleView->GetUnitCounters())
	{
		bounds2f bounds = GetUnitCurrentBounds(unitMarker->_unit);

		vertices.AddVertex(Vertex_2f(bounds.p11()));
		vertices.AddVertex(Vertex_2f(bounds.p12()));
		vertices.AddVertex(Vertex_2f(bounds.p12()));
		vertices.AddVertex(Vertex_2f(bounds.p22()));
		vertices.AddVertex(Vertex_2f(bounds.p22()));
		vertices.AddVertex(Vertex_2f(bounds.p21()));
		vertices.AddVertex(Vertex_2f(bounds.p21()));
		vertices.AddVertex(Vertex_2f(bounds.p11()));

		bounds = GetUnitFutureBounds(unitMarker->_unit);
		if (!bounds.is_empty())
		{
			vertices.AddVertex(Vertex_2f(bounds.p11()));
			vertices.AddVertex(Vertex_2f(bounds.p12()));
			vertices.AddVertex(Vertex_2f(bounds.p12()));
			vertices.AddVertex(Vertex_2f(bounds.p22()));
			vertices.AddVertex(Vertex_2f(bounds.p22()));
			vertices.AddVertex(Vertex_2f(bounds.p21()));
			vertices.AddVertex(Vertex_2f(bounds.p21()));
			vertices.AddVertex(Vertex_2f(bounds.p11()));
		}

		bounds = GetUnitModifierBounds(unitMarker->_unit);
		if (!bounds.is_empty())
		{
			vertices.AddVertex(Vertex_2f(bounds.p11()));
			vertices.AddVertex(Vertex_2f(bounds.p12()));
			vertices.AddVertex(Vertex_2f(bounds.p12()));
			vertices.AddVertex(Vertex_2f(bounds.p22()));
			vertices.AddVertex(Vertex_2f(bounds.p22()));
			vertices.AddVertex(Vertex_2f(bounds.p21()));
			vertices.AddVertex(Vertex_2f(bounds.p21()));
			vertices.AddVertex(Vertex_2f(bounds.p11()));
		}
	}

	renderers::singleton->_plain_renderer->get_uniform<glm::mat4>("transform").set_value(transform);
	renderers::singleton->_plain_renderer->get_uniform<float>("point_size").set_value(1);
	renderers::singleton->_plain_renderer->get_uniform<glm::vec4>("color").set_value(glm::vec4(0, 0, 0, 0.2f));
	renderers::singleton->_plain_renderer->render(vertices);


	/*PlainLineRenderer renderer;
	renderer.Reset();
	for (UnitCounter* unitMarker : _battleView->GetBattleSimulator()->_unitMarkers)
	{
		glm::vec2 center = !unitMarker->_unit->command.path.empty() ? unitMarker->_unit->command.path.back() : unitMarker->_unit->state.center;
		float facing = unitMarker->_unit->command.facing;

		glm::vec2 p1 = center + MODIFIER_AREA_RADIUS_MAX * vector2_from_angle(facing - 0.5f * glm::half_pi<float>());
		glm::vec2 p2 = center + MODIFIER_AREA_RADIUS_MAX * vector2_from_angle(facing + 0.5f * glm::half_pi<float>());

		renderer.AddLine(_battleView->GetPosition(center), _battleView->GetPosition(p1));
		renderer.AddLine(_battleView->GetPosition(center), _battleView->GetPosition(p2));

		for (int i = 1; i <= 10; ++i)
		{
			float a1 = facing + ((i - 1) / 10.0f - 0.5f) * glm::half_pi<float>();
			float a2 = facing + (i / 10.0f - 0.5f) * glm::half_pi<float>();

			p1 = center + MODIFIER_AREA_RADIUS_MIN * vector2_from_angle(a1);
			p2 = center + MODIFIER_AREA_RADIUS_MIN * vector2_from_angle(a2);
			renderer.AddLine(_battleView->GetPosition(p1), _battleView->GetPosition(p2));

			p1 = center + MODIFIER_AREA_RADIUS_MAX * vector2_from_angle(a1);
			p2 = center + MODIFIER_AREA_RADIUS_MAX * vector2_from_angle(a2);
			renderer.AddLine(_battleView->GetPosition(p1), _battleView->GetPosition(p2));
		}
	}
	renderer.Draw(_battleView->GetTransform(), glm::vec4(0, 0, 0, 0.2f));*/
}


void BattleGesture::TouchBegan(Touch* touch)
{
	if (touch->GetSurface() != _battleView->GetSurface())
		return;
	if (touch->HasGesture())
		return;
	if (!_battleView->GetFrame().contains(touch->GetPosition()))
		return;

	glm::vec2 screenPosition = touch->GetPosition();
	glm::vec2 terrainPosition = _battleView->GetTerrainPosition3(screenPosition).xy();
	Unit* unit = FindCommandableUnit(screenPosition, terrainPosition);

	if (_trackingTouch == nullptr)
	{
		if (unit == nullptr)
			return;

		if (unit != nullptr && _battleView->GetTrackingMarker(unit) == nullptr)
		{
			const UnitCommand command = unit->GetCommand();

			_allowTargetEnemyUnit = unit->stats.missileType != MissileType::None;
			_trackingMarker = _battleView->AddTrackingMarker(unit);

			float distanceToUnitCenter = glm::distance(GetUnitCurrentBounds(unit).center(), screenPosition);
			float distanceToDestination = glm::distance(GetUnitFutureBounds(unit).center(), screenPosition);
			float distanceToModifierArea = glm::distance(GetUnitModifierBounds(unit).center(), screenPosition);
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

				glm::vec2 orientation = unit->state.center + 18.0f * vector2_from_angle(unit->state.bearing);
				_trackingMarker->SetOrientation(&orientation);
			}

			if (touch->GetTapCount() > 1 && _tappedUnitCenter && !_tappedDestination)
			{
				UnitCommand command;

				command.ClearPathAndSetDestination(unit->state.center);

				_battleView->GetSimulator()->SetUnitCommand(unit, command, _battleView->GetSimulator()->GetTimerDelay());
			}

			_trackingMarker->SetRunning(touch->GetTapCount() > 1 || (!_tappedUnitCenter && command.running));

			CaptureTouch(touch);
			_trackingTouch = touch;
		}
		else if (_modifierTouch == nullptr)
		{
			CaptureTouch(touch);
			_modifierTouch = touch;
		}
		else
		{
			CaptureTouch(touch);
			_trackingTouch = touch;
		}
	}
	else if (_modifierTouch == nullptr)
	{
		if (unit != nullptr)
			return;

		if (_gestures != nullptr)
		{
			for (Gesture* g : *_gestures)
			{
				BattleGesture* gesture = dynamic_cast<BattleGesture*>(g);
				if (gesture != nullptr && gesture != this && gesture->_trackingTouch != nullptr)
				{
					if (glm::length(_trackingTouch->GetPosition() - touch->GetPosition()) > glm::length(gesture->_trackingTouch->GetPosition() - touch->GetPosition()))
						return;
				}
			}
		}

		CaptureTouch(touch);
		_modifierTouch = touch;
	}
}


void BattleGesture::TouchMoved()
{
	if (_trackingMarker != nullptr)
	{
		int icon_size = 0;

#if TARGET_OS_IPHONE
		static int* _icon_size = nullptr;
		if (_icon_size == nullptr)
			_icon_size = new int([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone ? 57 : 72);
		icon_size = *_icon_size;
#endif
#ifdef __ANDROID__
		icon_size = 96;
#endif

		glm::vec2 oldPosition = _trackingTouch->GetPrevious();//_boardView->GetTerrainPosition(_trackingTouch->_previous).xy();
		glm::vec2 newPosition = _trackingTouch->GetPosition();//_boardView->GetTerrainPosition(_trackingTouch->_position).xy();

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
	if (touch == _trackingTouch)
	{
		if (_trackingMarker != nullptr)
		{
			Unit* unit = _trackingMarker->GetUnit();
			UnitCommand command;

			command.path = _trackingMarker->_path;
			command.running = _trackingMarker->GetRunning();
			command.meleeTarget = _trackingMarker->GetMeleeTarget();

			Unit* missileTarget = _trackingMarker->GetMissileTarget();
			glm::vec2* orientation = _trackingMarker->GetOrientationX();

			if (missileTarget != nullptr)
			{
				command.missileTarget = missileTarget;
				command.missileTargetLocked = true;
				if (missileTarget != unit)
					command.bearing = angle(missileTarget->state.center - command.GetDestination());
				unit->state.loadingTimer = 0;
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
					command.ClearPathAndSetDestination(unit->state.center);
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

			unit->timeUntilSwapFighters = 0.2f;

			_battleView->RemoveTrackingMarker(_trackingMarker);
			_trackingMarker = nullptr;

			_battleView->GetSimulator()->SetUnitCommand(unit, command, _battleView->GetSimulator()->GetTimerDelay());

			if (touch->GetTapCount() == 1)
				SoundPlayer::singleton->Play(SoundBufferCommandAck);

		}
	}


	if (touch == _modifierTouch && _trackingTouch != nullptr)
	{
		_trackingTouch->ResetHasMoved();
	}


	if (_trackingTouch != nullptr && _modifierTouch != nullptr)
	{
		_trackingTouch->ResetVelocity();
		_modifierTouch->ResetVelocity();
	}


	/*if (_trackingTouch == nullptr && _modifierTouch != nullptr)
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


void BattleGesture::TouchWasCancelled(Touch* touch)
{
	if (_trackingMarker)
	{
		_battleView->RemoveTrackingMarker(_trackingMarker);
		_trackingMarker = nullptr;
	}

}


/***/



void BattleGesture::UpdateTrackingMarker()
{
	Unit* unit = _trackingMarker->GetUnit();

	glm::vec2 screenTouchPosition = _trackingTouch->GetPosition();
	glm::vec2 screenMarkerPosition = screenTouchPosition + glm::vec2(0, 1) * (_offsetToMarker * GetFlipSign());
	glm::vec2 touchPosition = _battleView->GetTerrainPosition3(screenTouchPosition).xy();
	glm::vec2 markerPosition = _battleView->GetTerrainPosition3(screenMarkerPosition).xy();

	Unit* enemyUnit = FindEnemyUnit(touchPosition, markerPosition);
	glm::vec2 unitCenter = unit->state.center;

	bool isModifierMode = _tappedModiferArea || _modifierTouch != nullptr || _trackingTouch->GetCurrentButtons().right;
	_trackingMarker->SetRenderOrientation(isModifierMode);

	if (!isModifierMode)
	{
		std::vector<glm::vec2>& path = _trackingMarker->_path;

		glm::vec2 currentDestination = path.size() != 0 ? *(path.end() - 1) : unit->state.center;

		bounds2f contentBounds = _battleView->GetContentBounds();
		glm::vec2 contentCenter = contentBounds.center();
		float contentRadius = contentBounds.width() / 2;

		glm::vec2 differenceToCenter = contentCenter - markerPosition;
		float distanceToCenter = glm::length(differenceToCenter);
		if (distanceToCenter > contentRadius)
		{
			markerPosition += differenceToCenter * (distanceToCenter - contentRadius) / distanceToCenter;
		}

		float movementLimit = -1;
		float delta = 1.0f / fmaxf(1, glm::length(currentDestination - markerPosition));
		for (float k = delta; k < 1; k += delta)
		{
			GroundMap* groundMap = _battleView->GetSimulator()->GetGroundMap();
			if (groundMap != nullptr && groundMap->IsImpassable(glm::mix(currentDestination, markerPosition, k)))
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
			SoundPlayer::singleton->Play(SoundBufferCommandMod);

		_trackingMarker->SetMeleeTarget(enemyUnit);
		_trackingMarker->SetDestination(&markerPosition);

		if (enemyUnit != nullptr)
			MovementRules::UpdateMovementPath(_trackingMarker->_path, unitCenter, enemyUnit->state.center);
		else
			MovementRules::UpdateMovementPath(_trackingMarker->_path, unitCenter, markerPosition);

		if (enemyUnit != nullptr)
		{
			glm::vec2 destination = enemyUnit->state.center;
			glm::vec2 orientation = destination + glm::normalize(destination - unitCenter) * 18.0f;
			_trackingMarker->SetOrientation(&orientation);
		}
		else if (MovementRules::Length(_trackingMarker->_path) > KEEP_ORIENTATION_TRESHHOLD)
		{
			glm::vec2 dir = glm::normalize(markerPosition - unitCenter);
			if (path.size() >= 2)
				dir = glm::normalize(*(path.end() - 1) - *(path.end() - 2));
			glm::vec2 orientation = markerPosition + dir * 18.0f;
			_trackingMarker->SetOrientation(&orientation);
		}
		else
		{
			glm::vec2 orientation = markerPosition + 18.0f * vector2_from_angle(unit->state.bearing);
			_trackingMarker->SetOrientation(&orientation);
		}
	}
	else
	{
		MovementRules::UpdateMovementPathStart(_trackingMarker->_path, unitCenter);

		bool holdFire = false;
		if (_trackingMarker->GetUnit()->state.unitMode == UnitMode_Standing && _trackingMarker->GetUnit()->stats.maximumRange > 0)
		{
			bounds2f unitCurrentBounds = GetUnitCurrentBounds(_trackingMarker->GetUnit());
			holdFire = glm::distance(screenMarkerPosition, unitCurrentBounds.center()) <= unitCurrentBounds.x().radius();
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

			if (enemyUnit != nullptr && _trackingMarker->GetMissileTarget() == nullptr)
				SoundPlayer::singleton->Play(SoundBufferCommandMod);

			_trackingMarker->SetMissileTarget(enemyUnit);
			_trackingMarker->SetOrientation(&markerPosition);
		}
	}
}


int BattleGesture::GetFlipSign() const
{
	return _battleView->GetFlip() ? -1 : 1;
}


Unit* BattleGesture::FindCommandableUnit(glm::vec2 screenPosition, glm::vec2 terrainPosition)
{
	if (disableUnitTracking)
		return nullptr;

	Unit* unitByPosition = FindCommandableUnitByCurrentPosition(screenPosition, terrainPosition);
	Unit* unitByDestination = FindCommandableUnitByFuturePosition(screenPosition, terrainPosition);
	Unit* unitByModifier = FindCommandableUnitByModifierArea(screenPosition, terrainPosition);

	if (unitByPosition != nullptr && unitByDestination == nullptr)
	{
		return unitByPosition;
	}

	if (unitByPosition == nullptr && unitByDestination != nullptr)
	{
		return unitByDestination;
	}

	if (unitByPosition != nullptr && unitByDestination != nullptr)
	{
		float distanceToPosition = glm::length(unitByPosition->state.center - screenPosition);
		float distanceToDestination = glm::length(unitByDestination->GetCommand().GetDestination() - screenPosition);
		return distanceToPosition < distanceToDestination
				? unitByPosition
				: unitByDestination;
	}

	if (unitByModifier != nullptr)
		return unitByModifier;

	return nullptr;
}


Unit* BattleGesture::FindCommandableUnitByCurrentPosition(glm::vec2 screenPosition, glm::vec2 terrainPosition)
{
	Unit* result = nullptr;
	UnitCounter* unitMarker = _battleView->GetNearestUnitCounter(terrainPosition, 0, _battleView->GetCommander());
	if (unitMarker != nullptr)
	{
		Unit* unit = unitMarker->_unit;
		if (!unit->state.IsRouting() && GetUnitCurrentBounds(unit).contains(screenPosition))
		{
			result = unit;
		}
	}
	return result;
}


Unit* BattleGesture::FindCommandableUnitByFuturePosition(glm::vec2 screenPosition, glm::vec2 terrainPosition)
{
	Unit* result = nullptr;
	UnitMovementMarker* movementMarker = _battleView->GetNearestMovementMarker(terrainPosition, _battleView->GetCommander());
	if (movementMarker != nullptr)
	{
		Unit* unit = movementMarker->GetUnit();
		if (!unit->state.IsRouting() && GetUnitFutureBounds(unit).contains(screenPosition))
		{
			result = unit;
		}
	}
	return result;
}


Unit* BattleGesture::FindCommandableUnitByModifierArea(glm::vec2 screenPosition, glm::vec2 terrainPosition)
{
	Unit* result = nullptr;
	float distance = 10000;

	for (Unit* unit : _battleView->GetSimulator()->GetUnits())
	{
		if (unit->IsCommandableBy(_battleView->GetCommander()))
		{
			const UnitCommand& command = unit->GetCommand();
			glm::vec2 center = !command.path.empty() ? command.path.back() : unit->state.center;
			float d = glm::distance(center, terrainPosition);
			if (d < distance && !unit->state.IsRouting() && GetUnitModifierBounds(unit).contains(screenPosition))
			{
				result = unit;
				distance = d;
			}
		}
	}

	return result;
}


Unit* BattleGesture::FindEnemyUnit(glm::vec2 touchPosition, glm::vec2 markerPosition)
{
	int enemyTeam = _trackingMarker->GetUnit()->commander->GetTeam() == 1 ? 2 : 1;

	UnitCounter* enemyMarker = nullptr;

	glm::vec2 p = markerPosition;
	glm::vec2 d = (touchPosition - markerPosition) / 4.0f;
	for (int i = 0; i < 4; ++i)
	{
		UnitCounter* unitMarker = _battleView->GetNearestUnitCounter(p, enemyTeam, 0);
		if (unitMarker && glm::length(unitMarker->_unit->state.center - p) <= SNAP_TO_UNIT_TRESHOLD)
		{
			enemyMarker = unitMarker;
			break;
		}
		p += d;
	}

	if (enemyMarker != nullptr)
		return enemyMarker->_unit;
	else
		return nullptr;
}


bounds2f BattleGesture::GetUnitCurrentBounds(Unit* unit)
{
	return _battleView->GetUnitCurrentIconViewportBounds(unit).grow(12);
}


bounds2f BattleGesture::GetUnitFutureBounds(Unit* unit)
{
	return _battleView->GetUnitFutureIconViewportBounds(unit).grow(12);
}


bounds2f BattleGesture::GetUnitModifierBounds(Unit* unit)
{
	switch (unit->state.unitMode)
	{
		case UnitMode_Standing: return _battleView->GetUnitCurrentFacingMarkerBounds(unit).grow(12);
		case UnitMode_Moving: return _battleView->GetUnitFutureFacingMarkerBounds(unit).grow(12);
		default: return bounds2f();
	}
}
