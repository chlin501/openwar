#ifndef InputEditor_H
#define InputEditor_H

#if defined(OPENWAR_PLATFORM_MAC)
#define OPENWAR_USE_INPUTEDITOR_MAC
#elif defined(OPENWAR_PLATFORM_IOS)
#define OPENWAR_USE_INPUTEDITOR_IOS
#elif defined(OPENWAR_PLATFORM_ANDROID)
#define OPENWAR_USE_INPUTEDITOR_ANDROID
#endif


class InputWidget;


class InputEditor
{
	InputWidget* _inputWidget;

public:
	InputEditor(InputWidget* inputWidget);
	virtual ~InputEditor();

	InputEditor(const InputEditor&) = delete;
	InputEditor& operator=(const InputEditor&) = delete;

	InputWidget* GetInputWidget() const;

	virtual const char* GetString() const = 0;
	virtual void SetString(const char* value) = 0;

	virtual void OnInputWidgetChanged() = 0;

	void NotifyEnter();
};


#ifdef OPENWAR_USE_INPUTEDITOR_MAC

#include <AppKit/AppKit.h>

class InputEditor_Mac;
@class InputEditorDelegate_Mac;
@class InputEditorFormatter;

class InputEditor_Mac : public InputEditor
{
	NSTextField* _textField;
	InputEditorDelegate_Mac* _delegate;
	InputEditorFormatter* _formatter;
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


#ifdef OPENWAR_USE_INPUTEDITOR_IOS

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


#ifdef OPENWAR_USE_INPUTEDITOR_ANDROID

#include <jni.h>
#include <string>

extern "C" void /*JNICALL*/ Java_org_openwar_InputEditor_notifyEnter(JNIEnv* env, jclass jcls);

class InputEditor_Android : public InputEditor
{
	friend /*JNIEXPORT*/ void /*JNICALL*/ ::Java_org_openwar_InputEditor_notifyEnter(JNIEnv*, jclass);

    //static JavaVM* _jvm;
	mutable std::string _string;

public:
	InputEditor_Android(InputWidget* inputWidget);
	virtual ~InputEditor_Android();

	virtual const char* GetString() const;
	virtual void SetString(const char* value);

	virtual void OnInputWidgetChanged();

private:
	void UpdateBounds();

	static std::string CallGetString();
	static void CallSetString(const char* value);
	static void CallSetBounds(int x, int y, int width, int height);
	static void CallShow(int maxLength);
	static void CallHide();

	void CallbackNotifyEnter();

public:
	static std::string ConvertFromJavaString(jstring value);
	static jstring ConvertToJavaString(const char* value);
};

#endif


#endif
