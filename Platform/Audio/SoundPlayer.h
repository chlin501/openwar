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


#define NUMBER_OF_SOUND_SAMPLES 14
#define NUMBER_OF_SOUND_CHANNELS 16


enum class SoundSampleID
{
	None = 0,
	ArrowsFlying = 1,
	CavalryMarching = 2,
	CavalryRunning = 3,
	CommandAck = 4,
	CommandMod = 5,
	InfantryFighting = 6,
	InfantryGrunting = 7,
	InfantryMarching = 8,
	InfantryRunning = 9,
	MatchlockFire1 = 10,
	MatchlockFire2 = 11,
	MatchlockFire3 = 12,
	MatchlockFire4 = 13
};


enum class SoundChannelID
{
	UserInterface = 0,
	InfantryWalking = 1,
	InfantryRunning = 2,
	CavalryWalking = 3,
	CavalryRunning = 4,
	Charging = 5,
	Fighting = 6,
	Matchlock1 = 7,
	Matchlock2 = 8,
	Matchlock3 = 9,
	Matchlock4 = 10,
	Arrows1 = 11,
	Arrows2 = 12,
	Arrows3 = 13,
	Arrows4 = 14,
	Grunts = 15
};


using SoundCookieID = int;


class SoundPlayer
{
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
		int _cookie{};
	};


	static SoundPlayer* _singleton;

#ifdef OPENWAR_USE_OPENAL
	ALCdevice* _device{};
	ALCcontext* _context{};
#endif

	Sample _samples[NUMBER_OF_SOUND_SAMPLES]{};
	Channel _channels[NUMBER_OF_SOUND_CHANNELS]{};

	SoundChannelID _nextChannelMatchlock{SoundChannelID::Matchlock1};
	SoundChannelID _nextChannelArrows{SoundChannelID::Arrows1};
	SoundCookieID _nextCookie{1};
	bool _isPaused{};

public:
	static void Initialize();
	static SoundPlayer* GetSingleton();

	SoundPlayer();
	~SoundPlayer();

	SoundPlayer(const SoundPlayer&) = delete;
	SoundPlayer& operator=(const SoundPlayer&) = delete;

	bool IsPaused() const;
	void Pause();
	void Resume();

	void UpdateInfantryWalking(bool value);
	void UpdateInfantryRunning(bool value);
	void UpdateCavalryWalking(bool value);
	void UpdateCavalryRunning(bool value);
	void UpdateCharging(bool value);
	void UpdateFighting(bool value);

	void PlayGrunts();
	void PlayMatchlock();
	SoundCookieID PlayArrows();

	void PlayUserInterfaceSound(SoundSampleID soundSampleID);
	void Stop(SoundCookieID cookie);
	void StopAll();

private:
	Sample& GetSoundSample(SoundSampleID soundSampleID);
	Channel& GetSoundChannel(SoundChannelID soundChannelID);

	SoundSampleID RandomMatchlockSample() const;
	SoundChannelID NextSoundChannel(SoundChannelID soundChannelID) const;

	void LoadSoundSample(Sample& soundSample, const char* name);

	int PlaySound(Channel& soundChannel, Sample* soundSample, bool looping);
	void StopSound(Channel& soundChannel);
};


#endif
