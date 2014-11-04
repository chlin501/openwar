// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef ButtonRendering_H
#define ButtonRendering_H

#include "Algebra/bounds.h"
#include "Texture.h"
#include "WidgetShape.h"
#include "CommonShaders.h"



struct ButtonRendering
{
	GraphicsContext* _gc;

	TextureFont* _string_font;
	TextureAtlas* _textureAtlas;

	TextureImage* buttonBackground;
	TextureImage* buttonHighlight;
	TextureImage* buttonSelected;

	TextureImage* buttonIconPlay;
	TextureImage* buttonIconPause;

	TextureImage* buttonEditorToolHand;
	TextureImage* buttonEditorToolPaint;
	TextureImage* buttonEditorToolErase;
	TextureImage* buttonEditorToolSmear;
	TextureImage* buttonEditorToolHills;
	TextureImage* buttonEditorToolTrees;
	TextureImage* buttonEditorToolWater;
	TextureImage* buttonEditorToolFords;

	ButtonRendering(GraphicsContext* gc);
};


#endif
