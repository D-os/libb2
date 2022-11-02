#include "View.h"

#define LOG_TAG "BView"

#include <Window.h>
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

	frame.left	 = roundf(frame.left);
	frame.top	 = roundf(frame.top);
	frame.right	 = roundf(frame.right);
	frame.bottom = roundf(frame.bottom);
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

void BView::MessageReceived(BMessage *msg)
{
	debugger(__PRETTY_FUNCTION__);
}

void BView::AddChild(BView *child, BView *before)
{
	debugger(__PRETTY_FUNCTION__);
}

bool BView::RemoveChild(BView *child)
{
	debugger(__PRETTY_FUNCTION__);
	return false;
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
	debugger(__PRETTY_FUNCTION__);
}

void BView::Invalidate(const BRegion *invalRegion)
{
	debugger(__PRETTY_FUNCTION__);
}

void BView::Invalidate()
{
	debugger(__PRETTY_FUNCTION__);
}

void BView::SetFlags(uint32 flags)
{
	debugger(__PRETTY_FUNCTION__);
}

void BView::SetResizingMode(uint32 mode)
{
	debugger(__PRETTY_FUNCTION__);
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
		ALOGE("Window '%s' already has an owner %p: '%s'", Name(), fOwner, fOwner->Name());
		debugger("Window already has an owner");
	}

	fOwner		  = window;
	fTopLevelView = true;

	AttachedToWindow();

	if (!fOwner->IsHidden())
		Invalidate();

	for (BView *child = fFirstChild; child != nullptr; child = child->fNextSibling) {
		// we need to check for fAttached as new views could have been
		// added in AttachedToWindow() - and those are already attached
		if (!child->fOwner)
			child->_attach(window);
	}

	AllAttached();
}
