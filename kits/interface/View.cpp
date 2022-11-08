#include "View.h"

#define LOG_TAG "BView"

#include <GraphicsDefs.h>
#include <Message.h>
#include <Window.h>
#include <doctest/doctest.h>
#include <include/core/SkCanvas.h>
#include <include/core/SkPaint.h>
#include <log/log.h>
#include <pimpl.h>

class BView::impl
{
   public:
	rgb_color view_color;
	rgb_color high_color;
	rgb_color low_color;

	impl()
		: view_color{255, 255, 255, 255},
		  high_color{0, 0, 0, 255},
		  low_color{255, 255, 255, 255}
	{
	}
};

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
	ALOGV("BView::MessageReceived 0x%x: %.4s", message->what, (char *)&message->what);
	switch (message->what) {
		case _UPDATE_: {
			const rgb_color view_color(ViewColor());

			// The Application Server paints the view with this color before any view-specific drawing functions are called.
			// If you set the view color to B_TRANSPARENT_COLOR, the Application Server won't erase the view's clipping region before an update.
			if ((fFlags & B_WILL_DRAW) && (view_color != B_TRANSPARENT_COLOR)) {
				BRect updateRect;
				if (message->FindRect("updateRect", &updateRect) == B_OK && updateRect.IsValid()) {
					SkPaint paint;
					paint.setARGB(view_color.alpha, view_color.red, view_color.green, view_color.blue);
					SkCanvas	 *canvas = static_cast<SkCanvas *>(fOwner->_get_canvas());
					if (!canvas) {
						// FIXME: what now?
						break;
					}
					const auto &bounds = Bounds();
					canvas->drawRect(SkRect::MakeLTRB(bounds.left, bounds.top, bounds.right, bounds.bottom), paint);

					// Call hook function to Draw content
					Draw(updateRect);
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
	debugger(__PRETTY_FUNCTION__);
}

void BView::SetPenSize(float size)
{
	debugger(__PRETTY_FUNCTION__);
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

void BView::SetFont(const BFont *font, uint32 mask)
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
	debugger(__PRETTY_FUNCTION__);
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
