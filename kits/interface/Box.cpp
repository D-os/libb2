#include "Box.h"

#include "GraphicsDefs.h"

#define LOG_TAG "BBox"

#include <log/log.h>

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
	free(const_cast<char *>(fLabel));
}

BBox::BBox(BMessage *data) : BView(data), fStyle{B_NO_BORDER} {}

status_t BBox::Archive(BMessage *data, bool deep) const
{
	return BView::Archive(data, deep);
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
	free(fLabel);
	fLabel	   = label ? strdup(label) : nullptr;

	if (fLabelView) {
		fLabelView->RemoveSelf();
		fLabelView = nullptr;
	}

	Invalidate();
}

status_t BBox::SetLabel(BView *view_label)
{
	if (!view_label)
		return B_BAD_VALUE;

	free(fLabel);
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

	font_height metrics;
	if (fLabel) {
		GetFontHeight(&metrics);
		frame.top += ceilf(metrics.ascent) - ceilf(metrics.x_height / 2);
	}
	else if (fLabelView) {
		frame.top += floorf(fLabelView->Bounds().Height() / 2);
	}

	PushState();
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
		frame.right -= pen_size;
		frame.bottom -= pen_size;
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
		BRect frame	 = BRect(left, bounds.top, left + width + pen_size * 3, ceilf(metrics.ascent) + ceilf(metrics.descent));
		FillRect(frame.InsetByCopy(-2, 0), B_SOLID_LOW);
		DrawString(fLabel, {left, ceilf(metrics.ascent) + 1});
	}
	else if (fLabelView) {
		SetLowColor(ViewColor());
		FillRect(fLabelView->Frame().InsetByCopy(-2, 0), B_SOLID_LOW);
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
