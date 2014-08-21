// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "SoundPlayer.h"
#include "SoundLoader.h"
#include "resource.h"

#include <cstdlib>

#import <Foundation/Foundation.h>


SoundPlayer* SoundPlayer::singleton = nullptr;


static void _LoadSound(SoundPlayer* soundPlayer, SoundBuffer soundBuffer, const char* name)
{
#ifdef OPENWAR_USE_OPENAL
	SoundLoader sound(name);
	soundPlayer->LoadSound(soundBuffer, sound.format, sound.data, sound.size, sound.freq);
#endif

#ifdef OPENWAR_USE_SDL_MIXER
	std::string path = resource((name + std::string(".wav")).c_str()).path();
	soundPlayer->LoadSound(soundBuffer, Mix_LoadWAV(path.c_str()));
#endif
}


void SoundPlayer::Initialize()
{
	if (SoundPlayer::singleton == nullptr)
	{
		SoundPlayer::singleton = new SoundPlayer();
		_LoadSound(SoundPlayer::singleton, SoundBufferArrowsFlying, "ArrowsFlying");
		_LoadSound(SoundPlayer::singleton, SoundBufferCavalryMarching, "CavalryMarching");
		_LoadSound(SoundPlayer::singleton, SoundBufferCavalryRunning, "CavalryRunning");
		_LoadSound(SoundPlayer::singleton, SoundBufferCommandAck, "CommandAck");
		_LoadSound(SoundPlayer::singleton, SoundBufferCommandMod, "CommandMod");
		_LoadSound(SoundPlayer::singleton, SoundBufferInfantryFighting, "InfantryFighting");
		_LoadSound(SoundPlayer::singleton, SoundBufferInfantryGrunting, "InfantryGrunting");
		_LoadSound(SoundPlayer::singleton, SoundBufferInfantryMarching, "InfantryMarching");
		_LoadSound(SoundPlayer::singleton, SoundBufferInfantryRunning, "InfantryRunning");
		_LoadSound(SoundPlayer::singleton, SoundBufferMatchlockFire1, "MatchlockFire1");
		_LoadSound(SoundPlayer::singleton, SoundBufferMatchlockFire2, "MatchlockFire2");
		_LoadSound(SoundPlayer::singleton, SoundBufferMatchlockFire3, "MatchlockFire3");
		_LoadSound(SoundPlayer::singleton, SoundBufferMatchlockFire4, "MatchlockFire4");
	}
}



SoundPlayer::SoundPlayer() :
#ifdef OPENWAR_USE_OPENAL
	_device(nullptr),
	_context(nullptr),
	_nextMatchlock(SoundSourceMatchlockFirst),
	_nextArrows(SoundSourceArrowsFirst),
#endif
	_nextCookie(1),
	_isPaused(false)
{
#ifdef OPENWAR_USE_OPENAL
	ALenum error;

	_device = alcOpenDevice(nullptr); // select the "preferred device"
	if (_device != nullptr)
	{
		_context = alcCreateContext(_device, 0);
	}

	alcMakeContextCurrent(_context);

	// Generate Buffers
	alGetError(); // clear error code
	alGenBuffers(NUMBER_OF_SOUND_BUFFERS, _buffers);
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		//DisplayALError("alGenBuffers :", error);
		return;
	}

	alGenSources(NUMBER_OF_SOUND_SOURCES, _sources);
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		return;
	}

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
		NSLog(@"Mix_OpenAudio error: %s", Mix_GetError());
	}

	Mix_AllocateChannels(NUMBER_OF_SOUND_SOURCES);
#endif
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


#ifdef OPENWAR_USE_OPENAL
void SoundPlayer::LoadSound(SoundBuffer soundBuffer, ALenum format, ALvoid* data, ALsizei size, ALsizei freq)
{
	int index = (int)soundBuffer;
	alBufferData(_buffers[index], format, data, size, freq);
}
#endif


#ifdef OPENWAR_USE_SDL_MIXER
void SoundPlayer::LoadSound(SoundBuffer soundBuffer, Mix_Chunk* chunk)
{
	int index = (int)soundBuffer;
	_chunks[index] = chunk;
}
#endif


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
		for (int i = 0; i < NUMBER_OF_SOUND_SOURCES; ++i)
			if (_playing[i])
				alSourcePause(_sources[i]);
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
		for (int i = 0; i < NUMBER_OF_SOUND_SOURCES; ++i)
			if (_playing[i])
				alSourcePlay(_sources[i]);
#endif

#ifdef OPENWAR_USE_SDL_MIXER
		Mix_Resume(-1);
#endif
	}
}


void SoundPlayer::UpdateInfantryWalking(bool value)
{
	if (value)
		PlaySound(SoundSourceInfantryWalking, SoundBufferInfantryMarching, true);
	else
		StopSound(SoundSourceInfantryWalking);
}


void SoundPlayer::UpdateInfantryRunning(bool value)
{
	if (value)
		PlaySound(SoundSourceInfantryRunning, SoundBufferInfantryRunning, true);
	else
		StopSound(SoundSourceInfantryRunning);
}


void SoundPlayer::UpdateCavalryWalking(bool value)
{
	if (value)
		PlaySound(SoundSourceCavalryWalking, SoundBufferCavalryMarching, true);
	else
		StopSound(SoundSourceCavalryWalking);
}


void SoundPlayer::UpdateCavalryRunning(bool value)
{
	if (value)
		PlaySound(SoundSourceCavalryRunning, SoundBufferCavalryRunning, true);
	else
		StopSound(SoundSourceCavalryRunning);
}


void SoundPlayer::UpdateCharging(bool value)
{
	if (value)
		PlaySound(SoundSourceCharging, SoundBufferInfantryFighting, true);
	else
		StopSound(SoundSourceCharging);
}


void SoundPlayer::UpdateFighting(bool value)
{
	if (value)
		PlaySound(SoundSourceFighting, SoundBufferInfantryFighting, true);
	else
		StopSound(SoundSourceFighting);
}


void SoundPlayer::PlayGrunts()
{
	PlaySound(SoundSourceGrunts, SoundBufferInfantryGrunting, false);
}


void SoundPlayer::PlayMatchlock()
{
	SoundBuffer soundBuffer;
	switch (std::rand() & 3)
	{
		case 0:
			soundBuffer = SoundBufferMatchlockFire1;
			break;
		case 1:
			soundBuffer = SoundBufferMatchlockFire2;
			break;
		case 2:
			soundBuffer = SoundBufferMatchlockFire3;
			break;
		default:
			soundBuffer = SoundBufferMatchlockFire4;
			break;
	}

	PlaySound(_nextMatchlock, soundBuffer, false);

	if (_nextMatchlock == SoundSourceMarchlockLast)
		_nextMatchlock = SoundSourceMatchlockFirst;
	else
		_nextMatchlock = (SoundSource)(_nextMatchlock + 1);
}


int SoundPlayer::PlayArrows()
{
	int cookie = PlaySound(_nextArrows, SoundBufferArrowsFlying, false);

	if (_nextArrows == SoundSourceArrowsLast)
		_nextArrows = SoundSourceArrowsFirst;
	else
		_nextArrows = (SoundSource)(_nextArrows + 1);

	return cookie;
}


void SoundPlayer::Play(SoundBuffer soundBuffer)
{
	PlaySound(SoundSourceUserInterface, soundBuffer, false);
}


void SoundPlayer::Stop(int cookie)
{
	for (int i = 0; i < NUMBER_OF_SOUND_SOURCES; ++i)
	{
		if (_cookies[i] == cookie)
		{
			StopSound((SoundSource)i);
		}
	}
}


void SoundPlayer::StopAll()
{
	for (int i = 0; i < NUMBER_OF_SOUND_SOURCES; ++i)
	{
		StopSound((SoundSource)i);
	}
}


int SoundPlayer::PlaySound(SoundSource soundSource, SoundBuffer soundBuffer, bool looping)
{
	int cookie = 0;

#ifdef OPENWAR_USE_OPENAL
	ALuint source = _sources[(int)soundSource];
	ALuint buffer = _buffers[(int)soundBuffer];

	if (looping && _playing[(int)soundSource] == buffer)
		return _cookies[(int)soundSource];

	alSourceStop(source);

	alSourcei(source, AL_BUFFER, buffer);
	alSourcef(source, AL_PITCH, 1.0f);
	alSourcef(source, AL_REFERENCE_DISTANCE, 500);
	alSourcef(source, AL_MIN_GAIN, 0.5f);
	alSource3f(source, AL_POSITION, 0, 0, 0);
	alSourcei(source, AL_LOOPING, looping ? AL_TRUE : AL_FALSE);
	alSourcePlay(source);

	cookie = _nextCookie++;

	_playing[(int)soundSource] = buffer;
	_cookies[(int)soundSource] = cookie;
#endif

#ifdef OPENWAR_USE_SDL_MIXER

	int channel = (int)soundSource;
	Mix_Chunk* chunk = _chunks[(int)soundBuffer];

	if (looping && _playing[(int)soundSource] == chunk)
		return _cookies[(int)soundSource];

	Mix_PlayChannel(channel, chunk, looping ? -1 : 0);

	cookie = _nextCookie++;

	_playing[(int)soundSource] = chunk;
	_cookies[(int)soundSource] = cookie;
#endif

	return cookie;
}


void SoundPlayer::StopSound(SoundSource soundSource)
{
#ifdef OPENWAR_USE_OPENAL
	ALuint source = _sources[(int)soundSource];

	alSourceStop(source);

	_playing[(int)soundSource] = AL_NONE;
	_cookies[(int)soundSource] = 0;
#endif

#ifdef OPENWAR_USE_SDL_MIXER
	Mix_HaltChannel((int)soundSource);

	_playing[(int)soundSource] = nullptr;
	_cookies[(int)soundSource] = 0;

#endif
}
