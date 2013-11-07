#ifndef BattleSurface_H
#define BattleSurface_H

#include "../Library/Graphics/renderer.h"
#include "../Library/ViewCore/Surface.h"

class BattleGesture;
class BattleModel;
class BattleScenario;
class BattleScript;
class BattleView;
class TerrainGesture;


class BattleSurface : public Surface
{
protected:
	renderers* _renderers;

private:
	bool _playing;
	bool _editing;
	BattleScenario* _scenario;
	BattleView* _battleView;
	BattleGesture* _battleGesture;
	TerrainGesture* _terrainGesture;

public:
	BattleSurface(glm::vec2 size, float pixelDensity);
	~BattleSurface();

	BattleScenario* GetScenario() const { return _scenario; }
	BattleView* GetBattleView() const { return _battleView; }

	virtual void Reset(BattleScenario* scenario);

	void SetPlaying(bool value);
	void SetEditing(bool value);
	bool IsPlaying() const { return _playing; }
	bool IsEditing() const { return _editing; }

	// Surface
	virtual void ScreenSizeChanged();
	virtual void Update(double secondsSinceLastUpdate);
	virtual bool NeedsRender() const;
	virtual void Render();

protected:
	void UpdateSoundPlayer();
};



#endif
