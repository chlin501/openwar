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
	WidgetShape* _string_shape;

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

	void RenderStringGlyph(const glm::mat4& transform, glm::vec2 position, const char* text);
	void RenderTexturePatch(const glm::mat4& transform, TextureImage* textureImage, bounds2f outer_xy, bounds2f inner_xy);
	void RenderButtonIcon(const glm::mat4& transform, glm::vec2 position, TextureImage* buttonIcon, bool disabled);
	void RenderTextureImage(const glm::mat4& transform, TextureImage* textureImage, bounds2f bounds_xy, float alpha = 1);

};


#endif
