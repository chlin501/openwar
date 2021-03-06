// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef Resource_H
#define Resource_H

#include "OpenWarPlatform.h"
#include <string>


class Resource
{
	static std::string _resources_path;

#if defined(OPENWAR_PLATFORM_IOS) || defined(OPENWAR_PLATFORM_MAC)
	NSData* _nsdata;
#endif
    
	std::string _name;
	std::string _type;
	const void* _data;
	size_t _size;

public:
	static void init_path(const char* path);
    static void init(const char* argv0);
    
	explicit Resource(const char* name);
	//resource(const char* name, const char* type);
	~Resource();

	const char* name() const { return _name.c_str(); }
	const char* type() const { return _type.c_str(); }
	const char* path() const;

	bool load(const char* type = nullptr);

	const void* data() const { return _data; }
	size_t size() const { return _size; }
};


#endif
