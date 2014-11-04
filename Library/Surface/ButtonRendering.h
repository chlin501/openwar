// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef ButtonRendering_H
#define ButtonRendering_H

#include "Algebra/bounds.h"
#include "Texture.h"
#include "WidgetShape.h"
#include "CommonShaders.h"


struct TextureImageX;

struct ButtonRendering
{
	GraphicsContext* _gc;

	TextureFont* _string_font;
	WidgetShape* _string_shape;

	TextureAtlas* _textureButtonBackground;
	TextureAtlas* _textureButtonHighlight;
	TextureAtlas* _textureButtonSelected;
	TextureAtlas* _textureButtonIcons;
	TextureAtlas* _textureEditorTools;

	TextureImageX* buttonIconPlay;
	TextureImageX* buttonIconPause;

	TextureImageX* buttonEditorToolHand;
	TextureImageX* buttonEditorToolPaint;
	TextureImageX* buttonEditorToolErase;
	TextureImageX* buttonEditorToolSmear;
	TextureImageX* buttonEditorToolHills;
	TextureImageX* buttonEditorToolTrees;
	TextureImageX* buttonEditorToolWater;
	TextureImageX* buttonEditorToolFords;

	ButtonRendering(GraphicsContext* gc);

	void RenderStringGlyph(const glm::mat4& transform, glm::vec2 position, const char* text);
	void RenderTexturePatch(const glm::mat4& transform, TextureAtlas* textureAtlas, bounds2f outer_xy, float radius);
	void RenderButtonIcon(const glm::mat4& transform, glm::vec2 position, TextureImageX* buttonIcon, bool disabled);
	void RenderTextureImage(const glm::mat4& transform, TextureAtlas* textureAtlas, bounds2f bounds_xy, bounds2f bounds_uv, float alpha = 1);

};


#endif
