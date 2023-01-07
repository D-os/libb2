#include "TabView.h"

#define LOG_TAG "BLooper"

#include <List.h>
#include <Region.h>
#include <Window.h>
#include <include/core/SkCanvas.h>
#include <include/core/SkPath.h>
#include <log/log.h>

#define TABVIEW_BORDER_WIDTH 3.0f
#define TABVIEW_DEFAULT_TAB_WIDTH 90.0f
#define TABVIEW_DEFAULT_TAB_PADDING 3.0f
#define TABVIEW_TAB_OVERLAP 1.0f
#define TABVIEW_DEFAULT_TAB_OFFSET (5.0f + TABVIEW_TAB_OVERLAP)

#pragma mark - BTab

BTab::BTab(BView* contents)
	: fEnabled{true},
	  fSelected{false},
	  fFocus{false},
	  fView{contents} {}

BTab::~BTab()
{
	if (fView) {
		if (fSelected) {
			fView->RemoveSelf();
		}
		delete fView;
	}
}

BTab::BTab(BMessage* data) : BArchivable(data) {}

status_t BTab::Archive(BMessage* data, bool deep) const
{
	return BArchivable::Archive(data, deep);
}

status_t BTab::Perform(uint32 d, void* arg)
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

const char* BTab::Label() const
{
	return fView ? fView->Name() : nullptr;
}

void BTab::SetLabel(const char* label)
{
	if (fView) {
		fView->SetName(label);
	}
}

bool BTab::IsSelected() const
{
	return fSelected;
}

void BTab::Select(BView* owner)
{
	fSelected = true;

	if (owner && fView && !fView->Parent()) {
		owner->AddChild(fView);
	}
}

void BTab::Deselect()
{
	if (fView && fView->Parent()) {
		fView->RemoveSelf();
	}

	fSelected = false;
}

void BTab::SetEnabled(bool on)
{
	fEnabled = on;
}

bool BTab::IsEnabled() const
{
	return fEnabled;
}

void BTab::MakeFocus(bool infocus)
{
	fFocus = infocus;
}

bool BTab::IsFocus() const
{
	return fFocus;
}

void BTab::SetView(BView* contents)
{
	if (!contents || fView == contents)
		return;

	if (fView) {
		fView->RemoveSelf();
		delete fView;
	}

	fView = contents;
}

BView* BTab::View() const
{
	return fView;
}

void BTab::DrawFocusMark(BView* owner, BRect tabFrame)
{
	float width = owner->StringWidth(Label());

	owner->SetHighColor(ui_color(B_KEYBOARD_NAVIGATION_COLOR));

	float offset = IsSelected() ? 3 : 2;
	owner->StrokeLine(BPoint((tabFrame.left + tabFrame.right - width) / 2.0,
							 tabFrame.bottom - offset),
					  BPoint((tabFrame.left + tabFrame.right + width) / 2.0,
							 tabFrame.bottom - offset));
}

void BTab::DrawLabel(BView* owner, BRect tabFrame)
{
	const auto label = Label();
	if (label) {
		BFont font;
		owner->GetFont(&font);
		font_height fh;
		font.GetHeight(&fh);

		// TODO: move below code to helper function
		float X = tabFrame.right - font.StringWidth(label);
		X -= roundf((X - tabFrame.left) / 2);
		float Y = ceilf(fh.ascent);
		if (tabFrame.Height() > Y)
			Y = ceilf((tabFrame.bottom - tabFrame.top + Y) / 2);
		BPoint pos{X, Y};

		owner->DrawString(label, pos);
	}
}

void BTab::DrawTab(BView* owner, BRect frame, tab_position position, bool full)
{
	auto width = frame.Width();
	frame.left -= TABVIEW_TAB_OVERLAP;
	frame.right += TABVIEW_TAB_OVERLAP;
	owner->_damage_rect(frame);	 // damage frame and prepare canvas

	SkCanvas* canvas = static_cast<SkCanvas*>(owner->Window()->_get_canvas());
	if (!canvas) return;
	const auto tab_view = dynamic_cast<BTabView*>(owner);

	auto active_color	= (tab_view ? tab_view->ContainerView() : owner)->ViewColor();
	auto inactive_color = tint_color(active_color, B_DARKEN_1_TINT);
	auto shadow_color	= ui_color(B_SHADOW_COLOR);
	auto shine_color	= IsSelected() ? ui_color(B_SHINE_COLOR) : active_color;

	SkPath path;
	path.moveTo(frame.left, frame.bottom);
	path.quadTo(frame.left + TABVIEW_DEFAULT_TAB_PADDING / 3, frame.top, frame.left + 3 * TABVIEW_DEFAULT_TAB_PADDING, frame.top);
	path.lineTo(frame.right - 3 * TABVIEW_DEFAULT_TAB_PADDING + 1, frame.top);
	SkPath shine_path(path);
	path.quadTo(frame.right - TABVIEW_DEFAULT_TAB_PADDING / 3 + 1, frame.top, frame.right + 1, frame.bottom);

	canvas->save();
	canvas->clipPath(path);
	if (position == B_TAB_ANY) {
		SkPath corner;
		path.offset(-width, 0, &corner);
		canvas->clipPath(corner, SkClipOp::kDifference);
	}
	if (!full) {
		SkPath corner;
		path.offset(width, 0, &corner);
		canvas->clipPath(corner, SkClipOp::kDifference);
	}

	SkPaint paint;
	paint.setStrokeCap(SkPaint::kSquare_Cap);

	paint.setAntiAlias(false);
	paint.setStroke(false);
	rgb_color& back_color = IsSelected() ? active_color : inactive_color;
	paint.setColor(SkColorSetARGB(back_color.alpha, back_color.red, back_color.green, back_color.blue));
	canvas->drawPath(path, paint);

	DrawLabel(owner, frame);

	paint.setAntiAlias(true);
	paint.setStroke(true);

	paint.setColor(SkColorSetARGB(shine_color.alpha, shine_color.red, shine_color.green, shine_color.blue));
	paint.setStrokeWidth(6 * owner->PenSize());
	canvas->drawPath(shine_path, paint);

	paint.setColor(SkColorSetARGB(inactive_color.alpha, inactive_color.red, inactive_color.green, inactive_color.blue));
	paint.setStrokeWidth(4 * owner->PenSize());
	canvas->drawPath(path, paint);

	paint.setColor(SkColorSetARGB(shadow_color.alpha, shadow_color.red, shadow_color.green, shadow_color.blue));
	paint.setStrokeWidth(2 * owner->PenSize());
	canvas->drawPath(path, paint);

	canvas->restore();
}

#pragma mark - BTabView

BTabView::BTabView(BRect frame, const char* name, button_width width, uint32 resizingMode, uint32 flags)
	: BView(frame, name, resizingMode, flags),
	  fTabList{new BList},
	  fContainerView{nullptr},
	  fTabWidthSetting{width},
	  fTabHeight{-1},
	  fSelection{-1},
	  fFocus{-1}

{
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	SetLowColor(ui_color(B_PANEL_BACKGROUND_COLOR));

	fContainerView = new BView(Bounds(), "view container", B_FOLLOW_ALL, B_WILL_DRAW);
	fContainerView->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	fContainerView->SetLowColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	AddChild(fContainerView);

	font_height fh;
	GetFontHeight(&fh);
	SetTabHeight(ceilf(fh.ascent + 2.0 * TABVIEW_DEFAULT_TAB_PADDING));
}

BTabView::~BTabView()
{
	debugger(__PRETTY_FUNCTION__);
}

BTabView::BTabView(BMessage* data) : BView(data) {}

status_t BTabView::Archive(BMessage*, bool deep) const
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

status_t BTabView::Perform(perform_code d, void* arg)
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

void BTabView::WindowActivated(bool state)
{
	BView::WindowActivated(state);
	DrawTabs();
}

void BTabView::AttachedToWindow()
{
	BView::AttachedToWindow();
	Select(0);
}

void BTabView::AllAttached()
{
	BView::AllAttached();
}

void BTabView::AllDetached()
{
	BView::AllDetached();
}

void BTabView::DetachedFromWindow()
{
	BView::DetachedFromWindow();
}

void BTabView::MessageReceived(BMessage* msg)
{
	BView::MessageReceived(msg);
}

void BTabView::FrameMoved(BPoint new_position)
{
	BView::FrameMoved(new_position);
}

void BTabView::FrameResized(float w, float h)
{
	BView::FrameResized(w, h);
}

void BTabView::KeyDown(const char* bytes, int32 numBytes)
{
	if (IsHidden())
		return;

	switch (bytes[0]) {
		case B_DOWN_ARROW:
		case B_LEFT_ARROW: {
			int32 focus = fFocus - 1;
			if (focus < 0)
				focus = CountTabs() - 1;
			SetFocusTab(focus, true);
			break;
		}

		case B_UP_ARROW:
		case B_RIGHT_ARROW: {
			int32 focus = fFocus + 1;
			if (focus >= CountTabs())
				focus = 0;
			SetFocusTab(focus, true);
			break;
		}

		case B_RETURN:
		case B_SPACE:
			Select(FocusTab());
			break;

		default:
			BView::KeyDown(bytes, numBytes);
	}
}

void BTabView::MouseDown(BPoint where)
{
	for (int32 i = 0; i < CountTabs(); i++) {
		if (TabFrame(i).Contains(where) && i != Selection()) {
			Select(i);
			return;
		}
	}

	BView::MouseDown(where);
}

void BTabView::MouseUp(BPoint where)
{
	BView::MouseUp(where);
}

void BTabView::MouseMoved(BPoint pt, uint32 transit, const BMessage* dnd)
{
	BView::MouseMoved(pt, transit, dnd);
}

void BTabView::Pulse()
{
	BView::Pulse();
}

void BTabView::Select(int32 tabIndex)
{
	if (tabIndex == Selection())
		return;

	if (tabIndex < 0 || tabIndex >= CountTabs())
		tabIndex = Selection();

	BTab* tab = TabAt(Selection());

	if (tab)
		tab->Deselect();

	tab = TabAt(tabIndex);
	if (tab && fContainerView) {
		tab->Select(fContainerView);
		fSelection = tabIndex;
	}

	Invalidate();

	SetFocusTab(tabIndex, true);
}

int32 BTabView::Selection() const
{
	return fSelection;
}

void BTabView::MakeFocus(bool focusState)
{
	BView::MakeFocus();
	SetFocusTab(Selection(), true);
}

void BTabView::SetFocusTab(int32 tabIndex, bool focusState)
{
	if (tabIndex >= CountTabs())
		tabIndex = 0;

	if (tabIndex < 0)
		tabIndex = CountTabs() - 1;

	if (focusState) {
		if (tabIndex == fFocus)
			return;

		if (fFocus != -1) {
			auto tab = TabAt(fFocus);
			if (tab)
				tab->MakeFocus(false);
			Invalidate(TabFrame(fFocus));
		}
		auto tab = TabAt(tabIndex);
		if (tab) {
			tab->MakeFocus(true);
			Invalidate(TabFrame(tabIndex));
			fFocus = tabIndex;
		}
	}
	else if (fFocus != -1) {
		TabAt(fFocus)->MakeFocus(false);
		Invalidate(TabFrame(fFocus));
		fFocus = -1;
	}
}

int32 BTabView::FocusTab() const
{
	return fFocus;
}

void BTabView::Draw(BRect)
{
	DrawTabs();

	if (IsFocus() && fFocus != -1)
		TabAt(fFocus)->DrawFocusMark(this, TabFrame(fFocus));
}

void BTabView::DrawAfterChildren(BRect)
{
	DrawBox(TabFrame(fSelection));
}

BRect BTabView::DrawTabs()
{
	BRect tabFrame(Bounds());
	tabFrame.bottom = fTabHeight;

	BRect activeTabFrame;
	int32 tabCount = CountTabs();
	for (int32 i = 0; i < tabCount; i++) {
		BRect tabFrame = TabFrame(i);
		if (i == fSelection)
			activeTabFrame = tabFrame;

		TabAt(i)->DrawTab(this, tabFrame,
						  i == fSelection ? B_TAB_FRONT
						  : (i == 0)	  ? B_TAB_FIRST
										  : B_TAB_ANY,
						  i != fSelection - 1);
#ifndef NDEBUG
		PushState();
		SetHighColor(make_color(200, 0, 0, 48));
		SetDrawingMode(B_OP_BLEND);
		StrokeRect(tabFrame);
		PopState();
#endif
	}

	return activeTabFrame;
}

void BTabView::DrawBox(BRect selectedTabFrame)
{
	auto hi = HighColor();

	BRect rect(Bounds());
	rect.top = fTabHeight;

	SetHighColor(ui_color(B_SHADOW_COLOR));
	SetPenSize(1.0f);
	MovePenTo({selectedTabFrame.left, rect.top});
	StrokeLine(rect.LeftTop());
	StrokeLine(rect.LeftBottom());
	StrokeLine(rect.RightBottom());
	StrokeLine(rect.RightTop());
	StrokeLine({selectedTabFrame.right + 1, rect.top});

	BRect inset = rect.InsetByCopy(1, 1);
	SetHighColor(tint_color(fContainerView->ViewColor(), B_DARKEN_3_TINT));
	MovePenTo(inset.LeftBottom());
	StrokeLine(inset.RightBottom());
	StrokeLine(inset.RightTop());
	SetHighColor(tint_color(fContainerView->ViewColor(), B_DARKEN_1_TINT));
	StrokeLine({selectedTabFrame.right + 1, inset.top});
	MovePenTo({selectedTabFrame.left, inset.top});
	StrokeLine(inset.LeftTop());
	StrokeLine(inset.LeftBottom());

	inset.InsetBy(1, 1);
	SetHighColor(tint_color(fContainerView->ViewColor(), B_DARKEN_2_TINT));
	MovePenTo(inset.LeftBottom());
	StrokeLine(inset.RightBottom());
	StrokeLine(inset.RightTop());
	SetHighColor(ui_color(B_SHINE_COLOR));
	StrokeLine({selectedTabFrame.right + 1, inset.top});
	MovePenTo({selectedTabFrame.left, rect.top});
	StrokeLine({selectedTabFrame.left, inset.top});
	StrokeLine(inset.LeftTop());
	StrokeLine(inset.LeftBottom());

	SetHighColor(hi);
}

BRect BTabView::TabFrame(int32 tabIndex) const
{
	if (tabIndex >= CountTabs() || tabIndex < 0)
		return BRect();

	float		width  = TABVIEW_DEFAULT_TAB_WIDTH;
	const float height = fTabHeight;
	const float offset = TABVIEW_DEFAULT_TAB_OFFSET;

	switch (fTabWidthSetting) {
		case B_WIDTH_FROM_LABEL: {
			float x = 0.0f;
			for (int32 i = 0; i < tabIndex; i++) {
				x += StringWidth(TabAt(i)->Label());
			}

			return BRect(offset + x, 0.0f,
						 offset + x + StringWidth(TabAt(tabIndex)->Label()), height);
		}
		case B_WIDTH_FROM_WIDEST:
			width = 0.0;
			for (int32 i = 0; i < CountTabs(); i++) {
				float tabWidth = StringWidth(TabAt(i)->Label());
				if (tabWidth > width)
					width = tabWidth;
			}
			// fall through

		case B_WIDTH_AS_USUAL:
		default:
			return BRect(offset + tabIndex * width, 0.0f,
						 offset + tabIndex * width + width, height);
	}
}

void BTabView::SetFlags(uint32 flags)
{
	BView::SetFlags(flags);
}

void BTabView::SetResizingMode(uint32 mode)
{
	BView::SetResizingMode(mode);
}

void BTabView::GetPreferredSize(float* width, float* height)
{
	BView::GetPreferredSize(width, height);
}

void BTabView::ResizeToPreferred()
{
	BView::ResizeToPreferred();
}

BHandler* BTabView::ResolveSpecifier(BMessage* msg, int32 index, BMessage* specifier, int32 form, const char* property)
{
	debugger(__PRETTY_FUNCTION__);
	return nullptr;
}

status_t BTabView::GetSupportedSuites(BMessage* data)
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

void BTabView::AddTab(BView* tabContents, BTab* tab)
{
	if (tab)
		tab->SetView(tabContents);
	else
		tab = new BTab(tabContents);

	fTabList->AddItem(tab);

	// When we haven't had a any tabs before, but are already attached to the
	// window, select this one.
	if (CountTabs() == 1 && Window())
		Select(0);
}

BTab* BTabView::RemoveTab(int32 tabIndex)
{
	if (tabIndex < 0 || tabIndex >= CountTabs())
		return nullptr;

	BTab* tab = static_cast<BTab*>(fTabList->RemoveItem(tabIndex));
	if (!tab)
		return nullptr;

	tab->Deselect();

	if (CountTabs() == 0)
		fFocus = -1;
	else if (tabIndex <= fSelection)
		Select(fSelection - 1);

	if (fFocus >= 0) {
		if (fFocus == CountTabs() - 1 || CountTabs() == 0)
			SetFocusTab(fFocus, false);
		else
			SetFocusTab(fFocus, true);
	}

	return tab;
}

BTab* BTabView::TabAt(int32 tabIndex) const
{
	return static_cast<BTab*>(fTabList->ItemAt(tabIndex));
}

void BTabView::SetTabWidth(button_width s)
{
	if (fTabWidthSetting == s)
		return;

	fTabWidthSetting = s;

	Invalidate();
}

button_width BTabView::TabWidth() const
{
	return fTabWidthSetting;
}

void BTabView::SetTabHeight(float height)
{
	if (fTabHeight == height)
		return;

	fTabHeight = height;

	// When you change the tab height, the container view for the target
	// views is resized so that the BTabView doesn't change size. Making the tabs
	// taller by N pixels causes the container view's top edge to move down by N pixels,
	// and decreasing the heights of the tabs increases the height of the container view.
	BRect bounds = Bounds();
	bounds.top += TabHeight();
	fContainerView->MoveTo(bounds.left, bounds.top);
	fContainerView->ResizeTo(bounds.Width(), bounds.Height());

	Invalidate();
}

float BTabView::TabHeight() const
{
	return fTabHeight;
}

BView* BTabView::ContainerView() const
{
	return fContainerView;
}

int32 BTabView::CountTabs() const
{
	return fTabList->CountItems();
}

BView* BTabView::ViewForTab(int32 tabIndex) const
{
	BTab* tab = TabAt(tabIndex);
	return tab ? tab->View() : nullptr;
}
