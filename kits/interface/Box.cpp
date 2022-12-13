#include "Box.h"

#define BOX_LABEL_OFFSET 8

BBox::BBox(BRect bounds, const char *name, uint32 resizeFlags, uint32 flags, border_style border)
	: BView(bounds, name, resizeFlags, flags),
	  fLabel{nullptr},
	  fStyle{border},
	  fLabelView{nullptr}
{
	SetFont(be_bold_font);
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

	auto left = PenSize() * 2 + BOX_LABEL_OFFSET;
	fLabelView->MoveTo(left, 0.0);
	AddChild(fLabelView);

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

	auto  pen_size = PenSize();
	BRect frame	   = Bounds();
	frame.InsetBy(pen_size / 2, pen_size / 2);

	font_height metrics;
	if (fLabel) {
		GetFontHeight(&metrics);
		frame.top += metrics.cap_height / 2;
	}
	else if (fLabelView) {
		frame.top += fLabelView->Bounds().Height() / 2;
	}

	PushState();
	if (fStyle == B_PLAIN_BORDER) {
		SetHighColor(ui_color(B_SHADOW_COLOR));
		MovePenTo(frame.LeftBottom() + BPoint{0, pen_size});
		StrokeLine(frame.RightBottom() + BPoint{pen_size, pen_size});
		StrokeLine(frame.RightTop() + BPoint{pen_size, 0});
		SetHighColor(ui_color(B_SHINE_COLOR));
		StrokeLine(frame.LeftTop());
		StrokeLine(frame.LeftBottom() + BPoint{0, pen_size});
	}
	if (fStyle == B_FANCY_BORDER) {
		SetHighColor(ui_color(B_SHADOW_COLOR));
		SetPenSize(pen_size * 2);
		StrokeRect(frame);
		SetPenSize(pen_size);
		SetHighColor(ui_color(B_SHINE_COLOR));
		StrokeRect(frame.OffsetByCopy(pen_size, pen_size));
	}
	PopState();

	PushState();
	if (fLabel) {
		SetLowColor(ViewColor());
		auto  bounds = Bounds();
		float width	 = StringWidth(fLabel);
		auto  left	 = frame.left + pen_size + BOX_LABEL_OFFSET + pen_size;
		FillRect(BRect(left, bounds.top, left + width + pen_size * 3, metrics.cap_height + metrics.descent), B_SOLID_LOW);
		BPoint pos{left, metrics.cap_height};
		DrawString(fLabel, pos);
	}
	else if (fLabelView) {
		SetLowColor(ViewColor());
		auto bounds = fLabelView->Frame();
		bounds.InsetBy(-pen_size, -pen_size);
		FillRect(bounds, B_SOLID_LOW);
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
