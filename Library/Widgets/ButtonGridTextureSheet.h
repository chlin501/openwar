// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef ButtonGridTextureSheet_H
#define ButtonGridTextureSheet_H

#include <memory>

class TextureAtlas;
class TextureImage;


struct ButtonGridTextureSheet
{
	std::shared_ptr<TextureImage> buttonBackground;
	std::shared_ptr<TextureImage> buttonHighlight;
	std::shared_ptr<TextureImage> buttonSelected;

	std::shared_ptr<TextureImage> buttonIconPlay;
	std::shared_ptr<TextureImage> buttonIconPause;

	std::shared_ptr<TextureImage> buttonEditorToolHand;
	std::shared_ptr<TextureImage> buttonEditorToolPaint;
	std::shared_ptr<TextureImage> buttonEditorToolErase;
	std::shared_ptr<TextureImage> buttonEditorToolSmear;
	std::shared_ptr<TextureImage> buttonEditorToolHills;
	std::shared_ptr<TextureImage> buttonEditorToolTrees;
	std::shared_ptr<TextureImage> buttonEditorToolWater;
	std::shared_ptr<TextureImage> buttonEditorToolFords;

	ButtonGridTextureSheet(TextureAtlas* textureAtlas);
};


#endif
