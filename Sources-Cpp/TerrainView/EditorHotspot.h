#ifndef EditorHotspot_H
#define EditorHotspot_H

#include "Surface/Hotspot.h"
#include "EditorGesture.h"

class BattleView;
class EditorModel;


class EditorHotspot : public Hotspot
{
	EditorGesture _gesture;
	BattleView* _battleView;
	EditorModel* _editorModel;

public:
	EditorHotspot(BattleView* battleView, EditorModel* editorModel);
	virtual ~EditorHotspot();

	Gesture* GetGesture() override;

	BattleView* GetBattleView() const;
	EditorModel* GetEditorModel() const;
};


#endif
