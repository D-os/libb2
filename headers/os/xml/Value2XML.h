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

#ifndef VALUE_TO_XML_H

#define VALUE_TO_XML_H

#include <support/Atom.h>
#include <support/Errors.h>
#include <support/IByteStream.h>
#include <support/Value.h>

namespace os {
namespace xml {

os::support::status_t ValueToXML(const os::support::sptr<os::support::IByteOutput>& stream,
                                 const os::support::SValue&                         value);

os::support::status_t XMLToValue(const os::support::sptr<os::support::IByteInput>& stream,
                                 os::support::SValue&                              value);

}  // namespace xml
}  // namespace os

#endif
