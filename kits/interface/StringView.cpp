#include "StringView.h"

#include <View.h>

#include <cstdlib>
#include <cstring>

BStringView::BStringView(BRect bounds, const char *name, const char *text, uint32 resizeFlags, uint32 flags)
	: BView(bounds, name, resizeFlags, flags),
	  fText(text ? strdup(text) : nullptr),
	  fAlign(B_ALIGN_LEFT) {}

BStringView::~BStringView()
{
	if (fText)
		free(const_cast<char *>(fText));
}

status_t BStringView::Archive(BMessage *data, bool deep) const
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

void BStringView::SetText(const char *text)
{
	const char *current = fText;

	fText = text ? strdup(text) : nullptr;

	if (current)
		free(const_cast<char *>(current));
	Invalidate();
}

const char *BStringView::Text() const
{
	return fText ? fText : "";
}

void BStringView::SetAlignment(alignment flag)
{
	alignment current = fAlign;

	fAlign = flag;

	if (current != fAlign)
		Invalidate();
}

alignment BStringView::Alignment() const
{
	return fAlign;
}

void BStringView::AttachedToWindow()
{
	BView::AttachedToWindow();
	SetLowColor(Parent()->LowColor());
	SetViewColor(Parent()->ViewColor());
}

void BStringView::Draw(BRect bounds)
{
	if (fText) {
		BFont font;
		GetFont(&font);
		font_height metrics;
		font.GetHeight(&metrics);

		auto frame = Frame();

		BPoint pos{frame.left, frame.bottom - metrics.descent};

		if (fAlign & B_ALIGN_RIGHT) {
			pos.x = frame.right - font.StringWidth(fText);
		}
		else if (fAlign & B_ALIGN_CENTER) {
			pos.x = frame.right - font.StringWidth(fText);
			pos.x -= (pos.x - frame.left) / 2;
		}

		PushState();
		DrawString(fText, pos);
		PopState();
	}
}

void BStringView::MessageReceived(BMessage *msg)
{
	BView::MessageReceived(msg);
}

void BStringView::MouseDown(BPoint pt)
{
	BView::MouseDown(pt);
}

void BStringView::MouseUp(BPoint pt)
{
	BView::MouseUp(pt);
}

void BStringView::MouseMoved(BPoint pt, uint32 code, const BMessage *msg)
{
	BView::MouseMoved(pt, code, msg);
}

void BStringView::DetachedFromWindow()
{
	BView::DetachedFromWindow();
}

void BStringView::FrameMoved(BPoint new_position)
{
	BView::FrameMoved(new_position);
}

void BStringView::FrameResized(float new_width, float new_height)
{
	BView::FrameResized(new_width, new_height);
}

BHandler *BStringView::ResolveSpecifier(BMessage *msg, int32 index, BMessage *specifier, int32 form, const char *property)
{
	return BView::ResolveSpecifier(msg, index, specifier, form, property);
}

void BStringView::ResizeToPreferred()
{
	BView::ResizeToPreferred();
}

void BStringView::GetPreferredSize(float *width, float *height)
{
	BFont font;
	GetFont(&font);

	if (width && fText) {
		*width = font.StringWidth(fText);
	}
	if (height) {
		font_height metrics;
		font.GetHeight(&metrics);
		*height = metrics.leading;
	}
}

void BStringView::MakeFocus(bool state)
{
	BView::MakeFocus(state);
}

void BStringView::AllAttached()
{
	BView::AllAttached();
}

void BStringView::AllDetached()
{
	BView::AllDetached();
}

status_t BStringView::GetSupportedSuites(BMessage *data)
{
	return BView::GetSupportedSuites(data);
}
