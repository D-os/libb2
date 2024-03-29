#include "CheckBox.h"

#include <Font.h>

#include "DosControlLook.h"

BCheckBox::BCheckBox(BRect frame, const char *name, const char *label, BMessage *message,
					 uint32 resizeMask, uint32 flags)
	: BControl(frame, name, label, message, resizeMask, flags)
{
	float preferredHeight;
	GetPreferredSize(nullptr, &preferredHeight);
	ResizeTo(Bounds().Width(), std::max(preferredHeight, Bounds().Height()));
}

BCheckBox::~BCheckBox() {}

BCheckBox::BCheckBox(BMessage *data) : BControl(data) {}

status_t BCheckBox::Archive(BMessage *data, bool deep) const
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

void BCheckBox::Draw(BRect updateRect)
{
	BControl::Draw(updateRect);

	auto bounds = Bounds();
	auto high_color = HighColor();

	PushState();

	auto pattern = IsEnabled() ? B_SOLID_HIGH : B_MIXED_COLORS;

	BRect box(0, ceilf((bounds.Height() - (CHECKBOX_BOX_SIZE)) / 2),
			  CHECKBOX_BOX_SIZE, ceilf((bounds.Height() + CHECKBOX_BOX_SIZE) / 2));
	StrokeRect(box, pattern);
	BRect inset = box.InsetByCopy(1, 1);
	SetHighColor(tint_color(ViewColor(), B_LIGHTEN_1_TINT));
	FillRect(inset, pattern);
	SetHighColor(tint_color(ViewColor(), B_LIGHTEN_2_TINT));
	MovePenTo(inset.LeftBottom());
	StrokeLine(inset.RightBottom());
	StrokeLine(inset.RightTop());
	SetHighColor(tint_color(ViewColor(), B_NO_TINT));
	StrokeLine(inset.LeftTop());
	StrokeLine(inset.LeftBottom());

	if (Value()) {
		SetHighColor(ui_color(B_NAVIGATION_BASE_COLOR));
		SetPenSize(2);
		StrokeLine(box.LeftTop() + BPoint{3.5, 3.5}, box.LeftTop() + BPoint{CHECKBOX_BOX_SIZE - 3.5, CHECKBOX_BOX_SIZE - 3.5});
		StrokeLine(box.LeftTop() + BPoint{3.5, CHECKBOX_BOX_SIZE - 3.5}, box.LeftTop() + BPoint{CHECKBOX_BOX_SIZE - 3.5, 3.5});
	}

	const auto label = Label();
	if (label) {
		font_height metrics;
		GetFontHeight(&metrics);

		BPoint pos{
			box.right + CHECKBOX_TEXT_PADDING,
			(bounds.Height() + metrics.ascent + PenSize()) / 2};

		SetHighColor(high_color);
		DrawString(label, pos);
	}

	PopState();
}

void BCheckBox::MouseDown(BPoint where)
{
	BControl::MouseDown(where);

	if (!IsEnabled())
		return;

	SetTracking(true);
	SetMouseEventMask(B_POINTER_EVENTS, B_LOCK_WINDOW_FOCUS);
}

void BCheckBox::MouseUp(BPoint where)
{
	BControl::MouseUp(where);

	if (!IsTracking())
		return;

	SetTracking(false);

	if (!IsEnabled() || !Bounds().Contains(where))
		return;

	SetValue(Value() == B_CONTROL_ON ? B_CONTROL_OFF : B_CONTROL_ON);
	Invoke();
}

void BCheckBox::MouseMoved(BPoint pt, uint32 transit, const BMessage *dnd)
{
	BControl::MouseMoved(pt, transit, dnd);
}

void BCheckBox::KeyDown(const char *bytes, int32 numBytes)
{
	if (bytes && numBytes > 0 && (bytes[0] == B_ENTER || bytes[0] == B_SPACE)) {
		SetValue(Value() == B_CONTROL_ON ? B_CONTROL_OFF : B_CONTROL_ON);
		Invoke();
	}
	else
		BControl::KeyDown(bytes, numBytes);
}

void BCheckBox::AttachedToWindow()
{
	BControl::AttachedToWindow();
}

void BCheckBox::MessageReceived(BMessage *msg)
{
	BControl::MessageReceived(msg);
}

void BCheckBox::WindowActivated(bool state)
{
	BControl::WindowActivated(state);
}

void BCheckBox::DetachedFromWindow()
{
	BControl::DetachedFromWindow();
}

void BCheckBox::SetValue(int32 value)
{
	BControl::SetValue(value);
}

void BCheckBox::GetPreferredSize(float *width, float *height)
{
	BFont font;
	GetFont(&font);
	if (width) {
		*width = CHECKBOX_BOX_SIZE + CHECKBOX_TEXT_PADDING + font.StringWidth(Label());
	}
	if (height) {
		font_height fh;
		font.GetHeight(&fh);
		*height = std::max(fh.ascent, CHECKBOX_BOX_SIZE);
	}
}

void BCheckBox::ResizeToPreferred()
{
	BControl::ResizeToPreferred();
}

status_t BCheckBox::Invoke(BMessage *msg)
{
	return BControl::Invoke(msg);
}

void BCheckBox::FrameMoved(BPoint new_position)
{
	BControl::FrameMoved(new_position);
}

void BCheckBox::FrameResized(float new_width, float new_height)
{
	BControl::FrameResized(new_width, new_height);
}

BHandler *BCheckBox::ResolveSpecifier(BMessage *msg, int32 index, BMessage *specifier, int32 form, const char *property)
{
	return BControl::ResolveSpecifier(msg, index, specifier, form, property);
}

status_t BCheckBox::GetSupportedSuites(BMessage *data)
{
	return BControl::GetSupportedSuites(data);
}

void BCheckBox::MakeFocus(bool state)
{
	BControl::MakeFocus(state);
}

void BCheckBox::AllAttached()
{
	BControl::AllAttached();
}

void BCheckBox::AllDetached()
{
	BControl::AllDetached();
}
