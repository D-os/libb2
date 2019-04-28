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

#ifndef SUPPORT_SORTEDVECTOR_H
#define SUPPORT_SORTEDVECTOR_H

/*!	@file support/SortedVector.h
	@ingroup CoreSupportUtilities
	@brief An SAbstractVector whose types are kept in a consistent order.
*/

#include <utils/SortedVector.h>

namespace os {
namespace support {

/*!	@addtogroup CoreSupportUtilities
	@{
*/

template <class TYPE>
using SSortedVector = ::android::SortedVector<TYPE>;

/*!	@} */
}  // namespace support
}  // namespace os

#endif /* SUPPORT_SORTEDVECTOR_H */
