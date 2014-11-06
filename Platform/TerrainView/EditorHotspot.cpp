#include "EditorHotspot.h"
#include "BattleView.h"
#include "EditorModel.h"


EditorHotspot::EditorHotspot(BattleView* battleView, EditorModel* editorModel) :
	_battleView(battleView),
	_editorModel(editorModel)
{
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
