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

#include <chrono>


enum class SoundSampleID
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

#define NUMBER_OF_SOUND_SAMPLES (static_cast<int>(SoundSampleID::NumberOfSoundSamples))


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
	Sword,
	NumberOfSoundChannels
};

#define NUMBER_OF_SOUND_CHANNELS (static_cast<int>(SoundChannelID::NumberOfSoundChannels))


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

	SoundChannelID _nextChannelMatchlock{SoundChannelID::MissileMatchlock1};
	SoundChannelID _nextChannelArrows{SoundChannelID::MissileArrows1};
	SoundCookieID _nextCookie{1};
	bool _isPaused{};
	std::chrono::system_clock::time_point _casualtyTimer{};
	bool _meleeCavalry{};
	bool _meleeInfantry{};
	int _cavalryCount{};
	double _horseTimer{};
	double _swordTimer{};

	bool _meleeCharging{};
	std::chrono::system_clock::time_point _meleeChargeTimer{};

public:
	static void Initialize();
	static SoundPlayer* GetSingleton();

	SoundPlayer();
	~SoundPlayer();

	SoundPlayer(const SoundPlayer&) = delete;
	SoundPlayer& operator=(const SoundPlayer&) = delete;

	void Tick(double secondsSinceLastUpdate);
	void TickHorse(double secondsSinceLastUpdate);
	void TickSword(double secondsSinceLastUpdate);

	bool IsPaused() const;
	void Pause();
	void Resume();
	void StopAll();

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

	void PlayMissileMatchlock();

	void PlayCasualty();

	void PlayUserInterfaceSound(SoundSampleID soundSampleID);

private:
	Sample& GetSample(SoundSampleID soundSampleID);
	Channel& GetChannel(SoundChannelID soundChannelID);

	SoundSampleID RandomCasualtySample() const;
	SoundSampleID RandomHorseSample() const;
	SoundSampleID RandomMatchlockSample() const;
	SoundSampleID RandomSwordSample() const;

	SoundChannelID NextSoundChannel(SoundChannelID soundChannelID) const;

	void LoadSample(Sample& sample, const char* name);

	int PlaySound(Channel& channel, Sample* sample, bool looping);
	void StopSound(Channel& channel);
};


#endif
