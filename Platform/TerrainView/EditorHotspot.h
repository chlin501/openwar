#ifndef EditorHotspot_H
#define EditorHotspot_H

#include "Hotspot.h"
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

	virtual Gesture* GetGesture() const;

	BattleView* GetBattleView() const;
	EditorModel* GetEditorModel() const;
};


#endif
