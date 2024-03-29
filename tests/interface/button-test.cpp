/* --------------------------------------------------------------------------
 *
 * ETK++ --- The Easy Toolkit for C++ programing
 * Copyright (C) 2004-2006, Anthony Lee, All Rights Reserved
 *
 * ETK++ library is a freeware; it may be used and distributed according to
 * the terms of The MIT License.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 * IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * File: button-test.cpp
 *
 * --------------------------------------------------------------------------*/

#include <app/Application.h>
#include <app/Message.h>
#include <interface/Button.h>
#include <interface/Window.h>
#include <stdio.h>

#define BTN_HELLO_WORLD_EN_MSG 'btn1'
#define BTN_HELLO_WORLD_IT_MSG 'btn2'
#define BTN_NOT_ENABLED_MSG 'btn3'
#define BTN_FOCUS_MSG 'btn4'
#define BTN_DEFAULT_MSG 'btn5'

class TView : public BView
{
   public:
	TView(BRect frame, const char *name, uint32 resizingMode, uint32 flags);
	virtual ~TView();
};

TView::TView(BRect frame, const char *name, uint32 resizingMode, uint32 flags)
	: BView(frame, name, resizingMode, flags)
{
	BFont font;

	BButton *btn = new BButton(BRect(10, 10, 150, 50), "b>hello", "Hello World", new BMessage(BTN_HELLO_WORLD_EN_MSG));
	btn->ForceFontAliasing(true);
	if (font.SetFamilyAndStyle("SimSun", "Regular") == B_OK) btn->SetFont(&font, B_FONT_FAMILY_AND_STYLE);
	btn->SetFontSize(20);
	AddChild(btn);

	btn = new BButton(BRect(10, 100, 50, 120), "b>ciao", "Ciao Mondo", new BMessage(BTN_HELLO_WORLD_IT_MSG));
	btn->ForceFontAliasing(true);
	if (font.SetFamilyAndStyle("SimHei", "Regular") == B_OK) {
		btn->SetFont(&font, B_FONT_FAMILY_AND_STYLE);
		btn->SetFontSize(24);
	}
	AddChild(btn);
	btn->ResizeToPreferred();

	btn = new BButton(BRect(10, 150, 40, 180), "b>disabled", "Disabled", new BMessage(BTN_NOT_ENABLED_MSG));
	btn->SetEnabled(false);
	AddChild(btn);
	btn->ResizeToPreferred();

	btn = new BButton(BRect(10, 300, 140, 340), "b>default", "Default button", new BMessage(BTN_DEFAULT_MSG));
	btn->MakeDefault(true);
	AddChild(btn);
}

TView::~TView()
{
}

class TWindow : public BWindow
{
   public:
	TWindow(BRect		frame,
			const char *title,
			window_type type,
			uint32		flags,
			uint32		workspace = B_CURRENT_WORKSPACE);
	virtual ~TWindow();

	virtual void WindowActivated(bool state);
	virtual bool QuitRequested();

	virtual void MessageReceived(BMessage *msg);

   private:
	bool quited;
};

class TApplication : public BApplication
{
   public:
	TApplication();
	virtual ~TApplication();

	virtual void ReadyToRun();
};

TWindow::TWindow(BRect frame, const char *title, window_type type, uint32 flags, uint32 workspace)
	: BWindow(frame, title, type, flags, workspace), quited(false)
{
	//	SetBackgroundColor(0, 255, 255);

	BButton *btn = new BButton(BRect(10, 200, 40, 230), "b>focus", "Focus Button", new BMessage(BTN_FOCUS_MSG));
	AddChild(btn);
	btn->ResizeToPreferred();
	btn->MakeFocus(true);

	BView *view = new TView(frame.OffsetToCopy(B_ORIGIN), "v>wrapper", B_FOLLOW_ALL, B_FRAME_EVENTS);
	AddChild(view);
}

TWindow::~TWindow()
{
}

void TWindow::MessageReceived(BMessage *msg)
{
	switch (msg->what) {
		case BTN_HELLO_WORLD_EN_MSG:
			dprintf(2, "=== Hello world button is pressed.\n");
			break;

		case BTN_HELLO_WORLD_IT_MSG:
			dprintf(2, "=== Ciao mondo button is pressed.\n");
			break;

		case BTN_NOT_ENABLED_MSG:
			dprintf(2, "=== Not enabled button is pressed, it must be some error!\n");
			break;

		case BTN_FOCUS_MSG:
			dprintf(2, "=== Focus button is pressed.\n");
			SetFlags((Flags() & B_AVOID_FOCUS) ? Flags() & ~B_AVOID_FOCUS : Flags() | B_AVOID_FOCUS);
			break;

		case BTN_DEFAULT_MSG:
			dprintf(2, "=== Default button is pressed.\n");
			break;

		default:
			BWindow::MessageReceived(msg);
	}
}

void TWindow::WindowActivated(bool state)
{
	dprintf(2, "Window activated %s.\n", state ? "true" : "false");
}

bool TWindow::QuitRequested()
{
	if (quited) return true;
	be_app->PostMessage(B_QUIT_REQUESTED);
	quited = true;
	return false;
}

TApplication::TApplication()
	: BApplication("application/x-vnd.lee-test-app")
{
}

TApplication::~TApplication()
{
}

void TApplication::ReadyToRun()
{
	TWindow *win = new TWindow(BRect(100, 100, 500, 500),
							   "Button Test", B_TITLED_WINDOW, B_CLOSE_ON_ESCAPE);
	win->Show();
}

int main(int argc, char **argv)
{
	TApplication app;
	app.Run();

	return 0;
}
