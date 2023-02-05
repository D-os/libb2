#include "MenuItem.h"

BSeparatorItem::BSeparatorItem()
	: BMenuItem("", nullptr)
{
	BMenuItem::SetEnabled(false);
}

BSeparatorItem::BSeparatorItem(BMessage *data) : BMenuItem(data)
{
	BMenuItem::SetEnabled(false);
}

BSeparatorItem::~BSeparatorItem() {}

status_t BSeparatorItem::Archive(BMessage *data, bool deep) const
{
	return BMenuItem::Archive(data, deep);
}

void BSeparatorItem::SetEnabled(bool state)
{
	// Don't do anything - we don't want to get enabled ever
}

void BSeparatorItem::GetContentSize(float *_width, float *_height)
{
	BMenu *menu = Menu();

	if (menu && menu->Layout() == B_ITEMS_IN_ROW) {
		if (_width)
			*_width = 2.0;

		if (_height)
			*_height = 2.0;
	}
	else {
		if (_width)
			*_width = 2.0;

		if (_height) {
			BFont font(be_plain_font);
			if (menu)
				menu->GetFont(&font);

			const float height = floorf((font.Size() * 0.8) / 2) * 2;
			*_height		   = max_c(4, height);
		}
	}
}

void BSeparatorItem::Draw()
{
	BMenu *menu = Menu();
	if (!menu)
		return;

	BRect	  bounds   = Frame();
	rgb_color oldColor = menu->HighColor();
	rgb_color lowColor = menu->LowColor();

	if (menu->Layout() == B_ITEMS_IN_ROW) {
		const float startLeft = bounds.left + (floor(bounds.Width())) / 2;
		menu->SetHighColor(tint_color(lowColor, B_DARKEN_1_TINT));
		menu->StrokeLine(BPoint(startLeft, bounds.top + 1.0f),
						 BPoint(startLeft, bounds.bottom - 1.0f));
		menu->SetHighColor(tint_color(lowColor, B_LIGHTEN_2_TINT));
		menu->StrokeLine(BPoint(startLeft + 1.0f, bounds.top + 1.0f),
						 BPoint(startLeft + 1.0f, bounds.bottom - 1.0f));
	}
	else {
		const float startTop = bounds.top + (floor(bounds.Height())) / 2;
		menu->SetHighColor(tint_color(lowColor, B_DARKEN_1_TINT));
		menu->StrokeLine(BPoint(bounds.left + 1.0f, startTop),
						 BPoint(bounds.right - 1.0f, startTop));
		menu->SetHighColor(tint_color(lowColor, B_LIGHTEN_2_TINT));
		menu->StrokeLine(BPoint(bounds.left + 1.0f, startTop + 1.0f),
						 BPoint(bounds.right - 1.0f, startTop + 1.0f));
	}

	menu->SetHighColor(oldColor);
}
