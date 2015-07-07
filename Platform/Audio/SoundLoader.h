// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef SOUNDLOADER_H
#define SOUNDLOADER_H

#ifdef OPENWAR_USE_OPENAL
#if defined(PHALANX_TARGET_OS_IOS) || defined(PHALANX_TARGET_OS_MAC)
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif
#endif

class SoundLoader
{
public:
#ifdef OPENWAR_USE_OPENAL
	ALenum format;
	ALvoid* data;
	ALsizei size;
	ALsizei freq;
#endif

	SoundLoader(const char* name);
	~SoundLoader();

private:
	SoundLoader(const SoundLoader&);
	SoundLoader& operator = (const SoundLoader&);
};



#endif
