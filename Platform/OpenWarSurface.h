// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef OpenWarSurface_H
#define OpenWarSurface_H

#include "Surface/Surface.h"
#include "BattleView/BattleLayer.h"
#include "TerrainView/EditorModel.h"

class ButtonGesture;
class ButtonItem;
class ButtonRendering;
class ButtonGrid;
class EditorGesture;
class GradientLineShape3;
//class SmoothTerrainSurfaceRenderer;
//class TiledTerrainSurfaceRenderer;


class OpenWarSurface : public Surface
{
public:
	ButtonRendering* _buttonRendering;

	EditorModel* _editorModel;
	EditorGesture* _editorGesture;

	ButtonGrid* _buttonsTopLeft;
	ButtonGrid* _buttonsTopRight;
	ButtonGesture* _buttonGesture;
	ButtonItem* _buttonItemHand;
	ButtonItem* _buttonItemPaint;
	ButtonItem* _buttonItemErase;
	ButtonItem* _buttonItemSmear;
	ButtonItem* _buttonItemHills;
	ButtonItem* _buttonItemTrees;
	ButtonItem* _buttonItemWater;
	ButtonItem* _buttonItemFords;

	GradientLineShape3* _scriptHintRenderer;

	BattleLayer* _battleLayer;

public:
	OpenWarSurface(glm::vec2 size, float pixelDensity);
	virtual ~OpenWarSurface();

	BattleLayer* GetBattleLayer() const { return _battleLayer; }

	// BattleSurface

	virtual void ResetBattleViews(BattleScenario* scenario, const std::vector<BattleCommander*>& commanders);    

	// Surface


	virtual bool NeedsRender() const;
	virtual void RenderSurface();

	virtual void MouseEnter(glm::vec2 position);
	virtual void MouseHover(glm::vec2 position);
	virtual void MouseLeave(glm::vec2 position);

	// Content
	virtual void OnBoundsChanged();
	virtual void Update(double secondsSinceLastUpdate);

protected:
	void ClickedPlay();
	void ClickedPause();

	void SetEditorMode(EditorMode editorMode);
	void SetEditorFeature(TerrainFeature editorFeature);

	void UpdateButtonsAndGestures();
};


#endif
