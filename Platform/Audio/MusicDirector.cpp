#include "MusicDirector.h"
#include "SoundPlayer.h"
#include <algorithm>


MusicDirector::MusicDirector(SoundPlayer* soundPlayer) :
	_soundPlayer{soundPlayer}
{
}


void MusicDirector::OnTitleScreen()
{
	_isBattle = false;
	SwitchTrack(SuggestTrack());
}


void MusicDirector::OnStartBattle()
{
	_isBattle = true;
	_unitsMoving = 0;
	_unitsRunning = 0;
	_meleeCavalry = 0;
	_meleeInfantry = 0;
	_friendlyUnits = 0;
	_enemyUnits = 0;
	_outcome = 0;
}


void MusicDirector::Tick(double secondsSinceLastTick)
{
	double& timer = _soundPlayer->IsTrackPlaying() ? _playingTimer : _silenceTimer;
	timer += secondsSinceLastTick;

	auto suggestion = SuggestTrack();
	if (suggestion.first == _currentTrack && _soundPlayer->IsTrackPlaying())
	{
		_currentPriority = std::max(_currentPriority, suggestion.second);
	}
	else if (ShouldSwitchTrack(suggestion))
	{
		SwitchTrack(suggestion);
	}
}


void MusicDirector::UpdateUnitsMoving(int count)
{
	_unitsMoving = count;
}


void MusicDirector::UpdateUnitsRunning(int count)
{
	_unitsRunning = count;
}


void MusicDirector::UpdateMeleeCavalry(int count)
{
	_meleeCavalry = count;
}


void MusicDirector::UpdateMeleeInfantry(int count)
{
	_meleeInfantry = count;
}


void MusicDirector::UpdateFriendlyUnits(int count)
{
	_friendlyUnits = count;
}


void MusicDirector::UpdateEnemyUnits(int count)
{
	_enemyUnits = count;
}


void MusicDirector::UpdateOutcome(int value)
{
	_outcome = value;
}


MusicDirector::TrackAndPriority MusicDirector::SuggestTrack() const
{
	/*
	- Samurai Wars Theme (title screen)
	- Dreaming Waves (before battle)
	- Amaterasu (before battle)
	- Geisha Garden (before battle)
	- Swift as an Arrow (matching, some units)
	- Storm of Susanoo (marching, most units)
	- Marching of the Battlefield (marching, quick)
	- Horse Charge (skirmishing)
	- Attack Command (fighting, winning)
	- Battle to the Death (fighting, uncertain)
	- Order of Shogun (victory)
	- The Humiliation (defeat)
 	*/

	if (!_isBattle)
		return std::make_pair(SoundTrackID::SamuraiWarsTheme, 0);

	// Priority 5

	if (_outcome != 0)
	{
		if (_outcome == 1)
			return std::make_pair(SoundTrackID::OrderOfShogun, 5);
		else
			return std::make_pair(SoundTrackID::TheHumiliation, 5);
	}

	// Priority 3

	if (_meleeInfantry + _meleeCavalry >= 6)
	{
		if (_friendlyUnits > 1.5 * _enemyUnits)
			return std::make_pair(SoundTrackID::AttackCommand, 3);
		else
			return std::make_pair(SoundTrackID::BattleToTheDeath, 3);
	}

	if (_meleeInfantry >= 3)
		return std::make_pair(SoundTrackID::StormOfSusanoo, 3);

	if (_meleeCavalry >= 1)
		return std::make_pair(SoundTrackID::HorseCharge, 3);

	// Priority 2

	if (_unitsMoving >= 7)
		return std::make_pair(SoundTrackID::StormOfSusanoo, 2);

	if (_unitsRunning >= 4)
		return std::make_pair(SoundTrackID::SwiftAsAnArrow, 2);

	if (_unitsMoving >= 4)
		return std::make_pair(SoundTrackID::MarchingOnTheBattlefield, 2);

	// Priority 1

	if (_currentTrack == SoundTrackID::SamuraiWarsTheme)
		return std::make_pair(SoundTrackID::DreamingWaves, 1);

	if (_currentTrack != SoundTrackID::GeishaGarden)
		return std::make_pair(SoundTrackID::GeishaGarden, 1);

	return std::make_pair(SoundTrackID::Amaterasu, 1);
}


bool MusicDirector::ShouldSwitchTrack(TrackAndPriority suggestion) const
{
	bool isTitle = _currentTrack == SoundTrackID::SamuraiWarsTheme;
	bool shouldBeTitle = !_isBattle;
	if (isTitle != shouldBeTitle)
		return true;

	if (suggestion.first == _currentTrack)
	{
		switch (_currentTrack)
		{
			case SoundTrackID::OrderOfShogun:
			case SoundTrackID::TheHumiliation:
				return false;

			default:
				return _silenceTimer > 7;
		}
	}

	return suggestion.second > _currentPriority
		? _playingTimer > 15
		: _silenceTimer > 15;
}


void MusicDirector::SwitchTrack(TrackAndPriority trackAndPriority)
{
	_currentTrack = trackAndPriority.first;
	_currentPriority = trackAndPriority.second;

	_soundPlayer->PlayTrack(_currentTrack);

	_playingTimer = 0;
	_silenceTimer = 0;
}
