/*
 * Copyright (c) 2005 Palmsource, Inc.
 *
 * This software is licensed as described in the file LICENSE, which
 * you should have received as part of this distribution. The terms
 * are also available at http://www.openbinder.org/license.html.
 *
 * This software consists of voluntary contributions made by many
 * individuals. For the exact contribution history, see the revision
 * history and logs, available at http://www.openbinder.org
 */

#ifndef SUPPORT_LIST_H
#define SUPPORT_LIST_H

/*!	@file support/List.h
	@ingroup CoreSupportUtilities
	@brief A list container class.

	Implemented as a general purpose abstract base-class
	SAbstractList, with the concrete class SList layered
	on top and templatized on the array type.
*/

#include <utils/List.h>

namespace os {
namespace support {

/*!	@addtogroup CoreSupportUtilities
	@{
*/

template <typename T>
using SList = ::android::List<T>;

}  // namespace support
}  // namespace os

#endif /* SUPPORT_LIST_H */
