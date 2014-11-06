#ifndef EditorHotspot_H
#define EditorHotspot_H

#include "Hotspot.h"
#include "EditorGesture.h"


class EditorHotspot : public Hotspot<EditorHotspot, EditorGesture>
{
	BattleView* _battleView;
	EditorModel* _editorModel;

public:
	EditorHotspot(BattleView* battleView, EditorModel* editorModel);
	virtual ~EditorHotspot();

	BattleView* GetBattleView() const;
	EditorModel* GetEditorModel() const;
};


#endif
