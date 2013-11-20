// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef OpenWarSurface_H
#define OpenWarSurface_H

#include "BattleSurface.h"
#include "TerrainView/EditorModel.h"

class ButtonGesture;
class ButtonItem;
class ButtonRendering;
class ButtonView;
class EditorGesture;
class GradientLineRenderer;
//class SmoothTerrainSurfaceRenderer;
//class TiledTerrainSurfaceRenderer;


class OpenWarSurface : public BattleSurface
{
public:
	ButtonRendering* _buttonRendering;

	EditorModel* _editorModel;
	EditorGesture* _editorGesture;

	ButtonView* _buttonsTopLeft;
	ButtonView* _buttonsTopRight;
	ButtonGesture* _buttonGesture;
	ButtonItem* _buttonItemHand;
	ButtonItem* _buttonItemPaint;
	ButtonItem* _buttonItemErase;
	ButtonItem* _buttonItemSmear;
	ButtonItem* _buttonItemHills;
	ButtonItem* _buttonItemTrees;
	ButtonItem* _buttonItemWater;
	ButtonItem* _buttonItemFords;

	GradientLineRenderer* _scriptHintRenderer;

public:
	OpenWarSurface(glm::vec2 size, float pixelDensity);
	virtual ~OpenWarSurface();

	virtual void ResetBattleViews(BattleScenario* scenario, const std::vector<BattleCommander*>& commanders);

	virtual void ScreenSizeChanged();
	virtual void Update(double secondsSinceLastUpdate);
	virtual void Render();

	virtual void MouseEnter(glm::vec2 position);
	virtual void MouseHover(glm::vec2 position);
	virtual void MouseLeave(glm::vec2 position);

protected:
	void ClickedPlay();
	void ClickedPause();

	void SetEditorMode(EditorMode editorMode);
	void SetEditorFeature(TerrainFeature editorFeature);

	void UpdateButtonsAndGestures();
};


#endif
