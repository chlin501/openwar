// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef SOUNDPLAYER_H
#define SOUNDPLAYER_H

#ifdef OPENWAR_USE_OPENAL
#ifdef OPENWAR_USE_XCODE_FRAMEWORKS
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif
#endif

#ifdef OPENWAR_USE_SDL_MIXER
#include <SDL2_mixer/SDL_mixer.h>
#endif

#if defined(OPENWAR_USE_AVFOUNDATION) || defined(OPENWAR_USE_AVAUDIOSESSION)
#import <AVFoundation/AVFoundation.h>
#endif

#include "MusicDirector.h"

#include <chrono>

#define OPENWAR_ENABLE_MUSIC 1

#define NUMBER_OF_SOUND_CHANNELS (static_cast<int>(SoundChannelID::NumberOfSoundChannels))
#define NUMBER_OF_SOUND_SAMPLES (static_cast<int>(SoundSampleID::NumberOfSoundSamples))
#define NUMBER_OF_SOUND_TRACKS (static_cast<int>(SoundTrackID::NumberOfSoundTracks))


enum class SoundChannelID
{
	UserInterface,
	Background,
	Casualty,
	CavalryWalking,
	CavalryRunning,
	Horse,
	InfantryWalking,
	InfantryRunning,
	MeleeCavalry,
	MeleeCharging,
	MeleeInfantry,
	MissileMatchlock1,
	MissileMatchlock2,
	MissileArrows1,
	MissileArrows2,
	MissileArrows3,
	MissileImpact,
	Sword,
	NumberOfSoundChannels
};


enum class SoundSampleID : int
{
	CommandAck,
	CommandMod,
	Background,
	Casualty1,
	Casualty2,
	Casualty3,
	Casualty4,
	Casualty5,
	Casualty6,
	Casualty7,
	Casualty8,
	CavalryRunning,
	CavalryWalking,
	HorseNeigh1,
	HorseNeigh2,
	HorseNeigh3,
	HorseSnort,
	InfantryRunning,
	InfantryWalking,
	MeleeCavalry,
	MeleeCharging,
	MeleeInfantry,
	MissileArrows,
	MissileImpact1,
	MissileImpact2,
	MissileImpact3,
	MissileImpact4,
	MissileMatchlock1,
	MissileMatchlock2,
	MissileMatchlock3,
	MissileMatchlock4,
	Sword1,
	Sword2,
	Sword3,
	Sword4,
	NumberOfSoundSamples
};


enum class SoundTrackID : int
{
	Amaterasu,
	AttackCommand,
	BattleToTheDeath,
	DreamingWaves,
	GeishaGarden,
	HorseCharge,
	MarchingOnTheBattlefield,
	OrderOfShogun,
	SamuraiWarsTheme,
	StormOfSusanoo,
	SwiftAsAnArrow,
	TheHumiliation,
	NumberOfSoundTracks
};


enum class SoundCookieID : int
{
	None
};


class SoundPlayer
{
	struct Track
	{
#ifdef OPENWAR_USE_AVFOUNDATION
		AVAudioPlayer* _player{};
#endif
#ifdef OPENWAR_USE_SDL_MIXER_MUSIC
		Mix_Music* _music{};
#endif
	};

	struct Sample
	{
#ifdef OPENWAR_USE_OPENAL
		ALuint _buffer{};
#endif
#ifdef OPENWAR_USE_SDL_MIXER
		Mix_Chunk* _chunk{};
#endif
	};

	struct Channel
	{
#ifdef OPENWAR_USE_OPENAL
		ALuint _source{};
#endif
#ifdef OPENWAR_USE_SDL_MIXER
		int _channel{};
#endif

		Sample* _current{};
		SoundCookieID _cookie{};
	};

	static SoundPlayer* _singleton;

#ifdef OPENWAR_USE_OPENAL
	ALCdevice* _device{};
	ALCcontext* _context{};
#endif

	Track _tracks[NUMBER_OF_SOUND_TRACKS]{};
	Sample _samples[NUMBER_OF_SOUND_SAMPLES]{};
	Channel _channels[NUMBER_OF_SOUND_CHANNELS]{};

	bool _disableMusic{};
	Track* _currentTrack{};
	SoundChannelID _nextChannelMatchlock{SoundChannelID::MissileMatchlock1};
	SoundChannelID _nextChannelArrows{SoundChannelID::MissileArrows1};
	int _lastSoundCookie{};
	bool _isPaused{};
	std::chrono::system_clock::time_point _casualtyTimer{};
	bool _meleeCavalry{};
	bool _meleeInfantry{};
	int _cavalryCount{};
	double _horseTimer{};
	double _swordTimer{};

	bool _meleeCharging{};
	std::chrono::system_clock::time_point _meleeChargeTimer{};
	MusicDirector _musicDirector{this};

public:
	static void Initialize();
	static SoundPlayer* GetSingleton();

	SoundPlayer();
	~SoundPlayer();

	SoundPlayer(const SoundPlayer&) = delete;
	SoundPlayer& operator=(const SoundPlayer&) = delete;

	MusicDirector& GetMusicDirector();

	void CheckOtherMusicPlaying();

	void Tick(double secondsSinceLastTick);
	void TickHorse(double secondsSinceLastTick);
	void TickSword(double secondsSinceLastTick);

	bool IsPaused() const;
	void Pause();
	void Resume();
	void StopAll();

	void PlayTrack(SoundTrackID soundTrackID);
	void StopTrack();
	bool IsTrackPlaying() const;

	void PlayBackground();

	void UpdateInfantryWalking(bool value);
	void UpdateInfantryRunning(bool value);
	void UpdateCavalryWalking(bool value);
	void UpdateCavalryRunning(bool value);
	void UpdateCavalryCount(int value);

	void UpdateMeleeCavalry(bool value);
	void UpdateMeleeInfantry(bool value);
	void UpdateMeleeCharging();

	SoundCookieID PlayMissileArrows();
	void StopMissileArrows(SoundCookieID soundCookieID);
	void PlayMissileImpact();

	void PlayMissileMatchlock();

	void PlayCasualty();

	void PlayUserInterfaceSound(SoundSampleID soundSampleID);

private:
	Track& GetTrack(SoundTrackID soundTrackID);
	Sample& GetSample(SoundSampleID soundSampleID);
	Channel& GetChannel(SoundChannelID soundChannelID);

	SoundSampleID RandomCasualtySample() const;
	SoundSampleID RandomHorseSample() const;
	SoundSampleID RandomMissileImpactSample() const;
	SoundSampleID RandomMatchlockSample() const;
	SoundSampleID RandomSwordSample() const;

	SoundChannelID NextSoundChannel(SoundChannelID soundChannelID) const;

	void LoadTrack(Track& track, const char* name, bool loop);
	void LoadSample(Sample& sample, const char* name);

	SoundCookieID PlaySound(Channel& channel, Sample* sample, bool looping);
	void StopSound(Channel& channel);
};


#endif
