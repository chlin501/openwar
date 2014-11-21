#include "InputEditor.h"
#include "InputWidget.h"
#include "Surface.h"


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


void InputEditor::NotifyEnterKey()
{
	if (_inputWidget->_enterAction)
		_inputWidget->_enterAction();
	_inputWidget->SetEditing(false);
}


#ifdef ENABLE_SURFACE_ADAPTER_MAC


InputEditorMac::InputEditorMac(InputWidget* inputWidget) : InputEditor(inputWidget),
	_textField(nil)
{
	if (_textField == nil)
	{
		NSView* view = inputWidget->GetWidgetView()->GetSurface()->GetNSView();

		_delegate = [[InputEditorMacDelegate alloc] initWithInputAdapter:this];


		_textField = [[NSTextField alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
		_textField.stringValue = [NSString stringWithUTF8String:inputWidget->GetString()];
		_textField.delegate = _delegate;
		_textField.wantsLayer = YES;
		_textField.bordered = YES;
		_textField.drawsBackground = NO;

		UpdateNSTextFieldFont();
		UpdateNSTextFieldFrame();
		UpdateNSTextFieldColor();

		[[NSNotificationCenter defaultCenter] addObserverForName:NSControlTextDidEndEditingNotification
											  object:_textField
											  queue:[NSOperationQueue mainQueue]
											  usingBlock:^(NSNotification*) {
												  inputWidget->SetString(_textField.stringValue.UTF8String);
												  inputWidget->SetEditing(false);
											  }];

		[view addSubview:_textField];

		[_textField becomeFirstResponder];
	}
}


InputEditorMac::~InputEditorMac()
{
	_textField.delegate = nil;
	[_textField removeFromSuperview];
	[_textField release];
	[_delegate release];
}


void InputEditorMac::OnInputWidgetChanged()
{
	UpdateNSTextFieldFont();
	UpdateNSTextFieldFrame();
	UpdateNSTextFieldColor();
}

void InputEditorMac::UpdateNSTextFieldFont()
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


void InputEditorMac::UpdateNSTextFieldFrame()
{
	bounds2f bounds = GetInputWidget()->GetBounds();
	float height = (float)_textField.font.pointSize + 8;
	float width = bounds.x().size() + 2;
	_textField.frame = NSMakeRect(bounds.min.x - 2, bounds.min.y - 2, width, height);
}


void InputEditorMac::UpdateNSTextFieldColor()
{
	glm::vec4 color = GetInputWidget()->GetColor();
	_textField.textColor = [NSColor colorWithRed:color.r green:color.g blue:color.b alpha:color.a];
}


@implementation InputEditorMacDelegate
{
	InputEditor* _inputAdapter;
}


- (id)initWithInputAdapter:(InputEditor*)inputAdapter
{
	self = [super init];
	if (self != nil)
	{
		_inputAdapter = inputAdapter;
	}
	return self;
}


- (BOOL)control:(NSControl *)control textView:(NSTextView *)textView doCommandBySelector:(SEL)commandSelector
{
	if (commandSelector == @selector(insertNewline:)
		|| commandSelector == @selector(insertLineBreak:))
	{
		_inputAdapter->NotifyEnterKey();
		return YES;
	}

	return NO;
}


@end


#endif
