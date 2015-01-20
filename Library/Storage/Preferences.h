#ifndef Preferences_H
#define Preferences_H

#include <string>


class Preferences
{
public:
	static std::string GetString(const char* key);
	static void SetString(const char* key, const char* value);
};


#endif
