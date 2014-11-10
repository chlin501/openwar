#ifndef BattleSurface_H
#define BattleSurface_H

#include "ShaderProgram.h"
#include "Surface/Container.h"
#include "TerrainView/EditorModel.h"
#include "CommonShaders.h"
#include "RenderLoopObserver.h"

class BattleCommander;
class BattleGesture;
class BattleModel;
class BattleScenario;
class BattleScript;
class BattleView;
class EditorHotspot;
class EditorGesture;
class TerrainGesture;
class SmoothTerrainRenderer;
class SmoothTerrainWater;
class SmoothTerrainSky;


class BattleLayer : public Container, public Renderable, RenderLoopObserver, EditorModelObserver
{
	GraphicsContext* _gc;

	bool _playing;
	bool _editing;

	BattleScenario* _scenario;
	std::vector<BattleCommander*> _commanders;

	std::vector<BattleView*> _battleViews;

	EditorModel* _editorModel;
	std::shared_ptr<EditorHotspot> _editorHotspot;

public:
	BattleLayer(GraphicsContext* gc);
	~BattleLayer();

	BattleScenario* GetScenario() const { return _scenario; }
	const std::vector<BattleView*>& GetBattleViews() const { return _battleViews; }
	BattleView* GetPrimaryBattleView() const { return _battleViews.empty() ? nullptr : _battleViews.front(); }
	EditorModel* GetEditorModel() const { return _editorModel; }

	virtual void ResetBattleViews(BattleScenario* scenario, const std::vector<BattleCommander*>& commanders);
	void ResetEditor(BattleScenario* scenario, const std::vector<BattleCommander*>& commanders);

	void ResetCameraPosition();

	void SetPlaying(bool value);
	void SetEditing(bool value);
	bool IsPlaying() const { return _playing; }
	bool IsEditing() const { return _editing; }

	// RenderLoopObserver
	virtual void OnRenderLoop(double secondsSinceLastUpdate);

	virtual void Render();
	virtual bool HasChangedSinceLastRender() const;

	// EditorModelObserver
	virtual void OnEditorModeChanged(EditorModel* editorModel);
	virtual void OnTerrainFeatureChanged(EditorModel* editorModel);

private:
	void CreateBattleView(BattleCommander* commander);
	void ResetBattleView(BattleView* battleView, BattleCommander* commander);
	void RemoveBattleView(BattleView* battleView);

	void UpdateBattleViewSize();

	void UpdateSoundPlayer();
};



#endif
