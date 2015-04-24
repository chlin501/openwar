#ifndef MusicDirector_H
#define MusicDirector_H

#include <utility>

class SoundPlayer;
enum class SoundTrackID;


class MusicDirector
{
	using TrackAndPriority = std::pair<SoundTrackID, int>;

	SoundPlayer* _soundPlayer{};
	SoundTrackID _currentTrack{};
	int _currentPriority{};
	double _silenceTimer{};
	double _playingTimer{};
	bool _isBattle{};
	int _unitsMoving{};
	int _unitsRunning{};
	int _meleeCavalry{};
	int _meleeInfantry{};
	int _friendlyUnits{};
	int _enemyUnits{};
	int _outcome;

public:
	MusicDirector(SoundPlayer* soundPlayer);

	void OnTitleScreen();
	void OnStartBattle();

	void Tick(double secondsSinceLastTick);

	void UpdateUnitsMoving(int count);
	void UpdateUnitsRunning(int count);
	void UpdateMeleeCavalry(int count);
	void UpdateMeleeInfantry(int count);
	void UpdateFriendlyUnits(int count);
	void UpdateEnemyUnits(int count);
	void UpdateOutcome(int value);

private:
	TrackAndPriority SuggestTrack() const;
	bool ShouldSwitchTrack(TrackAndPriority suggestion) const;
	void SwitchTrack(TrackAndPriority trackAndPriority);
};


#endif
