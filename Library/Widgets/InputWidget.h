#ifndef InputWidget_H
#define InputWidget_H

#include "StringWidget.h"

class InputAdapter;
class InputHotspot;


class InputWidget : public StringWidget
{
	InputAdapter* _inputAdater;
	std::shared_ptr<InputHotspot> _inputHotspot;

public:
	InputWidget(WidgetOwner* widgetOwner);
	virtual ~InputWidget();

	virtual glm::vec2 GetPosition() const;
	virtual void SetPosition(glm::vec2 value);

	virtual const float GetWidth() const;
	virtual void SetWidth(float value);

	virtual bounds2f GetBounds() const;
	virtual void SetBounds(const bounds2f& value);

	virtual void OnTouchBegin(Touch* touch);

	virtual void RenderVertices(std::vector<Vertex_2f_2f_4f_1f>& vertices);

	void ShowInputAdapter();

private:
};


#endif
