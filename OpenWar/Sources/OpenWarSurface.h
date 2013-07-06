// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef OpenWarSurface_H
#define OpenWarSurface_H

#include "Surface.h"
#include "EditorModel.h"

class BattleGesture;
class BattleModel;
class BattleScript;
class BattleView;
class ButtonGesture;
class ButtonItem;
class ButtonRendering;
class ButtonView;
class EditorGesture;
class BattleSimulator;
class SmoothTerrainSurfaceRenderer;
class TerrainGesture;
class TiledTerrainSurfaceRenderer;


class OpenWarSurface : public Surface
{
public: // TODO: just testing
	enum class Mode { None, Editing, Playing };
	Mode _mode;

	BattleView* _battleView;
	BattleSimulator* _battleSimulator;

	renderers* _renderers;
	ButtonRendering* _buttonRendering;

	EditorModel* _editorModel;

	ButtonView* _buttonsTopLeft;
	ButtonView* _buttonsTopRight;

	TerrainGesture* _terrainGesture;
	BattleGesture* _battleGesture;
	EditorGesture* _editorGesture;
	ButtonGesture* _buttonGesture;

	ButtonItem* _buttonItemHand;
	ButtonItem* _buttonItemPaint;
	ButtonItem* _buttonItemErase;
	ButtonItem* _buttonItemSmear;
	ButtonItem* _buttonItemHills;
	ButtonItem* _buttonItemWater;
	ButtonItem* _buttonItemTrees;

public:
	OpenWarSurface(glm::vec2 size, float pixelDensity);
	virtual ~OpenWarSurface();

	void Reset(BattleModel* battleModel);

	virtual void ScreenSizeChanged();
	virtual void Update(double secondsSinceLastUpdate);
	virtual void Render();

	virtual void MouseEnter(glm::vec2 position);
	virtual void MouseHover(glm::vec2 position);
	virtual void MouseLeave(glm::vec2 position);

private:
	void UpdateSoundPlayer();

	void ClickedPlay();
	void ClickedPause();
	void ClickedRewind();

	void SetEditorMode(EditorMode editorMode);
	void SetEditorFeature(EditorFeature editorFeature);

	void UpdateButtonsAndGestures();
};


#endif
