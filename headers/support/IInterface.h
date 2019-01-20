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

#ifndef SUPPORT_INTERFACE_INTERFACE_H
#define SUPPORT_INTERFACE_INTERFACE_H

/*!	@file support/IInterface.h
	@ingroup CoreSupportBinder
	@brief Common base class for abstract binderized interfaces.
*/

#include <binder/IInterface.h>
#include <support/Errors.h>

namespace os {
namespace support {

using IInterface = ::android::IInterface;

template <typename INTERFACE>
inline ::android::sp<INTERFACE> interface_cast(const ::android::sp<::android::IBinder>& obj)
{
  return ::android::interface_cast<INTERFACE>(obj);
}

}  // namespace support
}  // namespace os

#endif /* SUPPORT_INTERFACE_INTERFACE_H */
