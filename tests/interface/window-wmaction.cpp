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
 * File: window-wmaction.cpp
 *
 * --------------------------------------------------------------------------*/

#include <app/Application.h>
#include <interface/Window.h>
#include <interface/Button.h>

#define WIN_MINIMIZB_MSG		'tst0'
#define WIN_INACTIVATB_MSG		'tst1'
#define WIN_NEXT_WORKSPACB_MSG		'tst2'
#define WIN_PREV_WORKSPACB_MSG		'tst3'
#define WIN_ALL_WORKSPACB_MSG		'tst4'
#define WIN_BORDERED_LOOK_MSG		'tst5'
#define WIN_NO_BORDER_LOOK_MSG		'tst6'
#define WIN_TITLED_LOOK_MSG		'tst7'
#define WIN_DOCUMENT_LOOK_MSG		'tst8'
#define WIN_MODAL_LOOK_MSG		'tst9'
#define WIN_FLOATING_LOOK_MSG		'tsta'


class TWindow : public BWindow
{
	public:
		TWindow(BRect frame,
		        const char *title,
		        window_type type,
		        uint32 flags,
		        uint32 workspace = B_CURRENT_WORKSPACE);
		TWindow(BRect frame,
		        const char *title,
		        window_look look,
		        window_feel feel,
		        uint32 flags,
		        uint32 workspace = B_CURRENT_WORKSPACE);
		virtual ~TWindow();

		virtual void WindowActivated(bool state);
		virtual void WorkspacesChanged(uint32 old_ws, uint32 new_ws);
		virtual bool QuitRequested();

		virtual void Minimize(bool minimize);

		virtual void MessageReceived(BMessage *msg);
};


class TApplication : public BApplication
{
	public:
		TApplication();
		virtual ~TApplication();

		virtual void ReadyToRun();
};


TWindow::TWindow(BRect frame, const char *title, window_type type, uint32 flags, uint32 workspace)
		: BWindow(frame, title, type, flags, workspace)
{
	SetSizeLimits(10, 800, 10, 600);
}


TWindow::TWindow(BRect frame, const char *title, window_look look, window_feel feel, uint32 flags, uint32 workspace)
		: BWindow(frame, title, look, feel, flags, workspace)
{
}


TWindow::~TWindow()
{
}


void
TWindow::MessageReceived(BMessage *msg)
{
	switch (msg->what) {
		case WIN_MINIMIZB_MSG:
			Minimize(true);
			break;

		case WIN_INACTIVATB_MSG:
			Activate(false);
			break;

		case WIN_PREV_WORKSPACB_MSG:
			SetWorkspaces((Workspaces() == B_ALL_WORKSPACES ? 0 : Workspaces() - 1));
			break;

		case WIN_NEXT_WORKSPACB_MSG:
			SetWorkspaces((Workspaces() == B_ALL_WORKSPACES ? 0 : Workspaces() + 1));
			break;

		case WIN_ALL_WORKSPACB_MSG:
			SetWorkspaces(B_ALL_WORKSPACES);
			break;

		case WIN_BORDERED_LOOK_MSG:
		case WIN_NO_BORDER_LOOK_MSG:
		case WIN_TITLED_LOOK_MSG:
		case WIN_DOCUMENT_LOOK_MSG:
		case WIN_MODAL_LOOK_MSG:
		case WIN_FLOATING_LOOK_MSG:
			SetLook(msg->what == WIN_BORDERED_LOOK_MSG ? B_BORDERED_WINDOW_LOOK : (
			            msg->what == WIN_NO_BORDER_LOOK_MSG ? B_NO_BORDER_WINDOW_LOOK : (
			                msg->what == WIN_TITLED_LOOK_MSG ? B_TITLED_WINDOW_LOOK : (
			                    msg->what == WIN_DOCUMENT_LOOK_MSG ? B_DOCUMENT_WINDOW_LOOK : (
			                        msg->what == WIN_MODAL_LOOK_MSG ? B_MODAL_WINDOW_LOOK :B_FLOATING_WINDOW_LOOK)))));
			break;

		default:
			BWindow::MessageReceived(msg);
	}
}


void
TWindow::WindowActivated(bool state)
{
	ETK_OUTPUT("Window activated %s.\n", state ? "true" : "false");
}


void
TWindow::WorkspacesChanged(uint32 old_ws, uint32 new_ws)
{
	ETK_OUTPUT("Window workspaces changed: old - %u, new - %u.\n", old_ws, new_ws);
}


void
TWindow::Minimize(bool minimize)
{
	BWindow::Minimize(minimize);
	ETK_OUTPUT("Window minimized %s.\n", minimize ? "true" : "false");
}


bool
TWindow::QuitRequested()
{
	app->PostMessage(B_QUIT_REQUESTED);
	return true;
}


TApplication::TApplication()
		: BApplication("application/x-vnd.lee-example-app")
{
}


TApplication::~TApplication()
{
}


void
TApplication::ReadyToRun()
{
	TWindow *win = new TWindow(BRect(20, 20, 400, 500), "Window Example: WM Action", B_TITLED_WINDOW, 0);

	win->Lock();

	BRect btnRect(10, 10, win->Bounds().Width() - 10, 35);
	BButton *btn = new BButton(btnRect, NULL, "Minimize me",
	                           new BMessage(WIN_MINIMIZB_MSG), B_FOLLOW_LEFT_RIGHT);
	win->AddChild(btn);

	btnRect.OffsetBy(0, 30);
	btn = new BButton(btnRect, NULL, "Inactivate me",
	                  new BMessage(WIN_INACTIVATB_MSG), B_FOLLOW_LEFT_RIGHT);
	win->AddChild(btn);

	btnRect.OffsetBy(0, 60);
	btn = new BButton(btnRect, NULL, "Send me to the next workspace",
	                  new BMessage(WIN_NEXT_WORKSPACB_MSG), B_FOLLOW_LEFT_RIGHT);
	win->AddChild(btn);

	btnRect.OffsetBy(0, 30);
	btn = new BButton(btnRect, NULL, "Send me to the previous workspace",
	                  new BMessage(WIN_PREV_WORKSPACB_MSG), B_FOLLOW_LEFT_RIGHT);
	win->AddChild(btn);

	btnRect.OffsetBy(0, 30);
	btn = new BButton(btnRect, NULL, "Let me stay on all workspaces",
	                  new BMessage(WIN_ALL_WORKSPACB_MSG), B_FOLLOW_LEFT_RIGHT);
	win->AddChild(btn);

	btnRect.OffsetBy(0, 60);
	btn = new BButton(btnRect, NULL, "Let my look to be B_BORDERED_WINDOW_LOOK",
	                  new BMessage(WIN_BORDERED_LOOK_MSG), B_FOLLOW_LEFT_RIGHT);
	win->AddChild(btn);

	btnRect.OffsetBy(0, 30);
	btn = new BButton(btnRect, NULL, "Let my look to be B_NO_BORDER_WINDOW_LOOK",
	                  new BMessage(WIN_NO_BORDER_LOOK_MSG), B_FOLLOW_LEFT_RIGHT);
	win->AddChild(btn);

	btnRect.OffsetBy(0, 30);
	btn = new BButton(btnRect, NULL, "Let my look to be B_TITLED_WINDOW_LOOK",
	                  new BMessage(WIN_TITLED_LOOK_MSG), B_FOLLOW_LEFT_RIGHT);
	win->AddChild(btn);

	btnRect.OffsetBy(0, 30);
	btn = new BButton(btnRect, NULL, "Let my look to be B_DOCUMENT_WINDOW_LOOK",
	                  new BMessage(WIN_DOCUMENT_LOOK_MSG), B_FOLLOW_LEFT_RIGHT);
	win->AddChild(btn);

	btnRect.OffsetBy(0, 30);
	btn = new BButton(btnRect, NULL, "Let my look to be B_MODAL_WINDOW_LOOK",
	                  new BMessage(WIN_MODAL_LOOK_MSG), B_FOLLOW_LEFT_RIGHT);
	win->AddChild(btn);

	btnRect.OffsetBy(0, 30);
	btn = new BButton(btnRect, NULL, "Let my look to be B_FLOATING_WINDOW_LOOK",
	                  new BMessage(WIN_FLOATING_LOOK_MSG), B_FOLLOW_LEFT_RIGHT);
	win->AddChild(btn);

	win->Show();

	win->Activate(false);

	win->Unlock();
}


int main(int argc, char **argv)
{
	TApplication app;
	app.Run();

	return 0;
}


#if defined(_WIN32) && !(defined(_MSC_VER) && defined(_DEBUG))
#include <windows.h>
int _stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	return main(__argc, __argv);
}
#endif // defined(_WIN32) && !(defined(_MSC_VER) && defined(_DEBUG))

