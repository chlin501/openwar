// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "Resource.h"
#include <cstdlib>

#ifdef OPENWAR_USE_SDL
#include <SDL2/SDL.h>
#endif


#if defined(PHALANX_TARGET_OS_IOS) || defined(PHALANX_TARGET_OS_MAC)
static NSString* GetPath(const char* name, const char* type)
{
	if (name[0] == '/')
	{
		return [NSString stringWithFormat:@"%s%s", name, type];
	}
	else
	{
		NSString* path = [NSString stringWithFormat:@"%@%@", [NSString stringWithUTF8String:name], [NSString stringWithUTF8String:type]];
		NSString* d = [path stringByDeletingLastPathComponent];
		NSString* file = [path lastPathComponent];
		NSString* n = [file stringByDeletingPathExtension];
		NSString* t = [file pathExtension];

		return [[NSBundle mainBundle] pathForResource:n ofType:t inDirectory:d];
	}
}
#endif


std::string Resource::_resources_path;


void Resource::init_path(const char* path)
{
	_resources_path = path;
}


void Resource::init(const char* argv0)
{
	std::string app_path(argv0);

	std::string::size_type i = app_path.rfind('/');
	if (i != std::string::npos)
	{
#ifdef PHALANX_TARGET_OS_IOS
		_resources_path = app_path.substr(0, i) + "/";
#else
		i = app_path.rfind('/', i - 1);
		if (i != std::string::npos)
		{
			_resources_path = app_path.substr(0, i) + "/Resources/";
		}
#endif
	}
}



Resource::Resource(const char* name) :
#if defined(PHALANX_TARGET_OS_IOS) || defined(PHALANX_TARGET_OS_MAC)
_nsdata(nil),
#endif
_name(),
_type(),
_data(nullptr),
_size(0)
{
#if defined(PHALANX_TARGET_OS_IOS) || defined(PHALANX_TARGET_OS_MAC)
    
	NSString* s = [NSString stringWithUTF8String:name];
	NSString* n = [s stringByDeletingPathExtension];
	NSString* t = [s pathExtension];
	_name.assign(n.UTF8String);
	_type.assign(".");
	_type.append(t.UTF8String);
    
#else

	std::string s(name);
	std::string::size_type i = s.rfind('.');
	if (i != std::string::npos)
	{
		_name = s.substr(0, i);
		_type = s.substr(i);
	}
	else
	{
		_name = s;
	}

#endif
}


/*resource::resource(const char* name, const char* type) :
#if defined(PHALANX_TARGET_OS_IOS) || defined(PHALANX_TARGET_OS_MAC)
_nsdata(nil),
#endif
_name(name),
_type(type),
_data(nullptr),
_size(0)
{
}*/


Resource::~Resource()
{
}


const char* Resource::path() const
{
/*#if defined(PHALANX_TARGET_OS_IOS) || defined(PHALANX_TARGET_OS_MAC)

    return GetPath(_name.c_str(), _type.c_str()).UTF8String;

#else*/

    static std::string s;
    
    if (_name.empty() || _name[0] == '/')
        s.clear();
    else
        s.assign(_resources_path);

	s.append(_name);
    s.append(_type);
    
    return s.c_str();

//#endif
}


bool Resource::load(char const* type)
{
#if defined(PHALANX_TARGET_OS_IOS) || defined(PHALANX_TARGET_OS_MAC)

	NSString* path = GetPath(_name.c_str(), type ?: _type.c_str());
	NSData* data = [NSData dataWithContentsOfFile:path];
	if (data != nil)
	{
		if (type)
			_type = type;
        
		_nsdata = data;
        
		_data = data.bytes;
		_size = data.length;
	}
    
	return data != nil;
    
#else

	SDL_RWops* rw = SDL_RWFromFile(path(), "rb");
    if (rw == nullptr)
        return false;

	_size = SDL_RWsize(rw);
	void* ptr = std::malloc(_size);
	SDL_RWread(rw, ptr, _size, 1);

	_data = ptr;

	SDL_RWclose(rw);

    return true;

/*  temporarily remove fopen, didn't work on Android

	FILE* file = fopen(path(), "rb");
	if (!file)
		return false;

	fseek(file, 0, SEEK_END);
	_size = (size_t)ftell(file);
	fseek(file, 0, SEEK_SET);

	void* ptr = std::malloc(_size);
	fread(ptr, _size, 1, file);

	_data = ptr;

	fclose(file);

	return true;
*/

#endif
}
