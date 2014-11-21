#ifndef InputEditor_H
#define InputEditor_H


class InputWidget;


class InputEditor
{
	InputWidget* _inputWidget;

public:
	InputEditor(InputWidget* inputWidget);
	virtual ~InputEditor();

	InputWidget* GetInputWidget() const;

	virtual void OnInputWidgetChanged() = 0;

	void NotifyEnterKey();
};


#ifdef ENABLE_SURFACE_ADAPTER_MAC

#include <AppKit/AppKit.h>

@interface InputEditorMacDelegate : NSObject<NSTextFieldDelegate>

- (id)initWithInputAdapter:(InputEditor*)inputAdapter;

@end

class InputEditorMac : public InputEditor
{
	NSTextField* _textField;
	InputEditorMacDelegate* _delegate;

public:
	InputEditorMac(InputWidget* inputWidget);
	virtual ~InputEditorMac();

	virtual void OnInputWidgetChanged();

private:
	void UpdateNSTextFieldFont();
	void UpdateNSTextFieldFrame();
	void UpdateNSTextFieldColor();
};


#endif


#endif
