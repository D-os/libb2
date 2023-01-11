#include "RadioButton.h"

#include "./theme.h"
#include "InterfaceDefs.h"

BRadioButton::BRadioButton(BRect frame, const char *name, const char *label, BMessage *message, uint32 resizeMask, uint32 flags)
	: BControl(frame, name, label, message, resizeMask, flags) {}

BRadioButton::BRadioButton(BMessage *data) : BControl(data) {}

BRadioButton::~BRadioButton() {}

status_t BRadioButton::Archive(BMessage *data, bool deep) const
{
	return BControl::Archive(data, deep);
}

void BRadioButton::AttachedToWindow()
{
	BControl::AttachedToWindow();

	// When the object is attached to a window, the height of the rectangle will be adjusted
	// so that there is exactly the right amount of room to accommodate the label.
	float preferredHeight;
	GetPreferredSize(nullptr, &preferredHeight);
	ResizeTo(Bounds().Width(), preferredHeight);
}

void BRadioButton::Draw(BRect updateRect)
{
	BControl::Draw(updateRect);

	auto bounds = Bounds();
	auto high_color = HighColor();

	PushState();

	auto pattern = IsEnabled() ? B_SOLID_HIGH : B_MIXED_COLORS;

	BRect box(0, ceilf((bounds.Height() - (CHECKBOX_BOX_SIZE)) / 2),
			  CHECKBOX_BOX_SIZE, ceilf((bounds.Height() + CHECKBOX_BOX_SIZE) / 2));
	box.PrintToStream();
	StrokeEllipse(box, pattern);
	BRect inset = box.InsetByCopy(1, 1);
	SetHighColor(tint_color(ViewColor(), B_LIGHTEN_1_TINT));
	FillEllipse(inset, pattern);
	SetHighColor(tint_color(ViewColor(), B_LIGHTEN_2_TINT));
	StrokeArc(inset, -135, 180, pattern);
	SetHighColor(tint_color(ViewColor(), B_NO_TINT));
	StrokeArc(inset, 45, 180, pattern);

	if (Value()) {
		SetHighColor(ui_color(B_NAVIGATION_BASE_COLOR));
		FillEllipse(box.InsetByCopy(3, 3), pattern);
		auto pen = PenSize();
		SetHighColor(ui_color(B_SHINE_COLOR));
		SetPenSize(0.5);
		StrokeArc(box.InsetByCopy(4, 4), 90, 90);
		SetHighColor(ui_color(B_SHADOW_COLOR));
		StrokeArc(box.InsetByCopy(3, 3), 270, 90);
		SetPenSize(pen);
	}

	const auto label = Label();
	if (label) {
		font_height metrics;
		GetFontHeight(&metrics);

		BPoint pos{
			box.right + CHECKBOX_TEXT_PADDING,
			roundf((bounds.Height() + metrics.ascent + PenSize()) / 2)};

		SetHighColor(high_color);
		DrawString(label, pos);
	}

	PopState();
}

void BRadioButton::MouseDown(BPoint where)
{
	BControl::MouseDown(where);

	if (!IsEnabled())
		return;

	SetTracking(true);
	SetMouseEventMask(B_POINTER_EVENTS, B_LOCK_WINDOW_FOCUS);
}

void BRadioButton::MouseUp(BPoint where)
{
	BControl::MouseUp(where);

	if (!IsTracking())
		return;

	SetTracking(false);

	if (!IsEnabled() || !Bounds().Contains(where))
		return;

	SetValue(B_CONTROL_ON);
	Invoke();
}

void BRadioButton::MouseMoved(BPoint pt, uint32 transit, const BMessage *dnd)
{
	BControl::MouseMoved(pt, transit, dnd);
}

void BRadioButton::KeyDown(const char *bytes, int32 numBytes)
{
	if (bytes && numBytes > 0 && (bytes[0] == B_ENTER || bytes[0] == B_SPACE)) {
		SetValue(B_CONTROL_ON);
		Invoke();
	}
	else
		BControl::KeyDown(bytes, numBytes);
}

void BRadioButton::SetValue(int32 value)
{
	BControl::SetValue(value);

	BView *parent = Parent();
	if (Value() != B_CONTROL_OFF && parent) {
		// turn off siblings
		BView *sibling = Parent()->ChildAt(0);
		while (sibling) {
			BRadioButton *radio = dynamic_cast<BRadioButton *>(sibling);
			if (radio && radio != this) {
				radio->SetValue(B_CONTROL_OFF);
			}
			sibling = sibling->NextSibling();
		}
	}
}

void BRadioButton::GetPreferredSize(float *width, float *height)
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

void BRadioButton::ResizeToPreferred()
{
	BControl::ResizeToPreferred();
}

status_t BRadioButton::Invoke(BMessage *msg)
{
	return BControl::Invoke(msg);
}

void BRadioButton::MessageReceived(BMessage *msg)
{
	BControl::MessageReceived(msg);
}

void BRadioButton::WindowActivated(bool state)
{
	BControl::WindowActivated(state);
}

void BRadioButton::DetachedFromWindow()
{
	BControl::DetachedFromWindow();
}

void BRadioButton::FrameMoved(BPoint new_position)
{
	BControl::FrameMoved(new_position);
}

void BRadioButton::FrameResized(float new_width, float new_height)
{
	BControl::FrameResized(new_width, new_height);
}

BHandler *BRadioButton::ResolveSpecifier(BMessage *msg, int32 index, BMessage *specifier, int32 form, const char *property)
{
	return BControl::ResolveSpecifier(msg, index, specifier, form, property);
}

void BRadioButton::MakeFocus(bool state)
{
	BControl::MakeFocus(state);
}

void BRadioButton::AllAttached()
{
	BControl::AllAttached();
}

void BRadioButton::AllDetached()
{
	BControl::AllDetached();
}

status_t BRadioButton::GetSupportedSuites(BMessage *data)
{
	return BControl::GetSupportedSuites(data);
}
