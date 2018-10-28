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

#ifndef _VALUE_TO_XML_H

#define _VALUE_TO_XML_H

#include <support/IByteStream.h>

#if _SUPPORTS_NAMESPACE
namespace os {
namespace xml {
#endif

status_t ValueToXML(const BNS(os::support::) sptr<BNS(os::support::) IByteOutput>& stream,
                    const BNS(os::support::) SValue&                               value);

status_t XMLToValue(const BNS(os::support::) sptr<BNS(os::support::) IByteInput>& stream,
                    BNS(os::support::) SValue&                                    value);

#if _SUPPORTS_NAMESPACE
}
}  // os::xml
#endif

#endif
