#ifndef ButtonWidget_H
#define ButtonWidget_H

#include "Widget.h"
#include "ImageWidget.h"
#include "StringWidget.h"

class ButtonHotspot;


class ButtonWidget : public WidgetGroup
{
private:
	bounds2f _bounds;
	std::shared_ptr<ButtonHotspot> _hotspot;
	BorderInset _inset;
	std::shared_ptr<TextureImage> _backgroundNormal;
	std::shared_ptr<TextureImage> _backgroundHighlight;
	std::shared_ptr<TextureImage> _backgroundDisabled;
	ImageWidget _borderImage;
	ImageWidget _iconImage;
	StringWidget _titleString;
	bool _disabled;

public:
	ButtonWidget(WidgetOwner* widgetOwner);

	virtual bounds2f GetBounds() const;
	virtual void SetBounds(const bounds2f& value);

	virtual std::function<void()> GetClickAction() const;
	virtual void SetClickAction(std::function<void()> value);

	virtual void SetBackgroundNormal(std::shared_ptr<TextureImage> value);
	virtual void SetBackgroundHighlight(std::shared_ptr<TextureImage> value);
	virtual void SetBackgroundDisabled(std::shared_ptr<TextureImage> value);

	virtual BorderInset GetInset() const;
	virtual void SetInset(const BorderInset& value);

	virtual const FontDescriptor& GetFontDescriptor() const;
	virtual void SetFontDescriptor(const FontDescriptor& fontDescriptor);

	virtual const char* GetTitle() const;
	virtual void SetTitle(const char* value);

	virtual std::shared_ptr<TextureImage> GetIcon() const;
	virtual void SetIcon(std::shared_ptr<TextureImage> value);

	virtual bool IsDisabled() const;
	virtual void SetDisabled(bool value);

	virtual void OnTouchEnter(Touch* touch);
	virtual void OnTouchBegin(Touch* touch);

	virtual void RenderVertices(std::vector<Vertex_2f_2f_4f_1f>& vertices);

private:
	void RefreshContent();
};


#endif
