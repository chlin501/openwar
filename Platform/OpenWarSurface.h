// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef OpenWarSurface_H
#define OpenWarSurface_H

#include "Surface/Animation.h"
#include "Surface/Surface.h"
#include "BattleView/BattleLayer.h"
#include "TerrainView/EditorModel.h"

class ClickGesture;
class ButtonItem;
class ButtonGridTextureSheet;
class ButtonGrid;
class EditorHotspot;


class OpenWarSurface : public Surface, AnimationHost
{
public:
	ButtonGridTextureSheet* _buttonGridTextureSheet;

	EditorModel* _editorModel;

	ButtonGrid* _buttonsTopLeft;
	ButtonGrid* _buttonsTopRight;
	ButtonItem* _buttonItemHand;
	ButtonItem* _buttonItemPaint;
	ButtonItem* _buttonItemErase;
	ButtonItem* _buttonItemSmear;
	ButtonItem* _buttonItemHills;
	ButtonItem* _buttonItemTrees;
	ButtonItem* _buttonItemWater;
	ButtonItem* _buttonItemFords;

	BattleLayer* _battleLayer;

public:
	OpenWarSurface(GraphicsContext* gc);
	virtual ~OpenWarSurface();

	// BattleSurface

	virtual void ResetBattleViews(BattleSimulator* scenario, const std::vector<BattleObjects::Commander*>& commanders);

	virtual void RenderViews();

private: // RenderLoopObserver
	virtual void Animate(double secondsSinceLastUpdate);

protected:
	void ClickedPlay();
	void ClickedPause();

	void SetEditorMode(EditorMode editorMode);
	void SetEditorFeature(TerrainFeature editorFeature);

	void UpdateButtons();
};


#endif
