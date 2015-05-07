// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef EDITORMODEL_H
#define EDITORMODEL_H

#include <set>
#include "SmoothTerrain/SmoothTerrainRenderer.h"

class BattleView;
class EditorModel;

enum class EditorMode { Hand, Paint, Erase, Smear };


class EditorModelObserver
{
	friend class EditorModel;
public:
	virtual ~EditorModelObserver();
private:
	virtual void OnEditorModeChanged(EditorModel* editorModel) = 0;
	virtual void OnTerrainFeatureChanged(EditorModel* editorModel) = 0;
};


class EditorModel
{
	std::set<EditorModelObserver*> _observers;
	BattleView* _battleView;
	SmoothTerrainRenderer* _smoothTerrainSurface;
	EditorMode _editorMode;
	TerrainFeature _terrainFeature;
	Image* _brush;
	Image* _mixer;
	glm::vec2 _brushPosition;
	float _brushDistance;

public:
	EditorModel(BattleView* battleView, SmoothTerrainRenderer* smoothTerrainSurface);

	void AddObserver(EditorModelObserver* observer);
	void RemoveObserver(EditorModelObserver* observer);

	EditorMode GetEditorMode() const;
	void SetEditorMode(EditorMode value);

	TerrainFeature GetTerrainFeature() const;
	void SetTerrainFeature(TerrainFeature value);

	void ToolBegan(glm::vec2 position);
	void ToolMoved(glm::vec2 position);
	void ToolEnded(glm::vec2 position);

	const Image& GetSmoothMapImage() const;

private:
	void Paint(TerrainFeature feature, glm::vec2 position, bool value);

	void SmearReset(TerrainFeature feature, glm::vec2 position);
	void SmearPaint(TerrainFeature feature, glm::vec2 position);
};


#endif
