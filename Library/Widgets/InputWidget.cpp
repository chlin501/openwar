#include "InputWidget.h"
#include "InputEditor.h"
#include "Surface/Surface.h"
#include "Surface/Touch.h"



InputWidget::InputWidget(WidgetOwner* widgetOwner) : StringWidget{widgetOwner}
{
	_inputHotspot.SetDistance([this](glm::vec2 position) {
		return _bounds.distance(GetViewport()->GlobalToLocal(position));
	});
	_inputHotspot.SetTolerance([this](Touch* touch) {
		return ClickHotspot::GetDefaultTolerance(touch, _bounds.size());
	});
	_inputHotspot.SetClickAction([this]() {
		SetEditing(!IsEditing());
	});
	_inputHotspot.SetImmediateClick(true);
}


InputWidget::~InputWidget()
{
	delete _inputEditor;
}


void InputWidget::SetEnterAction(std::function<void()> value)
{
	_enterAction = value;
	SetEditing(false);
}


bool InputWidget::IsEditing() const
{
	return _editing;
}


void InputWidget::SetEditing(bool value)
{
	_editing = value;
}


void InputWidget::SetPosition(glm::vec2 value)
{
	bounds2f bounds = _bounds;
	bounds += value - bounds.min;
	SetBounds(bounds);
}


void InputWidget::SetWidth(float value)
{
	bounds2f bounds = _bounds;
	bounds.max.x = bounds.min.x + value;
	SetBounds(bounds);
}


const char* InputWidget::GetString() const
{
	if (_inputEditor != nullptr)
		return _inputEditor->GetString();
	else
		return StringWidget::GetString();
}


void InputWidget::SetString(const char* value)
{
	if (_inputEditor != nullptr)
		_inputEditor->SetString(value);
	else
		StringWidget::SetString(value);
}


bounds2f InputWidget::GetBounds() const
{
	return _bounds;
}


void InputWidget::SetBounds(const bounds2f& value)
{
	_bounds = value;
	StringWidget::SetPosition(value.min);
	StringWidget::SetWidth(value.x().size());
}


void InputWidget::RenderVertices(std::vector<Vertex_2f_2f_4f_1f>& vertices)
{
	//RenderSolid(GetViewport()->GetTransform(), GetBounds(), glm::vec4(1, 1, 1, 0.7f));
	RefreshContent();

	if (_inputEditor == nullptr)
		StringWidget::RenderVertices(vertices);
}


void InputWidget::OnTouchBegin(Touch* touch)
{
	if (_inputHotspot.IsTouchInside(touch) != IsEditing())
		_inputHotspot.SubscribeTouch(touch);
}


void InputWidget::RefreshContent()
{
	if (_inputEditor != nullptr)
		_inputEditor->OnInputWidgetChanged();
}


void InputWidget::OnRenderLoop(double secondsSinceLastLoop)
{
	if (_editing && _inputEditor == nullptr)
		ShowInputEditor();
	else if (!_editing && _inputEditor != nullptr)
		HideInputEditor();
}


void InputWidget::ShowInputEditor()
{
	if (_inputEditor == nullptr)
	{
#if defined(ENABLE_INPUT_EDITOR_MAC)
		_inputEditor = new InputEditor_Mac(this);
#elif defined(ENABLE_INPUT_EDITOR_IOS)
		_inputEditor = new InputEditor_iOS(this);
#elif defined(ENABLE_INPUT_EDITOR_ANDROID)
		_inputEditor = new InputEditor_Android(this);
#endif
	}
	else
	{
		_inputEditor->OnInputWidgetChanged();
	}
}


void InputWidget::HideInputEditor()
{
	StringWidget::SetString(_inputEditor->GetString());
	delete _inputEditor;
	_inputEditor = nullptr;
}


/*
#include "CommonShaders.h"
#include "VertexShape.h"

void InputWidget::RenderSolid(const glm::mat4& transform, bounds2f bounds, glm::vec4 color) const
{
	VertexShape_2f vertices;
	vertices._mode = GL_TRIANGLE_STRIP;
	vertices.AddVertex(Vertex_2f(bounds.mix_00()));
	vertices.AddVertex(Vertex_2f(bounds.mix_01()));
	vertices.AddVertex(Vertex_2f(bounds.mix_10()));
	vertices.AddVertex(Vertex_2f(bounds.mix_11()));

	GraphicsContext* gc = GetGraphicsContext();
	RenderCall<PlainShader_2f>(gc)
		.SetVertices(&vertices, "position")
		.SetUniform("transform", transform)
		.SetUniform("point_size", 1)
		.SetUniform("color", color)
		.Render();
}
*/
