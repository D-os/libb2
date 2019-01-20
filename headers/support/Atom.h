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

#ifndef SUPPORT_ATOM_H
#define SUPPORT_ATOM_H

/*!	@file support/Atom.h
	@ingroup CoreSupportUtilities

	@brief Basic reference-counting classes, and smart pointer templates
	for working with them.
*/

#include <utils/RefBase.h>

namespace os {
namespace support {

/*!	@addtogroup CoreSupportUtilities Utility Classes and Functions
	@ingroup CoreSupport
	@brief Reference counting, data containers, and other tools.
	@{
*/

using SAtom = ::android::RefBase;

template <typename T>
using sptr = ::android::sp<T>;
template <typename T>
using wptr = ::android::wp<T>;

/*!	@} */
}  // namespace support
}  // namespace os

#endif /* SUPPORT_ATOM_H */
