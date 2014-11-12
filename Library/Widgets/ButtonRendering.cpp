// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "ButtonRendering.h"


ButtonRendering::ButtonRendering(GraphicsContext* gc) :
	_gc(gc)
{
	_string_font = gc->GetWidgetTextureAtlas()->GetTextureFont(FontDescriptor(true, 18));

	TextureAtlas* widgetTextureAtlas = gc->GetWidgetTextureAtlas();

	TextureSheet buttonBackgroundSheet = widgetTextureAtlas->AddTextureSheet(Image().LoadFromResource(resource("Textures/ButtonNormal.png")));
	TextureSheet buttonHighlightSheet = widgetTextureAtlas->AddTextureSheet(Image().LoadFromResource(resource("Textures/ButtonHighlight.png")));
	TextureSheet buttonSelectedSheet = widgetTextureAtlas->AddTextureSheet(Image().LoadFromResource(resource("Textures/ButtonSelected.png")));
	TextureSheet buttonIcons = widgetTextureAtlas->AddTextureSheet(Image().LoadFromResource(resource("Textures/ButtonIcons.png")));
	TextureSheet editorTools = widgetTextureAtlas->AddTextureSheet(Image().LoadFromResource(resource("Textures/EditorTools.png")));

	buttonBackground = buttonBackgroundSheet.NewTextureImage(0, 0, 64, 64);
	buttonBackground->_inner = bounds2f(buttonBackground->_inner.center());

	buttonHighlight = buttonHighlightSheet.NewTextureImage(0, 0, 64, 64);
	buttonHighlight->_inner = bounds2f(buttonHighlight->_inner.center());

	buttonSelected = buttonSelectedSheet.NewTextureImage(0, 0, 64, 64);
	buttonSelected->_inner = bounds2f(buttonSelected->_inner.center());

	buttonIconPlay = buttonIcons.NewTextureImage(0, 0, 25, 32);
	buttonIconPause = buttonIcons.NewTextureImage(25, 0, 25, 32);

	buttonEditorToolHand  = editorTools.NewTextureImage(0, 0, 64, 64);
	buttonEditorToolPaint = editorTools.NewTextureImage(64, 0, 64, 64);
	buttonEditorToolErase = editorTools.NewTextureImage(128, 0, 64, 64);
	buttonEditorToolSmear = editorTools.NewTextureImage(196, 0, 64, 64);
	buttonEditorToolHills = editorTools.NewTextureImage(0, 64, 64, 64);
	buttonEditorToolTrees = editorTools.NewTextureImage(64, 64, 64, 64);
	buttonEditorToolWater = editorTools.NewTextureImage(128, 64, 64, 64);
	buttonEditorToolFords = editorTools.NewTextureImage(196, 64, 64, 64);
}
