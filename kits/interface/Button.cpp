#include "Button.h"

#define LOG_TAG "BButton"

#include <Font.h>
#include <Window.h>
#include <log/log.h>

BButton::BButton(BRect frame, const char *name, const char *label, BMessage *message, uint32 resizeMask, uint32 flags)
	: BControl(frame, name, label, message, resizeMask, flags),
	  fDrawAsDefault{false} {}

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

/// Draws the button and labels it.
/// If the BButton's value is anything but 0, the button is highlighted.
/// If it's disabled, it drawn in muted shades of gray.
/// Otherwise, it's drawn in its ordinary, enabled, unhighlighted state.
void BButton::Draw(BRect updateRect)
{
	BView::Draw(updateRect);

	const auto label = Label();
	if (label) {
		BFont font;
		GetFont(&font);
		font_height metrics;
		font.GetHeight(&metrics);

		auto bounds = Bounds();

		PushState();
		auto currentHighColor = HighColor();
		bool negate			  = false;

		if (!IsEnabled()) {
			FillRect(bounds, B_MIXED_COLORS);
		}
		else if (Value()) {
			FillRect(bounds, B_SOLID_HIGH);
			negate = true;
		}

		float X = bounds.right - font.StringWidth(label);
		X -= (X - bounds.left) / 2;
		float Y = bounds.bottom - metrics.descent;
		if (bounds.Height() > metrics.leading)
			Y -= (bounds.Height() - metrics.leading) / 2;
		BPoint pos{X, Y};

		if (negate) SetHighColor(LowColor());  // negate color
		DrawString(label, pos);
		SetHighColor(currentHighColor);

		SetPenSize(IsFocus() ? 2 : (IsDefault() ? 3 : 1));
		StrokeRect(bounds);

		PopState();
	}
}

void BButton::MouseDown(BPoint where)
{
	BControl::MouseDown(where);

	if (!IsEnabled())
		return;

	SetValue(B_CONTROL_ON);

	SetTracking(true);
	SetMouseEventMask(B_POINTER_EVENTS, B_LOCK_WINDOW_FOCUS);
}

void BButton::MouseUp(BPoint where)
{
	BControl::MouseUp(where);

	if (!IsTracking())
		return;

	SetTracking(false);

	if (Value() != B_CONTROL_OFF) {
		SetValue(B_CONTROL_OFF);

		if (IsEnabled()) Invoke();
	}
}

void BButton::MouseMoved(BPoint pt, uint32 transit, const BMessage *dnd)
{
	BControl::MouseMoved(pt, transit, dnd);

	if (!IsTracking())
		return;

	if (transit == B_EXITED_VIEW)
		SetValue(B_CONTROL_OFF);

	if (transit == B_ENTERED_VIEW)
		SetValue(B_CONTROL_ON);
}

void BButton::KeyDown(const char *bytes, int32 numBytes)
{
	if (bytes && numBytes > 0 && (bytes[0] == B_ENTER || bytes[0] == B_SPACE)) {
		SetValue(B_CONTROL_ON);

		// FIXME: Window()->UpdateIfNeeded();	 // make sure the user saw that
		Invoke();
	}
	else
		BControl::KeyDown(bytes, numBytes);
}

void BButton::MakeDefault(bool state)
{
	fDrawAsDefault = state;
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
	else {
		return fDrawAsDefault;
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

void BButton::AttachedToWindow()
{
	BControl::AttachedToWindow();

	if (fDrawAsDefault) {
		Window()->SetDefaultButton(this);
	}

	// A button is automatically resized to its preferred height (but not to its preferred width)
	float preferredHeight;
	GetPreferredSize(nullptr, &preferredHeight);
	ResizeTo(Bounds().Width(), std::max(preferredHeight, Bounds().Height()));
}

void BButton::DetachedFromWindow()
{
	BControl::DetachedFromWindow();
}

void BButton::SetValue(int32 value)
{
	BControl::SetValue(value);
}

void BButton::GetPreferredSize(float *width, float *height)
{
	BFont font;
	GetFont(&font);
	if (width) {
		*width = font.StringWidth(Label());
	}
	if (height) {
		font_height fh;
		font.GetHeight(&fh);
		*height = fh.leading;
	}
}

void BButton::ResizeToPreferred()
{
	BView::ResizeToPreferred();
}

status_t BButton::Invoke(BMessage *msg)
{
	status_t err = BControl::Invoke(msg);
	SetValue(B_CONTROL_OFF);
	return err;
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
