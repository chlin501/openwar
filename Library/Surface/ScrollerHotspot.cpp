#include "ScrollerHotspot.h"
#import "Content.h"


ScrollerHotspot::ScrollerHotspot(Content* content) :
	_content(content)
{
}


Content* ScrollerHotspot::GetContent() const
{
	return _content;
}
