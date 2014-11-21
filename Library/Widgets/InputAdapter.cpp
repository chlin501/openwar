#include "InputAdapter.h"
#include "InputWidget.h"
#include "Surface.h"


InputAdapter::~InputAdapter()
{
}



#ifdef ENABLE_SURFACE_ADAPTER_MAC


InputAdapterMac::InputAdapterMac(InputWidget* inputWidget) :
	_inputWidget(inputWidget),
	_textField(nil)
{
	if (_textField == nil)
	{
		NSView* view = inputWidget->GetWidgetView()->GetSurface()->GetNSView();

		_delegate = [[InputAdapterMacDelegate alloc] init];


		NSRect frame = NSMakeRect(20, 20, 100, 140);
		_textField = [[NSTextField alloc] initWithFrame:frame];
		_textField.wantsLayer = YES;
		_textField.bordered = YES;
		_textField.drawsBackground = NO;
		_textField.delegate = _delegate;

		[[NSNotificationCenter defaultCenter] addObserverForName:NSControlTextDidEndEditingNotification
											  object:_textField
											  queue:[NSOperationQueue mainQueue]
											  usingBlock:^(NSNotification*) {
												  NSLog(@"NSControlTextDidEndEditingNotification");
											  }];

		[[NSNotificationCenter defaultCenter] addObserverForName:NSControlTextDidChangeNotification
											  object:_textField
											  queue:[NSOperationQueue mainQueue]
											  usingBlock:^(NSNotification*) {
												  NSLog(@"NSControlTextDidChangeNotification");
											  }];

		[view addSubview:_textField];
	}
}


InputAdapterMac::~InputAdapterMac()
{
	[_delegate release];
}


void InputAdapterMac::OnInputWidgetChanged()
{
}


@implementation InputAdapterMacDelegate


- (BOOL)control:(NSControl *)control textView:(NSTextView *)textView doCommandBySelector:(SEL)commandSelector
{
	if (commandSelector == @selector(insertNewline:)
		|| commandSelector == @selector(insertLineBreak:))
	{
		NSLog(@"InputAdapterMacDelegate control:textView:doCommandBySelector:");
		return YES;
	}

	return NO;
}


@end


#endif
