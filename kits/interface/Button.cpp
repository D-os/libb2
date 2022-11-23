#include "Button.h"

#include <Window.h>

BButton::BButton(BRect frame, const char *name, const char *label, BMessage *message, uint32 resizeMask, uint32 flags)
	: BControl(frame, name, label, message, resizeMask, flags) {}

BButton::~BButton() {}

BButton::BButton(BMessage *data) : BControl(data) {}

BArchivable *BButton::Instantiate(BMessage *data)
{
	debugger(__PRETTY_FUNCTION__);
	return nullptr;
}

status_t BButton::Archive(BMessage *data, bool deep) const
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

void BButton::Draw(BRect updateRect)
{
	debugger(__PRETTY_FUNCTION__);
}

void BButton::MouseDown(BPoint where)
{
	debugger(__PRETTY_FUNCTION__);
}

void BButton::AttachedToWindow()
{
	BControl::AttachedToWindow();
	if (IsDefault()) {
		Window()->SetDefaultButton(this);
	}
}

void BButton::KeyDown(const char *bytes, int32 numBytes)
{
	debugger(__PRETTY_FUNCTION__);
}

void BButton::MakeDefault(bool state)
{
	if (IsDefault() == state) return;

	Window()->SetDefaultButton(state ? this : nullptr);
}

void BButton::SetLabel(const char *text)
{
	debugger(__PRETTY_FUNCTION__);
}

bool BButton::IsDefault() const
{
	if (Window()) {
		return Window()->DefaultButton() == this;
	}

	return false;
}

void BButton::MessageReceived(BMessage *msg)
{
	BControl::MessageReceived(msg);
}

void BButton::WindowActivated(bool state)
{
	debugger(__PRETTY_FUNCTION__);
}

void BButton::MouseUp(BPoint pt)
{
	debugger(__PRETTY_FUNCTION__);
}

void BButton::MouseMoved(BPoint pt, uint32 code, const BMessage *msg)
{
	debugger(__PRETTY_FUNCTION__);
}

void BButton::DetachedFromWindow()
{
	BControl::DetachedFromWindow();
}

void BButton::SetValue(int32 value)
{
	debugger(__PRETTY_FUNCTION__);
}

void BButton::GetPreferredSize(float *width, float *height)
{
	BView::GetPreferredSize(width, height);
}

void BButton::ResizeToPreferred()
{
	BView::ResizeToPreferred();
}

status_t BButton::Invoke(BMessage *msg)
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

void BButton::FrameMoved(BPoint new_position)
{
	debugger(__PRETTY_FUNCTION__);
}

void BButton::FrameResized(float new_width, float new_height)
{
	debugger(__PRETTY_FUNCTION__);
}

void BButton::MakeFocus(bool state)
{
	BControl::MakeFocus(state);
}

void BButton::AllAttached()
{
	BControl::AllAttached();
}

void BButton::AllDetached()
{
	BControl::AllDetached();
}

BHandler *BButton::ResolveSpecifier(BMessage *msg, int32 index, BMessage *specifier, int32 form, const char *property)
{
	return BControl::ResolveSpecifier(msg, index, specifier, form, property);
}

status_t BButton::GetSupportedSuites(BMessage *data)
{
	return BControl::GetSupportedSuites(data);
}
