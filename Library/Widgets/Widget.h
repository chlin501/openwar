#ifndef Widget_H
#define Widget_H

class WidgetGlyph;
class WidgetView;


class Widget
{
	friend class WidgetView;
	WidgetView* _widgetView;

public:
	Widget();
	virtual ~Widget();

	virtual WidgetGlyph* GetWidgetGlyph() const = 0;
};


#endif
