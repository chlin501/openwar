// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "ButtonRendering.h"
#include "ButtonGrid.h"


ButtonRendering::ButtonRendering(GraphicsContext* gc) :
_gc(gc)
{
	_string_font = new TextureFont(gc->GetWidgetTextureAtlas(), FontDescriptor(true, 18));
	_string_shape = new WidgetShape(gc->GetWidgetTextureAtlas());

	_textureAtlas = new TextureAtlas(gc);

	TextureSheet buttonBackgroundSheet = _textureAtlas->AddTextureSheet(Image().LoadFromResource(resource("Textures/ButtonNormal.png")));
	TextureSheet buttonHighlightSheet = _textureAtlas->AddTextureSheet(Image().LoadFromResource(resource("Textures/ButtonHighlight.png")));
	TextureSheet buttonSelectedSheet = _textureAtlas->AddTextureSheet(Image().LoadFromResource(resource("Textures/ButtonSelected.png")));
	TextureSheet buttonIcons = _textureAtlas->AddTextureSheet(Image().LoadFromResource(resource("Textures/ButtonIcons.png")));
	TextureSheet editorTools = _textureAtlas->AddTextureSheet(Image().LoadFromResource(resource("Textures/EditorTools.png")));

	buttonBackground = buttonBackgroundSheet.GetTextureImage(0, 0, 64, 64);
	buttonBackground->_inner = bounds2f(buttonBackground->_inner.center());

	buttonHighlight = buttonHighlightSheet.GetTextureImage(0, 0, 64, 64);
	buttonHighlight->_inner = bounds2f(buttonHighlight->_inner.center());

	buttonSelected = buttonSelectedSheet.GetTextureImage(0, 0, 64, 64);
	buttonSelected->_inner = bounds2f(buttonSelected->_inner.center());

	buttonIconPlay = buttonIcons.GetTextureImage(0, 0, 25, 32);
	buttonIconPause = buttonIcons.GetTextureImage(25, 0, 25, 32);

	buttonEditorToolHand  = editorTools.GetTextureImage(  0,  0, 64, 64);
	buttonEditorToolPaint = editorTools.GetTextureImage( 64,  0, 64, 64);
	buttonEditorToolErase = editorTools.GetTextureImage(128,  0, 64, 64);
	buttonEditorToolSmear = editorTools.GetTextureImage(196,  0, 64, 64);
	buttonEditorToolHills = editorTools.GetTextureImage(  0, 64, 64, 64);
	buttonEditorToolTrees = editorTools.GetTextureImage( 64, 64, 64, 64);
	buttonEditorToolWater = editorTools.GetTextureImage(128, 64, 64, 64);
	buttonEditorToolFords = editorTools.GetTextureImage(196, 64, 64, 64);
}


void ButtonRendering::RenderTexturePatch(const glm::mat4& transform, TextureImage* textureImage, bounds2f outer_xy, bounds2f inner_xy)
{
	PatchGlyph patchGlyph;

	patchGlyph.outer_xy = outer_xy;
	patchGlyph.inner_xy = inner_xy;
	patchGlyph.outer_uv = textureImage->GetOuterUV();
	patchGlyph.inner_uv = textureImage->GetInnerUV();

	WidgetShape widgetShape(textureImage->GetTextureAtlas());
	widgetShape.AddGlyph(&patchGlyph);

	RenderCall<WidgetShader> renderCall(_gc);

	renderCall.SetVertices(widgetShape.GetVertices());
	renderCall.SetTexture("texture", textureImage->GetTextureAtlas());
	renderCall.SetUniform("transform", transform);
	renderCall.SetUniform("color", glm::vec4(1, 1, 1, 1));
	renderCall.Render();
}


void ButtonRendering::RenderButtonIcon(const glm::mat4& transform, glm::vec2 position, TextureImage* buttonIcon, bool disabled)
{
	if (buttonIcon != nullptr)
	{
		bounds2f bounds = buttonIcon->GetOuterBounds();
		bounds -= bounds.center();
		bounds += position;

		RenderTextureImage(transform, buttonIcon, bounds, disabled ? 0.5f : 1.0f);
	}
}


void ButtonRendering::RenderTextureImage(const glm::mat4& transform, TextureImage* textureImage, bounds2f bounds_xy, float alpha)
{
	PatchGlyph patchGlyph;

	patchGlyph.outer_xy = bounds_xy;
	patchGlyph.inner_xy = bounds_xy;
	patchGlyph.outer_uv = textureImage->GetOuterUV();
	patchGlyph.inner_uv = textureImage->GetInnerUV();
	patchGlyph._alpha = alpha;

	WidgetShape widgetShape(textureImage->GetTextureAtlas());
	widgetShape.AddGlyph(&patchGlyph);

	RenderCall<WidgetShader> renderCall(_gc);

	renderCall.SetVertices(widgetShape.GetVertices());
	renderCall.SetTexture("texture", textureImage->GetTextureAtlas());
	renderCall.SetUniform("transform", transform);
	renderCall.SetUniform("color", glm::vec4(1, 1, 1, 1));
	renderCall.Render();
}


void ButtonRendering::RenderStringGlyph(const glm::mat4& transform, glm::vec2 position, const char* text)
{
	StringGlyph stringGlyph(text, glm::mat4x4());
	_string_shape->AddGlyph(&stringGlyph);

	glm::vec2 p = position - 0.5f * _string_shape->MeasureGlyph(&stringGlyph) - glm::vec2(0, 1);

	RenderCall<WidgetShader> renderCall(_gc);

	renderCall.SetVertices(_string_shape->GetVertices());
	renderCall.SetTexture("texture", _gc->GetWidgetTextureAtlas());
	renderCall.SetUniform("transform", glm::translate(transform, glm::vec3(p, 0)));
	renderCall.SetUniform("color", glm::vec4(1, 1, 1, 1));
	renderCall.Render();
}
