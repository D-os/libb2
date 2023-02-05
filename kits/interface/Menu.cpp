#include "Menu.h"

#define LOG_TAG "BMenu"

#include <Application.h>
#include <ControlLook.h>
#include <MenuBar.h>
#include <MenuItem.h>
#include <Screen.h>
#include <Window.h>
#include <log/log.h>

#include <cstdint>

#include "MenuPrivate.h"
#include "MenuWindow.h"

#define USE_CACHED_MENUWINDOW 1

using BPrivate::BMenuWindow;
using std::nothrow;

class BMenu::impl
{
   public:
	BSize  preferred;
	uint32 lastResizingMode;

	// Used to track when the menu would be drawn offscreen and instead gets
	// shifted back on the screen towards the left. This information
	// allows us to draw submenus in the same direction as their parents.
	bool frameShiftedLeft;

	impl()
		: lastResizingMode{0},
		  frameShiftedLeft{false}
	{
	}
};

#pragma mark - BMenu

const char *BPrivate::kEmptyMenuLabel = "<empty>";

BMenu::BMenu(const char *title, menu_layout layout)
	: BMenu(BRect(), title, 0, B_WILL_DRAW, layout, true) {}

BMenu::BMenu(const char *title, float width, float height)
	: BMenu(BRect(0, 0, width, height), title, 0, B_WILL_DRAW, B_ITEMS_IN_MATRIX, false) {}

BMenu::BMenu(BRect frame, const char *viewName, uint32 resizeMask, uint32 flags, menu_layout layout, bool resizeToFit)
	: BView(frame, viewName, resizeMask, flags),
	  fChosenItem{nullptr},
	  fSelected{nullptr},
	  fCachedMenuWindow{nullptr},
	  fSuper{nullptr},
	  fSuperitem{nullptr},
	  fAscent{-1.0f},
	  fDescent{-1.0f},
	  fFontHeight{-1.0f},
	  fState{MENU_STATE_CLOSED},
	  fLayout{layout},
	  fExtraRect{nullptr},
	  fMaxContentWidth{0.0f},
	  fExtraMenuData{new BMenu::impl()},
	  fResizeToFit{resizeToFit},
	  fUseCachedMenuLayout{false},
	  fEnabled{true},
	  fDynamicName{false},
	  fRadioMode{false},
	  fStickyMode{false},
	  fTriggerEnabled{true},
	  fHasSubmenus{false},
	  fAttachAborted{false}
{
	fExtraMenuData->lastResizingMode = ResizingMode();
}

BMenu::~BMenu()
{
	delete fExtraMenuData;
}

BMenu::BMenu(BMessage *data) : BView(data) {}

status_t BMenu::Archive(BMessage *data, bool deep) const
{
	return BView::Archive(data, deep);
}

void BMenu::AttachedToWindow()
{
	BView::AttachedToWindow();

	// _GetShiftKey(sShiftKey);
	// _GetControlKey(sControlKey);
	// _GetCommandKey(sCommandKey);
	// _GetOptionKey(sOptionKey);
	// _GetMenuKey(sMenuKey);

	// The menu should be added to the menu hierarchy and made visible if:
	// * the mouse is over the menu,
	// * the user has requested the menu via the keyboard.
	// So if we don't pass keydown in here, keyboard navigation breaks since
	// fAttachAborted will return false if the mouse isn't over the menu
	bool keyDown   = Supermenu()
						 ? Supermenu()->fState == MENU_STATE_KEY_TO_SUBMENU
						 : false;
	fAttachAborted = _AddDynamicItems(keyDown);

	if (!fAttachAborted) {
		_CacheFontInfo();
		_LayoutItems(0);
		_UpdateWindowViewSize(false);
	}
}

void BMenu::DetachedFromWindow()
{
	BView::DetachedFromWindow();
}

bool BMenu::AddItem(BMenuItem *item)
{
	return AddItem(item, CountItems());
}

bool BMenu::AddItem(BMenuItem *item, int32 index)
{
	if (!item || fLayout == B_ITEMS_IN_MATRIX)
		return false;

	if (!_AddItem(item, index))
		return false;

	InvalidateLayout();
	if (LockLooper()) {
		if (!Window()->IsHidden()) {
			_LayoutItems(index);
			// _UpdateWindowViewSize(false);
			Invalidate();
		}
		UnlockLooper();
	}
	return true;
}

bool BMenu::AddItem(BMenuItem *item, BRect frame)
{
	if (!item || fLayout != B_ITEMS_IN_MATRIX)
		return false;

	item->fBounds = frame;

	int32 index = CountItems();
	if (!_AddItem(item, index))
		return false;

	if (LockLooper()) {
		if (!Window()->IsHidden()) {
			_LayoutItems(index);
			Invalidate();
		}
		UnlockLooper();
	}
	return true;
}

bool BMenu::AddItem(BMenu *menu)
{
	return AddItem(menu, CountItems());
}

bool BMenu::AddItem(BMenu *menu, int32 index)
{
	if (!menu || fLayout == B_ITEMS_IN_MATRIX)
		return false;

	BMenuItem *item = new BMenuItem(menu);
	if (!AddItem(item, index)) {
		item->fSubmenu = nullptr;
		delete item;
		return false;
	}

	return true;
}

bool BMenu::AddItem(BMenu *menu, BRect frame)
{
	if (!menu || fLayout != B_ITEMS_IN_MATRIX)
		return false;

	BMenuItem *item = new BMenuItem(menu);
	if (!AddItem(item, frame)) {
		item->fSubmenu = nullptr;
		delete item;
		return false;
	}

	return true;
}

bool BMenu::AddList(BList *list, int32 index)
{
	// this function is not documented in the bebook
	debugger(__PRETTY_FUNCTION__);
	return false;
}

bool BMenu::AddSeparatorItem()
{
	BMenuItem *item = new BSeparatorItem();
	if (!fItems.AddItem(item)) {
		delete item;
		return false;
	}

	return true;
}

bool BMenu::RemoveItem(BMenuItem *item)
{
	if (!item)
		return false;

	if (fItems.RemoveItem(item)) {
		// if (item == fSelected && Window())
		// 	SelectItem(nullptr);

		// item->Uninstall();
		// item->SetSuper(nullptr);

		delete item;
		return true;
	}

	return false;
}

BMenuItem *BMenu::RemoveItem(int32 index)
{
	debugger(__PRETTY_FUNCTION__);
	return nullptr;
}

bool BMenu::RemoveItems(int32 index, int32 count, bool del)
{
	debugger(__PRETTY_FUNCTION__);
	return false;
}

bool BMenu::RemoveItem(BMenu *menu)
{
	debugger(__PRETTY_FUNCTION__);
	return false;
}

BMenuItem *BMenu::ItemAt(int32 index) const
{
	return static_cast<BMenuItem *>(fItems.ItemAt(index));
}

BMenu *BMenu::SubmenuAt(int32 index) const
{
	BMenuItem *item = static_cast<BMenuItem *>(fItems.ItemAt(index));
	return item ? item->Submenu() : nullptr;
}

int32 BMenu::CountItems() const
{
	return fItems.CountItems();
}

int32 BMenu::IndexOf(BMenuItem *item) const
{
	return fItems.IndexOf(item);
}

int32 BMenu::IndexOf(BMenu *menu) const
{
	for (int32 i = 0; i < fItems.CountItems(); i++) {
		if (ItemAt(i)->Submenu() == menu)
			return i;
	}

	return -1;
}

BMenuItem *BMenu::FindItem(uint32 command) const
{
	BMenuItem *item = nullptr;

	for (int32 i = 0; i < CountItems(); i++) {
		item = ItemAt(i);

		if (item->Command() == command)
			return item;

		if (item->Submenu()) {
			item = item->Submenu()->FindItem(command);
			if (item)
				return item;
		}
	}

	return nullptr;
}

BMenuItem *BMenu::FindItem(const char *name) const
{
	BMenuItem *item = nullptr;

	for (int32 i = 0; i < CountItems(); i++) {
		item = ItemAt(i);

		if (item->Label() && strcmp(item->Label(), name) == 0)
			return item;

		if (item->Submenu()) {
			item = item->Submenu()->FindItem(name);
			if (item)
				return item;
		}
	}

	return nullptr;
}

status_t BMenu::SetTargetForItems(BHandler *target)
{
	status_t status = B_OK;
	for (int32 i = 0; i < fItems.CountItems(); i++) {
		status = ItemAt(i)->SetTarget(target);
		if (status < B_OK)
			break;
	}

	return status;
}

status_t BMenu::SetTargetForItems(BMessenger messenger)
{
	status_t status = B_OK;
	for (int32 i = 0; i < fItems.CountItems(); i++) {
		status = ItemAt(i)->SetTarget(messenger);
		if (status < B_OK)
			break;
	}

	return status;
}

void BMenu::SetEnabled(bool enable)
{
	if (fEnabled == enable)
		return;

	fEnabled = enable;

	// if (dynamic_cast<_BMCMenuBar_ *>(Supermenu()))
	// 	Supermenu()->SetEnabled(enable);

	if (fSuperitem)
		fSuperitem->SetEnabled(enable);
}

void BMenu::SetRadioMode(bool on)
{
	fRadioMode = on;
	if (!on)
		SetLabelFromMarked(false);
}

void BMenu::SetTriggersEnabled(bool enable)
{
	fTriggerEnabled = enable;
}

void BMenu::SetMaxContentWidth(float width)
{
	fMaxContentWidth = width;
}

void BMenu::SetLabelFromMarked(bool on)
{
	fDynamicName = on;
	if (on)
		SetRadioMode(true);
}

bool BMenu::IsLabelFromMarked()
{
	return fDynamicName;
}

bool BMenu::IsEnabled() const
{
	if (!fEnabled)
		return false;

	return fSuper ? fSuper->IsEnabled() : true;
}

bool BMenu::IsRadioMode() const
{
	return fRadioMode;
}

bool BMenu::AreTriggersEnabled() const
{
	return fTriggerEnabled;
}

bool BMenu::IsRedrawAfterSticky() const
{
	return false;
}

float BMenu::MaxContentWidth() const
{
	return fMaxContentWidth;
}

BMenuItem *BMenu::FindMarked()
{
	for (int32 i = 0; i < fItems.CountItems(); i++) {
		BMenuItem *item = ItemAt(i);

		if (item->IsMarked())
			return item;
	}

	return nullptr;
}

BMenu *BMenu::Supermenu() const
{
	return fSuper;
}

BMenuItem *BMenu::Superitem() const
{
	return fSuperitem;
}

void BMenu::MessageReceived(BMessage *message)
{
	// if (message->HasSpecifiers())
	// 	return _ScriptReceived(message);

	switch (message->what) {
		case B_MOUSE_WHEEL_CHANGED: {
			float deltaY = 0;
			message->FindFloat("be:wheel_delta_y", &deltaY);
			if (deltaY == 0)
				return;

			BMenuWindow *window = dynamic_cast<BMenuWindow *>(Window());
			if (window == NULL)
				return;

			float largeStep;
			float smallStep;
			window->GetSteps(&smallStep, &largeStep);

			// pressing the shift key scrolls faster
			// FIXME: if ((modifiers() & B_SHIFT_KEY) != 0)
			// 	deltaY *= largeStep;
			// else
			deltaY *= smallStep;

			window->TryScrollBy(deltaY);
			break;
		}

		default:
			BView::MessageReceived(message);
			break;
	}
}

void BMenu::KeyDown(const char *bytes, int32 numBytes)
{
	// TODO: Test how it works on BeOS R5 and implement this correctly
	switch (bytes[0]) {
		case B_UP_ARROW:
			if (fLayout == B_ITEMS_IN_COLUMN)
				_SelectNextItem(fSelected, false);
			break;

		case B_DOWN_ARROW: {
			BMenuBar *bar = dynamic_cast<BMenuBar *>(Supermenu());
			if (bar && fState == MENU_STATE_CLOSED) {
				// tell MenuBar's _Track:
				bar->fState = MENU_STATE_KEY_TO_SUBMENU;
			}
			if (fLayout == B_ITEMS_IN_COLUMN)
				_SelectNextItem(fSelected, true);
			break;
		}

		case B_LEFT_ARROW:
			if (fLayout == B_ITEMS_IN_ROW)
				_SelectNextItem(fSelected, false);
			else {
				// this case has to be handled a bit specially.
				BMenuItem *item = Superitem();
				if (item) {
					if (dynamic_cast<BMenuBar *>(Supermenu())) {
						// If we're at the top menu below the menu bar, pass
						// the keypress to the menu bar so we can move to
						// another top level menu.
						BMessenger messenger(Supermenu());
						messenger.SendMessage(Window()->CurrentMessage());
					}
					else {
						// tell _Track
						fState = MENU_STATE_KEY_LEAVE_SUBMENU;
					}
				}
			}
			break;

		case B_RIGHT_ARROW:
			if (fLayout == B_ITEMS_IN_ROW)
				_SelectNextItem(fSelected, true);
			else {
				if (fSelected && fSelected->Submenu()) {
					fSelected->Submenu()->_SetStickyMode(true);
					// fix me: this shouldn't be needed but dynamic menus
					// aren't getting it set correctly when keyboard
					// navigating, which aborts the attach
					fState = MENU_STATE_KEY_TO_SUBMENU;
					_SelectItem(fSelected, true, true, true);
				}
				else if (dynamic_cast<BMenuBar *>(Supermenu())) {
					// if we have no submenu and we're an
					// item in the top menu below the menubar,
					// pass the keypress to the menubar
					// so you can use the keypress to switch menus.
					BMessenger messenger(Supermenu());
					messenger.SendMessage(Window()->CurrentMessage());
				}
			}
			break;

		case B_PAGE_UP:
		case B_PAGE_DOWN: {
			BMenuWindow *window = dynamic_cast<BMenuWindow *>(Window());
			if (window == NULL || !window->HasScrollers())
				break;

			int32 deltaY = bytes[0] == B_PAGE_UP ? -1 : 1;

			float largeStep;
			window->GetSteps(NULL, &largeStep);
			window->TryScrollBy(deltaY * largeStep);
			break;
		}

		case B_ENTER:
		case B_SPACE:
			if (fSelected) {
				fChosenItem = fSelected;
				// preserve for exit handling
				_QuitTracking(false);
			}
			break;

		case B_ESCAPE:
			_SelectItem(nullptr);
			if (fState == MENU_STATE_CLOSED
				&& dynamic_cast<BMenuBar *>(Supermenu())) {
				// Keyboard may show menu without tracking it
				BMessenger messenger(Supermenu());
				messenger.SendMessage(Window()->CurrentMessage());
			}
			else
				_QuitTracking(false);
			break;

		default: {
			if (AreTriggersEnabled()) {
				char trigger = bytes[0];

				for (uint32 i = CountItems(); i-- > 0;) {
					BMenuItem *item = ItemAt(i);
					if (item->fTriggerIndex < 0 || (item->fUserTrigger != trigger && item->fSysTrigger != trigger))
						continue;

					_InvokeItem(item);
					_QuitTracking(false);
					break;
				}
			}
			break;
		}
	}
}

void BMenu::Draw(BRect updateRect)
{
	if (_RelayoutIfNeeded()) {
		Invalidate();
		return;
	}

	DrawBackground(updateRect);
	DrawItems(updateRect);
}

void BMenu::GetPreferredSize(float *_width, float *_height)
{
	_ValidatePreferredSize();

	if (_width)
		*_width = fExtraMenuData->preferred.width;

	if (_height)
		*_height = fExtraMenuData->preferred.height;
}

void BMenu::ResizeToPreferred()
{
	BView::ResizeToPreferred();
}

void BMenu::FrameMoved(BPoint new_position)
{
	BView::FrameMoved(new_position);
}

void BMenu::FrameResized(float new_width, float new_height)
{
	BView::FrameResized(new_width, new_height);
}

void BMenu::InvalidateLayout()
{
	fUseCachedMenuLayout = false;
	// // This method exits for backwards compatibility reasons, it is used to
	// // invalidate the menu layout, but we also use call
	// // BView::InvalidateLayout() for good measure. Don't delete this method!
	// BView::InvalidateLayout(false);
}

BHandler *BMenu::ResolveSpecifier(BMessage *msg, int32 index, BMessage *specifier, int32 form, const char *property)
{
	return BView::ResolveSpecifier(msg, index, specifier, form, property);
}

status_t BMenu::GetSupportedSuites(BMessage *data)
{
	return BView::GetSupportedSuites(data);
}

void BMenu::MakeFocus(bool focused)
{
	BView::MakeFocus(focused);
}

void BMenu::AllAttached()
{
	BView::AllAttached();
}

void BMenu::AllDetached()
{
	BView::AllDetached();
}

BPoint BMenu::ScreenLocation()
{
	BMenu	  *superMenu = Supermenu();
	BMenuItem *superItem = Superitem();

	if (!superMenu || !superItem) {
		debugger(
			"BMenu can't determine where to draw. "
			"Override BMenu::ScreenLocation() to determine location.");
	}

	BPoint point;
	if (superMenu->Layout() == B_ITEMS_IN_COLUMN)
		point = superItem->Frame().RightTop() + BPoint(1.0f, 1.0f);
	else
		point = superItem->Frame().LeftBottom() + BPoint(1.0f, 1.0f);

	superMenu->ConvertToScreen(&point);

	return point;
}

void BMenu::SetItemMargins(float left, float top, float right, float bottom)
{
	fPad.Set(left, top, right, bottom);
}

void BMenu::GetItemMargins(float *_left, float *_top, float *_right, float *_bottom) const
{
	if (_left)
		*_left = fPad.left;

	if (_top)
		*_top = fPad.top;

	if (_right)
		*_right = fPad.right;

	if (_bottom)
		*_bottom = fPad.bottom;
}

menu_layout BMenu::Layout() const
{
	return fLayout;
}

void BMenu::Show()
{
	Show(false);
}

void BMenu::Show(bool selectFirstItem)
{
	_Install(nullptr);
	_Show(selectFirstItem);
}

void BMenu::Hide()
{
	_Hide();
	_Uninstall();
}

BMenuItem *BMenu::Track(bool sticky, BRect *clickToOpenRect)
{
	if (sticky && LockLooper()) {
		// RedrawAfterSticky(Bounds());
		//  the call above didn't do anything, so I've removed it for now
		UnlockLooper();
	}

	if (clickToOpenRect && LockLooper()) {
		fExtraRect = clickToOpenRect;
		ConvertFromScreen(fExtraRect);
		UnlockLooper();
	}

	_SetStickyMode(sticky);

	int		   action;
	BMenuItem *menuItem = _Track(&action);

	fExtraRect = nullptr;

	return menuItem;
}

bool BMenu::AddDynamicItem(add_state s)
{
	// Implemented in subclasses
	return false;
}

void BMenu::DrawBackground(BRect updateRect)
{
	rgb_color base	= ui_color(B_MENU_BACKGROUND_COLOR);
	uint32	  flags = 0;
	if (!IsEnabled())
		flags |= BControlLook::B_DISABLED;

	if (IsFocus())
		flags |= BControlLook::B_FOCUSED;

	BRect  rect	   = Bounds();
	uint32 borders = BControlLook::B_LEFT_BORDER | BControlLook::B_RIGHT_BORDER;

	auto window = Window();
	auto parent = Parent();
	if (window && parent) {
		if (parent->Frame().top == window->Bounds().top)
			borders |= BControlLook::B_TOP_BORDER;

		if (parent->Frame().bottom == window->Bounds().bottom)
			borders |= BControlLook::B_BOTTOM_BORDER;
	}
	else {
		borders |= BControlLook::B_TOP_BORDER | BControlLook::B_BOTTOM_BORDER;
	}
	be_control_look->DrawMenuBackground(this, rect, updateRect, base, flags, borders);
}

void BMenu::DrawItems(BRect updateRect)
{
	int32 itemCount = fItems.CountItems();
	for (int32 i = 0; i < itemCount; i++) {
		BMenuItem *item = ItemAt(i);
		if (item->Frame().Intersects(updateRect))
			item->Draw();
	}
}

#pragma mark - private

bool BMenu::_Show(bool selectFirstItem, bool keyDown)
{
	if (Window())
		return false;

	// See if the supermenu has a cached menuwindow,
	// and use that one if possible.
	BMenuWindow *window	   = nullptr;
	bool		 ourWindow = false;
	if (fSuper) {
		fSuperbounds = fSuper->ConvertToScreen(fSuper->Bounds());
		window		 = fSuper->_MenuWindow();
	}

	// Otherwise, create a new one
	// This happens for "stand alone" BPopUpMenus
	// (i.e. not within a BMenuField)
	if (window == NULL) {
		// Menu windows get the BMenu's handler name
		window	  = new (nothrow) BMenuWindow(Name());
		ourWindow = true;
	}

	if (window == NULL)
		return false;

	if (window->Lock()) {
		bool addAborted = false;
		if (keyDown)
			addAborted = _AddDynamicItems(keyDown);

		if (addAborted) {
			if (ourWindow)
				window->Quit();
			else
				window->Unlock();
			return false;
		}
		fAttachAborted = false;

		window->AttachMenu(this);

		if (ItemAt(0)) {
			float width, height;
			ItemAt(0)->GetContentSize(&width, &height);

			window->SetSmallStep(ceilf(height));
		}

		// Menu didn't have the time to add its items: aborting...
		if (fAttachAborted) {
			window->DetachMenu();
			// TODO: Probably not needed, we can just let _hide() quit the
			// window.
			if (ourWindow)
				window->Quit();
			else
				window->Unlock();
			return false;
		}

		_UpdateWindowViewSize(true);
		window->Show();

		if (selectFirstItem)
			_SelectItem(ItemAt(0), false);

		window->Unlock();
	}

	return true;
}

void BMenu::_Hide()
{
	BMenuWindow *window = dynamic_cast<BMenuWindow *>(Window());
	if (window == NULL || !window->Lock())
		return;

	if (fSelected)
		_SelectItem(nullptr);

	window->Hide();
	window->DetachMenu();  // we don't want to be deleted when the window is removed

#if USE_CACHED_MENUWINDOW
	if (fSuper)
		window->Unlock();
	else
#endif
		window->Quit();	 // it's our window, quit it

	_DeleteMenuWindow();  // Delete the menu window used by our submenus
}

bool BMenu::_AddItem(BMenuItem *item, int32 index)
{
	LOG_ALWAYS_FATAL_IF(!item, "item cannot be null");
	if (index < 0 || index > fItems.CountItems())
		return false;

	if (item->IsMarked()) {
		if (IsRadioMode()) {
			for (int32 i = 0; i < CountItems(); i++) {
				if (ItemAt(i) != item)
					ItemAt(i)->SetMarked(false);
			}
		}

		if (IsLabelFromMarked() && Superitem())
			Superitem()->SetLabel(item->Label());
	}

	if (!fItems.AddItem(item, index))
		return false;

	// install the item on the supermenu's window
	// or onto our window, if we are a root menu
	BWindow *window = nullptr;
	if (Superitem())
		window = Superitem()->fWindow;
	else
		window = Window();
	if (window)
		item->Install(window);

	item->SetSuper(this);
	return true;
}

bool BMenu::_RemoveItems(int32 index, int32 count, BMenuItem *item, bool deleteItems)
{
	bool success		  = false;
	bool invalidateLayout = false;

	bool	 locked = LockLooper();
	BWindow *window = Window();

	// The plan is simple: If we're given a BMenuItem directly, we use it
	// and ignore index and count. Otherwise, we use them instead.
	if (item) {
		if (fItems.RemoveItem(item)) {
			if (item == fSelected && window)
				_SelectItem(nullptr);
			item->Uninstall();
			item->SetSuper(nullptr);
			if (deleteItems)
				delete item;
			success = invalidateLayout = true;
		}
	}
	else {
		// We iterate backwards because it's simpler
		int32 i = std::min(index + count - 1, fItems.CountItems() - 1);
		// NOTE: the range check for "index" is done after
		// calculating the last index to be removed, so
		// that the range is not "shifted" unintentionally
		index = std::max((int32)0, index);
		for (; i >= index; i--) {
			item = static_cast<BMenuItem *>(fItems.ItemAt(i));
			if (item) {
				if (fItems.RemoveItem(i)) {
					if (item == fSelected && window)
						_SelectItem(nullptr);
					item->Uninstall();
					item->SetSuper(nullptr);
					if (deleteItems)
						delete item;
					success			 = true;
					invalidateLayout = true;
				}
				else {
					// operation not entirely successful
					success = false;
					break;
				}
			}
		}
	}

	if (invalidateLayout) {
		InvalidateLayout();
		if (locked && window) {
			_LayoutItems(0);
			// _UpdateWindowViewSize(false);
			Invalidate();
		}
	}

	if (locked)
		UnlockLooper();

	return success;
}

bool BMenu::_RelayoutIfNeeded()
{
	if (!fUseCachedMenuLayout) {
		fUseCachedMenuLayout = true;
		_CacheFontInfo();
		_LayoutItems(0);
		_UpdateWindowViewSize(false);
		return true;
	}
	return false;
}

void BMenu::_LayoutItems(int32 index)
{
	_CalcTriggers();

	float width;
	float height;
	_ComputeLayout(index, fResizeToFit, true, &width, &height);
	ALOGV("width: %f, height: %f", width, height);

	if (fResizeToFit)
		ResizeTo(width, height);
}

BSize BMenu::_ValidatePreferredSize()
{
	if (!fExtraMenuData->preferred.IsWidthSet() || ResizingMode() != fExtraMenuData->lastResizingMode) {
		_ComputeLayout(0, true, false, NULL, NULL);
		// ResetLayoutInvalidation();
	}

	return fExtraMenuData->preferred;
}

void BMenu::_ComputeLayout(int32 index, bool bestFit, bool moveItems, float *_width, float *_height)
{
	// TODO: Take "bestFit", "moveItems", "index" into account,
	// Recalculate only the needed items,
	// not the whole layout every time

	fExtraMenuData->lastResizingMode = ResizingMode();

	BRect frame;
	switch (fLayout) {
		case B_ITEMS_IN_COLUMN: {
			BRect  parentFrame;
			BRect *overrideFrame = nullptr;

			{
				bool command = false;
				bool control = false;
				bool shift	 = false;
				bool option	 = false;
				bool submenu = false;

				if (index > 0)
					frame = ItemAt(index - 1)->Frame();
				else if (overrideFrame)
					frame.Set(0, 0, overrideFrame->right, -1);
				else
					frame.Set(0, 0, 0, -1);

				BFont font;
				GetFont(&font);

				// Loop over all items to set their top, bottom and left coordinates,
				// all while computing the width of the menu
				for (; index < fItems.CountItems(); index++) {
					BMenuItem *item = ItemAt(index);

					float width;
					float height;
					item->GetContentSize(&width, &height);

					if (item->fModifiers && item->fShortcutChar) {
						width += font.Size();
						if ((item->fModifiers & B_COMMAND_KEY) != 0)
							command = true;

						if ((item->fModifiers & B_CONTROL_KEY) != 0)
							control = true;

						if ((item->fModifiers & B_SHIFT_KEY) != 0)
							shift = true;

						if ((item->fModifiers & B_OPTION_KEY) != 0)
							option = true;
					}

					item->fBounds.left	 = 0.0f;
					item->fBounds.top	 = frame.bottom + 1.0f;
					item->fBounds.bottom = item->fBounds.top + height + fPad.top + fPad.bottom;

					if (item->fSubmenu)
						submenu = true;

					frame.right	 = std::max(frame.right, width + fPad.left + fPad.right);
					frame.bottom = item->fBounds.bottom;
				}

				// Compute the extra space needed for shortcuts and submenus
				if (command) {
					// frame.right += BPrivate::MenuPrivate::MenuItemCommand()->Bounds().Width() + 1;
				}
				if (control) {
					// frame.right += BPrivate::MenuPrivate::MenuItemControl()->Bounds().Width() + 1;
				}
				if (option) {
					// frame.right += BPrivate::MenuPrivate::MenuItemOption()->Bounds().Width() + 1;
				}
				if (shift) {
					// frame.right += BPrivate::MenuPrivate::MenuItemShift()->Bounds().Width() + 1;
				}
				if (submenu) {
					frame.right += ItemAt(0)->Frame().Height() / 2;
					fHasSubmenus = true;
				}
				else {
					fHasSubmenus = false;
				}

				if (fMaxContentWidth > 0)
					frame.right = std::min(frame.right, fMaxContentWidth);

				frame.top	= 0;
				frame.right = ceilf(frame.right);

				// Finally update the "right" coordinate of all items
				if (moveItems) {
					for (int32 i = 0; i < fItems.CountItems(); i++)
						ItemAt(i)->fBounds.right = frame.right;
				}
			}
			break;
		}

		case B_ITEMS_IN_ROW: {
			font_height fh;
			GetFontHeight(&fh);
			frame.Set(0.0f, 0.0f, 0.0f, ceilf(fh.ascent + fh.descent + fPad.top + fPad.bottom));

			ALOGV("count %d", fItems.CountItems());
			for (int32 i = 0; i < fItems.CountItems(); i++) {
				BMenuItem *item = ItemAt(i);

				float width, height;
				item->GetContentSize(&width, &height);

				item->fBounds.left	= frame.right;
				item->fBounds.top	= 0.0f;
				item->fBounds.right = item->fBounds.left + width + fPad.left + fPad.right;

				frame.right	 = item->Frame().right + 1.0f;
				frame.bottom = std::max(frame.bottom, height + fPad.top + fPad.bottom);
			}

			if (moveItems) {
				for (int32 i = 0; i < fItems.CountItems(); i++)
					ItemAt(i)->fBounds.bottom = frame.bottom;
			}

			if (bestFit)
				frame.right = ceilf(frame.right);
			else
				frame.right = Bounds().right;
		} break;

		case B_ITEMS_IN_MATRIX: {
			frame.Set(0, 0, 0, 0);
			for (int32 i = 0; i < CountItems(); i++) {
				BMenuItem *item = ItemAt(i);
				if (item) {
					frame.left	 = std::min(frame.left, item->Frame().left);
					frame.right	 = std::max(frame.right, item->Frame().right);
					frame.top	 = std::min(frame.top, item->Frame().top);
					frame.bottom = std::max(frame.bottom, item->Frame().bottom);
				}
			}
		} break;
	}

	// change width depending on resize mode
	BSize size;
	if ((ResizingMode() & B_FOLLOW_LEFT_RIGHT) == B_FOLLOW_LEFT_RIGHT) {
		if (Parent())
			size.width = Parent()->Frame().Width();
		else if (Window())
			size.width = Window()->Frame().Width();
		else
			size.width = Bounds().Width();
	}
	else
		size.width = frame.Width();

	size.height = frame.Height();

	if (_width)
		*_width = size.width;

	if (_height)
		*_height = size.height;

	if (bestFit)
		fExtraMenuData->preferred = size;

	if (moveItems)
		fUseCachedMenuLayout = true;
}

BRect BMenu::_CalcFrame(BPoint where, bool *scrollOn)
{
	// TODO: Improve me
	BRect bounds = Bounds();
	BRect frame	 = bounds.OffsetToCopy(where);

	BScreen screen(Window());
	BRect	screenFrame = screen.Frame();

	BMenu	  *superMenu = Supermenu();
	BMenuItem *superItem = Superitem();

	// Reset frame shifted state since this menu is being redrawn
	fExtraMenuData->frameShiftedLeft = false;

	// TODO: Horrible hack:
	// When added to a BMenuField, a BPopUpMenu is the child of
	// a _BMCMenuBar_ to "fake" the menu hierarchy
	// bool inMenuField = dynamic_cast<_BMCMenuBar_ *>(superMenu);
	bool inMenuField = false;

	// Offset the menu field menu window left by the width of the checkmark
	// so that the text when the menu is closed lines up with the text when
	// the menu is open.
	if (inMenuField)
		frame.OffsetBy(-8.0f, 0.0f);

	if (superMenu == NULL || superItem == NULL || inMenuField) {
		// just move the window on screen
		if (frame.bottom > screenFrame.bottom)
			frame.OffsetBy(0, screenFrame.bottom - frame.bottom);
		else if (frame.top < screenFrame.top)
			frame.OffsetBy(0, -frame.top);

		if (frame.right > screenFrame.right) {
			frame.OffsetBy(screenFrame.right - frame.right, 0);
			fExtraMenuData->frameShiftedLeft = true;
		}
		else if (frame.left < screenFrame.left)
			frame.OffsetBy(-frame.left, 0);
	}
	else if (superMenu->Layout() == B_ITEMS_IN_COLUMN) {
		if (frame.right > screenFrame.right || superMenu->fExtraMenuData->frameShiftedLeft) {
			frame.OffsetBy(-superItem->Frame().Width() - frame.Width() - 2, 0);
			fExtraMenuData->frameShiftedLeft = true;
		}

		if (frame.left < 0)
			frame.OffsetBy(-frame.left + 6, 0);

		if (frame.bottom > screenFrame.bottom)
			frame.OffsetBy(0, screenFrame.bottom - frame.bottom);
	}
	else {
		if (frame.bottom > screenFrame.bottom) {
			float spaceBelow = screenFrame.bottom - frame.top;
			float spaceOver	 = frame.top - screenFrame.top - superItem->Frame().Height();
			if (spaceOver > spaceBelow) {
				frame.OffsetBy(0, -superItem->Frame().Height() - frame.Height() - 3);
			}
		}

		if (frame.right > screenFrame.right)
			frame.OffsetBy(screenFrame.right - frame.right, 0);
	}

	if (scrollOn) {
		// basically, if this returns false, it means
		// that the menu frame won't fit completely inside the screen
		// TODO: Scrolling will currently only work up/down,
		// not left/right
		*scrollOn = screenFrame.top > frame.top || screenFrame.bottom < frame.bottom;
	}

	return frame;
}

bool BMenu::_OverSuper(BPoint location)
{
	if (!Supermenu())
		return false;

	return fSuperbounds.Contains(location);
}

bool BMenu::_OverSubmenu(BMenuItem *item, BPoint loc)
{
	if (item == NULL)
		return false;

	BMenu *subMenu = item->Submenu();
	if (subMenu == NULL || subMenu->Window() == NULL)
		return false;

	// assume that loc is in screen coordinates
	if (subMenu->Window()->Frame().Contains(loc))
		return true;

	return subMenu->_OverSubmenu(subMenu->fSelected, loc);
}

BMenuWindow *BMenu::_MenuWindow()
{
#if USE_CACHED_MENUWINDOW
	if (fCachedMenuWindow == NULL) {
		char windowName[64];
		snprintf(windowName, 64, "%s cached menu", Name());
		fCachedMenuWindow = new (nothrow) BMenuWindow(windowName);
	}
#endif
	return fCachedMenuWindow;
}

void BMenu::_DeleteMenuWindow()
{
	if (fCachedMenuWindow) {
		fCachedMenuWindow->Lock();
		fCachedMenuWindow->Quit();
		fCachedMenuWindow = nullptr;
	}
}

BMenuItem *BMenu::_HitTestItems(BPoint where, BPoint slop) const
{
	// TODO: Take "slop" into account ?

	// if the point doesn't lie within the menu's
	// bounds, bail out immediately
	if (!Bounds().Contains(where))
		return nullptr;

	int32 itemCount = CountItems();
	for (int32 i = 0; i < itemCount; i++) {
		BMenuItem *item = ItemAt(i);
		if (item->Frame().Contains(where) && dynamic_cast<BSeparatorItem *>(item) == NULL) {
			return item;
		}
	}

	return nullptr;
}

void BMenu::_CacheFontInfo()
{
	font_height fh;
	GetFontHeight(&fh);
	fAscent		= fh.ascent;
	fDescent	= fh.descent;
	fFontHeight = ceilf(fh.ascent + fh.descent + fh.leading);
}

void BMenu::_ItemMarked(BMenuItem *item)
{
	if (IsRadioMode()) {
		for (int32 i = 0; i < CountItems(); i++) {
			if (ItemAt(i) != item)
				ItemAt(i)->SetMarked(false);
		}
	}

	if (IsLabelFromMarked() && Superitem())
		Superitem()->SetLabel(item->Label());
}

void BMenu::_Install(BWindow *target)
{
	for (int32 i = 0; i < CountItems(); i++)
		ItemAt(i)->Install(target);
}

void BMenu::_Uninstall()
{
	for (int32 i = 0; i < CountItems(); i++)
		ItemAt(i)->Uninstall();
}

void BMenu::_SelectItem(BMenuItem *item, bool showSubmenu, bool selectFirstItem, bool keyDown)
{
	// Avoid deselecting and then reselecting the same item
	// which would cause flickering
	if (item != fSelected) {
		if (fSelected) {
			fSelected->Select(false);
			BMenu *subMenu = fSelected->Submenu();
			if (subMenu && subMenu->Window())
				subMenu->_Hide();
		}

		fSelected = item;
		if (fSelected)
			fSelected->Select(true);
	}

	if (fSelected && showSubmenu) {
		BMenu *subMenu = fSelected->Submenu();
		if (subMenu && subMenu->Window() == NULL) {
			if (!subMenu->_Show(selectFirstItem, keyDown)) {
				// something went wrong, deselect the item
				fSelected->Select(false);
				fSelected = nullptr;
			}
		}
	}
}

bool BMenu::_SelectNextItem(BMenuItem *item, bool forward)
{
	if (CountItems() == 0)	// cannot select next item in an empty menu
		return false;

	BMenuItem *nextItem = _NextItem(item, forward);
	if (nextItem == NULL)
		return false;

	_SelectItem(nextItem, dynamic_cast<BMenuBar *>(this));

	if (LockLooper()) {
		be_app->ObscureCursor();
		UnlockLooper();
	}

	return true;
}

BMenuItem *BMenu::_NextItem(BMenuItem *item, bool forward) const
{
	const int32 numItems = fItems.CountItems();
	if (numItems == 0)
		return nullptr;

	int32 index		= fItems.IndexOf(item);
	int32 loopCount = numItems;
	while (--loopCount) {
		// Cycle through menu items in the given direction...
		if (forward)
			index++;
		else
			index--;

		// ... wrap around...
		if (index < 0)
			index = numItems - 1;
		else if (index >= numItems)
			index = 0;

		// ... and return the first suitable item found.
		BMenuItem *nextItem = ItemAt(index);
		if (nextItem->IsEnabled())
			return nextItem;
	}

	// If no other suitable item was found, return NULL.
	return nullptr;
}

void BMenu::_SetStickyMode(bool sticky)
{
	if (fStickyMode == sticky)
		return;

	fStickyMode = sticky;

	if (fSuper) {
		// propagate the status to the super menu
		fSuper->_SetStickyMode(sticky);
	}
	else {
		// TODO: Ugly hack, but it needs to be done in this method
		BMenuBar *menuBar = dynamic_cast<BMenuBar *>(this);
		if (sticky && menuBar && menuBar->LockLooper()) {
			// If we are switching to sticky mode,
			// steal the focus from the current focus view
			// (needed to handle keyboard navigation)
			// menuBar->_StealFocus(); FIXME: re-enable
			menuBar->UnlockLooper();
		}
	}
}

bool BMenu::_IsStickyMode() const
{
	return fStickyMode;
}

void BMenu::_CalcTriggers()
{
	BList triggersList;

	// Gathers the existing triggers set by the user
	for (int32 i = 0; i < CountItems(); i++) {
		char trigger = ItemAt(i)->Trigger();
		if (trigger != 0)
			triggersList.AddItem(reinterpret_cast<void *>((uintptr_t)trigger));
	}

	// Set triggers for items which don't have one yet
	for (int32 i = 0; i < CountItems(); i++) {
		BMenuItem *item = ItemAt(i);
		if (item->Trigger() == 0) {
			const char *newTrigger = _ChooseTrigger(item->Label(), &triggersList);
			if (newTrigger)
				item->fSysTrigger = *newTrigger;
		}
	}
}

const char *BMenu::_ChooseTrigger(const char *title, BList *chars)
{
	LOG_ALWAYS_FATAL_IF(!chars, "chars cannot be null");

	if (!title)
		return nullptr;

	char trigger;

	// two runs: first we look out for alphanumeric ASCII characters
	while ((trigger = title[0]) != '\0') {
		if (isalpha(trigger) && !chars->HasItem(reinterpret_cast<void *>((uintptr_t)trigger))) {
			chars->AddItem(reinterpret_cast<void *>((uintptr_t)trigger));
			return title;
		}

		title++;
	}

	// then, if we still haven't found something, we accept anything
	while ((trigger = title[0]) != '\0') {
		if (!isspace(trigger) && !chars->HasItem(reinterpret_cast<void *>((uintptr_t)trigger))) {
			chars->AddItem(reinterpret_cast<void *>((uintptr_t)trigger));
			return title;
		}

		title++;
	}

	return nullptr;
}

void BMenu::_UpdateWindowViewSize(const bool &move)
{
	BMenuWindow *window = static_cast<BMenuWindow *>(Window());
	if (window == NULL)
		return;

	if (dynamic_cast<BMenuBar *>(this))
		return;

	if (!fResizeToFit)
		return;

	bool		 scroll			= false;
	const BPoint screenLocation = move ? ScreenLocation()
									   : window->Frame().LeftTop();
	BRect		 frame			= _CalcFrame(screenLocation, &scroll);
	ResizeTo(frame.Width(), frame.Height());

	if (fItems.CountItems() > 0) {
		if (!scroll) {
			if (fLayout == B_ITEMS_IN_COLUMN)
				window->DetachScrollers();

			window->ResizeTo(Bounds().Width(), Bounds().Height());
		}
		else {
			// Resize the window to fit the screen without overflowing the
			// frame, and attach scrollers to our cached BMenuWindow.
			BScreen screen(window);
			frame = frame & screen.Frame();
			window->ResizeTo(Bounds().Width(), frame.Height());

			// we currently only support scrolling for B_ITEMS_IN_COLUMN
			if (fLayout == B_ITEMS_IN_COLUMN) {
				window->AttachScrollers();

				BMenuItem *selectedItem = FindMarked();
				if (selectedItem) {
					// scroll to the selected item
					if (Supermenu() == NULL) {
						window->TryScrollTo(selectedItem->Frame().top);
					}
					else {
						BPoint point	  = selectedItem->Frame().LeftTop();
						BPoint superPoint = Superitem()->Frame().LeftTop();
						Supermenu()->ConvertToScreen(&superPoint);
						ConvertToScreen(&point);
						window->TryScrollTo(point.y - superPoint.y);
					}
				}
			}
		}
	}
	else {
		_CacheFontInfo();
		window->ResizeTo(StringWidth(BPrivate::kEmptyMenuLabel) + fPad.left + fPad.right,
						 fFontHeight + fPad.top + fPad.bottom);
	}

	if (move)
		window->MoveTo(frame.LeftTop());
}

bool BMenu::_AddDynamicItems(bool keyDown)
{
	bool addAborted = false;
	if (AddDynamicItem(B_INITIAL_ADD)) {
		BMenuItem *superItem = Superitem();
		BMenu	  *superMenu = Supermenu();
		do {
			if (superMenu
				&& !superMenu->_OkToProceed(superItem, keyDown)) {
				AddDynamicItem(B_ABORT);
				addAborted = true;
				break;
			}
		} while (AddDynamicItem(B_PROCESSING));
	}

	return addAborted;
}

bool BMenu::_OkToProceed(BMenuItem *item, bool keyDown)
{
	BPoint where;
	uint32 buttons;
	GetMouse(&where, &buttons, false);
	bool stickyMode = _IsStickyMode();
	// Quit if user clicks the mouse button in sticky mode
	// or releases the mouse button in nonsticky mode
	// or moves the pointer over another item
	// TODO: I added the check for BMenuBar to solve a problem with Deskbar.
	// BeOS seems to do something similar. This could also be a bug in
	// Deskbar, though.
	if ((buttons != 0 && stickyMode)
		|| ((dynamic_cast<BMenuBar *>(this) == nullptr && (buttons == 0 && !stickyMode))
			|| ((_HitTestItems(where) != item) && !keyDown))) {
		return false;
	}

	return true;
}

void BMenu::_InvokeItem(BMenuItem *item, bool now)
{
	if (!item->IsEnabled())
		return;

	// Do the "selected" animation
	// TODO: Doesn't work. This is supposed to highlight
	// and dehighlight the item, works on beos but not on haiku.
	if (!item->Submenu() && LockLooper()) {
		snooze(50000);
		item->Select(true);
		Window()->UpdateIfNeeded();
		snooze(50000);
		item->Select(false);
		Window()->UpdateIfNeeded();
		snooze(50000);
		item->Select(true);
		Window()->UpdateIfNeeded();
		snooze(50000);
		item->Select(false);
		Window()->UpdateIfNeeded();
		UnlockLooper();
	}

	// Lock the root menu window before calling BMenuItem::Invoke()
	BMenu *parent	= this;
	BMenu *rootMenu = NULL;
	do {
		rootMenu = parent;
		parent	 = rootMenu->Supermenu();
	} while (parent);

	if (rootMenu->LockLooper()) {
		item->Invoke();
		rootMenu->UnlockLooper();
	}
}

void BMenu::_QuitTracking(bool onlyThis)
{
	_SelectItem(nullptr);
	// if (BMenuBar *menuBar = dynamic_cast<BMenuBar *>(this))
	// 	menuBar->_RestoreFocus(); FIXME: re-enable

	fState = MENU_STATE_CLOSED;

	if (!onlyThis) {
		// Close the whole menu hierarchy
		if (Supermenu())
			Supermenu()->fState = MENU_STATE_CLOSED;

		if (_IsStickyMode())
			_SetStickyMode(false);

		if (LockLooper()) {
			be_app->ShowCursor();
			UnlockLooper();
		}
	}

	_Hide();
}

#pragma mark - mouse tracking

const static bigtime_t kOpenSubmenuDelay	  = 0;
const static bigtime_t kNavigationAreaTimeout = 1000000;

BMenuItem *
BMenu::_Track(int *action, long start)
{
	// TODO: cleanup
	BMenuItem *item = nullptr;
	BRect	   navAreaRectAbove;
	BRect	   navAreaRectBelow;
	bigtime_t  selectedTime		  = system_time();
	bigtime_t  navigationAreaTime = 0;

	fState		= MENU_STATE_TRACKING;
	fChosenItem = nullptr;	// we will use this for keyboard selection

	BPoint location;
	uint32 buttons = 0;
	if (LockLooper()) {
		GetMouse(&location, &buttons);
		UnlockLooper();
	}

	bool releasedOnce = buttons == 0;
	while (fState != MENU_STATE_CLOSED) {
		// if (_CustomTrackingWantsToQuit())
		// 	break;

		if (!LockLooper())
			break;

		BMenuWindow *window			= static_cast<BMenuWindow *>(Window());
		BPoint		 screenLocation = ConvertToScreen(location);
		if (window->CheckForScrolling(screenLocation)) {
			UnlockLooper();
			continue;
		}

		// The order of the checks is important
		// to be able to handle overlapping menus:
		// first we check if mouse is inside a submenu,
		// then if the mouse is inside this menu,
		// then if it's over a super menu.
		if (_OverSubmenu(fSelected, screenLocation)
			|| fState == MENU_STATE_KEY_TO_SUBMENU) {
			if (fState == MENU_STATE_TRACKING) {
				// not if from R.Arrow
				fState = MENU_STATE_TRACKING_SUBMENU;
			}
			navAreaRectAbove = BRect();
			navAreaRectBelow = BRect();

			// Since the submenu has its own looper,
			// we can unlock ours. Doing so also make sure
			// that our window gets any update message to
			// redraw itself
			UnlockLooper();

			// To prevent NULL access violation, ensure a menu has actually
			// been selected and that it has a submenu. Because keyboard and
			// mouse interactions set selected items differently, the menu
			// tracking thread needs to be careful in triggering the navigation
			// to the submenu.
			if (fSelected) {
				BMenu *submenu		 = fSelected->Submenu();
				int	   submenuAction = MENU_STATE_TRACKING;
				if (submenu) {
					submenu->_SetStickyMode(_IsStickyMode());

					// The following call blocks until the submenu
					// gives control back to us, either because the mouse
					// pointer goes out of the submenu's bounds, or because
					// the user closes the menu
					BMenuItem *submenuItem = submenu->_Track(&submenuAction);
					if (submenuAction == MENU_STATE_CLOSED) {
						item   = submenuItem;
						fState = MENU_STATE_CLOSED;
					}
					else if (submenuAction == MENU_STATE_KEY_LEAVE_SUBMENU) {
						if (LockLooper()) {
							BMenuItem *temp = fSelected;
							// close the submenu:
							_SelectItem(nullptr);
							// but reselect the item itself for user:
							_SelectItem(temp, false);
							UnlockLooper();
						}
						// cancel  key-nav state
						fState = MENU_STATE_TRACKING;
					}
					else
						fState = MENU_STATE_TRACKING;
				}
			}
			if (!LockLooper())
				break;
		}
		else if ((item = _HitTestItems(location, B_ORIGIN))) {
			_UpdateStateOpenSelect(item, location, navAreaRectAbove,
								   navAreaRectBelow, selectedTime, navigationAreaTime);
			releasedOnce = true;
		}
		else if (_OverSuper(screenLocation)
				 && fSuper->fState != MENU_STATE_KEY_TO_SUBMENU) {
			fState = MENU_STATE_TRACKING;
			UnlockLooper();
			break;
		}
		else if (fState == MENU_STATE_KEY_LEAVE_SUBMENU) {
			UnlockLooper();
			break;
		}
		else if (fSuper == NULL || fSuper->fState != MENU_STATE_KEY_TO_SUBMENU) {
			// Mouse pointer outside menu:
			// If there's no other submenu opened,
			// deselect the current selected item
			if (fSelected
				&& (fSelected->Submenu() == NULL || fSelected->Submenu()->Window() == NULL)) {
				_SelectItem(nullptr);
				fState = MENU_STATE_TRACKING;
			}

			if (fSuper) {
				// Give supermenu the chance to continue tracking
				*action = fState;
				UnlockLooper();
				return nullptr;
			}
		}

		UnlockLooper();

		if (releasedOnce)
			_UpdateStateClose(item, location, buttons);

		if (fState != MENU_STATE_CLOSED) {
			bigtime_t snoozeAmount = 50000;

			BPoint newLocation = location;
			uint32 newButtons  = buttons;

			// If user doesn't move the mouse, loop here,
			// so we don't interfere with keyboard menu navigation
			do {
				snooze(snoozeAmount);
				if (!LockLooper())
					break;
				GetMouse(&newLocation, &newButtons, true);
				UnlockLooper();
			} while (newLocation == location && newButtons == buttons
					 && !(item && item->Submenu() && item->Submenu()->Window() == NULL)
					 && fState == MENU_STATE_TRACKING);

			if (newLocation != location || newButtons != buttons) {
				if (!releasedOnce && newButtons == 0 && buttons != 0)
					releasedOnce = true;
				location = newLocation;
				buttons	 = newButtons;
			}

			if (releasedOnce)
				_UpdateStateClose(item, location, buttons);
		}
	}

	if (action)
		*action = fState;

	// keyboard Enter will set this
	if (fChosenItem)
		item = fChosenItem;
	else if (fSelected == NULL) {
		// needed to cover (rare) mouse/ESC combination
		item = nullptr;
	}

	if (fSelected && LockLooper()) {
		_SelectItem(nullptr);
		UnlockLooper();
	}

	// delete the menu window recycled for all the child menus
	_DeleteMenuWindow();

	return item;
}

void BMenu::_UpdateNavigationArea(BPoint position, BRect &navAreaRectAbove, BRect &navAreaRectBelow)
{
#define NAV_AREA_THRESHOLD 8

	// The navigation area is a region in which mouse-overs won't select
	// the item under the cursor. This makes it easier to navigate to
	// submenus, as the cursor can be moved to submenu items directly instead
	// of having to move it horizontally into the submenu first. The concept
	// is illustrated below:
	//
	// +-------+----+---------+
	// |       |   /|         |
	// |       |  /*|         |
	// |[2]--> | /**|         |
	// |       |/[4]|         |
	// |------------|         |
	// |    [1]     |   [6]   |
	// |------------|         |
	// |       |\[5]|         |
	// |[3]--> | \**|         |
	// |       |  \*|         |
	// |       |   \|         |
	// |       +----|---------+
	// |            |
	// +------------+
	//
	// [1] Selected item, cursor position ('position')
	// [2] Upper navigation area rectangle ('navAreaRectAbove')
	// [3] Lower navigation area rectangle ('navAreaRectBelow')
	// [4] Upper navigation area
	// [5] Lower navigation area
	// [6] Submenu
	//
	// The rectangles are used to calculate if the cursor is in the actual
	// navigation area (see _UpdateStateOpenSelect()).

	if (fSelected == NULL)
		return;

	BMenu *submenu = fSelected->Submenu();

	if (submenu) {
		BRect menuBounds = ConvertToScreen(Bounds());

		BRect submenuBounds;
		if (fSelected->Submenu()->LockLooper()) {
			submenuBounds = fSelected->Submenu()->ConvertToScreen(fSelected->Submenu()->Bounds());
			fSelected->Submenu()->UnlockLooper();
		}

		if (menuBounds.left < submenuBounds.left) {
			navAreaRectAbove.Set(position.x + NAV_AREA_THRESHOLD, submenuBounds.top, menuBounds.right, position.y);
			navAreaRectBelow.Set(position.x + NAV_AREA_THRESHOLD, position.y, menuBounds.right, submenuBounds.bottom);
		}
		else {
			navAreaRectAbove.Set(menuBounds.left, submenuBounds.top, position.x - NAV_AREA_THRESHOLD, position.y);
			navAreaRectBelow.Set(menuBounds.left, position.y, position.x - NAV_AREA_THRESHOLD, submenuBounds.bottom);
		}
	}
	else {
		navAreaRectAbove = BRect();
		navAreaRectBelow = BRect();
	}
}

void BMenu::_UpdateStateOpenSelect(BMenuItem *item, BPoint position,
								   BRect &navAreaRectAbove, BRect &navAreaRectBelow, bigtime_t &selectedTime,
								   bigtime_t &navigationAreaTime)
{
	if (fState == MENU_STATE_CLOSED)
		return;

	if (item != fSelected) {
		if (navigationAreaTime == 0)
			navigationAreaTime = system_time();

		position = ConvertToScreen(position);

		bool inNavAreaRectAbove = navAreaRectAbove.Contains(position);
		bool inNavAreaRectBelow = navAreaRectBelow.Contains(position);

		if (fSelected == NULL
			|| (!inNavAreaRectAbove && !inNavAreaRectBelow)) {
			_SelectItem(item, false);
			navAreaRectAbove   = BRect();
			navAreaRectBelow   = BRect();
			selectedTime	   = system_time();
			navigationAreaTime = 0;
			return;
		}

		bool   isLeft = ConvertFromScreen(navAreaRectAbove).left == 0;
		BPoint p1, p2;

		if (inNavAreaRectAbove) {
			if (!isLeft) {
				p1 = navAreaRectAbove.LeftBottom();
				p2 = navAreaRectAbove.RightTop();
			}
			else {
				p2 = navAreaRectAbove.RightBottom();
				p1 = navAreaRectAbove.LeftTop();
			}
		}
		else {
			if (!isLeft) {
				p2 = navAreaRectBelow.LeftTop();
				p1 = navAreaRectBelow.RightBottom();
			}
			else {
				p1 = navAreaRectBelow.RightTop();
				p2 = navAreaRectBelow.LeftBottom();
			}
		}
		bool inNavArea = (p1.y - p2.y) * position.x + (p2.x - p1.x) * position.y
							 + (p1.x - p2.x) * p1.y + (p2.y - p1.y) * p1.x
						 >= 0;

		bigtime_t systime = system_time();

		if (!inNavArea || (navigationAreaTime > 0 && systime - navigationAreaTime > kNavigationAreaTimeout)) {
			// Don't delay opening of submenu if the user had
			// to wait for the navigation area timeout anyway
			_SelectItem(item, inNavArea);

			if (inNavArea) {
				_UpdateNavigationArea(position, navAreaRectAbove, navAreaRectBelow);
			}
			else {
				navAreaRectAbove = BRect();
				navAreaRectBelow = BRect();
			}

			selectedTime	   = system_time();
			navigationAreaTime = 0;
		}
	}
	else if (fSelected->Submenu() && system_time() - selectedTime > kOpenSubmenuDelay) {
		_SelectItem(fSelected, true);

		if (!navAreaRectAbove.IsValid() && !navAreaRectBelow.IsValid()) {
			position = ConvertToScreen(position);
			_UpdateNavigationArea(position, navAreaRectAbove, navAreaRectBelow);
		}
	}

	if (fState != MENU_STATE_TRACKING)
		fState = MENU_STATE_TRACKING;
}

void BMenu::_UpdateStateClose(BMenuItem *item, const BPoint &where, const uint32 &buttons)
{
	if (fState == MENU_STATE_CLOSED)
		return;

	if (buttons != 0 && _IsStickyMode()) {
		if (item == NULL) {
			if (item != fSelected && LockLooper()) {
				_SelectItem(item, false);
				UnlockLooper();
			}
			fState = MENU_STATE_CLOSED;
		}
		else
			_SetStickyMode(false);
	}
	else if (buttons == 0 && !_IsStickyMode()) {
		if (fExtraRect && fExtraRect->Contains(where)) {
			_SetStickyMode(true);
			fExtraRect = nullptr;
			// Setting this to NULL will prevent this code
			// to be executed next time
		}
		else {
			if (item != fSelected && LockLooper()) {
				_SelectItem(item, false);
				UnlockLooper();
			}
			fState = MENU_STATE_CLOSED;
		}
	}
}
