#include "InterfaceDefs.h"

#include <Font.h>
#include <GraphicsDefs.h>
#include <String.h>
#include <log/log.h>

static const rgb_color _kDefaultColors[] = {
	[B_PANEL_BACKGROUND_COLOR]		   = {0xDD, 0xDD, 0xDD, 255},
	[B_PANEL_TEXT_COLOR]			   = {0, 0, 0, 255},
	[B_DOCUMENT_BACKGROUND_COLOR]	   = {255, 255, 255, 255},
	[B_DOCUMENT_TEXT_COLOR]			   = {0, 0, 0, 255},
	[B_CONTROL_BACKGROUND_COLOR]	   = {245, 245, 245, 255},
	[B_CONTROL_TEXT_COLOR]			   = {0, 0, 0, 255},
	[B_CONTROL_BORDER_COLOR]		   = {172, 172, 172, 255},
	[B_CONTROL_HIGHLIGHT_COLOR]		   = {0xCC, 0xCD, 0xFD, 255},
	[B_NAVIGATION_BASE_COLOR]		   = {0x66, 0x68, 0xC8, 255},
	[B_NAVIGATION_PULSE_COLOR]		   = {0, 0, 0, 255},
	[B_SHINE_COLOR]					   = {255, 255, 255, 255},
	[B_SHADOW_COLOR]				   = {0, 0, 0, 255},
	[B_MENU_BACKGROUND_COLOR]		   = {0xDD, 0xDD, 0xDD, 255},
	[B_MENU_SELECTED_BACKGROUND_COLOR] = {0x00, 0x04, 0x53, 255},
	[B_MENU_ITEM_TEXT_COLOR]		   = {0, 0, 0, 255},
	[B_MENU_SELECTED_ITEM_TEXT_COLOR]  = {0xFF, 0xFF, 0xFF, 255},
	[B_MENU_SELECTED_BORDER_COLOR]	   = {0, 0, 0, 255},
	[B_TOOLTIP_BACKGROUND_COLOR]	   = {255, 255, 0xDD, 255},
	[B_TOOLTIP_TEXT_COLOR]			   = {0, 0, 0, 255},

	[B_SUCCESS_COLOR] = {46, 204, 64, 255},
	[B_FAILURE_COLOR] = {255, 65, 54, 255},

	[B_DESKTOP_COLOR]	 = {51, 102, 152, 255},
	[B_WINDOW_TAB_COLOR] = {255, 203, 0, 255},

};

rgb_color ui_color(color_which which)
{
	ssize_t index = which;
	if (index < 0 || index >= sizeof(_kDefaultColors) / sizeof(_kDefaultColors[0])) {
		ALOGE("ui_color(): unknown color_which %d", which);
		return make_color(0, 0, 0);
	}

	return _kDefaultColors[index];
}

rgb_color
tint_color(rgb_color color, float tint)
{
	rgb_color result;

#define LIGHTEN(x) ((uint8)(255.0f - (255.0f - x) * tint))
#define DARKEN(x) ((uint8)(x * (2 - tint)))

	if (tint < 1.0f) {
		result.red	 = LIGHTEN(color.red);
		result.green = LIGHTEN(color.green);
		result.blue	 = LIGHTEN(color.blue);
		result.alpha = color.alpha;
	}
	else {
		result.red	 = DARKEN(color.red);
		result.green = DARKEN(color.green);
		result.blue	 = DARKEN(color.blue);
		result.alpha = color.alpha;
	}

#undef LIGHTEN
#undef DARKEN

	return result;
}

#pragma mark - truncate string

void truncate_string(BString& string, uint32 mode, float width,
					 const float* escapementArray, float fontSize, float ellipsisWidth,
					 int32 charCount)
{
	// add a tiny amount to the width to make floating point inaccuracy
	// not drop chars that would actually fit exactly
	width += 1.f / 128;

	switch (mode) {
		case B_TRUNCATE_BEGINNING: {
			float totalWidth = 0;
			for (int32 i = charCount - 1; i >= 0; i--) {
				float charWidth = escapementArray[i] * fontSize;
				if (totalWidth + charWidth > width) {
					// we need to truncate
					while (totalWidth + ellipsisWidth > width) {
						// remove chars until there's enough space for the
						// ellipsis
						if (++i == charCount) {
							// we've reached the end of the string and still
							// no space, so return an empty string
							string.Truncate(0);
							return;
						}

						totalWidth -= escapementArray[i] * fontSize;
					}

					string.RemoveChars(0, i + 1);
					string.PrependChars(B_UTF8_ELLIPSIS, 1);
					return;
				}

				totalWidth += charWidth;
			}

			break;
		}

		case B_TRUNCATE_END: {
			float totalWidth = 0;
			for (int32 i = 0; i < charCount; i++) {
				float charWidth = escapementArray[i] * fontSize;
				if (totalWidth + charWidth > width) {
					// we need to truncate
					while (totalWidth + ellipsisWidth > width) {
						// remove chars until there's enough space for the
						// ellipsis
						if (i-- == 0) {
							// we've reached the start of the string and still
							// no space, so return an empty string
							string.Truncate(0);
							return;
						}

						totalWidth -= escapementArray[i] * fontSize;
					}

					string.RemoveChars(i, charCount - i);
					string.AppendChars(B_UTF8_ELLIPSIS, 1);
					return;
				}

				totalWidth += charWidth;
			}

			break;
		}

		case B_TRUNCATE_MIDDLE:
		case B_TRUNCATE_SMART: {
			float leftWidth	 = 0;
			float rightWidth = 0;
			int32 leftIndex	 = 0;
			int32 rightIndex = charCount - 1;
			bool  left		 = true;

			for (int32 i = 0; i < charCount; i++) {
				float charWidth
					= escapementArray[left ? leftIndex : rightIndex] * fontSize;

				if (leftWidth + rightWidth + charWidth > width) {
					// we need to truncate
					while (leftWidth + rightWidth + ellipsisWidth > width) {
						// remove chars until there's enough space for the
						// ellipsis
						if (leftIndex == 0 && rightIndex == charCount - 1) {
							// we've reached both ends of the string and still
							// no space, so return an empty string
							string.Truncate(0);
							return;
						}

						if (leftIndex > 0 && (rightIndex == charCount - 1 || leftWidth > rightWidth)) {
							// remove char on the left
							leftWidth -= escapementArray[--leftIndex] * fontSize;
						}
						else {
							// remove char on the right
							rightWidth -= escapementArray[++rightIndex] * fontSize;
						}
					}

					string.RemoveChars(leftIndex, rightIndex + 1 - leftIndex);
					string.InsertChars(B_UTF8_ELLIPSIS, 1, leftIndex);
					return;
				}

				if (left) {
					leftIndex++;
					leftWidth += charWidth;
				}
				else {
					rightIndex--;
					rightWidth += charWidth;
				}

				left = rightWidth > leftWidth;
			}

			break;
		}
	}

	// we've run through without the need to truncate, leave the string as it is
}
