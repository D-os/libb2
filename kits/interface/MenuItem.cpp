#include "MenuItem.h"

#define LOG_TAG "BMenuItem"

#include <ControlLook.h>
#include <String.h>
#include <Window.h>
#include <log/log.h>

#define BMENU_PADDING_LEFT 12.0f
#define BMENU_PADDING_TOP 8.0f

static const float kMarkTint = 0.75f;

// map control key shortcuts to drawable Unicode characters
// cf. http://unicode.org/charts/PDF/U2190.pdf
const char *kUTF8ControlMap[] = {
	NULL,
	"\xe2\x86\xb8", /* B_HOME U+21B8 */
	NULL, NULL,
	NULL, /* B_END */
	NULL, /* B_INSERT */
	NULL, NULL,
	NULL,			/* B_BACKSPACE */
	"\xe2\x86\xb9", /* B_TAB U+21B9 */
	"\xe2\x8f\x8e", /* B_ENTER, U+23CE */
	NULL,			/* B_PAGE_UP */
	NULL,			/* B_PAGE_DOWN */
	NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL,
	"\xe2\x86\x90", /* B_LEFT_ARROW */
	"\xe2\x86\x92", /* B_RIGHT_ARROW */
	"\xe2\x86\x91", /* B_UP_ARROW */
	"\xe2\x86\x93", /* B_DOWN_ARROW */
};

BMenuItem::BMenuItem(const char *label, BMessage *message, char shortcut, uint32 modifiers)
	: BInvoker(message, BMessenger()),
	  fLabel(label ? strdup(label) : nullptr),
	  fSubmenu{nullptr},
	  fWindow{nullptr},
	  fSuper{nullptr},
	  fModifiers(shortcut ? modifiers | B_COMMAND_KEY : 0),
	  fCachedWidth{0.0},
	  fTriggerIndex{-1},
	  fUserTrigger{0},
	  fSysTrigger{0},
	  fShortcutChar(shortcut),
	  fMark{false},
	  fEnabled{true},
	  fSelected{false}
{
}

BMenuItem::BMenuItem(BMenu *menu, BMessage *message)
	: BMenuItem(nullptr, message, '\0', 0)
{
	LOG_ALWAYS_FATAL_IF(!menu, "menu cannot be null");

	fSubmenu = menu;

	fSubmenu->fSuperitem = this;

	BMenuItem *item = menu->FindMarked();

	if (menu->IsRadioMode() && menu->IsLabelFromMarked() && item)
		SetLabel(item->Label());
	else
		SetLabel(menu->Name());
}

BMenuItem::BMenuItem(BMessage *data) {}

BMenuItem::~BMenuItem()
{
	if (fSuper)
		fSuper->RemoveItem(this);

	free(fLabel);
	delete fSubmenu;
}

status_t BMenuItem::Archive(BMessage *data, bool deep) const
{
	return BArchivable::Archive(data, deep);
}

void BMenuItem::SetLabel(const char *name)
{
	free(fLabel);
	fLabel = name ? strdup(name) : nullptr;
}

void BMenuItem::SetEnabled(bool state)
{
	if (fEnabled == state)
		return;

	fEnabled = state;

	if (fSubmenu)
		fSubmenu->SetEnabled(state);

	BMenu *menu = fSuper;
	if (menu && menu->LockLooper()) {
		menu->Invalidate(fBounds);
		menu->UnlockLooper();
	}
}

void BMenuItem::SetMarked(bool state)
{
	fMark = state;

	if (state && fSuper) {
		fSuper->_ItemMarked(this);
	}
}

void BMenuItem::SetTrigger(char trigger)
{
	fUserTrigger = trigger;

	// try uppercase letters first
	const char *pos = strchr(Label(), toupper(trigger));
	trigger			= tolower(trigger);

	if (pos == NULL) {
		// take lowercase, too
		pos = strchr(Label(), trigger);
	}

	if (pos) {
		fTriggerIndex = pos - Label();
	}
	else {
		fUserTrigger  = 0;
		fTriggerIndex = -1;
	}

	if (fSuper)
		fSuper->InvalidateLayout();
}

void BMenuItem::SetShortcut(char ch, uint32 modifiers)
{
	if (fShortcutChar != 0 && (fModifiers & B_COMMAND_KEY) != 0 && fWindow) {
		fWindow->RemoveShortcut(fShortcutChar, fModifiers);
	}

	fShortcutChar = ch;

	if (ch != 0)
		fModifiers = modifiers | B_COMMAND_KEY;
	else
		fModifiers = 0;

	if (fShortcutChar != 0 && (fModifiers & B_COMMAND_KEY) && fWindow)
		fWindow->AddShortcut(fShortcutChar, fModifiers, this);

	if (fSuper) {
		fSuper->InvalidateLayout();

		if (fSuper->LockLooper()) {
			fSuper->Invalidate();
			fSuper->UnlockLooper();
		}
	}
}

const char *BMenuItem::Label() const
{
	return fLabel;
}

bool BMenuItem::IsEnabled() const
{
	if (fSubmenu)
		return fSubmenu->IsEnabled();

	if (!fEnabled)
		return false;

	return fSuper ? fSuper->IsEnabled() : true;
}

bool BMenuItem::IsMarked() const
{
	return fMark;
}

char BMenuItem::Trigger() const
{
	return fUserTrigger;
}

char BMenuItem::Shortcut(uint32 *modifiers) const
{
	if (modifiers)
		*modifiers = fModifiers;

	return fShortcutChar;
}

BMenu *BMenuItem::Submenu() const
{
	return fSubmenu;
}

BMenu *BMenuItem::Menu() const
{
	return fSuper;
}

BRect BMenuItem::Frame() const
{
	return fBounds;
}

void BMenuItem::GetContentSize(float *_width, float *_height)
{
	fCachedWidth = fSuper->StringWidth(fLabel);

	if (_width)
		*_width = ceilf(fCachedWidth);
	if (_height)
		*_height = fSuper->fFontHeight;
}

void BMenuItem::TruncateLabel(float maxWidth, char *newLabel)
{
	BFont font;
	fSuper->GetFont(&font);

	BString string(fLabel);

	font.TruncateString(&string, B_TRUNCATE_MIDDLE, maxWidth);

	string.CopyInto(newLabel, 0, string.Length());
	newLabel[string.Length()] = '\0';
}

void BMenuItem::DrawContent()
{
	if (!fSuper)
		return;

	fSuper->_CacheFontInfo();

	fSuper->MovePenBy(0, fSuper->fAscent);
	BPoint lineStart = fSuper->PenLocation();

	fSuper->SetDrawingMode(B_OP_OVER);

	float labelWidth;
	float labelHeight;
	GetContentSize(&labelWidth, &labelHeight);

	const BRect &padding		 = fSuper->fPad;
	float		 maxContentWidth = fSuper->MaxContentWidth();
	float		 frameWidth		 = maxContentWidth > 0 ? maxContentWidth
													   : fSuper->Frame().Width() - padding.left - padding.right;

	if (roundf(frameWidth) >= roundf(labelWidth))
		fSuper->DrawString(fLabel);
	else {
		// truncate label to fit
		char *truncatedLabel = new char[strlen(fLabel) + 4];
		TruncateLabel(frameWidth, truncatedLabel);
		fSuper->DrawString(truncatedLabel);
		delete[] truncatedLabel;
	}

	if (fSuper->AreTriggersEnabled() && fTriggerIndex != -1) {
		float escapements[fTriggerIndex + 1];
		BFont font;
		fSuper->GetFont(&font);

		font.GetEscapements(fLabel, fTriggerIndex + 1, escapements);

		for (int32 i = 0; i < fTriggerIndex; i++)
			lineStart.x += escapements[i] * font.Size();

		lineStart.x--;
		lineStart.y++;

		BPoint lineEnd(lineStart);
		lineEnd.x += escapements[fTriggerIndex] * font.Size();

		fSuper->StrokeLine(lineStart, lineEnd);
	}
}

void BMenuItem::Draw()
{
	const rgb_color lowColor  = fSuper->LowColor();
	const rgb_color highColor = fSuper->HighColor();

	fSuper->SetLowColor(_LowColor());
	fSuper->SetHighColor(_HighColor());

	if (_IsActivated()) {
		// fill in the background
		BRect frame(Frame());
		be_control_look->DrawMenuItemBackground(fSuper, frame, frame,
												fSuper->LowColor(), BControlLook::B_ACTIVATED);
	}

	// draw content
	fSuper->MovePenTo(ContentLocation());
	DrawContent();

	// draw extra symbols
	const menu_layout layout = fSuper->Layout();
	if (layout == B_ITEMS_IN_COLUMN) {
		if (IsMarked())
			_DrawMarkSymbol();

		if (fShortcutChar)
			_DrawShortcutSymbol(fSuper->fHasSubmenus);

		if (Submenu() != NULL)
			_DrawSubmenuSymbol();
	}

	// restore the parent menu's low color and high color
	fSuper->SetLowColor(lowColor);
	fSuper->SetHighColor(highColor);
}

void BMenuItem::Highlight(bool on)
{
	fSuper->Invalidate(Frame());
}

bool BMenuItem::IsSelected() const
{
	return fSelected;
}

BPoint BMenuItem::ContentLocation() const
{
	return BPoint(fBounds.left + BMENU_PADDING_LEFT, fBounds.top + BMENU_PADDING_TOP);
}

void BMenuItem::Install(BWindow *window)
{
	if (fSubmenu)
		fSubmenu->_Install(window);

	fWindow = window;

	if (fShortcutChar != 0 && (fModifiers & B_COMMAND_KEY) && fWindow)
		window->AddShortcut(fShortcutChar, fModifiers, this);

	if (!Messenger().IsValid())
		SetTarget(window);
}

void BMenuItem::Uninstall()
{
	if (fSubmenu)
		fSubmenu->_Uninstall();

	if (Target() == fWindow)
		SetTarget(BMessenger());

	if (fShortcutChar != 0 && (fModifiers & B_COMMAND_KEY) != 0
		&& fWindow) {
		fWindow->RemoveShortcut(fShortcutChar, fModifiers);
	}

	fWindow = nullptr;
}

void BMenuItem::SetSuper(BMenu *super)
{
	if (fSuper && super) {
		debugger(
			"Error - can't add menu or menu item to more than 1 container (either menu or menubar).");
	}

	if (fSubmenu)
		fSubmenu->fSuper = super;

	fSuper = super;
}

void BMenuItem::Select(bool selected)
{
	if (fSelected == selected)
		return;

	if (Submenu() || IsEnabled()) {
		fSelected = selected;
		Highlight(selected);
	}
}

status_t BMenuItem::Invoke(BMessage *msg)
{
	if (!IsEnabled())
		return B_ERROR;

	if (fSuper->IsRadioMode())
		SetMarked(true);

	bool   notify = false;
	uint32 kind	  = InvokeKind(&notify);

	BMessage clone(kind);
	status_t err = B_BAD_VALUE;

	if (!msg && !notify)
		msg = Message();

	if (!msg) {
		if (!fSuper->IsWatched())
			return err;
	}
	else
		clone = *msg;

	clone.AddInt32("index", fSuper->IndexOf(this));
	clone.AddInt64("when", (int64)system_time());
	clone.AddPointer("source", this);
	clone.AddMessenger("be:sender", BMessenger(fSuper));

	if (msg)
		err = BInvoker::Invoke(&clone);

	// TODO: assynchronous messaging
	// SendNotices(kind, &clone);

	return err;
}

bool BMenuItem::_IsActivated()
{
	return IsSelected() && (IsEnabled() || fSubmenu != NULL);
}

rgb_color
BMenuItem::_LowColor()
{
	return _IsActivated() ? ui_color(B_MENU_SELECTED_BACKGROUND_COLOR)
						  : ui_color(B_MENU_BACKGROUND_COLOR);
}

rgb_color
BMenuItem::_HighColor()
{
	rgb_color highColor;

	bool isEnabled	= IsEnabled();
	bool isSelected = IsSelected();

	if (isEnabled && isSelected)
		highColor = ui_color(B_MENU_SELECTED_ITEM_TEXT_COLOR);
	else if (isEnabled)
		highColor = ui_color(B_MENU_ITEM_TEXT_COLOR);
	else {
		rgb_color bgColor = fSuper->LowColor();
		if (bgColor.red + bgColor.green + bgColor.blue > 128 * 3)
			highColor = tint_color(bgColor, B_DISABLED_LABEL_TINT);
		else
			highColor = tint_color(bgColor, B_LIGHTEN_2_TINT);
	}

	return highColor;
}

void BMenuItem::_DrawMarkSymbol()
{
	fSuper->PushState();

	BRect r(fBounds);
	float leftMargin;
	fSuper->GetItemMargins(&leftMargin, NULL, NULL, NULL);
	float gap = leftMargin / 4;
	r.right	  = r.left + leftMargin - gap;
	r.left += gap / 3;

	BPoint center(floorf((r.left + r.right) / 2.0),
				  floorf((r.top + r.bottom) / 2.0));

	float size = std::min(r.Height() - 2, r.Width());
	r.top	   = floorf(center.y - size / 2 + 0.5);
	r.bottom   = floorf(center.y + size / 2 + 0.5);
	r.left	   = floorf(center.x - size / 2 + 0.5);
	r.right	   = floorf(center.x + size / 2 + 0.5);

	center.x += 0.5;
	center.y += 0.5;
	size *= 0.3;

	fSuper->SetHighColor(tint_color(_HighColor(), kMarkTint));
	fSuper->SetDrawingMode(B_OP_OVER);
	fSuper->SetPenSize(2.0);
	fSuper->MovePenTo(BPoint(center.x - size, center.y - size * 0.25));
	fSuper->StrokeLine(BPoint(center.x - size * 0.25, center.y + size));
	fSuper->StrokeLine(BPoint(center.x + size, center.y - size));

	fSuper->PopState();
}

void BMenuItem::_DrawShortcutSymbol(bool submenus)
{
	BMenu *menu = fSuper;
	BFont  font;
	menu->GetFont(&font);
	BPoint where = ContentLocation();
	// Start from the right and walk our way back
	where.x = fBounds.right - font.Size();

	// Leave space for the submenu arrow if any item in the menu has a submenu
	if (submenus)
		where.x -= fBounds.Height() / 2;

	if (fShortcutChar < B_SPACE && kUTF8ControlMap[(int)fShortcutChar])
		_DrawControlChar(fShortcutChar, where + BPoint(0, fSuper->fAscent));
	else
		fSuper->DrawChar(fShortcutChar, where + BPoint(0, fSuper->fAscent));

	where.y += (fBounds.Height() - 11) / 2 - 1;
	where.x -= 4;

	// TODO: It would be nice to draw these taking into account the text (low)
	// color.
	if ((fModifiers & B_COMMAND_KEY) != 0) {
		// const BBitmap *command = MenuPrivate::MenuItemCommand();
		// const BRect	  &rect	   = command->Bounds();
		// where.x -= rect.Width() + 1;
		// fSuper->DrawBitmap(command, where);
	}

	if ((fModifiers & B_CONTROL_KEY) != 0) {
		// const BBitmap *control = MenuPrivate::MenuItemControl();
		// const BRect	  &rect	   = control->Bounds();
		// where.x -= rect.Width() + 1;
		// fSuper->DrawBitmap(control, where);
	}

	if ((fModifiers & B_OPTION_KEY) != 0) {
		// const BBitmap *option = MenuPrivate::MenuItemOption();
		// const BRect	  &rect	  = option->Bounds();
		// where.x -= rect.Width() + 1;
		// fSuper->DrawBitmap(option, where);
	}

	if ((fModifiers & B_SHIFT_KEY) != 0) {
		// const BBitmap *shift = MenuPrivate::MenuItemShift();
		// const BRect	  &rect	 = shift->Bounds();
		// where.x -= rect.Width() + 1;
		// fSuper->DrawBitmap(shift, where);
	}
}

void BMenuItem::_DrawSubmenuSymbol()
{
	fSuper->PushState();

	float symbolSize = roundf(Frame().Height() * 2 / 3);

	BRect rect(fBounds);
	rect.left = rect.right - symbolSize;

	// 14px by default, scaled with font size up to right margin - padding
	BRect symbolRect(0, 0, symbolSize, symbolSize);
	symbolRect.OffsetTo(BPoint(rect.left,
							   fBounds.top + (fBounds.Height() - symbolSize) / 2));

	be_control_look->DrawArrowShape(Menu(), symbolRect, symbolRect,
									_HighColor(), BControlLook::B_RIGHT_ARROW, 0, kMarkTint);

	fSuper->PopState();
}

void BMenuItem::_DrawControlChar(char shortcut, BPoint where)
{
	// TODO: If needed, take another font for the control characters
	//      (or have font overlays in the app_server!)
	const char *symbol = " ";
	if (kUTF8ControlMap[(int)fShortcutChar])
		symbol = kUTF8ControlMap[(int)fShortcutChar];

	fSuper->DrawString(symbol, where);
}
