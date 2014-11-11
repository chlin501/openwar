#include "WidgetView.h"
#include "GraphicsContext.h"
#include "WidgetShape.h"
#include "ScrollerViewport.h"
#include "Widget.h"
#include "RenderCall.h"


WidgetView::WidgetView(GraphicsContext* gc) :
	_gc(gc),
	_viewport(nullptr),
	_widgetShape(nullptr),
	_widgets()
{
}


WidgetView::~WidgetView()
{
}


ScrollerViewport* WidgetView::GetViewport() const
{
	if (_viewport == nullptr)
		_viewport = new ScrollerViewport(_gc);

	return _viewport;
}


WidgetShape* WidgetView::GetWidgetShape() const
{
	if (_widgetShape == nullptr)
		_widgetShape = new WidgetShape(_gc->GetWidgetTextureAtlas());

	return _widgetShape;
}


void WidgetView::Render()
{
	_viewport->UseViewport();

	RenderCall<WidgetShader>(_gc)
		.SetVertices(_widgetShape->GetVertices(), "position", "texcoord", "colorize", "alpha")
		.SetUniform("transform", _viewport->GetTransform())
		.SetTexture("texture", _widgetShape->GetTextureAtlas())
		.Render();
}


bool WidgetView::HasChangedSinceLastRender() const
{
	return true;
}


void WidgetView::AddWidget(Widget* widget)
{
	if (widget->_widgetView != nullptr)
		widget->_widgetView->RemoveWidget(widget);

	_widgets.push_back(widget);
	widget->_widgetView = this;

}


void WidgetView::RemoveWidget(Widget* widget)
{
	if (widget->_widgetView == this)
	{
		_widgets.erase(
			std::remove(_widgets.begin(), _widgets.end(), widget),
			_widgets.end());
		widget->_widgetView = nullptr;
	}
}
