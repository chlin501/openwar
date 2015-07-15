// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "OpenWarSurface.h"
#include "Widgets/ButtonGrid.h"
#include "BattleView/BattleGesture.h"
#include "BattleView/BattleView.h"
#include "TerrainView/EditorGesture.h"
#include "TerrainView/EditorHotspot.h"
#include "Graphics/Viewport.h"
#include "Audio/SoundPlayer.h"
#include "Widgets/ButtonGrid.h"
#include <glm/gtc/matrix_transform.hpp>


OpenWarSurface::OpenWarSurface(GraphicsContext* gc) : Surface(gc),
_buttonGridTextureSheet(nullptr),
_editorModel(nullptr),
_buttonsTopLeft(nullptr),
_buttonsTopRight(nullptr),
_buttonItemHand(nullptr),
_buttonItemPaint(nullptr),
_buttonItemErase(nullptr),
_buttonItemSmear(nullptr),
_buttonItemHills(nullptr),
_buttonItemTrees(nullptr),
_buttonItemWater(nullptr),
_buttonItemFords(nullptr),
_battleLayer(nullptr)
{
	SoundPlayer::Initialize();

	_battleLayer = new BattleLayer(this);

	_buttonGridTextureSheet = new ButtonGridTextureSheet(gc->GetTextureAtlas(WIDGET_TEXTURE_ATLAS));

	std::shared_ptr<ScrollerViewport> viewport = std::make_shared<ScrollerViewport>(gc);

	_buttonsTopLeft = new ButtonGrid(this, viewport, _buttonGridTextureSheet, ButtonAlignment::TopLeft);
	_buttonsTopRight = new ButtonGrid(this, viewport, _buttonGridTextureSheet, ButtonAlignment::TopRight);

	ButtonArea* toolButtonArea = _buttonsTopLeft->AddButtonArea(4);
	_buttonItemHand = toolButtonArea->AddButtonItem(_buttonGridTextureSheet->buttonEditorToolHand);
	_buttonItemSmear = toolButtonArea->AddButtonItem(_buttonGridTextureSheet->buttonEditorToolSmear);
	_buttonItemPaint = toolButtonArea->AddButtonItem(_buttonGridTextureSheet->buttonEditorToolPaint);
	_buttonItemErase = toolButtonArea->AddButtonItem(_buttonGridTextureSheet->buttonEditorToolErase);

	ButtonArea* featureButtonArea = _buttonsTopLeft->AddButtonArea(4);
	_buttonItemHills = featureButtonArea->AddButtonItem(_buttonGridTextureSheet->buttonEditorToolHills);
	_buttonItemTrees = featureButtonArea->AddButtonItem(_buttonGridTextureSheet->buttonEditorToolTrees);
	_buttonItemWater = featureButtonArea->AddButtonItem(_buttonGridTextureSheet->buttonEditorToolWater);
	_buttonItemFords = featureButtonArea->AddButtonItem(_buttonGridTextureSheet->buttonEditorToolFords);

	ButtonArea* xxx = _buttonsTopLeft->AddButtonArea(2);
	ButtonItem* item1 = xxx->AddButtonItem(_buttonGridTextureSheet->buttonEditorToolHand);
	ButtonItem* item2 = xxx->AddButtonItem(_buttonGridTextureSheet->buttonEditorToolHand);
	ButtonItem* item3 = xxx->AddButtonItem(_buttonGridTextureSheet->buttonEditorToolHand);
	ButtonItem* item4 = xxx->AddButtonItem(_buttonGridTextureSheet->buttonEditorToolHand);

	item1->SetButtonText("Test");
	/*item2->SetButtonText("Selected");
	item3->SetButtonText("Highlight");
	item4->SetButtonText("Disabled");*/

	item1->SetAction([](){});
	item2->SetSelected(true);
	item3->SetHighlight(true);
	item4->SetDisabled(true);

	_buttonItemHand->SetAction([this](){ SetEditorMode(EditorMode::Hand); });
	_buttonItemPaint->SetAction([this](){ SetEditorMode(EditorMode::Paint); });
	_buttonItemErase->SetAction([this](){ SetEditorMode(EditorMode::Erase); });
	_buttonItemSmear->SetAction([this](){ SetEditorMode(EditorMode::Smear); });
	_buttonItemHills->SetAction([this](){ SetEditorFeature(TerrainFeature::Hills); });
	_buttonItemTrees->SetAction([this](){ SetEditorFeature(TerrainFeature::Trees); });
	_buttonItemWater->SetAction([this](){ SetEditorFeature(TerrainFeature::Water); });
	_buttonItemFords->SetAction([this](){ SetEditorFeature(TerrainFeature::Fords); });

	_buttonItemHand->SetKeyboardShortcut('1');
	_buttonItemSmear->SetKeyboardShortcut('2');
	_buttonItemPaint->SetKeyboardShortcut('3');
	_buttonItemErase->SetKeyboardShortcut('4');
	_buttonItemHills->SetKeyboardShortcut('5');
	_buttonItemTrees->SetKeyboardShortcut('6');
	_buttonItemWater->SetKeyboardShortcut('7');
	_buttonItemFords->SetKeyboardShortcut('8');

	UpdateButtons();
}


OpenWarSurface::~OpenWarSurface()
{

}


void OpenWarSurface::ResetBattleViews(BattleSimulator* battleSimulator, const std::vector<BattleCommander*>& commanders)
{
	_battleLayer->ResetEditor(battleSimulator, commanders);
	_battleLayer->SetPlaying(false);
	_editorModel = _battleLayer->GetEditorModel();
	UpdateButtons();
}


void OpenWarSurface::RenderViews()
{
	_battleLayer->Render();
	_buttonsTopLeft->Render();
	_buttonsTopRight->Render();
}


void OpenWarSurface::Animate(double secondsSinceLastUpdate)
{
	SoundPlayer::GetSingleton()->Tick(secondsSinceLastUpdate);

	bounds2f viewportBounds = bounds2f(0, 0, GetVirtualSize());

	_buttonsTopLeft->GetViewport().SetViewportBounds(viewportBounds);
	_buttonsTopRight->GetViewport().SetViewportBounds(viewportBounds);

	if (_battleLayer->GetBattleSimulator() && _battleLayer->IsPlaying())
		_battleLayer->GetBattleSimulator()->AdvanceTime((float)secondsSinceLastUpdate);
}


void OpenWarSurface::ClickedPlay()
{
	_battleLayer->SetPlaying(true);
	UpdateButtons();
}


void OpenWarSurface::ClickedPause()
{
	_battleLayer->SetPlaying(false);
	UpdateButtons();
}


void OpenWarSurface::SetEditorMode(EditorMode editorMode)
{
	if (_editorModel)
	{
		_editorModel->SetEditorMode(editorMode);
		UpdateButtons();
	}
}


void OpenWarSurface::SetEditorFeature(TerrainFeature terrainFeature)
{
	if (_editorModel)
	{
		_editorModel->SetTerrainFeature(terrainFeature);
		UpdateButtons();
	}
}


void OpenWarSurface::UpdateButtons()
{
	bool playing = _battleLayer->IsPlaying();
	bool editing = _editorModel && _editorModel->GetEditorMode() != EditorMode::Hand;
	_battleLayer->SetEditing(editing);

	/*
	_buttonItemHand->SetDisabled(playing);
	_buttonItemPaint->SetDisabled(playing);
	_buttonItemErase->SetDisabled(playing);
	_buttonItemSmear->SetDisabled(playing);
	_buttonItemHills->SetDisabled(playing);
	_buttonItemTrees->SetDisabled(playing);
	_buttonItemWater->SetDisabled(playing);
	_buttonItemFords->SetDisabled(playing);
	*/

	if (_editorModel)
	{
		_buttonItemHand->SetSelected(_editorModel->GetEditorMode() == EditorMode::Hand);
		_buttonItemPaint->SetSelected(_editorModel->GetEditorMode() == EditorMode::Paint);
		_buttonItemErase->SetSelected(_editorModel->GetEditorMode() == EditorMode::Erase);
		_buttonItemSmear->SetSelected(_editorModel->GetEditorMode() == EditorMode::Smear);
		_buttonItemHills->SetSelected(_editorModel->GetTerrainFeature() == TerrainFeature::Hills);
		_buttonItemTrees->SetSelected(_editorModel->GetTerrainFeature() == TerrainFeature::Trees);
		_buttonItemWater->SetSelected(_editorModel->GetTerrainFeature() == TerrainFeature::Water);
		_buttonItemFords->SetSelected(_editorModel->GetTerrainFeature() == TerrainFeature::Fords);
	}

	/*if (_editorGesture)
	{
		_editorGesture->SetEnabled(editing);
	}*/

	_buttonsTopRight->Reset();
	if (playing)
		_buttonsTopRight->AddButtonArea()->AddButtonItem(_buttonGridTextureSheet->buttonIconPause)->SetAction([this](){ ClickedPause(); });
	else
		_buttonsTopRight->AddButtonArea()->AddButtonItem(_buttonGridTextureSheet->buttonIconPlay)->SetAction([this](){ ClickedPlay(); });
}
