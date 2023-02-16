#include "Screen.h"

#include "PrivateScreen.h"

BScreen::BScreen(BWindow *win)
{
	screen = win->_get_private_screen();
}

BScreen::~BScreen()
{
	delete screen;
}

BRect BScreen::Frame()
{
	return screen->Frame();
}
