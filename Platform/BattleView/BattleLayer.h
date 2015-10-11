#ifndef BattleSurface_H
#define BattleSurface_H

#include "Graphics/ShaderProgram.h"
#include "Graphics/CommonShaders.h"
#include "Surface/Animation.h"
#include "TerrainView/EditorModel.h"
#include "BattleModel/BattleObjects.h"

class BattleGesture;
class BattleModel;
class BattleSimulator;
class BattleScript;
class BattleView;
class EditorHotspot;
class EditorGesture;
class TerrainGesture;
class SmoothTerrainRenderer;
class SmoothTerrainWater;
class SmoothTerrainSky;
class Surface;


class BattleLayer : AnimationHost, EditorModelObserver
{
	Surface* _surface{};
	GraphicsContext* _gc{};

	bool _playing{};
	bool _editing{};

	BattleScenario* _battleScenario{};
	BattleSimulator* _battleSimulator{};
	std::vector<BattleCommander*> _commanders{};

	std::vector<BattleView*> _battleViews{};

	EditorModel* _editorModel{};
	std::shared_ptr<EditorHotspot> _editorHotspot{};

public:
	BattleLayer(Surface* surface);
	~BattleLayer();

	BattleSimulator* GetBattleSimulator() const { return _battleSimulator; }
	const std::vector<BattleView*>& GetBattleViews() const { return _battleViews; }
	BattleView* GetPrimaryBattleView() const { return _battleViews.empty() ? nullptr : _battleViews.front(); }
	EditorModel* GetEditorModel() const { return _editorModel; }

	void ResetBattleViews(BattleScenario* scenario, const std::vector<BattleCommander*>& commanders);
	void ResetEditor(BattleScenario* scenario, const std::vector<BattleCommander*>& commanders);

	void ResetCameraPosition();

	void SetPlaying(bool value);
	void SetEditing(bool value);
	bool IsPlaying() const { return _playing; }
	bool IsEditing() const { return _editing; }

	void Render();

private: // RenderLoopObserver
	virtual void Animate(double secondsSinceLastUpdate);

public: // EditorModelObserver
	virtual void OnEditorModeChanged(EditorModel* editorModel);
	virtual void OnTerrainFeatureChanged(EditorModel* editorModel);

private:
	void CreateBattleView(BattleCommander* commander);
	void ResetBattleView(BattleView* battleView, BattleCommander* commander);
	void RemoveBattleView(BattleView* battleView);

	void UpdateBattleViewSize();
};



#endif
