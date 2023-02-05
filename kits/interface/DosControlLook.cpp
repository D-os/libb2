#define CHECKBOX_BOX_SIZE 12.0f
#define CHECKBOX_TEXT_PADDING 5.0f

#include "DosControlLook.h"

#include <Bitmap.h>
#include <Control.h>
#include <Font.h>
#include <String.h>
#include <Window.h>

using namespace BPrivate;

DosControlLook::DosControlLook() {}

DosControlLook::~DosControlLook() {}

BAlignment DosControlLook::DefaultLabelAlignment() const
{
	return BAlignment(B_ALIGN_LEFT, B_ALIGN_VERTICAL_CENTER);
}

float DosControlLook::DefaultLabelSpacing() const
{
	return ceilf(be_plain_font->Size() / 2.0);
}

float DosControlLook::DefaultItemSpacing() const
{
	return ceilf(be_plain_font->Size() * 0.85);
}

uint32 DosControlLook::Flags(BControl* control) const
{
	uint32 flags = B_IS_CONTROL;

	if (!control->IsEnabled())
		flags |= B_DISABLED;

	if (control->IsFocus() && control->Window() && control->Window()->IsActive()) {
		flags |= B_FOCUSED;
	}

	switch (control->Value()) {
		case B_CONTROL_ON:
			flags |= B_ACTIVATED;
			break;
		case B_CONTROL_PARTIALLY_ON:
			flags |= B_PARTIALLY_ACTIVATED;
			break;
	}

	if (control->Parent() && (control->Parent()->Flags() & B_DRAW_ON_CHILDREN) != 0) {
		// In this constellation, assume we want to render the control
		// against the already existing view contents of the parent view.
		flags |= B_BLEND_FRAME;
	}

	return flags;
}

void DosControlLook::DrawMenuBarBackground(BView* view, BRect& rect,
										   const BRect&		updateRect,
										   const rgb_color& base,
										   uint32			flags,
										   uint32			borders)
{
	if (!rect.IsValid() || !rect.Intersects(updateRect))
		return;

	// the surface edges

	// colors
	float topTint;
	float bottomTint;

	if ((flags & B_ACTIVATED) != 0) {
		rgb_color bevelColor1 = tint_color(base, 1.40);
		rgb_color bevelColor2 = tint_color(base, 1.25);

		topTint	   = 1.25;
		bottomTint = 1.20;

		_DrawFrame(view, rect,
				   bevelColor1, bevelColor1,
				   bevelColor2, bevelColor2,
				   borders & B_TOP_BORDER);
	}
	else {
		rgb_color cornerColor			= tint_color(base, 0.9);
		rgb_color bevelColorTop			= tint_color(base, 0.5);
		rgb_color bevelColorLeft		= tint_color(base, 0.7);
		rgb_color bevelColorRightBottom = tint_color(base, 1.08);

		topTint	   = 0.69;
		bottomTint = 1.03;

		_DrawFrame(view, rect,
				   bevelColorLeft, bevelColorTop,
				   bevelColorRightBottom, bevelColorRightBottom,
				   cornerColor, cornerColor,
				   borders);
	}

	// draw surface top
	_FillGradient(view, rect, base, topTint, bottomTint);
}

void DosControlLook::DrawMenuBackground(BView* view,
										BRect& rect, const BRect& updateRect,
										const rgb_color& base, uint32 flags,
										uint32 borders)
{
	if (!rect.IsValid() || !rect.Intersects(updateRect))
		return;

	// inner bevel colors
	rgb_color bevelLightColor;
	rgb_color bevelShadowColor;

	if ((flags & B_DISABLED) != 0) {
		bevelLightColor	 = tint_color(base, 0.80);
		bevelShadowColor = tint_color(base, 1.07);
	}
	else {
		bevelLightColor	 = tint_color(base, 0.6);
		bevelShadowColor = tint_color(base, 1.12);
	}

	// draw inner bevel
	_DrawFrame(view, rect,
			   bevelLightColor, bevelLightColor,
			   bevelShadowColor, bevelShadowColor,
			   borders);

	// draw surface top
	view->SetHighColor(base);
	view->FillRect(rect);
}

void DosControlLook::DrawMenuItemBackground(BView* view, BRect& rect,
											const BRect& updateRect, const rgb_color& base, uint32 flags,
											uint32 borders)
{
	if (!rect.IsValid() || !rect.Intersects(updateRect))
		return;

	// surface edges
	float	  topTint;
	float	  bottomTint;
	rgb_color selectedColor = base;

	if ((flags & B_ACTIVATED) != 0) {
		topTint	   = 0.9;
		bottomTint = 1.05;
	}
	else if ((flags & B_DISABLED) != 0) {
		topTint	   = 0.80;
		bottomTint = 1.07;
	}
	else {
		topTint	   = 0.6;
		bottomTint = 1.12;
	}

	rgb_color bevelLightColor  = tint_color(selectedColor, topTint);
	rgb_color bevelShadowColor = tint_color(selectedColor, bottomTint);

	// draw surface edges
	_DrawFrame(view, rect,
			   bevelLightColor, bevelLightColor,
			   bevelShadowColor, bevelShadowColor,
			   borders);

	// draw surface top
	view->SetLowColor(selectedColor);
	//      _FillGradient(view, rect, selectedColor, topTint, bottomTint);
	_FillGradient(view, rect, selectedColor, bottomTint, topTint);
}

void DosControlLook::DrawArrowShape(BView* view, BRect& rect,
									const BRect& updateRect, const rgb_color& base, uint32 direction,
									uint32 flags, float tint)
{
	BPoint tri1, tri2, tri3;
	float  hInset = rect.Width() / 3;
	float  vInset = rect.Height() / 3;
	rect.InsetBy(hInset, vInset);

	switch (direction) {
		case B_LEFT_ARROW:
			tri1.Set(rect.right, rect.top);
			tri2.Set(rect.right - rect.Width() / 1.33,
					 (rect.top + rect.bottom + 1) / 2);
			tri3.Set(rect.right, rect.bottom + 1);
			break;
		case B_RIGHT_ARROW:
			tri1.Set(rect.left + 1, rect.bottom + 1);
			tri2.Set(rect.left + 1 + rect.Width() / 1.33,
					 (rect.top + rect.bottom + 1) / 2);
			tri3.Set(rect.left + 1, rect.top);
			break;
		case B_UP_ARROW:
			tri1.Set(rect.left, rect.bottom);
			tri2.Set((rect.left + rect.right + 1) / 2,
					 rect.bottom - rect.Height() / 1.33);
			tri3.Set(rect.right + 1, rect.bottom);
			break;
		case B_DOWN_ARROW:
		default:
			tri1.Set(rect.left, rect.top + 1);
			tri2.Set((rect.left + rect.right + 1) / 2,
					 rect.top + 1 + rect.Height() / 1.33);
			tri3.Set(rect.right + 1, rect.top + 1);
			break;
		case B_LEFT_UP_ARROW:
			tri1.Set(rect.left, rect.bottom);
			tri2.Set(rect.left, rect.top);
			tri3.Set(rect.right - 1, rect.top);
			break;
		case B_RIGHT_UP_ARROW:
			tri1.Set(rect.left + 1, rect.top);
			tri2.Set(rect.right, rect.top);
			tri3.Set(rect.right, rect.bottom);
			break;
		case B_RIGHT_DOWN_ARROW:
			tri1.Set(rect.right, rect.top);
			tri2.Set(rect.right, rect.bottom);
			tri3.Set(rect.left + 1, rect.bottom);
			break;
		case B_LEFT_DOWN_ARROW:
			tri1.Set(rect.right - 1, rect.bottom);
			tri2.Set(rect.left, rect.bottom);
			tri3.Set(rect.left, rect.top);
			break;
	}

	if ((flags & B_DISABLED) != 0)
		tint = (tint + B_NO_TINT + B_NO_TINT) / 3;

	view->SetHighColor(tint_color(base, tint));

	float		 penSize = view->PenSize();
	drawing_mode mode	 = view->DrawingMode();

	view->SetPenSize(ceilf(hInset / 2.0));
	view->SetDrawingMode(B_OP_OVER);
	view->MovePenTo(tri1);
	view->StrokeLine(tri2);
	view->StrokeLine(tri3);

	view->SetPenSize(penSize);
	view->SetDrawingMode(mode);
}

void DosControlLook::DrawBorder(BView* view, BRect& rect,
								const BRect&	 updateRect,
								const rgb_color& base,
								border_style borderStyle, uint32 flags,
								uint32 borders)
{
	if (borderStyle == B_NO_BORDER)
		return;

	rgb_color scrollbarFrameColor = tint_color(base, B_DARKEN_2_TINT);
	if (base.red + base.green + base.blue <= 128 * 3) {
		scrollbarFrameColor = tint_color(base, B_LIGHTEN_1_TINT);
	}

	if ((flags & B_FOCUSED) != 0)
		scrollbarFrameColor = ui_color(B_KEYBOARD_NAVIGATION_COLOR);

	if (borderStyle == B_FANCY_BORDER)
		_DrawOuterResessedFrame(view, rect, base, 1.0, 1.0, flags, borders);

	_DrawFrame(view, rect, scrollbarFrameColor, scrollbarFrameColor,
			   scrollbarFrameColor, scrollbarFrameColor, borders);
}

void DosControlLook::DrawLabel(BView* view, const char* label, BRect rect,
							   const BRect& updateRect, const rgb_color& base, uint32 flags,
							   const rgb_color* textColor)
{
	DrawLabel(view, label, NULL, rect, updateRect, base, flags,
			  DefaultLabelAlignment(), textColor);
}

void DosControlLook::DrawLabel(BView* view, const char* label, BRect rect,
							   const BRect& updateRect, const rgb_color& base, uint32 flags,
							   const BAlignment& alignment, const rgb_color* textColor)
{
	DrawLabel(view, label, NULL, rect, updateRect, base, flags, alignment,
			  textColor);
}

void DosControlLook::DrawLabel(BView* view, const char* label, const rgb_color& base,
							   uint32 flags, const BPoint& where, const rgb_color* textColor)
{
	rgb_color low;
	rgb_color color;
	rgb_color glowColor;

	if (textColor)
		glowColor = *textColor;
	else if ((flags & B_IS_CONTROL) != 0)
		glowColor = ui_color(B_CONTROL_TEXT_COLOR);
	else
		glowColor = ui_color(B_PANEL_TEXT_COLOR);

	color = glowColor;
	low	  = base;

	if ((flags & B_DISABLED) != 0) {
		color.red	= (uint8)(((int32)low.red + color.red + 1) / 2);
		color.green = (uint8)(((int32)low.green + color.green + 1) / 2);
		color.blue	= (uint8)(((int32)low.blue + color.blue + 1) / 2);
	}

	drawing_mode oldMode = view->DrawingMode();

	view->SetHighColor(color);
	view->SetDrawingMode(B_OP_OVER);
	view->DrawString(label, where);
	view->SetDrawingMode(oldMode);
}

void DosControlLook::DrawLabel(BView* view, const char* label, const BBitmap* icon,
							   BRect rect, const BRect& updateRect, const rgb_color& base, uint32 flags,
							   const BAlignment& alignment, const rgb_color* textColor)
{
	if (!rect.Intersects(updateRect))
		return;

	if (!label && !icon)
		return;

	if (!label) {
		// icon only
		BRect		 alignedRect = _AlignInFrame(rect, icon->Bounds().Size(), alignment);
		drawing_mode oldMode	 = view->DrawingMode();
		view->SetDrawingMode(B_OP_OVER);
		view->DrawBitmap(icon, alignedRect.LeftTop());
		view->SetDrawingMode(oldMode);
		return;
	}

	// label, possibly with icon
	float availableWidth = rect.Width() + 1;
	float width			 = 0;
	float textOffset	 = 0;
	float height		 = 0;

	if (icon) {
		width	   = icon->Bounds().Width() + DefaultLabelSpacing() + 1;
		height	   = icon->Bounds().Height() + 1;
		textOffset = width;
		availableWidth -= textOffset;
	}

	// truncate the label if necessary and get the width and height
	BString truncatedLabel(label);

	BFont font;
	view->GetFont(&font);

	font.TruncateString(&truncatedLabel, B_TRUNCATE_END, availableWidth);
	width += ceilf(font.StringWidth(truncatedLabel.String()));

	font_height fontHeight;
	font.GetHeight(&fontHeight);
	float textHeight = ceilf(fontHeight.ascent) + ceilf(fontHeight.descent);
	height			 = std::max(height, textHeight);

	// handle alignment
	BRect alignedRect(_AlignOnRect(rect, BSize(width - 1, height - 1), alignment));

	if (icon != NULL) {
		BPoint location(alignedRect.LeftTop());
		if (icon->Bounds().Height() + 1 < height)
			location.y += ceilf((height - icon->Bounds().Height() - 1) / 2);

		drawing_mode oldMode = view->DrawingMode();
		view->SetDrawingMode(B_OP_OVER);
		view->DrawBitmap(icon, location);
		view->SetDrawingMode(oldMode);
	}

	BPoint location(alignedRect.left + textOffset,
					alignedRect.top + ceilf(fontHeight.ascent));
	if (textHeight < height)
		location.y += ceilf((height - textHeight) / 2);

	DrawLabel(view, truncatedLabel.String(), base, flags, location, textColor);
}

#pragma mark -

void DosControlLook::_DrawOuterResessedFrame(BView* view, BRect& rect,
											 const rgb_color& base, float contrast, float brightness, uint32 flags,
											 uint32 borders)
{
	rgb_color edgeLightColor  = ui_color(B_SHINE_COLOR);
	rgb_color edgeShadowColor = ui_color(B_SHADOW_COLOR);

	if ((flags & B_BLEND_FRAME) != 0) {
		// assumes the background has already been painted
		drawing_mode oldDrawingMode = view->DrawingMode();
		view->SetDrawingMode(B_OP_ALPHA);

		_DrawFrame(view, rect, edgeShadowColor, edgeShadowColor,
				   edgeLightColor, edgeLightColor, borders);

		view->SetDrawingMode(oldDrawingMode);
	}
	else {
		_DrawFrame(view, rect, edgeShadowColor, edgeShadowColor,
				   edgeLightColor, edgeLightColor, borders);
	}
}

void DosControlLook::_DrawFrame(BView* view, BRect& rect, const rgb_color& left,
								const rgb_color& top, const rgb_color& right, const rgb_color& bottom,
								uint32 borders)
{
	view->BeginLineArray(4);

	if (borders & B_LEFT_BORDER) {
		view->AddLine(
			BPoint(rect.left, rect.bottom),
			BPoint(rect.left, rect.top), left);
		rect.left++;
	}
	if (borders & B_TOP_BORDER) {
		view->AddLine(
			BPoint(rect.left, rect.top),
			BPoint(rect.right, rect.top), top);
		rect.top++;
	}
	if (borders & B_RIGHT_BORDER) {
		view->AddLine(
			BPoint(rect.right, rect.top),
			BPoint(rect.right, rect.bottom), right);
		rect.right--;
	}
	if (borders & B_BOTTOM_BORDER) {
		view->AddLine(
			BPoint(rect.left, rect.bottom),
			BPoint(rect.right, rect.bottom), bottom);
		rect.bottom--;
	}

	view->EndLineArray();
}

void DosControlLook::_DrawFrame(BView* view, BRect& rect, const rgb_color& left,
								const rgb_color& top, const rgb_color& right, const rgb_color& bottom,
								const rgb_color& rightTop, const rgb_color& leftBottom, uint32 borders)
{
	view->BeginLineArray(6);

	if (borders & B_TOP_BORDER) {
		if (borders & B_RIGHT_BORDER) {
			view->AddLine(
				BPoint(rect.left, rect.top),
				BPoint(rect.right - 1, rect.top), top);
			view->AddLine(
				BPoint(rect.right, rect.top),
				BPoint(rect.right, rect.top), rightTop);
		}
		else {
			view->AddLine(
				BPoint(rect.left, rect.top),
				BPoint(rect.right, rect.top), top);
		}
		rect.top++;
	}

	if (borders & B_LEFT_BORDER) {
		view->AddLine(
			BPoint(rect.left, rect.top),
			BPoint(rect.left, rect.bottom - 1), left);
		view->AddLine(
			BPoint(rect.left, rect.bottom),
			BPoint(rect.left, rect.bottom), leftBottom);
		rect.left++;
	}

	if (borders & B_BOTTOM_BORDER) {
		view->AddLine(
			BPoint(rect.left, rect.bottom),
			BPoint(rect.right, rect.bottom), bottom);
		rect.bottom--;
	}

	if (borders & B_RIGHT_BORDER) {
		view->AddLine(
			BPoint(rect.right, rect.bottom),
			BPoint(rect.right, rect.top), right);
		rect.right--;
	}

	view->EndLineArray();
}

// AlignInFrame
// This method restricts the dimensions of the resulting rectangle according
// to the available size specified by maxSize.
BRect DosControlLook::_AlignInFrame(BRect frame, BSize maxSize, BAlignment alignment)
{
	// align according to the given alignment
	if (maxSize.width < frame.Width()
		&& alignment.horizontal != B_ALIGN_USE_FULL_WIDTH) {
		frame.left += (int)((frame.Width() - maxSize.width) * alignment.RelativeHorizontal());
		frame.right = frame.left + maxSize.width;
	}
	if (maxSize.height < frame.Height()
		&& alignment.vertical != B_ALIGN_USE_FULL_HEIGHT) {
		frame.top += (int)((frame.Height() - maxSize.height) * alignment.RelativeVertical());
		frame.bottom = frame.top + maxSize.height;
	}

	return frame;
}

void DosControlLook::_FillGradient(BView* view, const BRect& rect,
								   const rgb_color& base, float topTint, float bottomTint,
								   orientation orientation)
{
	// BGradientLinear gradient;
	// _MakeGradient(gradient, rect, base, topTint, bottomTint, orientation);
	// view->FillRect(rect, gradient);

	// FIXME: temporary hack
	view->SetHighColor(base);
	view->FillRect(rect);
}

// AlignOnRect
// This method, unlike AlignInFrame(), provides the possibility to return
// a rectangle with dimensions greater than the available size.
BRect DosControlLook::_AlignOnRect(BRect rect, BSize size, BAlignment alignment)
{
	rect.left += (int)((rect.Width() - size.width) * alignment.RelativeHorizontal());
	rect.top += (int)(((rect.Height() - size.height)) * alignment.RelativeVertical());
	rect.right	= rect.left + size.width;
	rect.bottom = rect.top + size.height;

	return rect;
}
