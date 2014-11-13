#include "WidgetView.h"
#include "GraphicsContext.h"
#include "WidgetShape.h"
#include "ScrollerViewport.h"
#include "RenderCall.h"


WidgetView::WidgetView(GraphicsContext* gc) :
	_gc(gc),
	_viewport(nullptr),
	_widgetShape(nullptr)
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
