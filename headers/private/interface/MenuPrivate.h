/*
 * Copyright 2006-2015 Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 * 		Stefano Ceccherini (stefano.ceccherini@gmail.com)
 */

#ifndef __MENU_PRIVATE_H
#define __MENU_PRIVATE_H

#include <Menu.h>

enum menu_states {
	MENU_STATE_TRACKING			 = 0,
	MENU_STATE_TRACKING_SUBMENU	 = 1,
	MENU_STATE_KEY_TO_SUBMENU	 = 2,
	MENU_STATE_KEY_LEAVE_SUBMENU = 3,
	MENU_STATE_CLOSED			 = 5
};

class BBitmap;
class BMenu;
class BWindow;

namespace BPrivate {

extern const char* kEmptyMenuLabel;

};	// namespace BPrivate

// Note: since sqrt is slow, we don't use it and return the square of the
// distance
#define square(x) ((x) * (x))
static inline float point_distance(const BPoint& pointA, const BPoint& pointB)
{
	return square(pointA.x - pointB.x) + square(pointA.y - pointB.y);
}
#undef square

#endif	// __MENU_PRIVATE_H
