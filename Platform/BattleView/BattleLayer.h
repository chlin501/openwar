#ifndef BattleSurface_H
#define BattleSurface_H

#include "Graphics/ShaderProgram.h"
#include "Graphics/CommonShaders.h"
#include "Surface/Animation.h"
#include "TerrainView/EditorModel.h"
#include "BattleObjects.h"

class BattleGesture;
class BattleModel;
class BattleSimulator_v1_0_0;
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

	BattleSimulator_v1_0_0* _scenario{};
	std::vector<BattleObjects::Commander*> _commanders{};

	std::vector<BattleView*> _battleViews{};

	EditorModel* _editorModel{};
	std::shared_ptr<EditorHotspot> _editorHotspot{};

public:
	BattleLayer(Surface* surface);
	~BattleLayer();

	BattleSimulator_v1_0_0* GetSimulator() const { return _scenario; }
	const std::vector<BattleView*>& GetBattleViews() const { return _battleViews; }
	BattleView* GetPrimaryBattleView() const { return _battleViews.empty() ? nullptr : _battleViews.front(); }
	EditorModel* GetEditorModel() const { return _editorModel; }

	void ResetBattleViews(BattleSimulator_v1_0_0* scenario, const std::vector<BattleObjects::Commander*>& commanders);
	void ResetEditor(BattleSimulator_v1_0_0* scenario, const std::vector<BattleObjects::Commander*>& commanders);

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
	void CreateBattleView(BattleObjects::Commander* commander);
	void ResetBattleView(BattleView* battleView, BattleObjects::Commander* commander);
	void RemoveBattleView(BattleView* battleView);

	void UpdateBattleViewSize();
};



#endif
