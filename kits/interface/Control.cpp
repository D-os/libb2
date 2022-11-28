#include "Control.h"

#define LOG_TAG "BControl"

#include <Window.h>
#include <log/log.h>

BControl::BControl(BRect frame, const char *name, const char *label, BMessage *message, uint32 resizeMask, uint32 flags)
	: BView(frame, name, resizeMask, flags | B_NAVIGABLE),
	  BInvoker(message, this),
	  fLabel(label ? strdup(label) : nullptr),
	  fValue{B_CONTROL_OFF},
	  fEnabled{true}
{
}

BControl::~BControl() {}

BControl::BControl(BMessage *data) : BView(data), BInvoker() {}

BArchivable *BControl::Instantiate(BMessage *data)
{
	debugger(__PRETTY_FUNCTION__);
	return nullptr;
}

status_t BControl::Archive(BMessage *data, bool deep) const
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

void BControl::WindowActivated(bool state)
{
	BView::WindowActivated(state);
	if (Window()->CurrentFocus() == this) {
		Invalidate();
	}
}

void BControl::AttachedToWindow()
{
	BView::AttachedToWindow();
	SetLowColor(Parent()->ViewColor());
	SetViewColor(Parent()->ViewColor());

	if (!Target()) {
		SetTarget(Window());
	}
}

void BControl::MessageReceived(BMessage *msg)
{
	BView::MessageReceived(msg);
}

void BControl::MakeFocus(bool state)
{
	BView::MakeFocus(state);
}

void BControl::KeyDown(const char *bytes, int32 numBytes)
{
	BView::KeyDown(bytes, numBytes);
}

void BControl::MouseDown(BPoint pt)
{
	BView::MouseDown(pt);
}

void BControl::MouseUp(BPoint pt)
{
	BView::MouseUp(pt);
}

void BControl::MouseMoved(BPoint pt, uint32 code, const BMessage *msg)
{
	BView::MouseMoved(pt, code, msg);
}

void BControl::DetachedFromWindow()
{
	BView::DetachedFromWindow();
}

void BControl::SetLabel(const char *text)
{
	if (!text && !fLabel) return;
	if (text && fLabel && strcmp(text, fLabel) == 0) return;

	const auto current_label = fLabel;

	fLabel = text ? strdup(text) : nullptr;

	if (current_label) free(const_cast<char *>(current_label));

	Invalidate();
}

const char *BControl::Label() const
{
	return fLabel;
}

void BControl::SetValue(int32 value)
{
	if (value == fValue) return;

	fValue = value;
	Invalidate();
}

int32 BControl::Value() const
{
	return fValue;
}

void BControl::SetEnabled(bool on)
{
	if (fEnabled == on)
		return;

	fEnabled = on;
	if (on)
		SetFlags(Flags() | B_NAVIGABLE);
	else
		SetFlags(Flags() & ~B_NAVIGABLE);

	Invalidate();
}

bool BControl::IsEnabled() const
{
	return fEnabled;
}

void BControl::GetPreferredSize(float *width, float *height)
{
	BView::GetPreferredSize(width, height);
}

void BControl::ResizeToPreferred()
{
	BView::ResizeToPreferred();
}

status_t BControl::Invoke(BMessage *msg)
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

BHandler *BControl::ResolveSpecifier(BMessage *msg, int32 index, BMessage *specifier, int32 form, const char *property)
{
	return BView::ResolveSpecifier(msg, index, specifier, form, property);
}

status_t BControl::GetSupportedSuites(BMessage *data)
{
	debugger(__PRETTY_FUNCTION__);
	return BView::GetSupportedSuites(data);
}

void BControl::AllAttached()
{
	BView::AllAttached();
}

void BControl::AllDetached()
{
	BView::AllDetached();
}
