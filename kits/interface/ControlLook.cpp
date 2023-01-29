/*
 * Copyright 2012-2020 Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */

#include "ControlLook.h"

#include <Font.h>

#include <algorithm>

namespace BPrivate {

BControlLook::BControlLook()
{
}

BControlLook::~BControlLook()
{
}

float BControlLook::ComposeSpacing(float spacing)
{
	switch ((int)spacing) {
		case B_USE_DEFAULT_SPACING:
		case B_USE_ITEM_SPACING:
			return be_control_look->DefaultItemSpacing();
		case B_USE_HALF_ITEM_SPACING:
			return ceilf(be_control_look->DefaultItemSpacing() * 0.5f);
		case B_USE_WINDOW_SPACING:
			return be_control_look->DefaultItemSpacing();
		case B_USE_SMALL_SPACING:
			return ceilf(be_control_look->DefaultItemSpacing() * 0.7f);
		case B_USE_CORNER_SPACING:
			return ceilf(be_control_look->DefaultItemSpacing() * 1.272f);
		case B_USE_BIG_SPACING:
			return ceilf(be_control_look->DefaultItemSpacing() * 1.8f);

		case B_USE_BORDER_SPACING:
			return std::max(1.0f, floorf(be_control_look->DefaultItemSpacing() / 11.0f));
	}

	return spacing;
}

BSize BControlLook::ComposeIconSize(int32 size)
{
	float scale = be_plain_font->Size() / 12.0f;
	if (scale < 1.0f)
		scale = 1.0f;

	const int32 scaled = (int32)(size * scale);
	return BSize(scaled - 1, scaled - 1);
}

void BControlLook::DrawLabel(BView* view, const char* label, const BBitmap* icon,
							 BRect rect, const BRect& updateRect, const rgb_color& base, uint32 flags,
							 const rgb_color* textColor)
{
	DrawLabel(view, label, icon, rect, updateRect, base, flags, DefaultLabelAlignment(), textColor);
}

// Initialized in InterfaceDefs.cpp
BControlLook* be_control_look = nullptr;

}  // namespace BPrivate
