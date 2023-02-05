#include "MenuBar.h"

#include <ControlLook.h>
#include <Window.h>

BMenuBar::BMenuBar(BRect frame, const char *title, uint32 resizeMask, menu_layout layout, bool resizeToFit)
	: BMenu(frame, title, resizeMask, B_WILL_DRAW | B_FRAME_EVENTS | B_FULL_UPDATE_ON_RESIZE, layout, resizeToFit),
	  fBorder{B_BORDER_FRAME},
	  fLastBounds{new BRect(Bounds())}
{
	SetLowColor(ui_color(B_MENU_BACKGROUND_COLOR));
	SetViewColor(B_TRANSPARENT_COLOR);
}

BMenuBar::BMenuBar(BMessage *data) : BMenu(data)
{
	fLastBounds = new BRect(Bounds());
}

BMenuBar::~BMenuBar()
{
	delete fLastBounds;
}

status_t BMenuBar::Archive(BMessage *data, bool deep) const
{
	return BMenu::Archive(data, deep);
}

void BMenuBar::SetBorder(menu_bar_border border)
{
	fBorder = border;
}

menu_bar_border BMenuBar::Border() const
{
	return fBorder;
}

void BMenuBar::Draw(BRect updateRect)
{
	BRect	  rect(Bounds());
	rgb_color base	= LowColor();
	uint32	  flags = 0;

	be_control_look->DrawBorder(this, rect, updateRect, base, B_PLAIN_BORDER, flags, BControlLook::B_BOTTOM_BORDER);

	be_control_look->DrawMenuBarBackground(this, rect, updateRect, base, 0, fBorder);

	DrawItems(updateRect);
}

void BMenuBar::AttachedToWindow()
{
	Window()->SetKeyMenuBar(this);

	BMenu::AttachedToWindow();

	*fLastBounds = Bounds();
}

void BMenuBar::DetachedFromWindow()
{
	BMenu::DetachedFromWindow();
}

void BMenuBar::MessageReceived(BMessage *msg)
{
	BMenu::MessageReceived(msg);
}

void BMenuBar::MouseDown(BPoint where)
{
	BView::MouseDown(where);
}

void BMenuBar::WindowActivated(bool state)
{
	BView::WindowActivated(state);
}

void BMenuBar::MouseUp(BPoint where)
{
	BView::MouseUp(where);
}

void BMenuBar::FrameMoved(BPoint new_position)
{
	BMenu::FrameMoved(new_position);
}

void BMenuBar::FrameResized(float new_width, float new_height)
{
	// invalidate right border
	if (new_width != fLastBounds->Width()) {
		BRect rect(min_c(fLastBounds->right, new_width), 0,
				   max_c(fLastBounds->right, new_width), new_height);
		Invalidate(rect);
	}

	// invalidate bottom border
	if (new_height != fLastBounds->Height()) {
		BRect rect(0, min_c(fLastBounds->bottom, new_height) - 1,
				   new_width, max_c(fLastBounds->bottom, new_height));
		Invalidate(rect);
	}

	fLastBounds->Set(0, 0, new_width, new_height);

	BMenu::FrameResized(new_width, new_height);
}

void BMenuBar::Show()
{
	BView::Show();
}

void BMenuBar::Hide()
{
	BView::Hide();
}

BHandler *BMenuBar::ResolveSpecifier(BMessage *msg, int32 index, BMessage *specifier, int32 form, const char *property)
{
	return BMenu::ResolveSpecifier(msg, index, specifier, form, property);
}

status_t BMenuBar::GetSupportedSuites(BMessage *data)
{
	return BMenu::GetSupportedSuites(data);
}

void BMenuBar::ResizeToPreferred()
{
	BMenu::ResizeToPreferred();
}

void BMenuBar::GetPreferredSize(float *width, float *height)
{
	BMenu::GetPreferredSize(width, height);
}

void BMenuBar::MakeFocus(bool state)
{
	BMenu::MakeFocus(state);
}

void BMenuBar::AllAttached()
{
	BMenu::AllAttached();
}

void BMenuBar::AllDetached()
{
	BMenu::AllDetached();
}
