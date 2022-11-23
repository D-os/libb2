#include "View.h"

#define LOG_TAG "BView"

#include <Font.h>
#include <GraphicsDefs.h>
#include <Message.h>
#include <Polygon.h>
#include <Region.h>
#include <Window.h>
#include <doctest/doctest.h>
#include <include/core/SkBitmap.h>
#include <include/core/SkCanvas.h>
#include <include/core/SkColor.h>
#include <include/core/SkData.h>
#include <include/core/SkFontMetrics.h>
#include <include/core/SkImage.h>
#include <include/core/SkPaint.h>
#include <include/core/SkPath.h>
#include <include/core/SkPoint.h>
#include <include/core/SkRegion.h>
#include <include/core/SkTextBlob.h>
#include <log/log.h>
#include <pimpl.h>

class BView::impl
{
   public:
	rgb_color view_color;
	rgb_color high_color;
	rgb_color low_color;

	drawing_mode drawing_mode;
	float		 pen_size;
	SkPoint		 pen_location;

	BFont font;

	impl()
		: view_color{255, 255, 255, 255},
		  high_color{0, 0, 0, 255},
		  low_color{255, 255, 255, 255},
		  drawing_mode{B_OP_COPY},
		  pen_size{1.0},
		  pen_location{0.0, 0.0},
		  font(be_plain_font)
	{
	}

	void fill_pattern(rgb_color pixels[8 * 8], pattern &p);
};

void BView::impl::fill_pattern(rgb_color pixels[8 * 8], pattern &p)
{
	const rgb_color low = (drawing_mode == B_OP_OVER
						   || drawing_mode == B_OP_ERASE
						   || drawing_mode == B_OP_INVERT
						   || drawing_mode == B_OP_SELECT)
							  ? B_TRANSPARENT_COLOR
							  : low_color;

	for (int row = 0; row < 8; ++row) {
		if (p.data[row] & 0b10000000)
			pixels[0 + row * 8] = high_color;
		else
			pixels[0 + row * 8] = low;
		if (p.data[row] & 0b01000000)
			pixels[1 + row * 8] = high_color;
		else
			pixels[1 + row * 8] = low;
		if (p.data[row] & 0b00100000)
			pixels[2 + row * 8] = high_color;
		else
			pixels[2 + row * 8] = low;
		if (p.data[row] & 0b00010000)
			pixels[3 + row * 8] = high_color;
		else
			pixels[3 + row * 8] = low;
		if (p.data[row] & 0b00001000)
			pixels[4 + row * 8] = high_color;
		else
			pixels[4 + row * 8] = low;
		if (p.data[row] & 0b00000100)
			pixels[5 + row * 8] = high_color;
		else
			pixels[5 + row * 8] = low;
		if (p.data[row] & 0b00000010)
			pixels[6 + row * 8] = high_color;
		else
			pixels[6 + row * 8] = low;
		if (p.data[row] & 0b00000001)
			pixels[7 + row * 8] = high_color;
		else
			pixels[7 + row * 8] = low;
	}
}

#pragma mark - BView

BView::BView(BRect frame, const char *name, uint32 resizeMask, uint32 flags)
	: BHandler(name),
	  fOwner{nullptr},
	  fParent{nullptr},
	  fNextSibling{nullptr},
	  fPrevSibling{nullptr},
	  fFirstChild{nullptr},
	  fShowLevel{0},
	  fTopLevelView{false}
{
	ALOGW_IF(((resizeMask & ~_RESIZE_MASK_) || (flags & _RESIZE_MASK_)),
			 "%s: resizing mode or flags swapped", name);

	// There are applications that swap the resize mask and the flags in the
	// BView constructor. This does not cause problems under BeOS as it just
	// ors the two fields to one 32bit flag.
	// For now we do the same but print the above warning message.
	// TODO: this should be removed at some point and the original
	// version restored:
	// fFlags = (resizeMask & _RESIZE_MASK_) | (flags & ~_RESIZE_MASK_);
	fFlags = resizeMask | flags;

	frame.left	 = floorf(frame.left);
	frame.top	 = floorf(frame.top);
	frame.right	 = floorf(frame.right);
	frame.bottom = floorf(frame.bottom);
	fParentOffset.Set(frame.left, frame.top);
	fBounds = frame.OffsetToCopy(B_ORIGIN);
}

BView::~BView() {}

status_t BView::Archive(BMessage *data, bool deep) const
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

void BView::AttachedToWindow()
{
	// The default version of this functions is empty.
}

void BView::AllAttached()
{
	// The default version of this functions is empty.
}

void BView::DetachedFromWindow()
{
	// The default version of this functions is empty.
}

void BView::AllDetached()
{
	// The default version of this functions is empty.
}

void BView::MessageReceived(BMessage *message)
{
	ALOGV("BView::MessageReceived @%s 0x%x: %.4s", Name(), message->what, (char *)&message->what);
	switch (message->what) {
		case _UPDATE_: {
			const rgb_color view_color(ViewColor());

			// The Application Server paints the view with this color before any view-specific drawing functions are called.
			// If you set the view color to B_TRANSPARENT_COLOR, the Application Server won't erase the view's clipping region before an update.
			if ((fFlags & B_WILL_DRAW) && (view_color != B_TRANSPARENT_COLOR)) {
				BRect updateRect;
				if (message->FindRect("updateRect", &updateRect) == B_OK && updateRect.IsValid()) {
					SkCanvas	 *canvas = static_cast<SkCanvas *>(fOwner->_get_canvas());
					if (!canvas) {
						// FIXME: now what? ¯\_(ツ)_/¯
						break;
					}

					int checkpoint = canvas->save();

					const auto &bounds = Bounds();
					canvas->clipRect(SkRect::MakeLTRB(bounds.left, bounds.top, bounds.right + 1, bounds.bottom + 1));
					canvas->clear(SkColorSetARGB(view_color.alpha, view_color.red, view_color.green, view_color.blue));

					// Call hook function to Draw content
					Draw(updateRect);

					canvas->restoreToCount(checkpoint);
				}
			}
			break;
		}
		default:
			BHandler::MessageReceived(message);
	}
}

void BView::AddChild(BView *child, BView *before)
{
	if (!child)
		return;

	if (child->fParent) {
		debugger("View already has a parent");
		return;
	}

	if (child == this) {
		debugger("Cannot add a view to itself");
		return;
	}

	if (before && before->fParent != this) {
		debugger("Invalid 'before' view");
		return;
	}

	bool lockedOwner = false;
	if (fOwner && !fOwner->IsLocked()) {
		fOwner->Lock();
		lockedOwner = true;
	}

	// AddChildToList
	if (before) {
		// add view before this one
		child->fNextSibling = before;
		child->fPrevSibling = before->fPrevSibling;
		if (child->fPrevSibling)
			child->fPrevSibling->fNextSibling = child;

		before->fPrevSibling = child;
		if (fFirstChild == before)
			fFirstChild = child;
	}
	else {
		// add view to the end of the list
		BView *last = fFirstChild;
		while (last && last->fNextSibling) {
			last = last->fNextSibling;
		}

		if (last) {
			last->fNextSibling	= child;
			child->fPrevSibling = last;
		}
		else {
			fFirstChild			= child;
			child->fPrevSibling = nullptr;
		}

		child->fNextSibling = nullptr;
	}

	child->fParent = this;

	if (fOwner) {
		child->_attach(fOwner);

		if (lockedOwner)
			fOwner->Unlock();
	}
}

bool BView::RemoveChild(BView *child)
{
	if (!child)
		return false;

	if (child->fParent != this)
		return false;

	ALOGE_IF(child->fOwner != fOwner, "Removing child '%s' of different owner %p than ours ('%s' %p)",
			 child->Name(), child->fOwner, Name(), fOwner);

	if (fOwner) {
		child->_detach();
	}

	BView *parent = fParent;
	if (!parent) {
		if (child->fParent != this)
			return false;

		if (fFirstChild == child) {
			// it's the first view in the list
			fFirstChild = child->fNextSibling;
		}
		else {
			// there must be a previous sibling
			child->fPrevSibling->fNextSibling = child->fNextSibling;
		}

		if (child->fNextSibling)
			child->fNextSibling->fPrevSibling = child->fPrevSibling;

		child->fParent		= nullptr;
		child->fNextSibling = nullptr;
		child->fPrevSibling = nullptr;
	}

	return true;
}

int32 BView::CountChildren() const
{
	debugger(__PRETTY_FUNCTION__);
	return -1;
}

BView *BView::ChildAt(int32 index) const
{
	debugger(__PRETTY_FUNCTION__);
	return nullptr;
}

BView *BView::NextSibling() const
{
	debugger(__PRETTY_FUNCTION__);
	return nullptr;
}

BView *BView::PreviousSibling() const
{
	debugger(__PRETTY_FUNCTION__);
	return nullptr;
}

bool BView::RemoveSelf()
{
	debugger(__PRETTY_FUNCTION__);
	return false;
}

BWindow *BView::Window() const
{
	return fOwner;
}

void BView::Draw(BRect updateRect)
{
	// Hook - default implementation does nothing
}

void BView::MouseDown(BPoint where)
{
	debugger(__PRETTY_FUNCTION__);
}

void BView::MouseUp(BPoint where)
{
	debugger(__PRETTY_FUNCTION__);
}

void BView::MouseMoved(BPoint where, uint32 code, const BMessage *a_message)
{
	debugger(__PRETTY_FUNCTION__);
}

void BView::WindowActivated(bool state)
{
	debugger(__PRETTY_FUNCTION__);
}

void BView::KeyDown(const char *bytes, int32 numBytes)
{
	debugger(__PRETTY_FUNCTION__);
}

void BView::KeyUp(const char *bytes, int32 numBytes)
{
	debugger(__PRETTY_FUNCTION__);
}

void BView::Pulse()
{
	debugger(__PRETTY_FUNCTION__);
}

void BView::FrameMoved(BPoint new_position)
{
	debugger(__PRETTY_FUNCTION__);
}

void BView::FrameResized(float new_width, float new_height)
{
	debugger(__PRETTY_FUNCTION__);
}

void BView::TargetedByScrollView(BScrollView *scroll_view)
{
	debugger(__PRETTY_FUNCTION__);
}

BView *BView::FindView(const char *name) const
{
	debugger(__PRETTY_FUNCTION__);
	return nullptr;
}

BView *BView::Parent() const
{
	return fParent;
}

BRect BView::Bounds() const
{
	return fBounds;
}

BRect BView::Frame() const
{
	return Bounds().OffsetToCopy(fParentOffset.x, fParentOffset.y);
}

void BView::ConstrainClippingRegion(BRegion *region)
{
	debugger(__PRETTY_FUNCTION__);
}

void BView::SetDrawingMode(drawing_mode mode)
{
	fState->drawing_mode = mode;
}

void BView::SetPenSize(float size)
{
	if (size < 0.0) return;

	fState->pen_size = size;
}

void BView::SetViewColor(rgb_color color)
{
	fState->view_color = color;
}

rgb_color BView::ViewColor() const
{
	return fState->view_color;
}

void BView::SetHighColor(rgb_color color)
{
	fState->high_color = color;
}

rgb_color BView::HighColor() const
{
	return fState->high_color;
}

void BView::SetLowColor(rgb_color color)
{
	fState->low_color = color;
}

rgb_color BView::LowColor() const
{
	return fState->low_color;
}

void BView::PushState()
{
	if (!fOwner) return;
	SkCanvas *canvas = static_cast<SkCanvas *>(fOwner->_get_canvas());
	if (!canvas) return;

	canvas->save();
}

void BView::PopState()
{
	if (!fOwner) return;
	SkCanvas *canvas = static_cast<SkCanvas *>(fOwner->_get_canvas());
	if (!canvas) return;

	canvas->restore();
}

void BView::MovePenTo(BPoint pt)
{
	MovePenTo(pt.x, pt.y);
}

void BView::MovePenTo(float x, float y)
{
	fState->pen_location.set(x, y);
}

void BView::MovePenBy(float x, float y)
{
	fState->pen_location.offset(x, y);
}

BPoint BView::PenLocation() const
{
	return BPoint(fState->pen_location.x(), fState->pen_location.y());
}

static SkBlendMode blend_modes[] = {
	[B_OP_COPY]		= SkBlendMode::kSrc,
	[B_OP_OVER]		= SkBlendMode::kSrcOver,
	[B_OP_ERASE]	= SkBlendMode::kDstOut,
	[B_OP_INVERT]	= SkBlendMode::kSrcATop,
	[B_OP_ADD]		= SkBlendMode::kPlus,
	[B_OP_SUBTRACT] = SkBlendMode::kScreen,
	[B_OP_BLEND]	= SkBlendMode::kMultiply,
	[B_OP_MIN]		= SkBlendMode::kDarken,
	[B_OP_MAX]		= SkBlendMode::kLighten,
	[B_OP_SELECT]	= SkBlendMode::kModulate,
};

#define DRAW_PRELUDE                                                                     \
	if (!fOwner) return;                                                                 \
	SkCanvas *canvas = fOwner->_get_canvas();                                            \
	if (!canvas) return;                                                                 \
                                                                                         \
	SkPaint paint;                                                                       \
	paint.setStrokeWidth(fState->pen_size > 0.0 ? /* scale * */ fState->pen_size : 1.0); \
                                                                                         \
	if (fState->drawing_mode < (sizeof(blend_modes) / sizeof(blend_modes[0])))           \
		paint.setBlendMode(blend_modes[fState->drawing_mode]);

#define DRAW_PRELUDE_WITH_COLOR    \
	DRAW_PRELUDE                   \
	paint.setColor(SkColorSetARGB( \
		fState->high_color.alpha,  \
		fState->high_color.red,    \
		fState->high_color.green,  \
		fState->high_color.blue));

#define DRAW_PRELUDE_WITH_PATTERN                                                         \
	DRAW_PRELUDE                                                                          \
	SkBitmap bitmap;                                                                      \
	bitmap.setInfo(SkImageInfo::Make(8, 8, kRGBA_8888_SkColorType, kOpaque_SkAlphaType)); \
	rgb_color pixels[8 * 8];                                                              \
	fState->fill_pattern(pixels, p);                                                      \
	bitmap.setPixels(pixels);                                                             \
	bitmap.setImmutable();                                                                \
	paint.setShader(                                                                      \
		bitmap.makeShader(SkTileMode::kRepeat, SkTileMode::kRepeat, SkSamplingOptions(), nullptr));

void BView::StrokePoint(BPoint pt, pattern p)
{
	DRAW_PRELUDE_WITH_PATTERN
	canvas->drawPoint(pt.x, pt.y, paint);
}

void BView::StrokeLine(BPoint toPt, pattern p)
{
	DRAW_PRELUDE_WITH_PATTERN
	canvas->drawLine(fState->pen_location.x(), fState->pen_location.y(), toPt.x, toPt.y, paint);
	fState->pen_location.set(toPt.x, toPt.y);
}

void BView::StrokeLine(BPoint pt0, BPoint pt1, pattern p)
{
	DRAW_PRELUDE_WITH_PATTERN
	canvas->drawLine(pt0.x, pt0.y, pt1.x, pt1.y, paint);
	fState->pen_location.set(pt1.x, pt1.y);
}

void BView::StrokePolygon(const BPolygon *aPolygon, bool closed, pattern p)
{
	if (!aPolygon) return;
	StrokePolygon(aPolygon->Points(), aPolygon->CountPoints(), closed, p);
}

void BView::StrokePolygon(const BPoint *ptArray, int32 numPts, bool closed, pattern p)
{
	if (!ptArray || numPts < 2) return;
	DRAW_PRELUDE_WITH_PATTERN
	paint.setStyle(SkPaint::Style::kStroke_Style);
	SkPath path;
	path.moveTo(ptArray->x, ptArray->y);
	ptArray += 1;
	for (int32 i = 1; i < numPts; ++i) {
		path.lineTo(ptArray->x, ptArray->y);
		ptArray += 1;
	}
	if (closed) path.close();
	canvas->drawPath(path, paint);
}

void BView::StrokePolygon(const BPoint *ptArray, int32 numPts, BRect bounds, bool closed, pattern p)
{
	debugger(__PRETTY_FUNCTION__);
}

void BView::FillPolygon(const BPolygon *aPolygon, pattern p)
{
	if (!aPolygon) return;
	FillPolygon(aPolygon->Points(), aPolygon->CountPoints(), p);
}

void BView::FillPolygon(const BPoint *ptArray, int32 numPts, pattern p)
{
	if (!ptArray || numPts < 2) return;
	DRAW_PRELUDE_WITH_PATTERN
	paint.setStyle(SkPaint::kFill_Style);
	SkPath path;
	path.moveTo(ptArray->x, ptArray->y);
	ptArray += 1;
	for (int32 i = 1; i < numPts; ++i) {
		path.lineTo(ptArray->x, ptArray->y);
		ptArray += 1;
	}
	canvas->drawPath(path, paint);
}

void BView::FillPolygon(const BPoint *ptArray, int32 numPts, BRect bounds, pattern p)
{
	debugger(__PRETTY_FUNCTION__);
}

void BView::StrokeRect(BRect r, pattern p)
{
	DRAW_PRELUDE_WITH_PATTERN
	paint.setStyle(SkPaint::kStroke_Style);
	canvas->drawRect(SkRect::MakeLTRB(r.left, r.top, r.right, r.bottom), paint);
}

void BView::FillRect(BRect r, pattern p)
{
	DRAW_PRELUDE_WITH_PATTERN
	paint.setStyle(SkPaint::kFill_Style);
	canvas->drawRect(SkRect::MakeLTRB(r.left, r.top, r.right, r.bottom), paint);
}

void BView::FillRegion(BRegion *a_region, pattern p)
{
	if (!a_region) return;
	DRAW_PRELUDE_WITH_PATTERN
	paint.setStyle(SkPaint::kFill_Style);
	canvas->drawRegion(*(a_region->_get_region()), paint);
}

void BView::InvertRect(BRect r)
{
	debugger(__PRETTY_FUNCTION__);
}

void BView::StrokeRoundRect(BRect r, float xRadius, float yRadius, pattern p)
{
	DRAW_PRELUDE_WITH_PATTERN
	paint.setStyle(SkPaint::kStroke_Style);
	canvas->drawRoundRect(SkRect::MakeLTRB(r.left, r.top, r.right, r.bottom), xRadius, yRadius, paint);
}

void BView::FillRoundRect(BRect r, float xRadius, float yRadius, pattern p)
{
	DRAW_PRELUDE_WITH_PATTERN
	paint.setStyle(SkPaint::kFill_Style);
	canvas->drawRoundRect(SkRect::MakeLTRB(r.left, r.top, r.right, r.bottom), xRadius, yRadius, paint);
}

void BView::StrokeEllipse(BPoint center, float xRadius, float yRadius, pattern p)
{
	debugger(__PRETTY_FUNCTION__);
}

void BView::StrokeEllipse(BRect r, pattern p)
{
	DRAW_PRELUDE_WITH_PATTERN
	paint.setStyle(SkPaint::kStroke_Style);
	canvas->drawOval(SkRect::MakeLTRB(r.left, r.top, r.right, r.bottom), paint);
}

void BView::FillEllipse(BPoint center, float xRadius, float yRadius, pattern p)
{
	debugger(__PRETTY_FUNCTION__);
}

void BView::FillEllipse(BRect r, pattern p)
{
	DRAW_PRELUDE_WITH_PATTERN
	paint.setStyle(SkPaint::kFill_Style);
	canvas->drawOval(SkRect::MakeLTRB(r.left, r.top, r.right, r.bottom), paint);
}

void BView::StrokeArc(BPoint center, float xRadius, float yRadius, float start_angle, float arc_angle, pattern p)
{
	debugger(__PRETTY_FUNCTION__);
}

void BView::StrokeArc(BRect r, float start_angle, float arc_angle, pattern p)
{
	DRAW_PRELUDE_WITH_PATTERN
	paint.setStyle(SkPaint::kStroke_Style);
	canvas->drawArc(SkRect::MakeLTRB(r.left, r.top, r.right, r.bottom), -start_angle, -arc_angle, false, paint);
}

void BView::FillArc(BPoint center, float xRadius, float yRadius, float start_angle, float arc_angle, pattern p)
{
	debugger(__PRETTY_FUNCTION__);
}

void BView::FillArc(BRect r, float start_angle, float arc_angle, pattern p)
{
	DRAW_PRELUDE_WITH_PATTERN
	paint.setStyle(SkPaint::kFill_Style);
	canvas->drawArc(SkRect::MakeLTRB(r.left, r.top, r.right, r.bottom), -start_angle, -arc_angle, true, paint);
}

void BView::DrawChar(char aChar)
{
	debugger(__PRETTY_FUNCTION__);
}

void BView::DrawChar(char aChar, BPoint location)
{
	debugger(__PRETTY_FUNCTION__);
}

void BView::DrawString(const char *aString, escapement_delta *delta)
{
	DrawString(aString, PenLocation(), delta);
}

void BView::DrawString(const char *aString, BPoint location, escapement_delta *delta)
{
	if (!aString) return;

	DrawString(aString, strlen(aString), location, delta);
}

void BView::DrawString(const char *aString, int32 length, escapement_delta *delta)
{
	DrawString(aString, length, PenLocation(), delta);
}

void BView::DrawString(const char *aString, int32 length, BPoint location, escapement_delta *delta)
{
	if (!aString || length <= 0) return;
	DRAW_PRELUDE_WITH_COLOR

	SkFont font;
	fState->font._get_font(&font);
	sk_sp<SkTextBlob> blob = SkTextBlob::MakeFromText(aString, length, font);
	canvas->drawTextBlob(blob, location.x, location.y, paint);

	MovePenTo(location.x + blob->bounds().width(), location.y);
}

#undef DRAW_PRELUDE

void BView::SetFont(const BFont *font, uint32 mask)
{
	if (!font) return;

	if (mask & B_FONT_FAMILY_AND_STYLE) {
		fState->font.fFamilyID = font->fFamilyID;
		fState->font.fStyleID  = font->fStyleID;
	}
	if (mask & B_FONT_SIZE) {
		fState->font.fSize = font->fSize;
	}
	if (mask & B_FONT_SHEAR) {
		fState->font.fShear = font->fShear;
	}
	if (mask & B_FONT_ROTATION) {
		fState->font.fRotation = font->fRotation;
	}
	if (mask & B_FONT_SPACING) {
		fState->font.fSpacing = font->fSpacing;
	}
	if (mask & B_FONT_ENCODING) {
		fState->font.fEncoding = font->fEncoding;
	}
	if (mask & B_FONT_FACE) {
		fState->font.fFace = font->fFace;
	}
	if (mask & B_FONT_FLAGS) {
		fState->font.fFlags = font->fFlags;
	}
}

void BView::GetFont(BFont *font) const
{
	if (!font) return;
	*font = fState->font;
}

void BView::TruncateString(BString *in_out, uint32 mode, float width) const
{
	debugger(__PRETTY_FUNCTION__);
}

float BView::StringWidth(const char *string) const
{
	debugger(__PRETTY_FUNCTION__);
	return 0.0;
}

float BView::StringWidth(const char *string, int32 length) const
{
	debugger(__PRETTY_FUNCTION__);
	return 0.0;
}

void BView::GetStringWidths(char *stringArray[], int32 lengthArray[], int32 numStrings, float widthArray[]) const
{
	debugger(__PRETTY_FUNCTION__);
}

void BView::SetFontSize(float size)
{
	fState->font.SetSize(size);
}

void BView::ForceFontAliasing(bool enable)
{
	debugger(__PRETTY_FUNCTION__);
}

void BView::GetFontHeight(font_height *height) const
{
	debugger(__PRETTY_FUNCTION__);
}

void BView::Invalidate(BRect invalRect)
{
	ALOGV("Invalidating %s", Name());
	if (fFlags & B_WILL_DRAW) {
		BMessage message(_UPDATE_);
		message.AddRect("updateRect", invalRect);
		if (Looper()) {
			Looper()->PostMessage(&message, this);
		}
		else {
			ALOGE("Tried invalidating not-attached View: '%s' %p", Name(), this);
		}
	}

	for (BView *child = fFirstChild; child; child = child->fNextSibling) {
		if (invalRect.Intersects(child->Frame())) {
			child->Invalidate(invalRect.OffsetByCopy(-child->fParentOffset) & child->Bounds());
		}
	}
}

void BView::Invalidate(const BRegion *invalRegion)
{
	debugger(__PRETTY_FUNCTION__);
}

void BView::Invalidate()
{
	Invalidate(Bounds());
}

void BView::SetFlags(uint32 flags)
{
	debugger(__PRETTY_FUNCTION__);
}

void BView::SetResizingMode(uint32 mode)
{
	debugger(__PRETTY_FUNCTION__);
}

void BView::ResizeBy(float dh, float dv)
{
	ResizeTo(fBounds.Width() + dh, fBounds.Height() + dv);
}

void BView::ResizeTo(float width, float height)
{
	LOG_FATAL_IF(width < 0, "width must be greater than zero");
	LOG_FATAL_IF(height < 0, "height must be greater than zero");

	fBounds.Set(fBounds.left, fBounds.top, floorf(fBounds.left + width), floorf(fBounds.top + height));
	if (fParent) fParent->Invalidate();
}

void BView::ScrollTo(BPoint where)
{
	debugger(__PRETTY_FUNCTION__);
}

void BView::MakeFocus(bool focusState)
{
	if (fOwner == NULL)
		return;

	// TODO: If this view has focus and focus == false,
	// will there really be no other view with focus? No
	// cycling to the next one?
	BView *focusView = fOwner->CurrentFocus();
	if (focusState) {
		// Unfocus a previous focus view
		if (focusView && focusView != this)
			focusView->MakeFocus(false);

		// if we want to make this view the current focus view
		fOwner->set_focus(this, true);
	}
	else {
		// we want to unfocus this view, but only if it actually has focus
		if (focusView == this)
			fOwner->set_focus(NULL, true);
	}
}

void BView::Show()
{
	debugger(__PRETTY_FUNCTION__);
}

void BView::Hide()
{
	debugger(__PRETTY_FUNCTION__);
}

void BView::GetPreferredSize(float *width, float *height)
{
	debugger(__PRETTY_FUNCTION__);
}

void BView::ResizeToPreferred()
{
	debugger(__PRETTY_FUNCTION__);
}

BHandler *BView::ResolveSpecifier(BMessage *msg, int32 index, BMessage *specifier, int32 form, const char *property)
{
	debugger(__PRETTY_FUNCTION__);
	return nullptr;
}

status_t BView::GetSupportedSuites(BMessage *data)
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

void BView::_attach(BWindow *window)
{
	LOG_FATAL_IF(!window, "Tried attaching '%s' to NULL window", Name());

	if (fOwner) {
		ALOGE("View '%s' already has an owner %p: '%s'", Name(), fOwner, fOwner->Name());
		debugger("View already has an owner");
	}

	fOwner		  = window;
	fOwner->AddHandler(this);
	fTopLevelView = true;

	AttachedToWindow();

	if (!fOwner->IsHidden())
		Invalidate();

	for (BView *child = fFirstChild; child; child = child->fNextSibling) {
		// we need to check for fAttached as new views could have been
		// added in AttachedToWindow() - and those are already attached
		if (!child->fOwner)
			child->_attach(window);
	}

	AllAttached();
}

void BView::_detach()
{
	if (!fOwner) {
		ALOGE("Cannot detach not attached view '%s'", Name());
		debugger("Cannot detach not attached view");
	}

	DetachedFromWindow();

	for (BView *child = fFirstChild; child; child = child->fNextSibling) {
		child->_detach();
	}

	AllDetached();

	if (!fOwner->IsHidden())
		Invalidate();

	// make sure our owner doesn't need us anymore
	{
		if (fOwner->CurrentFocus() == this) {
			MakeFocus(false);
			// MakeFocus() is virtual and might not be
			// passing through to the BView version,
			// but we need to make sure at this point
			// that we are not the focus view anymore.
			// FIXME:
			// if (fOwner->CurrentFocus() == this)
			// 	fOwner->_SetFocus(NULL, true);
		}

		// if (fOwner->fDefaultButton == this)
		// 	fOwner->SetDefaultButton(NULL);

		// if (fOwner->fKeyMenuBar == this)
		// 	fOwner->fKeyMenuBar = NULL;
	}

	if (fOwner->fLastMouseMovedView == this)
		fOwner->fLastMouseMovedView = NULL;

	fOwner->RemoveHandler(this);
	fOwner = nullptr;
}

TEST_SUITE("BView")
{
	TEST_CASE("Children")
	{
		BView main(BRect(0, 0, 100, 100), "main", 0, 0);
		BView child1(BRect(0, 0, 20, 20), "child1", 0, 0);
		BView child11(BRect(0, 0, 10, 10), "child11", 0, 0);
		BView child2(BRect(50, 50, 70, 70), "child2", 0, 0);
		BView child21(BRect(10, 10, 100, 100), "child21", 0, 0);

		main.AddChild(&child1);
		child1.AddChild(&child11);
		child2.AddChild(&child21);
		main.AddChild(&child2, &child1);

		SUBCASE("Invalidate")
		{
			main.Invalidate(BRect(10, 10, 80, 60));
		}

		CHECK(main.RemoveChild(&child2));
		CHECK_FALSE(main.RemoveChild(&child21));
		CHECK(child2.RemoveChild(&child21));
		CHECK(child1.RemoveChild(&child11));
		CHECK(main.RemoveChild(&child1));
		CHECK_FALSE(main.RemoveChild(&child1));
	}
}
