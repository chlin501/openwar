// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "ButtonRendering.h"
#include "Graphics/TextureAtlas.h"


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

	buttonBackground = buttonBackgroundSheet.NewTextureImage(BorderBounds(0, 0, 64, 64).Inset(32));
	buttonHighlight = buttonHighlightSheet.NewTextureImage(BorderBounds(0, 0, 64, 64).Inset(32));
	buttonSelected = buttonSelectedSheet.NewTextureImage(BorderBounds(0, 0, 64, 64).Inset(32));

	buttonIconPlay = buttonIcons.NewTextureImage(BorderBounds(0, 0, 25, 32));
	buttonIconPause = buttonIcons.NewTextureImage(BorderBounds(25, 0, 25, 32));

	buttonEditorToolHand  = editorTools.NewTextureImage(BorderBounds(0, 0, 64, 64));
	buttonEditorToolPaint = editorTools.NewTextureImage(BorderBounds(64, 0, 64, 64));
	buttonEditorToolErase = editorTools.NewTextureImage(BorderBounds(128, 0, 64, 64));
	buttonEditorToolSmear = editorTools.NewTextureImage(BorderBounds(196, 0, 64, 64));
	buttonEditorToolHills = editorTools.NewTextureImage(BorderBounds(0, 64, 64, 64));
	buttonEditorToolTrees = editorTools.NewTextureImage(BorderBounds(64, 64, 64, 64));
	buttonEditorToolWater = editorTools.NewTextureImage(BorderBounds(128, 64, 64, 64));
	buttonEditorToolFords = editorTools.NewTextureImage(BorderBounds(196, 64, 64, 64));
}
