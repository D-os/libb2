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

#ifndef SUPPORT_LOOPER_H
#define SUPPORT_LOOPER_H

/*!	@file support/Looper.h
	@ingroup CoreSupportBinder
	@brief The Binder's per-thread state and utilities.
*/

#include <binder/IPCThreadState.h>
#include <utils/Looper.h>

namespace os {
namespace support {

using SLooper = ::android::Looper;

using ::android::ProcessState;

}  // namespace support
}  // namespace os

#endif /* SUPPORT_LOOPER_H */
