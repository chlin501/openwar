// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef ButtonRendering_H
#define ButtonRendering_H

#include "Algebra/bounds.h"
#include "Graphics/texture.h"
#include "StringShapeX.h"


struct ButtonIcon;

struct ButtonRendering
{
	renderers* _renderers;
	StringFont* _string_font;
	StringShapeX* _string_shape;

	texture* _textureButtonBackground;
	texture* _textureButtonHighlight;
	texture* _textureButtonSelected;
	texture* _textureButtonIcons;
	texture* _textureEditorTools;

	ButtonIcon* buttonIconPlay;
	ButtonIcon* buttonIconPause;
	ButtonIcon* buttonIconRewind;
	ButtonIcon* buttonIconHelp;
	ButtonIcon* buttonIconChat;

	ButtonIcon* buttonEditorToolHand;
	ButtonIcon* buttonEditorToolPaint;
	ButtonIcon* buttonEditorToolErase;
	ButtonIcon* buttonEditorToolSmear;
	ButtonIcon* buttonEditorToolHills;
	ButtonIcon* buttonEditorToolTrees;
	ButtonIcon* buttonEditorToolWater;
	ButtonIcon* buttonEditorToolFords;

	ButtonRendering(renderers* r, float pixelDensity);

	void RenderBackground(const glm::mat4& transform, bounds2f bounds);
	void RenderHighlight(const glm::mat4& transform, bounds2f bounds);
	void RenderSelected(const glm::mat4& transform, bounds2f bounds);
	void RenderButtonText(const glm::mat4& transform, glm::vec2 position, const char* text);

	void RenderCornerButton(const glm::mat4& transform, texture* texture, bounds2f bounds, float radius);
	void RenderButtonIcon(const glm::mat4& transform, glm::vec2 position, ButtonIcon* buttonIcon, bool disabled);
	void RenderTextureRect(const glm::mat4& transform, texture* texture, bounds2f b, bounds2f t, float alpha = 1);

};


#endif
