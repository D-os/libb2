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

#ifndef SUPPORT_VALUE_H_
#define SUPPORT_VALUE_H_

/*!	@file support/Value.h
	@ingroup CoreSupportUtilities
	@brief A general-purpose data container.
*/

#include <binder/Map.h>
#include <binder/Value.h>

namespace os {
namespace support {

/*!	@addtogroup CoreSupportUtilities
	@{
*/

using SValue    = ::android::binder::Value;
using SValueMap = ::android::binder::Map;

/*!	@} */
}  // namespace support
}  // namespace os

#endif /* SUPPORT_VALUE_H_ */
