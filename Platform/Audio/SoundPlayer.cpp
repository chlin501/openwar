// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "SoundPlayer.h"
#include "SoundLoader.h"
#include "Storage/Resource.h"

#include <cstdlib>


SoundPlayer* SoundPlayer::_singleton = nullptr;


void SoundPlayer::Initialize()
{
	if (SoundPlayer::_singleton == nullptr)
		SoundPlayer::_singleton = new SoundPlayer();
}


SoundPlayer* SoundPlayer::GetSingleton()
{
	return _singleton;
}


SoundPlayer::SoundPlayer()
{
#ifdef OPENWAR_USE_OPENAL
	[[AVAudioSession sharedInstance] setCategory: AVAudioSessionCategoryAmbient error: nil];

	_device = alcOpenDevice(nullptr); // select the "preferred device"
	if (_device != nullptr)
	{
		_context = alcCreateContext(_device, 0);
	}

	alcMakeContextCurrent(_context);

	// Generate Buffers
	alGetError(); // clear error code

	ALuint buffers[NUMBER_OF_SOUND_SAMPLES];
	alGenBuffers(NUMBER_OF_SOUND_SAMPLES, buffers);
	if (ALenum error = alGetError())
	{
		//DisplayALError("alGenBuffers :", error);
		return;
	}
	for (int i = 0; i < NUMBER_OF_SOUND_SAMPLES; ++i)
		_samples[i]._buffer = buffers[i];

	ALuint sources[NUMBER_OF_SOUND_CHANNELS];
	alGenSources(NUMBER_OF_SOUND_CHANNELS, sources);
	if (ALenum error = alGetError())
	{
		return;
	}
	for (int i = 0; i < NUMBER_OF_SOUND_CHANNELS; ++i)
		_channels[i]._source = sources[i];


	float orientation[6] =
			{
					0.0f, 0.0f, -1.0f, // direction
					0.0f, 1.0f, 0.0f //up
			};

	orientation[0] = 0; //sin(rads);
	orientation[1] = 0.0f;            // No Change to the Y vector
	orientation[2] = -1; //-cos(rads);

	alListenerfv(AL_ORIENTATION, orientation);
#endif

#ifdef OPENWAR_USE_SDL_MIXER
	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 4096) == -1)
	{
	}

	Mix_AllocateChannels(NUMBER_OF_SOUND_CHANNELS);
	for (int i = 0; i < NUMBER_OF_SOUND_CHANNELS; ++i)
		_channels[i]._channel = i;

#endif

	LoadSample(GetSample(SoundSampleID::CommandAck), "CommandAck");
	LoadSample(GetSample(SoundSampleID::CommandMod), "CommandMod");

	LoadSample(GetSample(SoundSampleID::Background), "Samurai Background");

	LoadSample(GetSample(SoundSampleID::Casualty1), "Samurai Casualty 1");
	LoadSample(GetSample(SoundSampleID::Casualty2), "Samurai Casualty 2");
	LoadSample(GetSample(SoundSampleID::Casualty3), "Samurai Casualty 3");
	LoadSample(GetSample(SoundSampleID::Casualty4), "Samurai Casualty 4");
	LoadSample(GetSample(SoundSampleID::Casualty5), "Samurai Casualty 5");
	LoadSample(GetSample(SoundSampleID::Casualty6), "Samurai Casualty 6");
	LoadSample(GetSample(SoundSampleID::Casualty7), "Samurai Casualty 7");
	LoadSample(GetSample(SoundSampleID::Casualty8), "Samurai Casualty 8");

	LoadSample(GetSample(SoundSampleID::CavalryRunning), "Samurai Cavalry Running");
	LoadSample(GetSample(SoundSampleID::CavalryWalking), "Samurai Cavalry Walking");

	LoadSample(GetSample(SoundSampleID::HorseNeigh1), "Samurai Horse Neigh 1");
	LoadSample(GetSample(SoundSampleID::HorseNeigh2), "Samurai Horse Neigh 2");
	LoadSample(GetSample(SoundSampleID::HorseNeigh3), "Samurai Horse Neigh 3");
	LoadSample(GetSample(SoundSampleID::HorseSnort), "Samurai Horse Snort");

	LoadSample(GetSample(SoundSampleID::InfantryRunning), "Samurai Infantry Running");
	LoadSample(GetSample(SoundSampleID::InfantryWalking), "Samurai Infantry Walking");

	LoadSample(GetSample(SoundSampleID::MeleeCavalry), "Samurai Melee Cavalry");
	LoadSample(GetSample(SoundSampleID::MeleeCharging), "Samurai Melee Charging");
	LoadSample(GetSample(SoundSampleID::MeleeInfantry), "Samurai Melee Infantry");

	LoadSample(GetSample(SoundSampleID::MissileArrows), "Samurai Missile Arrows");
	LoadSample(GetSample(SoundSampleID::MissileMatchlock1), "Samurai Missile Matchlock 1");
	LoadSample(GetSample(SoundSampleID::MissileMatchlock2), "Samurai Missile Matchlock 2");
	LoadSample(GetSample(SoundSampleID::MissileMatchlock3), "Samurai Missile Matchlock 3");
	LoadSample(GetSample(SoundSampleID::MissileMatchlock4), "Samurai Missile Matchlock 4");

	LoadSample(GetSample(SoundSampleID::Sword1), "Samurai Sword 1");
	LoadSample(GetSample(SoundSampleID::Sword2), "Samurai Sword 2");
	LoadSample(GetSample(SoundSampleID::Sword3), "Samurai Sword 3");
	LoadSample(GetSample(SoundSampleID::Sword4), "Samurai Sword 4");

	LoadTrack(GetTrack(SoundTrackID::Title), "Samurai Sample", true);
	LoadTrack(GetTrack(SoundTrackID::DreamingWaves), "Dreaming Waves", false);
	LoadTrack(GetTrack(SoundTrackID::Amaterasu), "Amaterasu", false);
	LoadTrack(GetTrack(SoundTrackID::GeishaGarden), "Geisha Garden", false);
	LoadTrack(GetTrack(SoundTrackID::StormOfSusanoo), "Storm of Susanoo", false);
	LoadTrack(GetTrack(SoundTrackID::HorseCharge), "Horse Charge", false);
}


SoundPlayer::~SoundPlayer()
{
#ifdef OPENWAR_USE_SDL_MIXER
	Mix_CloseAudio();
#endif

#ifdef OPENWAR_USE_AVFOUNDATION
	for (int i = 0; i < NUMBER_OF_SOUND_TRACKS; ++i)
		[_tracks[i]._player release];
#endif

	/*
	Context=alcGetCurrentContext();
	Device=alcGetContextsDevice(Context);
	alcMakeContextCurrent(NULL);
	alcDestroyContext(Context);
	alcCloseDevice(Device);
	*/
}


MusicDirector& SoundPlayer::GetMusicDirector()
{
	return _musicDirector;
}


void SoundPlayer::Tick(double secondsSinceLastTick)
{
	_musicDirector.Tick(secondsSinceLastTick);

	TickHorse(secondsSinceLastTick);
	TickSword(secondsSinceLastTick);
}


static double Random(double min, double max)
{
	return min + (max - min) * ((std::rand() & 0x7fff) / (double)0x7fff);
}


void SoundPlayer::TickHorse(double secondsSinceLastTick)
{
	_horseTimer -= secondsSinceLastTick;
	if (_horseTimer < 0)
	{
		if (_cavalryCount != 0)
			PlaySound(GetChannel(SoundChannelID::Horse), &GetSample(RandomHorseSample()), false);

		_horseTimer = Random(5.0, 15.0);
	}
}


void SoundPlayer::TickSword(double secondsSinceLastTick)
{
	_swordTimer -= secondsSinceLastTick;
	if (_swordTimer < 0)
	{
		if (_meleeInfantry || _meleeCavalry)
			PlaySound(GetChannel(SoundChannelID::Sword), &GetSample(RandomSwordSample()), false);
		_swordTimer = Random(1.0, 3.0);
	}
}


bool SoundPlayer::IsPaused() const
{
	return _isPaused;
}


void SoundPlayer::Pause()
{
	if (!_isPaused)
	{
		_isPaused = true;
#ifdef OPENWAR_USE_OPENAL
		for (int i = 0; i < NUMBER_OF_SOUND_CHANNELS; ++i)
			if (_channels[i]._current)
				alSourcePause(_channels[i]._source);
#endif

#ifdef OPENWAR_USE_SDL_MIXER
		Mix_Pause(-1);
#endif
	}
}


void SoundPlayer::Resume()
{
	if (_isPaused)
	{
		_isPaused = false;
#ifdef OPENWAR_USE_OPENAL
		for (int i = 0; i < NUMBER_OF_SOUND_CHANNELS; ++i)
			if (_channels[i]._current)
				alSourcePlay(_channels[i]._source);
#endif

#ifdef OPENWAR_USE_SDL_MIXER
		Mix_Resume(-1);
#endif
	}
}


void SoundPlayer::StopAll()
{
	_cavalryCount = 0;
	_meleeInfantry = false;
	_meleeCavalry = false;

	for (int i = 0; i < NUMBER_OF_SOUND_CHANNELS; ++i)
	{
		StopSound(GetChannel((SoundChannelID)i));
	}
}


void SoundPlayer::PlayTrack(SoundTrackID soundTrackID)
{
	Track* track = &GetTrack(soundTrackID);

#ifdef OPENWAR_USE_AVFOUNDATION
	if (_currentTrack)
		[_currentTrack->_player stop];
	[track->_player prepareToPlay];
	track->_player.currentTime = 0;
	[track->_player play];
#endif

	_currentTrack = track;
}


bool SoundPlayer::IsTrackPlaying() const
{
#ifdef OPENWAR_USE_AVFOUNDATION
	return _currentTrack && [_currentTrack->_player isPlaying];
#endif

	return false;
}


void SoundPlayer::PlayBackground()
{
	PlaySound(GetChannel(SoundChannelID::Background), &GetSample(SoundSampleID::Background), true);
}


void SoundPlayer::UpdateInfantryWalking(bool value)
{
	if (value)
		PlaySound(GetChannel(SoundChannelID::InfantryWalking), &GetSample(SoundSampleID::InfantryWalking), true);
	else
		StopSound(GetChannel(SoundChannelID::InfantryWalking));
}


void SoundPlayer::UpdateInfantryRunning(bool value)
{
	if (value)
		PlaySound(GetChannel(SoundChannelID::InfantryRunning), &GetSample(SoundSampleID::InfantryRunning), true);
	else
		StopSound(GetChannel(SoundChannelID::InfantryRunning));
}


void SoundPlayer::UpdateCavalryWalking(bool value)
{
	if (value)
		PlaySound(GetChannel(SoundChannelID::CavalryWalking), &GetSample(SoundSampleID::CavalryWalking), true);
	else
		StopSound(GetChannel(SoundChannelID::CavalryWalking));
}


void SoundPlayer::UpdateCavalryRunning(bool value)
{
	if (value)
		PlaySound(GetChannel(SoundChannelID::CavalryRunning), &GetSample(SoundSampleID::CavalryRunning), true);
	else
		StopSound(GetChannel(SoundChannelID::CavalryRunning));
}


void SoundPlayer::UpdateCavalryCount(int value)
{
	_cavalryCount = value;
}


void SoundPlayer::UpdateMeleeCavalry(bool value)
{
	if (value)
		PlaySound(GetChannel(SoundChannelID::MeleeCavalry), &GetSample(SoundSampleID::MeleeCavalry), true);
	else
		StopSound(GetChannel(SoundChannelID::MeleeCavalry));
	_meleeCavalry = value;
	UpdateMeleeCharging();
}


void SoundPlayer::UpdateMeleeInfantry(bool value)
{
	if (value)
		PlaySound(GetChannel(SoundChannelID::MeleeInfantry), &GetSample(SoundSampleID::MeleeInfantry), true);
	else
		StopSound(GetChannel(SoundChannelID::MeleeInfantry));
	_meleeInfantry = value;
	UpdateMeleeCharging();
}


void SoundPlayer::UpdateMeleeCharging()
{
	bool isMelee = _meleeCavalry || _meleeInfantry;
	if (!_meleeCharging && isMelee)
	{
		if (std::chrono::system_clock::now() > _meleeChargeTimer)
		{
			PlaySound(GetChannel(SoundChannelID::MeleeCharging), &GetSample(SoundSampleID::MeleeCharging), false);
			_meleeCharging = true;
		}
	}
	else if (_meleeCharging && !isMelee)
	{
		_meleeCharging = false;
		_meleeChargeTimer = std::chrono::system_clock::now() + std::chrono::seconds(15);
	}
}


SoundCookieID SoundPlayer::PlayMissileArrows()
{
	SoundCookieID cookie = PlaySound(GetChannel(_nextChannelArrows), &GetSample(SoundSampleID::MissileArrows), false);
	_nextChannelArrows = NextSoundChannel(_nextChannelArrows);
	return cookie;
}


void SoundPlayer::StopMissileArrows(SoundCookieID soundCookieID)
{
	if (soundCookieID != SoundCookieID::None)
		for (int i = 0; i < NUMBER_OF_SOUND_CHANNELS; ++i)
			if (_channels[i]._cookie == soundCookieID)
				StopSound(_channels[i]);
}


void SoundPlayer::PlayMissileMatchlock()
{
	SoundSampleID soundSample = RandomMatchlockSample();
	PlaySound(GetChannel(_nextChannelMatchlock), &GetSample(soundSample), false);
	_nextChannelMatchlock = NextSoundChannel(_nextChannelMatchlock);
}


void SoundPlayer::PlayCasualty()
{
	auto now = std::chrono::system_clock::now();
	if (now > _casualtyTimer)
	{
		SoundSampleID soundSample = RandomCasualtySample();
		PlaySound(GetChannel(SoundChannelID::Casualty), &GetSample(soundSample), false);
		_casualtyTimer = now + std::chrono::seconds(2);
	}
}


void SoundPlayer::PlayUserInterfaceSound(SoundSampleID soundSampleID)
{
	PlaySound(GetChannel(SoundChannelID::UserInterface), &GetSample(soundSampleID), false);
}


SoundPlayer::Track& SoundPlayer::GetTrack(SoundTrackID soundTrackID)
{
	return _tracks[static_cast<int>(soundTrackID)];
}


SoundPlayer::Sample& SoundPlayer::GetSample(SoundSampleID soundSampleID)
{
	return _samples[static_cast<int>(soundSampleID)];
}


SoundPlayer::Channel& SoundPlayer::GetChannel(SoundChannelID soundChannelID)
{
	return _channels[static_cast<int>(soundChannelID)];
}


SoundChannelID SoundPlayer::NextSoundChannel(SoundChannelID soundChannelID) const
{
	switch (soundChannelID)
	{
		case SoundChannelID::MissileMatchlock1: return SoundChannelID::MissileMatchlock2;
		case SoundChannelID::MissileMatchlock2: return SoundChannelID::MissileMatchlock1;
		case SoundChannelID::MissileArrows1: return SoundChannelID::MissileArrows2;
		case SoundChannelID::MissileArrows2: return SoundChannelID::MissileArrows1;
		default: return SoundChannelID::UserInterface;
	}
}


SoundSampleID SoundPlayer::RandomCasualtySample() const
{
	switch (std::rand() & 7)
	{
		case 0: return SoundSampleID::Casualty1;
		case 1: return SoundSampleID::Casualty2;
		case 2: return SoundSampleID::Casualty3;
		case 3: return SoundSampleID::Casualty4;
		case 4: return SoundSampleID::Casualty5;
		case 5: return SoundSampleID::Casualty6;
		case 6: return SoundSampleID::Casualty7;
		default: return SoundSampleID::Casualty8;
	}
}


SoundSampleID SoundPlayer::RandomHorseSample() const
{
	switch (std::rand() & 3)
	{
		case 0: return SoundSampleID::HorseNeigh1;
		case 1: return SoundSampleID::HorseNeigh2;
		case 2: return SoundSampleID::HorseNeigh3;
		default: return SoundSampleID::HorseSnort;
	}
}



SoundSampleID SoundPlayer::RandomMatchlockSample() const
{
	switch (std::rand() & 3)
	{
		case 0: return SoundSampleID::MissileMatchlock1;
		case 1: return SoundSampleID::MissileMatchlock2;
		case 2: return SoundSampleID::MissileMatchlock3;
		default: return SoundSampleID::MissileMatchlock4;
	}
}


SoundSampleID SoundPlayer::RandomSwordSample() const
{
	switch (std::rand() & 3)
	{
		case 0: return SoundSampleID::Sword1;
		case 1: return SoundSampleID::Sword2;
		case 2: return SoundSampleID::Sword3;
		default: return SoundSampleID::Sword4;
	}
}


void SoundPlayer::LoadTrack(Track& track, const char* name, bool loop)
{
#ifdef OPENWAR_USE_AVFOUNDATION
	NSString* path = [[NSBundle mainBundle] pathForResource:[NSString stringWithUTF8String:name] ofType:@"mp3"];
	if (path == nil)
	{
		NSLog(@"SoundPlayer::LoadTrack: missing track: %s", name);
		return;
	}

	NSURL * url = [[NSURL alloc] initFileURLWithPath:path];
	track._player = [[AVAudioPlayer alloc] initWithContentsOfURL:url error:nil];
	track._player.numberOfLoops = loop ? -1 : 0;
	[url release];
#endif
}


void SoundPlayer::LoadSample(Sample& sample, const char* name)
{
#ifdef OPENWAR_USE_OPENAL
	SoundLoader s(name);
	alBufferData(sample._buffer, s.format, s.data, s.size, s.freq);
#endif

#ifdef OPENWAR_USE_SDL_MIXER
	std::string path = Resource((std::string("Sounds/") + name + std::string(".wav")).c_str()).path();
	sample._chunk = Mix_LoadWAV(path.c_str());
#endif
}


SoundCookieID SoundPlayer::PlaySound(Channel& channel, Sample* sample, bool looping)
{
	if (looping && channel._current == sample)
		return channel._cookie;

#ifdef OPENWAR_USE_OPENAL
	ALuint source = channel._source;
	ALuint buffer = sample->_buffer;

	alSourceStop(source);

	alSourcei(source, AL_BUFFER, buffer);
	alSourcef(source, AL_PITCH, 1.0f);
	alSourcef(source, AL_REFERENCE_DISTANCE, 500);
	alSourcef(source, AL_MIN_GAIN, 0.5f);
	alSource3f(source, AL_POSITION, 0, 0, 0);
	alSourcei(source, AL_LOOPING, looping ? AL_TRUE : AL_FALSE);
	alSourcePlay(source);
#endif

#ifdef OPENWAR_USE_SDL_MIXER
	Mix_PlayChannel(channel._channel, sample->_chunk, looping ? -1 : 0);
#endif

	channel._current = sample;
	channel._cookie = static_cast<SoundCookieID>(++_lastSoundCookie);

	return channel._cookie;
}


void SoundPlayer::StopSound(Channel& channel)
{
#ifdef OPENWAR_USE_OPENAL
	alSourceStop(channel._source);
#endif

#ifdef OPENWAR_USE_SDL_MIXER
	Mix_HaltChannel(channel._channel);
#endif

	channel._current = nullptr;
	channel._cookie = SoundCookieID::None;
}
