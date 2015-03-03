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


void MusicDirector::UpdateMeleeCavalry(int count)
{
	_meleeCavalry = count;
}


void MusicDirector::UpdateMeleeInfantry(int count)
{
	_meleeInfantry = count;
}


MusicDirector::TrackAndPriority MusicDirector::SuggestTrack() const
{
	if (!_isBattle)
		return std::make_pair(SoundTrackID::Title, 0);

	// Priority 3

	if (_meleeInfantry >= 3)
		return std::make_pair(SoundTrackID::StormOfSusanoo, 3);

	if (_meleeCavalry >= 1)
		return std::make_pair(SoundTrackID::HorseCharge, 3);

	// Priority 2

	if (_unitsMoving >= 5)
	{
		if (_currentTrack != SoundTrackID::GeishaGarden)
			return std::make_pair(SoundTrackID::GeishaGarden, 2);
		else
			return std::make_pair(SoundTrackID::Amaterasu, 2);
	}

	// Priority 1

	if (_currentTrack == SoundTrackID::Title)
		return std::make_pair(SoundTrackID::DreamingWaves, 1);

	return std::make_pair(SoundTrackID::Amaterasu, 1);
}


bool MusicDirector::ShouldSwitchTrack(TrackAndPriority suggestion) const
{
	bool isTitle = _currentTrack == SoundTrackID::Title;
	bool shouldBeTitle = !_isBattle;
	if (isTitle != shouldBeTitle)
		return true;

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
