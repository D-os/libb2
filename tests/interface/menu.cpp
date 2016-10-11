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
 * File: menu.cpp
 *
 * --------------------------------------------------------------------------*/

#include <app/Application.h>
#include <interface/Window.h>
#include <interface/PopUpMenu.h>
#include <interface/MenuBar.h>

#define MENU_TEST_OPEN_MSG	'mtop'
#define MENU_TEST_QUIT_MSG	'mtqi'
#define MENU_TEST_MARK_MSG	'mtma'
#define MENU_TEST_SUBM_MSG	'mtsu'


class TView : public BView
{
	public:
		TView(BRect frame, const char *name, uint32 resizingMode, uint32 flags);
		virtual ~TView();

		virtual void	AllAttached();
		virtual void	MouseDown(BPoint where);

	private:
		BMenu *fMenu;
		BPopUpMenu *fPopUp;
};


class TWindow : public BWindow
{
	public:
		TWindow(BRect frame, const char *title, window_type type, uint32 flags, uint32 workspace = B_CURRENT_WORKSPACE);
		virtual ~TWindow();

		virtual bool QuitRequested();
		virtual void MessageReceived(BMessage *msg);
};


class TApplication : public BApplication
{
	public:
		TApplication();
		virtual ~TApplication();

		virtual void ReadyToRun();
};


inline void setMenuTarget(BMenu *menu, BMessenger msgr)
{
	if (!menu) return;

	BMenuItem *item;
	for (int32 i = 0; (item = menu->ItemAt(i)) != NULL; i++) {
		if (item->Submenu() != NULL) setMenuTarget(item->Submenu(), msgr);
		else item->SetTarget(msgr);
	}
}


TView::TView(BRect frame, const char *name, uint32 resizingMode, uint32 flags)
		: BView(frame, name, resizingMode, flags), fMenu(NULL), fPopUp(NULL)
{
	fMenu = new BMenu("Test Menu", B_ITEMS_IN_COLUMN);
//	fMenu->SetEventMask(B_KEYBOARD_EVENTS);

	fMenu->AddItem(new BMenuItem("Open...", new BMessage(MENU_TEST_OPEN_MSG)));
	fMenu->AddSeparatorItem();
	fMenu->AddSeparatorItem();
	fMenu->AddItem(new BMenuItem("Quit", new BMessage(MENU_TEST_QUIT_MSG), 'q', B_COMMAND_KEY));
	fMenu->AddItem(new BMenuItem("Just A Test", NULL), 2);

	BMenuItem *item = new BMenuItem("Disabled menuitem", NULL);
	item->SetEnabled(false);
	fMenu->AddItem(item, 3);

	item = new BMenuItem("标志菜单", new BMessage(MENU_TEST_MARK_MSG));
	item->SetMarked(true);
	fMenu->AddItem(item, 3);

	item = new BMenuItem("Shortcut Test", NULL, B_F12_KEY, B_FUNCTIONS_KEY|B_CONTROL_KEY);
	fMenu->AddItem(item, 3);

	fMenu->AddItem(new BMenuItem("Shortcut Test", NULL, 'e', B_SHIFT_KEY|B_CONTROL_KEY));
	fMenu->AddSeparatorItem();
	fMenu->AddItem(new BMenuItem("Shortcut Test", NULL, B_HOME, B_SHIFT_KEY|B_CONTROL_KEY|B_COMMAND_KEY));

	BMenu *submenu1 = new BMenu("Sub Menu 1", B_ITEMS_IN_COLUMN);
	submenu1->AddItem(new BMenuItem("Test 1", NULL));
	submenu1->AddSeparatorItem();
	submenu1->AddItem(new BMenuItem("Test 2", NULL));

	BMenu *submenu2 = new BMenu("Sub Menu 2", B_ITEMS_IN_COLUMN);
	submenu2->AddItem(new BMenuItem("Test Message", new BMessage(MENU_TEST_SUBM_MSG)));
	submenu2->AddSeparatorItem();
	submenu2->AddItem(new BMenuItem("Test", NULL));

	submenu1->AddItem(submenu2);

	fMenu->AddItem(submenu1, 3);

	fMenu->ResizeToPreferred();
	fMenu->MoveTo(50, 50);

	AddChild(fMenu);

	fPopUp = new BPopUpMenu("Test PopUp Menu");

	fPopUp->AddItem(new BMenuItem("Open...", new BMessage(MENU_TEST_OPEN_MSG)));
	fPopUp->AddSeparatorItem();
	fPopUp->AddSeparatorItem();
	fPopUp->AddItem(new BMenuItem("Quit", new BMessage(MENU_TEST_QUIT_MSG), 'q', B_COMMAND_KEY));
	fPopUp->AddItem(new BMenuItem("Just A Test", NULL), 2);

	BMenu *submenu3 = new BMenu("Sub Menu 1", B_ITEMS_IN_COLUMN);
	submenu3->AddItem(new BMenuItem("Test 1", NULL));
	submenu3->AddSeparatorItem();
	submenu3->AddItem(new BMenuItem("Test 2", NULL));
	fPopUp->AddItem(submenu3, 3);

	BMenu *submenu4 = new BMenu("Sub Menu 2", B_ITEMS_IN_COLUMN);
	submenu4->AddItem(new BMenuItem("Test Message", new BMessage(MENU_TEST_SUBM_MSG)));
	submenu4->AddSeparatorItem();
	submenu4->AddItem(new BMenuItem("Test", NULL));
	fPopUp->AddItem(submenu4, 3);
}


void
TView::AllAttached()
{
	setMenuTarget(fMenu, BMessenger(Window()));
	setMenuTarget(fPopUp, BMessenger(Window()));
}


TView::~TView()
{
	if (fPopUp) delete fPopUp;
}


void
TView::MouseDown(BPoint where)
{
	if (fPopUp && (!fMenu || fMenu->Frame().Contains(where) == false)) {
		ConvertToScreen(&where);
		BMenuItem *item = fPopUp->Go(where, false, false, false, true);
		if (item) {
			BMessage *msg = item->Message();
			if (msg) msg->PrintToStream();
			item->Invoke();
		} else {
			ETK_OUTPUT("None selected.\n");
		}
	}
}


TWindow::TWindow(BRect frame, const char *title, window_type type, uint32 flags, uint32 workspace)
		: BWindow(frame, title, type, flags, workspace)
{
	BRect rect = frame;
	rect.bottom = rect.top + 20;
	rect.OffsetTo(B_ORIGIN);

	BMenuBar *menubar = new BMenuBar(rect, "Test Menu Bar");

	BMenu *menu = new BMenu("File", B_ITEMS_IN_COLUMN);
	menu->AddItem(new BMenuItem("Open...", new BMessage(MENU_TEST_OPEN_MSG)));
	menu->AddSeparatorItem();
	menu->AddSeparatorItem();
	menu->AddItem(new BMenuItem("Quit", new BMessage(MENU_TEST_QUIT_MSG), 'q', B_COMMAND_KEY));

	BMenuItem *item = new BMenuItem("Disabled menuitem", NULL);
	item->SetEnabled(false);
	menu->AddItem(item, 2);

	item = new BMenuItem("标志菜单", new BMessage(MENU_TEST_MARK_MSG));
	item->SetMarked(true);
	menu->AddItem(item, 2);

	menubar->AddItem(menu);

	menu = new BMenu("Edit", B_ITEMS_IN_COLUMN);
	menu->AddItem(new BMenuItem("Copy", NULL));
	menu->AddItem(new BMenuItem("Cut", NULL));
	menu->AddItem(new BMenuItem("Paste", NULL));
	menubar->AddItem(menu);

	menubar->AddItem(new BMenuItem("Help", NULL));

	BMenu *submenu1 = new BMenu("Test", B_ITEMS_IN_COLUMN);
	submenu1->AddItem(new BMenuItem("Test 1", NULL));
	submenu1->AddSeparatorItem();
	submenu1->AddItem(new BMenuItem("Test 2", NULL));

	BMenu *submenu2 = new BMenu("Sub Menu 2", B_ITEMS_IN_COLUMN);
	submenu2->AddItem(new BMenuItem("Test Message", new BMessage(MENU_TEST_SUBM_MSG)));
	submenu2->AddSeparatorItem();
	submenu2->AddItem(new BMenuItem("Test", NULL));

	submenu1->AddItem(submenu2);

	menubar->AddItem(submenu1, 2);

	AddChild(menubar);
	setMenuTarget(menubar, BMessenger(this));

	rect = frame;
	rect.OffsetTo(B_ORIGIN);
	rect.top += 20;
	BView *menu_view = new TView(rect, NULL, B_FOLLOW_ALL, 0);
//	menu_view->SetViewColor(233, 233, 200);
	AddChild(menu_view);
}


TWindow::~TWindow()
{
}


void
TWindow::MessageReceived(BMessage *msg)
{
	switch (msg->what) {
		case MENU_TEST_OPEN_MSG:
			ETK_OUTPUT("Open menu selected.\n");
			break;

		case MENU_TEST_QUIT_MSG:
			ETK_OUTPUT("Quit menu selected.\n");
			app->PostMessage(B_QUIT_REQUESTED);
			break;

		case MENU_TEST_SUBM_MSG:
			ETK_OUTPUT("Sub menu selected.\n");
			break;

		case MENU_TEST_MARK_MSG: {
			BMenuItem *item = NULL;
			if (msg->FindPointer("source", (void**)&item) == false || item == NULL) break;
			item->SetMarked(item->IsMarked() ? false : true);
		}
		break;

		default:
			BWindow::MessageReceived(msg);
	}
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
	TWindow *win = new TWindow(BRect(100, 100, 500, 500), "Menu Example", B_TITLED_WINDOW, 0);
	win->Show();
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

