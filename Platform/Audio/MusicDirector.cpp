#include "MusicDirector.h"
#include "SoundPlayer.h"


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
	if (!_soundPlayer->IsTrackPlaying())
		_silenceTimer += secondsSinceLastTick;

	auto suggestion = SuggestTrack();
	if (ShouldSwitchTrack(suggestion))
		SwitchTrack(suggestion);
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

	if (_meleeInfantry >= 3)
		return std::make_pair(SoundTrackID::StormOfSusanoo, 3);

	if (_meleeCavalry >= 1)
		return std::make_pair(SoundTrackID::HorseCharge, 3);

	if (_unitsMoving >= 3)
		return std::make_pair(SoundTrackID::GeishaGarden, 2);

	if (_currentTrack == SoundTrackID::Title)
		return std::make_pair(SoundTrackID::DreamingWaves, 1);

	return std::make_pair(SoundTrackID::Amaterasu, 1);
}


bool MusicDirector::ShouldSwitchTrack(TrackAndPriority suggestion) const
{
	return suggestion.second > _currentPriority
		|| _silenceTimer > 10;
}


void MusicDirector::SwitchTrack(TrackAndPriority trackAndPriority)
{
	_currentTrack = trackAndPriority.first;
	_currentPriority = trackAndPriority.second;
	_soundPlayer->PlayTrack(_currentTrack);
	_silenceTimer = 0;
}
