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

#ifndef SUPPORT_PARCEL_H
#define SUPPORT_PARCEL_H

/*!	@file support/Parcel.h
	@ingroup CoreSupportBinder
	@brief Container for a raw block of data that can be transfered through IPC.
*/

#include <binder/Parcel.h>

namespace os {
namespace support {

using SParcel = ::android::Parcel;

}  // namespace support
}  // namespace os

#endif  // SUPPORT_PARCEL_H
