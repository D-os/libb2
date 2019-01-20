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

#ifndef SUPPORT_STRING_H
#define SUPPORT_STRING_H

/*!	@file support/String.h
	@ingroup CoreSupportUtilities
	@brief Unicode (UTF-8) string class.
*/

#include <utils/String8.h>

namespace os {
namespace support {

/*!	@addtogroup CoreSupportUtilities
	@{
*/

//!	UTF8 string container.
/*!	This class represents a UTF-8 string.  It includes a variety
	of common string operations as methods (in fact as way too
	many methods); additional less common operations are available
	in the StringUtils.h header.

	SString uses copy-on-write for its internal data buffer, so
	it is efficient to copy (and usually to convert back and
	forth with SValue, which shares the same copy-on-write
	mechanism).

	@nosubgrouping
*/

using SString = ::android::String8;

/*!	@} */
}  // namespace support
}  // namespace os

#endif /* SUPPORT_STRING_H */
