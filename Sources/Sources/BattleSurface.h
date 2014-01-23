#ifndef BattleSurface_H
#define BattleSurface_H

#include "shaderprogram.h"
#include "../Library/Surface/Surface.h"

class BattleCommander;
class BattleGesture;
class BattleModel;
class BattleScenario;
class BattleScript;
class BattleView;
class TerrainGesture;
class SmoothTerrainRenderer;
class SmoothTerrainWater;
class SmoothTerrainSky;


class BattleSurface : public Surface
{
protected:
	renderers* _renderers;

protected:
	bool _playing;
	bool _editing;

	BattleScenario* _scenario;
	std::vector<BattleCommander*> _commanders;

	std::vector<BattleView*> _battleViews;
	std::vector<BattleGesture*> _battleGestures;
	std::vector<TerrainGesture*> _terrainGestures;

public:
	BattleSurface(glm::vec2 size, float pixelDensity);
	~BattleSurface();

	BattleScenario* GetScenario() const { return _scenario; }
	const std::vector<BattleView*>& GetBattleViews() const { return _battleViews; }
	BattleView* GetPrimaryBattleView() const { return _battleViews.empty() ? nullptr : _battleViews.front(); }

	virtual void ResetBattleViews(BattleScenario* scenario, const std::vector<BattleCommander*>& commanders);

	void ResetCameraPosition();

	void SetPlaying(bool value);
	void SetEditing(bool value);
	bool IsPlaying() const { return _playing; }
	bool IsEditing() const { return _editing; }

	// Surface
	virtual void ScreenSizeChanged();
	virtual void Update(double secondsSinceLastUpdate);
	virtual bool NeedsRender() const;
	virtual void Render();

private:
	void CreateBattleView(BattleCommander* commander);
	void ResetBattleView(BattleView* battleView, BattleCommander* commander);
	void RemoveBattleView(BattleView* battleView);

	void UpdateBattleViewSize();

	void UpdateSoundPlayer();
};



#endif
