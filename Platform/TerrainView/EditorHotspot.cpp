#include "EditorHotspot.h"
#include "BattleView.h"
#include "EditorModel.h"


EditorHotspot::EditorHotspot(BattleView* battleView, EditorModel* editorModel) :
	_gesture(this),
	_battleView(battleView),
	_editorModel(editorModel)
{
}


Gesture* EditorHotspot::GetGesture() const
{
	return const_cast<EditorGesture*>(&_gesture);
}


EditorHotspot::~EditorHotspot()
{

}


BattleView* EditorHotspot::GetBattleView() const
{
	return _battleView;
}


EditorModel* EditorHotspot::GetEditorModel() const
{
	return _editorModel;
}
