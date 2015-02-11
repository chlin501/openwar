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
	bool _isBattle{};
	int _unitsMoving{};
	int _meleeCavalry{};
	int _meleeInfantry{};

public:
	MusicDirector(SoundPlayer* soundPlayer);

	void OnTitleScreen();
	void OnStartBattle();

	void Tick(double secondsSinceLastTick);

	void UpdateUnitsMoving(int count);
	void UpdateMeleeCavalry(int count);
	void UpdateMeleeInfantry(int count);

private:
	TrackAndPriority SuggestTrack() const;
	bool ShouldSwitchTrack(TrackAndPriority suggestion) const;
	void SwitchTrack(TrackAndPriority trackAndPriority);
};


#endif
