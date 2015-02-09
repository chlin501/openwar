// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "SoundPlayer.h"
#include "SoundLoader.h"
#include "Storage/Resource.h"

#include <cstdlib>

#ifdef OPENWAR_USE_OPENAL
#import <AVFoundation/AVFoundation.h>
#endif

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

	LoadSoundSample(GetSoundSample(SoundSampleID::ArrowsFlying), "ArrowsFlying");
	LoadSoundSample(GetSoundSample(SoundSampleID::CavalryMarching), "CavalryMarching");
	LoadSoundSample(GetSoundSample(SoundSampleID::CavalryRunning), "CavalryRunning");
	LoadSoundSample(GetSoundSample(SoundSampleID::CommandAck), "CommandAck");
	LoadSoundSample(GetSoundSample(SoundSampleID::CommandMod), "CommandMod");
	LoadSoundSample(GetSoundSample(SoundSampleID::InfantryFighting), "InfantryFighting");
	LoadSoundSample(GetSoundSample(SoundSampleID::InfantryGrunting), "InfantryGrunting");
	LoadSoundSample(GetSoundSample(SoundSampleID::InfantryMarching), "InfantryMarching");
	LoadSoundSample(GetSoundSample(SoundSampleID::InfantryRunning), "InfantryRunning");
	LoadSoundSample(GetSoundSample(SoundSampleID::MatchlockFire1), "MatchlockFire1");
	LoadSoundSample(GetSoundSample(SoundSampleID::MatchlockFire2), "MatchlockFire2");
	LoadSoundSample(GetSoundSample(SoundSampleID::MatchlockFire3), "MatchlockFire3");
	LoadSoundSample(GetSoundSample(SoundSampleID::MatchlockFire4), "MatchlockFire4");
}


SoundPlayer::~SoundPlayer()
{
#ifdef OPENWAR_USE_SDL_MIXER
	Mix_CloseAudio();
#endif

	/*
	Context=alcGetCurrentContext();
	Device=alcGetContextsDevice(Context);
	alcMakeContextCurrent(NULL);
	alcDestroyContext(Context);
	alcCloseDevice(Device);
	*/
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


void SoundPlayer::UpdateInfantryWalking(bool value)
{
	if (value)
		PlaySound(GetSoundChannel(SoundChannelID::InfantryWalking), &GetSoundSample(SoundSampleID::InfantryMarching), true);
	else
		StopSound(GetSoundChannel(SoundChannelID::InfantryWalking));
}


void SoundPlayer::UpdateInfantryRunning(bool value)
{
	if (value)
		PlaySound(GetSoundChannel(SoundChannelID::InfantryRunning), &GetSoundSample(SoundSampleID::InfantryRunning), true);
	else
		StopSound(GetSoundChannel(SoundChannelID::InfantryRunning));
}


void SoundPlayer::UpdateCavalryWalking(bool value)
{
	if (value)
		PlaySound(GetSoundChannel(SoundChannelID::CavalryWalking), &GetSoundSample(SoundSampleID::CavalryMarching), true);
	else
		StopSound(GetSoundChannel(SoundChannelID::CavalryWalking));
}


void SoundPlayer::UpdateCavalryRunning(bool value)
{
	if (value)
		PlaySound(GetSoundChannel(SoundChannelID::CavalryRunning), &GetSoundSample(SoundSampleID::CavalryRunning), true);
	else
		StopSound(GetSoundChannel(SoundChannelID::CavalryRunning));
}


void SoundPlayer::UpdateCharging(bool value)
{
	if (value)
		PlaySound(GetSoundChannel(SoundChannelID::Charging), &GetSoundSample(SoundSampleID::InfantryFighting), true);
	else
		StopSound(GetSoundChannel(SoundChannelID::Charging));
}


void SoundPlayer::UpdateFighting(bool value)
{
	if (value)
		PlaySound(GetSoundChannel(SoundChannelID::Fighting), &GetSoundSample(SoundSampleID::InfantryFighting), true);
	else
		StopSound(GetSoundChannel(SoundChannelID::Fighting));
}


void SoundPlayer::PlayGrunts()
{
	PlaySound(GetSoundChannel(SoundChannelID::Grunts), &GetSoundSample(SoundSampleID::InfantryGrunting), false);
}


void SoundPlayer::PlayMatchlock()
{
	SoundSampleID soundSample = RandomMatchlockSample();
	PlaySound(GetSoundChannel(_nextChannelMatchlock), &GetSoundSample(soundSample), false);
	_nextChannelMatchlock = NextSoundChannel(_nextChannelMatchlock);
}


SoundCookieID SoundPlayer::PlayArrows()
{
	int cookie = PlaySound(GetSoundChannel(_nextChannelArrows), &GetSoundSample(SoundSampleID::ArrowsFlying), false);
	_nextChannelArrows = NextSoundChannel(_nextChannelArrows);

	return cookie;
}


void SoundPlayer::PlayUserInterfaceSound(SoundSampleID soundSampleID)
{
	PlaySound(GetSoundChannel(SoundChannelID::UserInterface), &GetSoundSample(soundSampleID), false);
}


void SoundPlayer::Stop(SoundCookieID cookie)
{
	for (int i = 0; i < NUMBER_OF_SOUND_CHANNELS; ++i)
	{
		if (_channels[i]._cookie == cookie)
		{
			StopSound(GetSoundChannel((SoundChannelID)i));
		}
	}
}


void SoundPlayer::StopAll()
{
	for (int i = 0; i < NUMBER_OF_SOUND_CHANNELS; ++i)
	{
		StopSound(GetSoundChannel((SoundChannelID)i));
	}
}


SoundPlayer::Sample& SoundPlayer::GetSoundSample(SoundSampleID soundSampleID)
{
	return _samples[static_cast<int>(soundSampleID)];
}


SoundPlayer::Channel& SoundPlayer::GetSoundChannel(SoundChannelID soundChannelID)
{
	return _channels[static_cast<int>(soundChannelID)];
}



SoundChannelID SoundPlayer::NextSoundChannel(SoundChannelID soundChannelID) const
{
	switch (soundChannelID)
	{
		case SoundChannelID::Matchlock1: return SoundChannelID::Matchlock2;
		case SoundChannelID::Matchlock2: return SoundChannelID::Matchlock3;
		case SoundChannelID::Matchlock3: return SoundChannelID::Matchlock4;
		case SoundChannelID::Matchlock4: return SoundChannelID::Matchlock1;
		case SoundChannelID::Arrows1: return SoundChannelID::Arrows2;
		case SoundChannelID::Arrows2: return SoundChannelID::Arrows3;
		case SoundChannelID::Arrows3: return SoundChannelID::Arrows4;
		case SoundChannelID::Arrows4: return SoundChannelID::Arrows1;
		default: return SoundChannelID::UserInterface;
	}
}


SoundSampleID SoundPlayer::RandomMatchlockSample() const
{
	switch (std::rand() & 3)
	{
		case 0: return SoundSampleID::MatchlockFire1;
		case 1: return SoundSampleID::MatchlockFire2;
		case 2: return SoundSampleID::MatchlockFire3;
		default: return SoundSampleID::MatchlockFire4;
	}
}


void SoundPlayer::LoadSoundSample(Sample& soundSample, const char* name)
{
#ifdef OPENWAR_USE_OPENAL
	SoundLoader s(name);
	alBufferData(soundSample._buffer, s.format, s.data, s.size, s.freq);
#endif

#ifdef OPENWAR_USE_SDL_MIXER
	std::string path = Resource((std::string("Sounds/") + name + std::string(".wav")).c_str()).path();
	soundSample._chunk = Mix_LoadWAV(path.c_str());
#endif
}


int SoundPlayer::PlaySound(Channel& soundChannel, Sample* soundSample, bool looping)
{
	if (looping && soundChannel._current == soundSample)
		return soundChannel._cookie;

#ifdef OPENWAR_USE_OPENAL
	ALuint source = soundChannel._source;
	ALuint buffer = soundSample->_buffer;

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

	Mix_PlayChannel(soundChannel._channel, soundSample->_chunk, looping ? -1 : 0);
#endif

	soundChannel._current = soundSample;
	soundChannel._cookie = _nextCookie++;

	return soundChannel._cookie;
}


void SoundPlayer::StopSound(Channel& soundChannel)
{
#ifdef OPENWAR_USE_OPENAL
	alSourceStop(soundChannel._source);
#endif

#ifdef OPENWAR_USE_SDL_MIXER
	Mix_HaltChannel(soundChannel._channel);
#endif

	soundChannel._current = nullptr;
	soundChannel._cookie = 0;
}
