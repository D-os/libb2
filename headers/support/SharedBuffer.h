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

#ifndef SUPPORT_SHARED_BUFFER_H_
#define SUPPORT_SHARED_BUFFER_H_

/*!	@file support/SharedBuffer.h
	@ingroup CoreSupportUtilities
	@brief Standard representation of a block of shared data that
	supports copy-on-write.
*/

#include <utils/SharedBuffer.h>

namespace os {
namespace support {

/*!	@addtogroup CoreSupportUtilities
	@{
*/

using SSharedBuffer = ::android::SharedBuffer;

}  // namespace support
}  // namespace os

#endif /* SUPPORT_SHARED_BUFFER_H_ */
