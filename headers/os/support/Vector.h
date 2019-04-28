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

#ifndef SUPPORT_VECTOR_H
#define SUPPORT_VECTOR_H

/*!	@file support/Vector.h
	@ingroup CoreSupportUtilities
	@brief Simple array-like container class.

	Implemented as a general purpose abstract base-class SAbstractVector,
	with the concrete class SVector layered on top and
	templatized on the array type.
*/

#include <utils/Vector.h>

namespace os {
namespace support {

/*!	@addtogroup CoreSupportUtilities
	@{
*/

template <class TYPE>
using SVector = ::android::Vector<TYPE>;

/*!	@} */
}  // namespace support
}  // namespace os

#endif /* SUPPORT_VECTOR_H */
