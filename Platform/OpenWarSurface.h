// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef OpenWarSurface_H
#define OpenWarSurface_H

#include "Surface/Surface.h"
#include "BattleView/BattleLayer.h"
#include "TerrainView/EditorModel.h"
#include "Graphics/RenderLoopObserver.h"

class ButtonGesture;
class ButtonItem;
class ButtonRendering;
class ButtonGrid;
class EditorHotspot;


class OpenWarSurface : public Surface, RenderLoopObserver
{
public:
	ButtonRendering* _buttonRendering;

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

	virtual void ResetBattleViews(BattleScenario* scenario, const std::vector<BattleCommander*>& commanders);    

	virtual void Render();

private: // RenderLoopObserver
	virtual void OnRenderLoop(double secondsSinceLastUpdate);

protected:
	void ClickedPlay();
	void ClickedPause();

	void SetEditorMode(EditorMode editorMode);
	void SetEditorFeature(TerrainFeature editorFeature);

	void UpdateButtons();
};


#endif
