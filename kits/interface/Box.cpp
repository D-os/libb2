#include "Box.h"

#define BOX_LABEL_OFFSET 10

BBox::BBox(BRect bounds, const char *name, uint32 resizeFlags, uint32 flags, border_style border)
	: BView(bounds, name, resizeFlags, flags),
	  fLabel{nullptr},
	  fStyle{border},
	  fLabelView{nullptr}
{
	SetFont(be_bold_font);
	SetFontSize(11);
}

BBox::~BBox()
{
	if (fLabel)
		free(const_cast<char *>(fLabel));
}

BBox::BBox(BMessage *data) : BView(data), fStyle{B_NO_BORDER} {}

status_t BBox::Archive(BMessage *data, bool deep) const
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

void BBox::SetBorder(border_style style)
{
	fStyle = style;
	Invalidate();
}

border_style BBox::Border() const
{
	return fStyle;
}

void BBox::SetLabel(const char *label)
{
	const char *current = fLabel;

	fLabel	   = label ? strdup(label) : nullptr;
	fLabelView = nullptr;

	if (current)
		free(const_cast<char *>(current));
	Invalidate();
}

status_t BBox::SetLabel(BView *view_label)
{
	if (!view_label)
		return B_BAD_VALUE;

	if (fLabel)
		free(const_cast<char *>(fLabel));
	fLabel	   = nullptr;
	fLabelView = view_label;
	Invalidate();
	return B_OK;
}

const char *BBox::Label() const
{
	return fLabel;
}

BView *BBox::LabelView() const
{
	return fLabelView;
}

void BBox::Draw(BRect updateRect)
{
	BView::Draw(updateRect);

	PushState();

	auto  pen_size = PenSize();
	BRect frame	   = Bounds();
	frame.InsetBy(pen_size / 2, pen_size / 2);

	if (fStyle == B_PLAIN_BORDER) {
		SetHighColor(ui_color(B_SHADOW_COLOR));
		MovePenTo(frame.LeftBottom());
		StrokeLine(frame.RightBottom());
		StrokeLine(frame.RightTop());
		SetHighColor(ui_color(B_SHINE_COLOR));
		StrokeLine(frame.LeftTop());
		StrokeLine(frame.LeftBottom());
	}
	if (fStyle == B_FANCY_BORDER) {
		SetHighColor(ui_color(B_SHADOW_COLOR));
		SetPenSize(pen_size * 2);
		StrokeRect(frame);
		SetPenSize(pen_size);
		SetHighColor(ui_color(B_SHINE_COLOR));
		StrokeRect(frame);
	}

	if (fLabel) {
		font_height metrics;
		GetFontHeight(&metrics);

		BPoint pos{frame.left + pen_size + BOX_LABEL_OFFSET, frame.top + metrics.leading - metrics.descent};

		DrawString(fLabel, pos);
	}

	PopState();
}

void BBox::AttachedToWindow()
{
	BView::AttachedToWindow();
	// BBox's background view color and its low color match the background color of its new parent
	SetViewColor(Parent()->ViewColor());
	SetLowColor(Parent()->ViewColor());
}

void BBox::DetachedFromWindow()
{
	BView::DetachedFromWindow();
}

void BBox::AllAttached()
{
	BView::AllAttached();
}

void BBox::AllDetached()
{
	BView::AllDetached();
}

void BBox::FrameResized(float new_width, float new_height)
{
	BView::FrameResized(new_width, new_height);
}

void BBox::MessageReceived(BMessage *msg)
{
	BView::MessageReceived(msg);
}

void BBox::MouseDown(BPoint pt)
{
	BView::MouseDown(pt);
}

void BBox::MouseUp(BPoint pt)
{
	BView::MouseUp(pt);
}

void BBox::WindowActivated(bool state)
{
	BView::WindowActivated(state);
}

void BBox::MouseMoved(BPoint pt, uint32 code, const BMessage *msg)
{
	BView::MouseMoved(pt, code, msg);
}

void BBox::FrameMoved(BPoint new_position)
{
	BView::FrameMoved(new_position);
}

BHandler *BBox::ResolveSpecifier(BMessage *msg, int32 index, BMessage *specifier, int32 form, const char *property)
{
	return BView::ResolveSpecifier(msg, index, specifier, form, property);
}

void BBox::ResizeToPreferred()
{
	BView::ResizeToPreferred();
}

void BBox::GetPreferredSize(float *width, float *height)
{
	BView::GetPreferredSize(width, height);
}

void BBox::MakeFocus(bool state)
{
	BView::MakeFocus(state);
}

status_t BBox::GetSupportedSuites(BMessage *data)
{
	return BView::GetSupportedSuites(data);
}
