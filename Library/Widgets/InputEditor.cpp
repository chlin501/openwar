#include "InputEditor.h"
#include "InputWidget.h"
#include "Surface/Surface.h"


InputEditor::InputEditor(InputWidget* inputWidget) :
	_inputWidget(inputWidget)
{
}


InputEditor::~InputEditor()
{
}


InputWidget* InputEditor::GetInputWidget() const
{
	return _inputWidget;
}


void InputEditor::NotifyEnter()
{
	if (_inputWidget->_enterAction)
		_inputWidget->_enterAction();
}


/******************************************************************************/
/* InputEditor_Mac */
/******************************************************************************/


#ifdef ENABLE_INPUT_EDITOR_MAC

InputEditor_Mac::InputEditor_Mac(InputWidget* inputWidget) : InputEditor(inputWidget),
	_textField(nil),
	_delegate(nil),
	_observer(nil)
{
	if (_textField == nil)
	{
		NSView* view = inputWidget->GetWidgetView()->GetSurface()->GetNSView();

		_delegate = [[InputEditorDelegate_Mac alloc] initWithInputEditor:this];


		_textField = [[NSTextField alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
		_textField.stringValue = [NSString stringWithUTF8String:inputWidget->GetString()];
		_textField.delegate = _delegate;
		_textField.wantsLayer = YES;
		_textField.drawsBackground = NO;
		_textField.bordered = NO;

		UpdateNSTextFieldFont();
		UpdateNSTextFieldFrame();
		UpdateNSTextFieldColor();

		_observer = [[[NSNotificationCenter defaultCenter] addObserverForName:NSControlTextDidEndEditingNotification
											  object:_textField
											  queue:[NSOperationQueue mainQueue]
											  usingBlock:^(NSNotification*) {
												  inputWidget->SetEditing(false);
											  }] retain];

		[view addSubview:_textField];

		[_textField becomeFirstResponder];
	}
}


InputEditor_Mac::~InputEditor_Mac()
{
	[[NSNotificationCenter defaultCenter] removeObserver:_observer];
	[_observer release];

	_textField.delegate = nil;
	[_textField removeFromSuperview];
	[_textField release];
	[_delegate release];
}


const char* InputEditor_Mac::GetString() const
{
	return _textField.stringValue.UTF8String;
}


void InputEditor_Mac::SetString(const char* value)
{
	_textField.stringValue = [NSString stringWithUTF8String:value];
}


void InputEditor_Mac::OnInputWidgetChanged()
{
	UpdateNSTextFieldFont();
	UpdateNSTextFieldFrame();
	UpdateNSTextFieldColor();
}


void InputEditor_Mac::UpdateNSTextFieldFont()
{
	FontDescriptor fontDescriptor = GetInputWidget()->GetFontDescriptor();
	if (fontDescriptor.name.empty())
	{
		if (fontDescriptor.bold)
			_textField.font = [NSFont boldSystemFontOfSize:fontDescriptor.size];
		else
			_textField.font = [NSFont systemFontOfSize:fontDescriptor.size];
	}
	else
	{
		_textField.font = [NSFont fontWithName:[NSString stringWithUTF8String:fontDescriptor.name.c_str()] size:fontDescriptor.size];
	}
}


void InputEditor_Mac::UpdateNSTextFieldFrame()
{
	bounds2f bounds = GetInputWidget()->GetBounds();
	float height = (float)_textField.font.pointSize + 6;
	float width = bounds.x().size() + 2;
	_textField.frame = NSMakeRect(bounds.min.x - 1, bounds.min.y - 2, width, height);
}


void InputEditor_Mac::UpdateNSTextFieldColor()
{
	glm::vec4 color = GetInputWidget()->GetColor();
	_textField.textColor = [NSColor colorWithRed:color.r green:color.g blue:color.b alpha:color.a];
}


@implementation InputEditorDelegate_Mac
{
	InputEditor_Mac* _inputEditor;
}


- (id)initWithInputEditor:(InputEditor_Mac*)inputEditor
{
	self = [super init];
	if (self != nil)
	{
		_inputEditor = inputEditor;
	}
	return self;
}


- (BOOL)control:(NSControl *)control textView:(NSTextView *)textView doCommandBySelector:(SEL)commandSelector
{
	if (commandSelector == @selector(insertNewline:)
		|| commandSelector == @selector(insertLineBreak:))
	{
		_inputEditor->NotifyEnter();
		return YES;
	}

	return NO;
}


@end

#endif


/******************************************************************************/
/* InputEditor_iOS */
/******************************************************************************/


#ifdef ENABLE_INPUT_EDITOR_IOS

InputEditor_iOS::InputEditor_iOS(InputWidget* inputWidget) : InputEditor(inputWidget),
	_textField(nil),
	_delegate(nil)
{
	if (_textField == nil)
	{
		UIView* view = inputWidget->GetWidgetView()->GetSurface()->GetUIView();

		_delegate = [[InputEditorDelegate_iOS alloc] initWithInputEditor:this];


		_textField = [[UITextField alloc] initWithFrame:CGRectMake(0, 0, 0, 0)];
		_textField.text = [NSString stringWithUTF8String:inputWidget->GetString()];
		_textField.delegate = _delegate;
		_textField.autocorrectionType = UITextAutocorrectionTypeNo;
		_textField.clearButtonMode = UITextFieldViewModeAlways;
		_textField.returnKeyType = UIReturnKeySend;
		//_textField.borderStyle = UITextBorderStyleLine; // TESTING

		UpdateNSTextFieldFont();
		UpdateNSTextFieldFrame();
		UpdateNSTextFieldColor();

		[view addSubview:_textField];

		[_textField becomeFirstResponder];
	}
}


InputEditor_iOS::~InputEditor_iOS()
{
	_textField.delegate = nil;
	[_textField removeFromSuperview];
	[_textField release];
	[_delegate release];
}


const char* InputEditor_iOS::GetString() const
{
	return _textField.text.UTF8String;
}


void InputEditor_iOS::SetString(const char* value)
{
	_textField.text = [NSString stringWithUTF8String:value];
}


void InputEditor_iOS::OnInputWidgetChanged()
{
	UpdateNSTextFieldFont();
	UpdateNSTextFieldFrame();
	UpdateNSTextFieldColor();
}

void InputEditor_iOS::UpdateNSTextFieldFont()
{
	FontDescriptor fontDescriptor = GetInputWidget()->GetFontDescriptor();
	if (fontDescriptor.name.empty())
	{
		if (fontDescriptor.bold)
			_textField.font = [UIFont boldSystemFontOfSize:fontDescriptor.size];
		else
			_textField.font = [UIFont systemFontOfSize:fontDescriptor.size];
	}
	else
	{
		_textField.font = [UIFont fontWithName:[NSString stringWithUTF8String:fontDescriptor.name.c_str()] size:fontDescriptor.size];
	}
}


void InputEditor_iOS::UpdateNSTextFieldFrame()
{
	InputWidget* intputWidget = GetInputWidget();
	Surface* surface = intputWidget->GetWidgetView()->GetSurface();
	bounds2f bounds = intputWidget->GetBounds();
	float height = (float)_textField.font.pointSize + 2;
	float width = bounds.x().size();
	_textField.frame = CGRectMake(bounds.min.x, surface->GetVirtualSize().y - bounds.min.y - height, width, height);
}


void InputEditor_iOS::UpdateNSTextFieldColor()
{
	glm::vec4 color = GetInputWidget()->GetColor();
	_textField.textColor = [UIColor colorWithRed:color.r green:color.g blue:color.b alpha:color.a];
}


@implementation InputEditorDelegate_iOS
{
	InputEditor_iOS* _inputEditor;
}


- (id)initWithInputEditor:(InputEditor_iOS*)inputEditor
{
	self = [super init];
	if (self != nil)
	{
		_inputEditor = inputEditor;
	}
	return self;
}


- (BOOL)textFieldShouldReturn:(UITextField *)textField
{
	_inputEditor->NotifyEnter();
	return NO;
}


- (void)textFieldDidEndEditing:(UITextField *)textField
{
	InputWidget* inputWidget = _inputEditor->GetInputWidget();

	inputWidget->SetEditing(false);
}


@end

#endif


/******************************************************************************/
/* InputEditor_Android */
/******************************************************************************/


#ifdef ENABLE_INPUT_EDITOR_ANDROID

#include <codecvt>
#include <cstring>
#include <locale>

JNIEnv* InputEditor_Android::_env = 0;
static InputEditor_Android* _instance = nullptr;


/*JNIEXPORT*/ void /*JNICALL*/ Java_org_openwar_InputEditor_notifyEnter(JNIEnv* env, jclass jcls)
{
	if (_instance != nullptr)
		_instance->CallbackNotifyEnter();
}


void InputEditor_Android::Initialize(JNIEnv* env)
{
	_env = env;
}


InputEditor_Android::InputEditor_Android(InputWidget* inputWidget) : InputEditor(inputWidget)
{
	_instance = this;

	UpdateBounds();
	CallShow();
}


InputEditor_Android::~InputEditor_Android()
{
	if (_instance == this)
		_instance = nullptr;

	CallHide();
}


const char* InputEditor_Android::GetString() const
{
	_string = CallGetString();
	return _string.c_str();
}


void InputEditor_Android::SetString(const char* value)
{
	CallSetString(value);
}


void InputEditor_Android::OnInputWidgetChanged()
{
	UpdateBounds();
}


void InputEditor_Android::UpdateBounds()
{
	InputWidget* inputWidget = GetInputWidget();

	bounds2f bounds = inputWidget->GetBounds();
	int x = (int)bounds.min.x;
	int y = (int)bounds.min.y;
	int width = (int)bounds.x().size();
	int height = (int)bounds.y().size();

	CallSetBounds(x, y, width, height);
}


std::string InputEditor_Android::CallGetString()
{
	jclass clazz = _env->FindClass("org/openwar/InputEditor");
	if (clazz == 0)
		return "";

	jmethodID method = _env->GetStaticMethodID(clazz, "getString", "()Ljava/lang/String;");
	if (method == 0)
		return "";

	jstring string = (jstring)_env->CallStaticObjectMethod(clazz, method);

	return ConvertFromJavaString(string);
}


void InputEditor_Android::CallSetString(const char* value)
{
	jclass clazz = _env->FindClass("org/openwar/InputEditor");
	if (clazz == 0)
		return;

	jmethodID method = _env->GetStaticMethodID(clazz, "setString", "(Ljava/lang/String;)V");
	if (method == 0)
		return;

	jstring string = ConvertToJavaString(value);

	_env->CallStaticVoidMethod(clazz, method, string);
}


void InputEditor_Android::CallSetBounds(int x, int y, int width, int height)
{
	jclass clazz = _env->FindClass("org/openwar/InputEditor");
	if (clazz == 0)
		return;

	jmethodID method = _env->GetStaticMethodID(clazz, "setBounds", "(IIII)V");
	if (method == 0)
		return;

	_env->CallStaticVoidMethod(clazz, method, x, y, width, height);
}


void InputEditor_Android::CallShow()
{
	jclass clazz = _env->FindClass("org/openwar/InputEditor");
	if (clazz == 0)
		return;

	jmethodID method = _env->GetStaticMethodID(clazz, "show", "()V");
	if (method == 0)
		return;

	_env->CallStaticVoidMethod(clazz, method);
}


void InputEditor_Android::CallHide()
{
	jclass clazz = _env->FindClass("org/openwar/InputEditor");
	if (clazz == 0)
		return;

	jmethodID method = _env->GetStaticMethodID(clazz, "hide", "()V");
	if (method == 0)
		return;

	_env->CallStaticVoidMethod(clazz, method);
}


void InputEditor_Android::CallbackNotifyEnter()
{
	NotifyEnter();
}


std::string InputEditor_Android::ConvertFromJavaString(jstring value)
{
	const jchar* data = _env->GetStringChars(value, 0);
	jsize size = _env->GetStringLength(value);

	std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> conv(".");
	std::string result = conv.to_bytes((const char16_t*)data, (const char16_t*)data + size);

	_env->ReleaseStringChars(value, data);

	return result;
}


jstring InputEditor_Android::ConvertToJavaString(const char* value)
{
	std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> conv(".");
	std::u16string result = conv.from_bytes(value, value + std::strlen(value));

	return _env->NewString((const jchar*)result.data(), (jsize)result.size());
}



#endif
