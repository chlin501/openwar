// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "ButtonGridTextureSheet.h"
#include "Graphics/TextureAtlas.h"


ButtonGridTextureSheet::ButtonGridTextureSheet(TextureAtlas* textureAtlas)
{
	TextureSheet buttonBackgroundSheet = textureAtlas->AddTextureSheet(Image().LoadFromResource(Resource("Textures/ButtonNormal.png")));
	TextureSheet buttonHighlightSheet = textureAtlas->AddTextureSheet(Image().LoadFromResource(Resource("Textures/ButtonHighlight.png")));
	TextureSheet buttonSelectedSheet = textureAtlas->AddTextureSheet(Image().LoadFromResource(Resource("Textures/ButtonSelected.png")));
	TextureSheet buttonIcons = textureAtlas->AddTextureSheet(Image().LoadFromResource(Resource("Textures/ButtonIcons.png")));
	TextureSheet editorTools = textureAtlas->AddTextureSheet(Image().LoadFromResource(Resource("Textures/EditorTools.png")));

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
