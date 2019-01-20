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

#ifndef SUPPORT_BINDER_INTERFACE_H_
#define SUPPORT_BINDER_INTERFACE_H_

/*!	@file support/IBinder.h
	@ingroup CoreSupportBinder
	@brief Abstract interface to a Binder object.
*/

#include <binder/IBinder.h>

namespace os {
namespace support {

/*!	@addtogroup CoreSupportBinder Binder
	@ingroup CoreSupport
	@brief Component-based framework for implementing application and system-level code.

	The @ref BinderKit defines a basic object model (built on C++) that
	allows instantiated objects to be distributed across any processes
	desired, taking care of the details of IPC and tracking object
	references between them.
	@{
*/

using IBinder = ::android::IBinder;

/*!	@} */
}  // namespace support
}  // namespace os

#endif  // SUPPORT_BINDER_INTERFACE_H_
