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

	virtual const char* GetString() const = 0;
	virtual void SetString(const char* value) = 0;

	virtual void OnInputWidgetChanged() = 0;

	void NotifyEnter();
};


#ifdef ENABLE_SURFACE_ADAPTER_MAC

#include <AppKit/AppKit.h>
class InputEditor_Mac;

@interface InputEditorDelegate_Mac : NSObject<NSTextFieldDelegate>

- (id)initWithInputEditor:(InputEditor_Mac*)inputEditor;

@end

class InputEditor_Mac : public InputEditor
{
	NSTextField* _textField;
	InputEditorDelegate_Mac* _delegate;
	id <NSObject> _observer;

public:
	InputEditor_Mac(InputWidget* inputWidget);
	virtual ~InputEditor_Mac();

	virtual const char* GetString() const;
	virtual void SetString(const char* value);

	virtual void OnInputWidgetChanged();

private:
	void UpdateNSTextFieldFont();
	void UpdateNSTextFieldFrame();
	void UpdateNSTextFieldColor();
};

#endif


#ifdef ENABLE_SURFACE_ADAPTER_IOS

#include <UIKit/UIKit.h>
class InputEditor_iOS;

@interface InputEditorDelegate_iOS : NSObject<UITextFieldDelegate>

- (id)initWithInputEditor:(InputEditor_iOS*)inputEditor;

@end

class InputEditor_iOS : public InputEditor
{
	UITextField* _textField;
	InputEditorDelegate_iOS* _delegate;

public:
	InputEditor_iOS(InputWidget* inputWidget);
	virtual ~InputEditor_iOS();

	virtual const char* GetString() const;
	virtual void SetString(const char* value);

	virtual void OnInputWidgetChanged();

private:
	void UpdateNSTextFieldFont();
	void UpdateNSTextFieldFrame();
	void UpdateNSTextFieldColor();
};

#endif


#endif
