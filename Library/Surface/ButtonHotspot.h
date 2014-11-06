// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef ButtonHotspot_H
#define ButtonHotspot_H

#include <functional>
#include "Hotspot.h"
#include "ButtonGesture.h"

class Button;


class ButtonHotspot : public Hotspot<ButtonHotspot, ButtonGesture>
{
	std::function<void()> _action;
	bool _highlight;
	bool _stationary;
	bool _immediate;

public:
	ButtonHotspot();
	virtual ~ButtonHotspot();

	virtual std::function<void()> GetClickAction() const;
	virtual void SetClickAction(std::function<void()> value);

	virtual bool ShouldReleaseWhenMoving() const;
	virtual void SetReleaseWhenMoving(bool value);

	virtual bool IsImmediateClick() const;
	virtual void SetImmediateClick(bool value);

	virtual bool IsHighlight() const;
	virtual void SetHighlight(bool value);
};


#endif
