#ifndef FontDescriptor_H
#define FontDescriptor_H

#include <string>


struct FontDescriptor
{
	std::string name;
	float size;
	bool bold;

	FontDescriptor() : name(), size(12), bold(false) { }
	FontDescriptor(const char* n, float s) : name(n), size(s), bold(false) { }
	FontDescriptor(bool b, float s) : name(), size(s), bold(b) { }
};


#endif
