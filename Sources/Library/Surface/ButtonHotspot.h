// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef ButtonHotspot_H
#define ButtonHotspot_H

#include <functional>
#include "Hotspot.h"

class Button;


class ButtonHotspot : public Hotspot
{
	std::function<void()> _action;
	bool _highlight;
	bool _stationary;
	bool _immediate;

public:
	ButtonHotspot();
	virtual ~ButtonHotspot();

	virtual std::function<void()> GetAction() const;
	virtual void SetAction(std::function<void()> value);

	virtual bool IsStationary() const;
	virtual void SetStationary(bool value);

	virtual bool IsImmediate() const;
	virtual void SetImmediate(bool value);

	virtual bool IsHighlight() const;
	virtual void SetHighlight(bool value);
};



#endif
