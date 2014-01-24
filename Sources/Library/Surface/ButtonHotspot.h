// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef ButtonHotspot_H
#define ButtonHotspot_H

#include "Hotspot.h"

class Button;


class ButtonHotspot : public Hotspot
{
	Button* _buttonControl;

public:
	ButtonHotspot(Button* buttonControl);
	virtual ~ButtonHotspot();

	Button* GetButtonControl() const { return _buttonControl; }
};



#endif
