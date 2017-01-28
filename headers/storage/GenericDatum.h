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

#ifndef _STORAGE_GENERICDATUM_H
#define _STORAGE_GENERICDATUM_H

/*!	@file storage/GenericDatum.h
    @ingroup CoreSupportDataModel
    @brief Common base implementation of IDatum interface.
*/

#include <os/support/BnDatum.h>
#include <support/Context.h>
#include <support/SupportDefs.h>

namespace os {
namespace storage {
using namespace support;

/*!	@addtogroup CoreSupportDataModel
    @{
*/

// ==========================================================================
// ==========================================================================

//!	Generic base class for implementations of IDatum.
/*!	This class takes care of providing a generic implementation
    of the CopyTo() and CopyFrom() methods.  You should always
    derive from this class instead of directly from BnDatum.

    Implementations will usually derive from an even more specialized
    class, typically BStreamDatum or one of its subclasses, instead
    of directly from GenericDatum.

    @nosubgrouping
*/
class GenericDatum : public BnDatum
{
public:
    GenericDatum();
    GenericDatum(const Context& context);

    virtual status_t copyTo(const sp<IDatum>& dest, uint32_t flags = 0);
    virtual Status copyFrom(const sp<IDatum>& src, int32_t flags = 0);

protected:
    virtual ~GenericDatum();

private:
    GenericDatum(const GenericDatum&);
    GenericDatum& operator=(const GenericDatum&);
};

// ==========================================================================
// ==========================================================================

/*!	@} */
}
} // namespace os::storage

#endif // _STORAGE_GENERICDATUM_H
