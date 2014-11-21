#ifndef InputAdapter_H
#define InputAdapter_H


class InputWidget;


class InputAdapter
{
public:
	virtual ~InputAdapter();
	virtual void OnInputWidgetChanged() = 0;
};


#ifdef ENABLE_SURFACE_ADAPTER_MAC

#include <AppKit/AppKit.h>

@interface InputAdapterMacDelegate : NSObject<NSTextFieldDelegate>
@end

class InputAdapterMac : public InputAdapter
{
	InputWidget* _inputWidget;
	NSTextField* _textField;
	InputAdapterMacDelegate* _delegate;

public:
	InputAdapterMac(InputWidget* inputWidget);
	virtual ~InputAdapterMac();

	virtual void OnInputWidgetChanged();
};


#endif


#endif
