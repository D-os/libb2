#include "InterfaceDefs.h"

#include <GraphicsDefs.h>
#include <log/log.h>

static const rgb_color _kDefaultColors[] = {
	[B_PANEL_BACKGROUND_COLOR]		   = {0xDD, 0xDD, 0xDD, 255},
	[B_PANEL_TEXT_COLOR]			   = {0, 0, 0, 255},
	[B_DOCUMENT_BACKGROUND_COLOR]	   = {255, 255, 255, 255},
	[B_DOCUMENT_TEXT_COLOR]			   = {0, 0, 0, 255},
	[B_CONTROL_BACKGROUND_COLOR]	   = {245, 245, 245, 255},
	[B_CONTROL_TEXT_COLOR]			   = {0, 0, 0, 255},
	[B_CONTROL_BORDER_COLOR]		   = {172, 172, 172, 255},
	[B_CONTROL_HIGHLIGHT_COLOR]		   = {102, 152, 203, 255},
	[B_NAVIGATION_BASE_COLOR]		   = {0, 0, 229, 255},
	[B_NAVIGATION_PULSE_COLOR]		   = {0, 0, 0, 255},
	[B_SHINE_COLOR]					   = {255, 255, 255, 255},
	[B_SHADOW_COLOR]				   = {0, 0, 0, 255},
	[B_MENU_BACKGROUND_COLOR]		   = {0xDD, 0xDD, 0xDD, 255},
	[B_MENU_SELECTED_BACKGROUND_COLOR] = {153, 153, 153, 255},
	[B_MENU_ITEM_TEXT_COLOR]		   = {0, 0, 0, 255},
	[B_MENU_SELECTED_ITEM_TEXT_COLOR]  = {0, 0, 0, 255},
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
