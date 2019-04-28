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

#ifndef SUPPORT_BINDER_H
#define SUPPORT_BINDER_H

/*!	@file support/Binder.h
	@ingroup CoreSupportBinder
	@brief Basic functionality of all Binder objects.
*/

#include <binder/Binder.h>
#include <binder/IInterface.h>
#include <support/IBinder.h>

namespace os {
namespace support {

/*!	@addtogroup CoreSupportBinder
	@{
*/

using BBinder = ::android::BBinder;

template <typename INTERFACE>
using BnInterface = ::android::BnInterface<INTERFACE>;

template <typename INTERFACE>
using BpInterface = ::android::BpInterface<INTERFACE>;

/*!	@} */
}  // namespace support
}  // namespace os

#endif /* SUPPORT_BINDER_H */
