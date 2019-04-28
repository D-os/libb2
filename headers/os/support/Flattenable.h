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

#ifndef SUPPORT_FLATTENABLE_H
#define SUPPORT_FLATTENABLE_H

/*!	@file support/Flattenable.h
	@ingroup CoreSupportUtilities
	@brief Abstract interface for an object that can be flattened into
	a byte buffer.
*/

#include <utils/Flattenable.h>

namespace os {
namespace support {

/*!	@addtogroup CoreSupportUtilities
	@{
*/

template <typename T>
using SFlattenable = ::android::Flattenable<T>;

}  // namespace support
}  // namespace os

#endif /* SUPPORT_FLATTENABLE_H */
