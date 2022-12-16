#include "View.h"

#define LOG_TAG "BView"

#include <Font.h>
#include <GraphicsDefs.h>
#include <Message.h>
#include <MessageQueue.h>
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

#include <stack>

class BView::impl
{
   public:
	struct State
	{
		int checkpoint;

		rgb_color view_color;
		rgb_color high_color;
		rgb_color low_color;

		SkPoint origin;

		drawing_mode drawing_mode;
		float		 pen_size;
		SkPoint		 pen_location;

		BFont font;
		/// causes subsequent printing to be done without antialiasing printed characters
		/// does not affect characters or strings drawn to the screen
		bool font_aliasing;
	};

	impl()
	{
		state_stack.push({.checkpoint	 = 0,
						  .view_color	 = {255, 255, 255, 255},
						  .high_color	 = {0, 0, 0, 255},
						  .low_color	 = {255, 255, 255, 255},
						  .origin		 = {0.0, 0.0},
						  .drawing_mode	 = B_OP_COPY,
						  .pen_size		 = 1.0,
						  .pen_location	 = {0.0, 0.0},
						  .font			 = be_plain_font,
						  .font_aliasing = false});
	}

	inline State &S() { return state_stack.top(); }

	void push_state(int checkpoint);
	int	 pop_state();
	bool state_stack_empty();
	void clear_state_stack();

	void fill_pattern(rgb_color pixels[8 * 8], pattern &p);

   private:
	std::stack<State> state_stack;
};

void BView::impl::push_state(int checkpoint)
{
	State new_state		 = state_stack.top();
	new_state.checkpoint = checkpoint;
	state_stack.push(new_state);
}

int BView::impl::pop_state()
{
	// There has to be at least one!
	if (state_stack.size() > 1) {
		int checkpoint = state_stack.top().checkpoint;
		state_stack.pop();
		return checkpoint;
	}

	ALOGE("Cannot pop from empty state stack");
	return -1;
}

bool BView::impl::state_stack_empty()
{
	return state_stack.size() <= 1;
}

void BView::impl::clear_state_stack()
{
	while (state_stack.size() > 1) state_stack.pop();
}

void BView::impl::fill_pattern(rgb_color pixels[8 * 8], pattern &p)
{
	const rgb_color low = (S().drawing_mode == B_OP_OVER
						   || S().drawing_mode == B_OP_ERASE
						   || S().drawing_mode == B_OP_INVERT
						   || S().drawing_mode == B_OP_SELECT)
							  ? B_TRANSPARENT_COLOR
							  : S().low_color;

	for (int row = 0; row < 8; ++row) {
		if (p.data[row] & 0b10000000)
			pixels[0 + row * 8] = S().high_color;
		else
			pixels[0 + row * 8] = low;
		if (p.data[row] & 0b01000000)
			pixels[1 + row * 8] = S().high_color;
		else
			pixels[1 + row * 8] = low;
		if (p.data[row] & 0b00100000)
			pixels[2 + row * 8] = S().high_color;
		else
			pixels[2 + row * 8] = low;
		if (p.data[row] & 0b00010000)
			pixels[3 + row * 8] = S().high_color;
		else
			pixels[3 + row * 8] = low;
		if (p.data[row] & 0b00001000)
			pixels[4 + row * 8] = S().high_color;
		else
			pixels[4 + row * 8] = low;
		if (p.data[row] & 0b00000100)
			pixels[5 + row * 8] = S().high_color;
		else
			pixels[5 + row * 8] = low;
		if (p.data[row] & 0b00000010)
			pixels[6 + row * 8] = S().high_color;
		else
			pixels[6 + row * 8] = low;
		if (p.data[row] & 0b00000001)
			pixels[7 + row * 8] = S().high_color;
		else
			pixels[7 + row * 8] = low;
	}
}

#pragma mark - BView

BView::BView(BRect frame, const char *name, uint32 resizeMask, uint32 flags)
	: BHandler(name),
	  fFlags{0},  // updated in constructor
	  fOwner{nullptr},
	  fParent{nullptr},
	  fNextSibling{nullptr},
	  fPrevSibling{nullptr},
	  fFirstChild{nullptr},
	  fShowLevel{0},
	  fEventMask{0},
	  fEventOptions{0},
	  fMouseEventMask{0},
	  fMouseEventOptions{0}
{
	LOG_ALWAYS_FATAL_IF(((resizeMask & ~_RESIZE_MASK_) || (flags & _RESIZE_MASK_)),
						"%s: resizing mode or flags swapped", name);
	SetFlags((resizeMask & _RESIZE_MASK_) | (flags & ~_RESIZE_MASK_));

	frame.left	 = floorf(frame.left);
	frame.top	 = floorf(frame.top);
	frame.right	 = floorf(frame.right);
	frame.bottom = floorf(frame.bottom);
	fParentOffset.Set(frame.left, frame.top);
	fBounds = frame.OffsetToCopy(B_ORIGIN);
}

BView::~BView() {}

BView::BView(BMessage *data)
{
	debugger(__PRETTY_FUNCTION__);
}

BArchivable *BView::Instantiate(BMessage *data)
{
	debugger(__PRETTY_FUNCTION__);
	return nullptr;
}

#define _CheckLock                                                        \
	if (fOwner && !fOwner->IsLocked()) {                                  \
		ALOGW("%s: Window '%s' should be locked when updating View '%s'", \
			  __PRETTY_FUNCTION__, fOwner->Name(), Name());               \
	}

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
	ALOGV_IF(message->what != B_MOUSE_MOVED, "MessageReceived @%s 0x%x: %.4s", Name(), message->what, (char *)&message->what);
	if (!message->HasSpecifiers()) {
		switch (message->what) {
			case _UPDATE_: {
				BRect updateRect;
				if (message->FindRect("updateRect", &updateRect) == B_OK && updateRect.IsValid()) {
					// combine with pending updates
					BMessage *pendingMessage;
					int32	  index = 0;
					while ((pendingMessage = Looper()->MessageQueue()->FindMessage(_UPDATE_, index))) {
						if (pendingMessage->_get_handler() == this) {
							Looper()->MessageQueue()->RemoveMessage(pendingMessage);

							BRect pendingRect;
							if (message->FindRect("updateRect", &pendingRect) == B_OK && pendingRect.IsValid()) {
								updateRect = updateRect | pendingRect;
							}

							delete pendingMessage;
						}
						else {
							// move to next
							index += 1;
						}
					}

					SkCanvas *canvas = static_cast<SkCanvas *>(fOwner->_get_canvas());
					if (!canvas) {
						// FIXME: now what? ¯\_(ツ)_/¯
						return;
					}

					std::function<void(BView *, BRect)> do_draw_view = [&](BView *view, BRect invalRect) {
						int checkpoint = canvas->save();

						auto bounds = view->Bounds();
						view->ConvertToScreen(&bounds);
						canvas->clipRect(SkRect::MakeLTRB(bounds.left, bounds.top, bounds.right + 1, bounds.bottom + 1));

						if (view->fFlags & B_WILL_DRAW) {
							// The Application Server paints the view with ViewColor before any view-specific drawing functions are called.
							const rgb_color view_color(view->ViewColor());
							// If you set the view color to B_TRANSPARENT_COLOR, the Application Server won't erase the view's clipping region before an update.
							if (view_color != B_TRANSPARENT_COLOR)
								canvas->clear(SkColorSetARGB(view_color.alpha, view_color.red, view_color.green, view_color.blue));

							// Call hook function to Draw content
							view->Draw(invalRect);
						}

						// draw children
						for (BView *child = view->fFirstChild; child; child = child->fNextSibling) {
							if (invalRect.Intersects(child->Frame())) {
								do_draw_view(child, invalRect.OffsetByCopy(-child->fParentOffset) & child->Bounds());
							}
						}

						if (view->fFlags & B_WILL_DRAW) {
							// Call hook function to Draw content after children
							view->DrawAfterChildren(invalRect);
						}

						if (!view->fState->state_stack_empty()) {
							ALOGE("View '%s' state stack is not empty after Draw(). Did you forget PopState() after PushState()?", view->Name());
							view->fState->clear_state_stack();
						}
						canvas->restoreToCount(checkpoint);
#ifndef NDEBUG
						// Draw green rectangles around views
						SkPaint paint;
						paint.setARGB(48, 0, 200, 0);
						paint.setStyle(SkPaint::kStroke_Style);
						canvas->drawRect(SkRect::MakeLTRB(bounds.left, bounds.top, bounds.right, bounds.bottom), paint);
#endif
					};

					canvas->restoreToCount(0);
					canvas->save();
					canvas->resetMatrix();
					// By default, the clipping region contains only the visible area of the view
					// and, during an update, only the area that actually needs to be drawn.
					BRect  bounds{ConvertToScreen(updateRect)};
					SkRect clipRect{SkRect::MakeLTRB(bounds.left, bounds.top, bounds.right + 1, bounds.bottom + 1)};
					canvas->clipRect(clipRect);

					do_draw_view(this, updateRect);

					canvas->restoreToCount(0);

					fOwner->_damage_window(clipRect.left(), clipRect.top(), clipRect.width(), clipRect.height());
				}
				break;
			}

			case B_KEY_DOWN: {
				// TODO: cannot use "string" here if we support having different
				// font encoding per view (it's supposed to be converted by
				// BWindow::_HandleKeyDown() one day)
				const char *string;
				ssize_t		bytes;
				if (message->FindData("bytes", B_STRING_TYPE, (const void **)&string, &bytes) == B_OK)
					KeyDown(string, bytes - 1);
				break;
			}

			case B_KEY_UP: {
				// TODO: same as above
				const char *string;
				ssize_t		bytes;
				if (message->FindData("bytes", B_STRING_TYPE, (const void **)&string, &bytes) == B_OK)
					KeyUp(string, bytes - 1);
				break;
			}

			case B_VIEW_MOVED:
				FrameMoved(fParentOffset);
				break;

			case B_MOUSE_MOVED: {
				BPoint where;
				uint32 transit = 0;
				if (message->FindPoint("be:view_where", &where) == B_OK
					&& message->FindUInt32("be:transit", &transit) == B_OK) {
					BMessage *dragMessage = new BMessage();
					if (message->FindMessage("be:drag_message", dragMessage) != B_OK) {
						delete dragMessage;
						dragMessage = nullptr;
					}

					MouseMoved(where, transit, dragMessage);
					delete dragMessage;
				}
				break;
			}

			case B_MOUSE_DOWN: {
				BPoint where;
				if (message->FindPoint("be:view_where", &where) == B_OK)
					MouseDown(where);
				break;
			}

			case B_MOUSE_UP: {
				BPoint where;
				if (message->FindPoint("be:view_where", &where) == B_OK)
					MouseUp(where);
				fMouseEventMask	   = 0;
				fMouseEventOptions = 0;
				break;
			}

			case B_MOUSE_WHEEL_CHANGED: {
				// BScrollBar *horizontal = ScrollBar(B_HORIZONTAL);
				// BScrollBar *vertical   = ScrollBar(B_VERTICAL);
				// if (horizontal == NULL && vertical == NULL) {
				// 	// Pass the message to the next handler
				// 	BHandler::MessageReceived(message);
				// 	break;
				// }

				// float deltaX = 0.0f;
				// float deltaY = 0.0f;

				// if (horizontal != NULL)
				// 	message->FindFloat("be:wheel_delta_x", &deltaX);

				// if (vertical != NULL)
				// 	message->FindFloat("be:wheel_delta_y", &deltaY);

				// if (deltaX == 0.0f && deltaY == 0.0f)
				// 	break;

				// if ((modifiers() & B_CONTROL_KEY) != 0)
				// 	std::swap(horizontal, vertical);

				// if (horizontal != NULL && deltaX != 0.0f)
				// 	ScrollWithMouseWheelDelta(horizontal, deltaX);

				// if (vertical != NULL && deltaY != 0.0f)
				// 	ScrollWithMouseWheelDelta(vertical, deltaY);

				break;
			}

			case B_SCREEN_CHANGED:
			case B_WORKSPACE_ACTIVATED:
			case B_WORKSPACES_CHANGED: {
				BWindow *window = Window();
				if (window == NULL)
					break;

				// propagate message to child views
				int32 childCount = CountChildren();
				for (int32 i = 0; i < childCount; i++) {
					BView *view = ChildAt(i);
					if (view != NULL)
						window->PostMessage(message, view);
				}
				break;
			}

			default:
				BHandler::MessageReceived(message);
		}
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
	_CheckLock;

	uint32 count = 0;
	BView *child = fFirstChild;

	while (child) {
		count++;
		child = child->fNextSibling;
	}

	return count;
}

BView *BView::ChildAt(int32 index) const
{
	_CheckLock;

	BView *child = fFirstChild;
	while (child && index > 0) {
		child = child->fNextSibling;
		index -= 1;
	}

	return child;
}

BView *BView::NextSibling() const
{
	return fNextSibling;
}

BView *BView::PreviousSibling() const
{
	return fPrevSibling;
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

void BView::DrawAfterChildren(BRect updateRect)
{
	// Hook - default implementation does nothing
}

void BView::MouseDown(BPoint where)
{
	// Hook - default implementation does nothing
}

void BView::MouseUp(BPoint where)
{
	// Hook - default implementation does nothing
}

void BView::MouseMoved(BPoint where, uint32 transit, const BMessage *dnd)
{
	// Hook - default implementation does nothing
}

void BView::WindowActivated(bool state)
{
	// Hook - default implementation does nothing
}

void BView::KeyDown(const char *bytes, int32 numBytes)
{
	// Hook - default implementation does nothing
}

void BView::KeyUp(const char *bytes, int32 numBytes)
{
	// Hook - default implementation does nothing
}

void BView::Pulse()
{
	// Hook - default implementation does nothing
}

void BView::FrameMoved(BPoint new_position)
{
	// Hook - default implementation does nothing
}

void BView::FrameResized(float new_width, float new_height)
{
	// Hook - default implementation does nothing
}

void BView::TargetedByScrollView(BScrollView *scroll_view)
{
	debugger(__PRETTY_FUNCTION__);
}

void BView::GetMouse(BPoint *location, uint32 *buttons, bool checkMessageQueue)
{
	// uint32 eventOptions = fEventOptions | fMouseEventOptions;
	// bool   noHistory	= eventOptions & B_NO_POINTER_HISTORY;
	// bool   fullHistory	= eventOptions & B_FULL_POINTER_HISTORY;
	if (checkMessageQueue /*&& !noHistory*/) {
		// comb through window message queue for mouse move/button messages
		debugger(__PRETTY_FUNCTION__);
	}

	// if not found read BWindow::impl cached ones
	if (location) {
		debugger(__PRETTY_FUNCTION__);
	}
	if (buttons) {
		debugger(__PRETTY_FUNCTION__);
	}
	if (checkMessageQueue) {
	}
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

void BView::ConvertToScreen(BPoint *pt) const
{
	if (!pt) return;

	ConvertToParent(pt);

	if (fParent)
		fParent->ConvertToScreen(pt);
	else if (fOwner)
		fOwner->ConvertToScreen(pt);
}

BPoint BView::ConvertToScreen(BPoint pt) const
{
	ConvertToScreen(&pt);
	return pt;
}

void BView::ConvertFromScreen(BPoint *pt) const
{
	if (!pt) return;

	ConvertFromParent(pt);

	if (fParent)
		fParent->ConvertFromScreen(pt);
	else if (fOwner)
		fOwner->ConvertFromScreen(pt);
}

BPoint BView::ConvertFromScreen(BPoint pt) const
{
	ConvertFromScreen(&pt);
	return pt;
}

void BView::ConvertToScreen(BRect *r) const
{
	if (!r) return;

	BPoint lt(r->LeftTop());
	ConvertToScreen(&lt);
	r->OffsetTo(lt);
}

BRect BView::ConvertToScreen(BRect r) const
{
	ConvertToScreen(&r);
	return r;
}

void BView::ConvertFromScreen(BRect *r) const
{
	if (!r) return;

	BPoint lt(r->LeftTop());
	ConvertFromScreen(&lt);
	r->OffsetTo(lt);
}

BRect BView::ConvertFromScreen(BRect r) const
{
	ConvertFromScreen(&r);
	return r;
}

void BView::ConvertToParent(BPoint *pt) const
{
	if (!pt) return;
	*pt += fParentOffset;
}

BPoint BView::ConvertToParent(BPoint pt) const
{
	ConvertToParent(&pt);
	return pt;
}

void BView::ConvertFromParent(BPoint *pt) const
{
	if (!pt) return;
	*pt -= fParentOffset;
}

BPoint BView::ConvertFromParent(BPoint pt) const
{
	ConvertFromParent(&pt);
	return pt;
}

void BView::ConvertToParent(BRect *r) const
{
	if (!r) return;
	r->OffsetBy(fParentOffset);
}

BRect BView::ConvertToParent(BRect r) const
{
	ConvertToParent(&r);
	return r;
}

void BView::ConvertFromParent(BRect *r) const
{
	if (!r) return;
	r->OffsetBy(-fParentOffset);
}

BRect BView::ConvertFromParent(BRect r) const
{
	ConvertFromParent(&r);
	return r;
}

BPoint BView::LeftTop() const
{
	return fBounds.LeftTop();
}

void BView::ConstrainClippingRegion(BRegion *region)
{
	SkCanvas *canvas = static_cast<SkCanvas *>(fOwner->_get_canvas());
	if (!canvas) {
		return;
	}
	if (region)
		canvas->clipRegion(*region->_get_region());
	else
		canvas->clipRect(SkRect::MakeEmpty(), SkClipOp::kDifference);
}

void BView::SetDrawingMode(drawing_mode mode)
{
	fState->S().drawing_mode = mode;
}

void BView::SetPenSize(float size)
{
	if (size < 0.0) return;

	fState->S().pen_size = size;
}

float BView::PenSize() const
{
	return fState->S().pen_size;
}

void BView::SetViewColor(rgb_color color)
{
	fState->S().view_color = color;
}

rgb_color BView::ViewColor() const
{
	return fState->S().view_color;
}

void BView::SetHighColor(rgb_color color)
{
	fState->S().high_color = color;
}

rgb_color BView::HighColor() const
{
	return fState->S().high_color;
}

void BView::SetLowColor(rgb_color color)
{
	fState->S().low_color = color;
}

rgb_color BView::LowColor() const
{
	return fState->S().low_color;
}

void BView::SetOrigin(BPoint pt)
{
	SetOrigin(pt.x, pt.y);
}

void BView::SetOrigin(float x, float y)
{
	fState->S().origin.set(x, y);
}

BPoint BView::Origin() const
{
	return BPoint(fState->S().origin.x(), fState->S().origin.y());
}

void BView::PushState()
{
	if (!fOwner) return;
	SkCanvas *canvas = static_cast<SkCanvas *>(fOwner->_get_canvas());
	if (!canvas) return;

	int checkpoint = canvas->save();
	fState->push_state(checkpoint);

	// When a state is saved to the stack, a new state context is created,
	// with a local scale of zero, a local origin at (0,0), and no clipping region.
	canvas->scale(1.0, 1.0);
	fState->S().origin.set(0.0, 0.0);
	// TODO: clear clipping
}

void BView::PopState()
{
	if (!fOwner) return;
	SkCanvas *canvas = static_cast<SkCanvas *>(fOwner->_get_canvas());
	if (!canvas) return;

	int checkpoint = fState->pop_state();
	if (checkpoint > 0)
		canvas->restoreToCount(checkpoint);
}

void BView::MovePenTo(BPoint pt)
{
	MovePenTo(pt.x, pt.y);
}

void BView::MovePenTo(float x, float y)
{
	fState->S().pen_location.set(x, y);
}

void BView::MovePenBy(float x, float y)
{
	fState->S().pen_location.offset(x, y);
}

BPoint BView::PenLocation() const
{
	return BPoint(fState->S().pen_location.x(), fState->S().pen_location.y());
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

#define DRAW_PRELUDE                                                                             \
	if (!fOwner) return;                                                                         \
	SkCanvas *canvas = fOwner->_get_canvas();                                                    \
	if (!canvas) return;                                                                         \
                                                                                                 \
	BPoint translation{LeftTop()};                                                               \
	ConvertToScreen(&translation);                                                               \
	canvas->setMatrix(SkMatrix::Translate(translation.x, translation.y));                        \
                                                                                                 \
	SkPaint paint;                                                                               \
	paint.setStrokeWidth(fState->S().pen_size > 0.0 ? /* scale * */ fState->S().pen_size : 1.0); \
	paint.setStrokeCap(SkPaint::kSquare_Cap);                                                    \
                                                                                                 \
	if (fState->S().drawing_mode < (sizeof(blend_modes) / sizeof(blend_modes[0])))               \
		paint.setBlendMode(blend_modes[fState->S().drawing_mode]);

#define DRAW_PRELUDE_WITH_COLOR       \
	DRAW_PRELUDE                      \
	paint.setColor(SkColorSetARGB(    \
		fState->S().high_color.alpha, \
		fState->S().high_color.red,   \
		fState->S().high_color.green, \
		fState->S().high_color.blue));

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

#define DAMAGE_RECT                    \
	ConvertToScreen(&r);               \
	r.InsetBy(-PenSize(), -PenSize()); \
	fOwner->_damage_window(r.left, r.top, r.Width() + 1, r.Height() + 1);

void BView::StrokePoint(BPoint pt, pattern p)
{
	DRAW_PRELUDE_WITH_PATTERN
	canvas->drawPoint(pt.x, pt.y, paint);
	BRect r(pt.x, pt.y, pt.x, pt.y);
	DAMAGE_RECT
}

void BView::StrokeLine(BPoint toPt, pattern p)
{
	DRAW_PRELUDE_WITH_PATTERN
	canvas->drawLine(
		fState->S().pen_location.x(), fState->S().pen_location.y(),
		toPt.x, toPt.y,
		paint);
	BRect r(fState->S().pen_location.x(), fState->S().pen_location.y(), toPt.x, toPt.y);
	DAMAGE_RECT
	fState->S().pen_location.set(toPt.x, toPt.y);
}

void BView::StrokeLine(BPoint fromPt, BPoint toPt, pattern p)
{
	DRAW_PRELUDE_WITH_PATTERN
	canvas->drawLine(fromPt.x, fromPt.y, toPt.x, toPt.y, paint);
	BRect r(fromPt.x, fromPt.y, toPt.x, toPt.y);
	DAMAGE_RECT
	fState->S().pen_location.set(toPt.x, toPt.y);
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
	BRect r(ptArray->x, ptArray->y, ptArray->x, ptArray->y);
	paint.setStyle(SkPaint::Style::kStroke_Style);
	SkPath path;
	path.moveTo(ptArray->x, ptArray->y);
	ptArray += 1;
	for (int32 i = 1; i < numPts; ++i) {
		path.lineTo(ptArray->x, ptArray->y);
		if (ptArray->x < r.left) r.left = ptArray->x;
		if (ptArray->y < r.top) r.top = ptArray->y;
		if (ptArray->x > r.right) r.right = ptArray->x;
		if (ptArray->y < r.bottom) r.bottom = ptArray->y;
		ptArray += 1;
	}
	if (closed) path.close();
	canvas->drawPath(path, paint);
	DAMAGE_RECT
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
	BRect r(ptArray->x, ptArray->y, ptArray->x, ptArray->y);
	paint.setStyle(SkPaint::kStrokeAndFill_Style);
	SkPath path;
	path.moveTo(ptArray->x, ptArray->y);
	ptArray += 1;
	for (int32 i = 1; i < numPts; ++i) {
		path.lineTo(ptArray->x, ptArray->y);
		if (ptArray->x < r.left) r.left = ptArray->x;
		if (ptArray->y < r.top) r.top = ptArray->y;
		if (ptArray->x > r.right) r.right = ptArray->x;
		if (ptArray->y < r.bottom) r.bottom = ptArray->y;
		ptArray += 1;
	}
	canvas->drawPath(path, paint);
	DAMAGE_RECT
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
	DAMAGE_RECT
}

void BView::FillRect(BRect r, pattern p)
{
	DRAW_PRELUDE_WITH_PATTERN
	paint.setStyle(SkPaint::kStrokeAndFill_Style);
	canvas->drawRect(SkRect::MakeLTRB(r.left, r.top, r.right, r.bottom), paint);
	DAMAGE_RECT
}

void BView::FillRegion(BRegion *a_region, pattern p)
{
	if (!a_region) return;
	DRAW_PRELUDE_WITH_PATTERN
	paint.setStyle(SkPaint::kStrokeAndFill_Style);
	canvas->drawRegion(*(a_region->_get_region()), paint);
	BRect r(a_region->Frame());
	DAMAGE_RECT
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
	DAMAGE_RECT
}

void BView::FillRoundRect(BRect r, float xRadius, float yRadius, pattern p)
{
	DRAW_PRELUDE_WITH_PATTERN
	paint.setStyle(SkPaint::kStrokeAndFill_Style);
	canvas->drawRoundRect(SkRect::MakeLTRB(r.left, r.top, r.right, r.bottom), xRadius, yRadius, paint);
	DAMAGE_RECT
}

void BView::StrokeEllipse(BPoint center, float xRadius, float yRadius, pattern p)
{
	debugger(__PRETTY_FUNCTION__);
}

void BView::StrokeEllipse(BRect r, pattern p)
{
	DRAW_PRELUDE_WITH_PATTERN
	paint.setStyle(SkPaint::kStroke_Style);
	canvas->drawOval(SkRect::MakeLTRB(r.left + 0.3, r.top + 0.3,
									  r.right + 0.7, r.bottom + 0.7),
					 paint);
	DAMAGE_RECT
}

void BView::FillEllipse(BPoint center, float xRadius, float yRadius, pattern p)
{
	debugger(__PRETTY_FUNCTION__);
}

void BView::FillEllipse(BRect r, pattern p)
{
	DRAW_PRELUDE_WITH_PATTERN
	paint.setStyle(SkPaint::kStrokeAndFill_Style);
	canvas->drawOval(SkRect::MakeLTRB(r.left + 0.3, r.top + 0.3,
									  r.right + 0.7, r.bottom + 0.7),
					 paint);
	DAMAGE_RECT
}

void BView::StrokeArc(BPoint center, float xRadius, float yRadius, float start_angle, float arc_angle, pattern p)
{
	debugger(__PRETTY_FUNCTION__);
}

void BView::StrokeArc(BRect r, float start_angle, float arc_angle, pattern p)
{
	DRAW_PRELUDE_WITH_PATTERN
	paint.setStyle(SkPaint::kStroke_Style);
	canvas->drawArc(SkRect::MakeLTRB(r.left + 0.3, r.top + 0.3,
									 r.right + 0.7, r.bottom + 0.7),
					-start_angle, -arc_angle, false, paint);
	DAMAGE_RECT
}

void BView::FillArc(BPoint center, float xRadius, float yRadius, float start_angle, float arc_angle, pattern p)
{
	debugger(__PRETTY_FUNCTION__);
}

void BView::FillArc(BRect r, float start_angle, float arc_angle, pattern p)
{
	DRAW_PRELUDE_WITH_PATTERN
	paint.setStyle(SkPaint::kStrokeAndFill_Style);
	canvas->drawArc(SkRect::MakeLTRB(r.left + 0.3, r.top + 0.3,
									 r.right + 0.7, r.bottom + 0.7),
					-start_angle, -arc_angle, true, paint);
	DAMAGE_RECT
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

	sk_sp<SkTextBlob> blob = SkTextBlob::MakeFromText(aString, length, fState->S().font._get_font());
	canvas->drawTextBlob(blob, location.x, location.y, paint);

	auto  bounds = blob->bounds();
	BRect r(bounds.left(), bounds.top(), bounds.right(), bounds.bottom());
	r.OffsetBy(location);
	DAMAGE_RECT
	MovePenTo(location.x + bounds.width(), location.y);
}

#undef DAMAGE_RECT
#undef DRAW_PRELUDE_WITH_PATTERN
#undef DRAW_PRELUDE_WITH_COLOR
#undef DRAW_PRELUDE

void BView::SetFont(const BFont *font, uint32 mask)
{
	if (!font) return;

	if (mask & B_FONT_ALL) {
		fState->S().font = *font;
		return;
	}

	if (mask & B_FONT_FAMILY_AND_STYLE) {
		fState->S().font.SetFamilyAndStyle(font->FamilyAndStyle());
	}
	if (mask & B_FONT_SIZE) {
		fState->S().font.SetSize(font->Size());
	}
	if (mask & B_FONT_SHEAR) {
		fState->S().font.SetShear(font->Shear());
	}
	if (mask & B_FONT_ROTATION) {
		fState->S().font.SetRotation(font->Rotation());
	}
	if (mask & B_FONT_SPACING) {
		fState->S().font.SetSpacing(font->Spacing());
	}
	if (mask & B_FONT_ENCODING) {
		fState->S().font.SetEncoding(font->Encoding());
	}
	if (mask & B_FONT_FACE) {
		fState->S().font.SetFace(font->Face());
	}
	if (mask & B_FONT_FLAGS) {
		fState->S().font.SetFlags(font->Flags());
	}
}

void BView::GetFont(BFont *font) const
{
	if (!font) return;
	*font = fState->S().font;
}

void BView::TruncateString(BString *in_out, uint32 mode, float width) const
{
	debugger(__PRETTY_FUNCTION__);
}

float BView::StringWidth(const char *string) const
{
	if (!string) return 0.0f;
	return StringWidth(string, strlen(string));
}

float BView::StringWidth(const char *string, int32 length) const
{
	if (!string || length <= 0) return 0.0f;
	return SkTextBlob::MakeFromText(string, length, fState->S().font._get_font())->bounds().width();
}

void BView::GetStringWidths(char *stringArray[], int32 lengthArray[], int32 numStrings, float widthArray[]) const
{
	debugger(__PRETTY_FUNCTION__);
}

void BView::SetFontSize(float size)
{
	fState->S().font.SetSize(size);
}

void BView::ForceFontAliasing(bool enable)
{
	fState->S().font_aliasing = enable;
}

void BView::GetFontHeight(font_height *height) const
{
	fState->S().font.GetHeight(height);
}

void BView::Invalidate(BRect invalRect)
{
	ALOGV("Invalidating %p %s", this, Name());
	if (fFlags & B_WILL_DRAW) {
		BMessage message(_UPDATE_);
		message.AddRect("updateRect", invalRect);
		if (Looper()) {
			Looper()->PostMessage(&message, this);
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

status_t BView::SetEventMask(uint32 mask, uint32 options)
{
	if (fEventMask == mask && fEventOptions == options)
		return B_OK;

	// don't change the mask if it's zero and we've got options
	if (mask != 0 || options == 0)
		fEventMask = mask | (fEventMask & 0xFFFF0000);
	fEventOptions = options;

	if (fOwner) {
		fOwner->fViewsEvents |= mask;
	}

	return B_OK;
}

uint32 BView::EventMask()
{
	return fEventMask | fMouseEventMask;
}

status_t BView::SetMouseEventMask(uint32 mask, uint32 options)
{
	// Just don't do anything if the view is not yet attached
	// or we were called outside of BView::MouseDown()
	if (fOwner && fOwner->CurrentMessage()
		&& fOwner->CurrentMessage()->what == B_MOUSE_DOWN) {
		fMouseEventMask	   = mask;
		fMouseEventOptions = options;

		fOwner->fViewsEvents |= mask;

		return B_OK;
	}

	return B_ERROR;
}

void BView::SetFlags(uint32 flags)
{
	if (Flags() == flags)
		return;

	/* Some useful info:
	 * fFlags is a unsigned long (32 bits)
	 * bits 1-16 are used for BView's flags
	 * bits 17-32 are used for BView' resize mask
	 * _RESIZE_MASK_ is used for that. Look into View.h to see how it is defined
	 */
	uint32 changes_flags = flags ^ fFlags;
	fFlags				 = (flags & ~_RESIZE_MASK_) | (fFlags & _RESIZE_MASK_);

	if (fOwner) {
		_CheckLock;
		if (changes_flags & B_PULSE_NEEDED) {
			fOwner->fViewsEvents |= B_PULSE_NEEDED;
			fOwner->_try_pulse();
		}
	}

	// fState->archiving_flags |= B_VIEW_FLAGS_BIT;
}

uint32 BView::Flags() const
{
	return fFlags & ~_RESIZE_MASK_;
}

void BView::SetResizingMode(uint32 mode)
{
	debugger(__PRETTY_FUNCTION__);
}

void BView::MoveBy(float dh, float dv)
{
	MoveTo(fBounds.left + dh, fBounds.top + dv);
}

void BView::MoveTo(BPoint where)
{
	MoveTo(where.x, where.y);
}

void BView::MoveTo(float x, float y)
{
	fParentOffset.x = floorf(x);
	fParentOffset.y = floorf(y);
	if (fParent) fParent->Invalidate();
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

bool BView::IsFocus() const
{
	return fOwner && fOwner->CurrentFocus() == this;
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
	if (width)
		*width = fBounds.Width();
	if (height)
		*height = fBounds.Height();
}

void BView::ResizeToPreferred()
{
	float width;
	float height;
	GetPreferredSize(&width, &height);

	ResizeTo(width, height);
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
	fOwner->fViewsEvents |= fFlags & B_PULSE_NEEDED;

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
			// 	fOwner->_SetFocus(nullptr, true);
		}

		// if (fOwner->fDefaultButton == this)
		// 	fOwner->SetDefaultButton(nullptr);

		// if (fOwner->fKeyMenuBar == this)
		// 	fOwner->fKeyMenuBar = nullptr;
	}

	if (fOwner->fLastMouseMovedView == this)
		fOwner->fLastMouseMovedView = nullptr;

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

	TEST_CASE("Conversions")
	{
		BView view1{{20, 10, 120, 110}, "view1", 0, 0};
		BView view2{{0, 0, 10, 10}, "view2", 0, 0};
		BView view3{{50, 60, 100, 100}, "view3", 0, 0};
		BView view4{{10, 10, 20, 20}, "view4", 0, 0};

		view1.AddChild(&view2);
		view1.AddChild(&view3);
		view3.AddChild(&view4);

		//   10
		// 20+--------------------------------------------+
		//   |+--------+  v1                              |
		//   || v2     |                                  |
		//   ||  10x10 |               100x100            |
		//   ||        |                                  |
		//   |+--------+                                  |
		//   |                 60                         |
		//   |               50+----------------+100      |
		//   |                 |   10     v3    |         |
		//   |                 | 10+---+        |         |
		//   |                 |   | v4|        |         |
		//   |                 |   +---+20      |         |
		//   |                 |      20        |         |
		//   |                 |                |         |
		//   |                 |     50x40      |         |
		//   |                 +----------------+100      |
		//   |                                100         |
		//   |                                            |
		//   +--------------------------------------------+120
		//                                              120

		SUBCASE("ToParent")
		{
			CHECK(view1.ConvertToParent(BPoint{11, 12}) == BPoint{31, 22});
			CHECK(view2.ConvertToParent(BPoint{0, 0}) == BPoint{0, 0});
			CHECK(view2.ConvertToParent(BPoint{15.5, 5.5}) == BPoint{15.5, 5.5});
			CHECK(view3.ConvertToParent(BPoint{0, 0}) == BPoint{50, 60});
			CHECK(view4.ConvertToParent(BPoint{10, 10}) == BPoint{20, 20});

			BPoint point{5, 10};
			view3.ConvertToParent(&point);
			CHECK(point == BPoint{55, 70});

			CHECK(view1.ConvertToParent(BRect{10, 11, 20, 21}) == BRect{30, 21, 40, 31});
			CHECK(view4.ConvertToParent(BRect{0, 0, 20.5, 20.5}) == BRect{10, 10, 30.5, 30.5});

			BRect rect{10, 20, 30, 40};
			view3.ConvertToParent(&rect);
			CHECK(rect == BRect{60, 80, 80, 100});
		}

		SUBCASE("FromParent")
		{
			CHECK(view1.ConvertFromParent(BPoint{20, 30}) == BPoint{0, 20});
			CHECK(view2.ConvertFromParent(BPoint{20, 30}) == BPoint{20, 30});
			CHECK(view3.ConvertFromParent(BPoint{100, 100}) == BPoint{50, 40});
			CHECK(view4.ConvertFromParent(BPoint{15, 10}) == BPoint{5, 0});

			BPoint point{25, 30};
			view1.ConvertFromParent(&point);
			CHECK(point == BPoint{5, 20});

			CHECK(view1.ConvertFromParent(BRect{20, 11, 40, 21}) == BRect{0, 1, 20, 11});
			CHECK(view3.ConvertFromParent(BRect{60, 60, 70.5, 70.5}) == BRect{10, 0, 20.5, 10.5});

			BRect rect{10, 20, 30, 40};
			view4.ConvertFromParent(&rect);
			CHECK(rect == BRect{0, 10, 20, 30});
		}

		SUBCASE("ToScreen")
		{
			CHECK(view1.ConvertToScreen(BPoint{0, 0}) == BPoint{20, 10});
			CHECK(view2.ConvertToScreen(BPoint{10, 10}) == BPoint{30, 20});
			CHECK(view3.ConvertToScreen(BPoint{50, 50}) == BPoint{120, 120});
			CHECK(view4.ConvertToScreen(BPoint{11.1, 12.3}) == BPoint{91.1, 92.3});

			BPoint point{5, 10};
			view3.ConvertToScreen(&point);
			CHECK(point == BPoint{75, 80});

			CHECK(view1.ConvertToScreen(BRect{10, 11, 20, 21}) == BRect{30, 21, 40, 31});
			CHECK(view4.ConvertToScreen(BRect{1, 2, 5.5, 5.5}) == BRect{81, 82, 85.5, 85.5});

			BRect rect{10, 20, 30, 40};
			view3.ConvertToScreen(&rect);
			CHECK(rect == BRect{80, 90, 100, 110});
		}

		SUBCASE("FromScreen")
		{
			CHECK(view1.ConvertFromScreen(BPoint{120, 120}) == BPoint{100, 110});
			CHECK(view4.ConvertFromScreen(BPoint{100.5, 99.5}) == BPoint{20.5, 19.5});

			BPoint point{5, 10};
			view3.ConvertFromScreen(&point);
			CHECK(point == BPoint{-65, -60});

			CHECK(view1.ConvertFromScreen(BRect{20, 11, 40, 21}) == BRect{0, 1, 20, 11});
			CHECK(view3.ConvertFromScreen(BRect{60, 60, 70.5, 70.5}) == BRect{-10, -10, 0.5, 0.5});

			BRect rect{90, 90, 100, 100};
			view4.ConvertFromScreen(&rect);
			CHECK(rect == BRect{10, 10, 20, 20});
		}

		SUBCASE("Siblings")
		{
			BView parent(BRect(), nullptr, 0, 0);
			BView child0(BRect(), nullptr, 0, 0);
			BView child1(BRect(), nullptr, 0, 0);
			BView child2(BRect(), nullptr, 0, 0);
			parent.AddChild(&child0);
			parent.AddChild(&child1);
			parent.AddChild(&child2);

			CHECK(child0.Parent() == &parent);
			CHECK(child1.Parent() == &parent);
			CHECK(child2.Parent() == &parent);

			CHECK(parent.CountChildren() == 3);
			CHECK(parent.ChildAt(0) == &child0);
			CHECK(parent.ChildAt(1) == &child1);
			CHECK(parent.ChildAt(2) == &child2);
			CHECK(parent.ChildAt(3) == nullptr);

			BView *child = parent.ChildAt(0);
			CHECK(child == &child0);
			child = child->NextSibling();
			CHECK(child == &child1);
			child = child->NextSibling();
			CHECK(child == &child2);
			child = child->NextSibling();
			CHECK(child == nullptr);
		}
	}
}
