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

#ifndef XML2_IXMLOSTR_H
#define XML2_IXMLOSTR_H

#include <support/IInterface.h>
#include <support/String.h>
#include <support/Value.h>

namespace os {
namespace xml {
using namespace support;

/**************************************************************************************/

class IXMLOStr : public IInterface
{
 public:
  //the following could be DECLARE_META_INTERFACE(), but we've no RXMLOStr
  DECLARE_META_INTERFACE(XMLOStr)

  virtual status_t StartTag(SString &name, SValue &attributes) = 0;
  virtual status_t EndTag(SString &name)                       = 0;
  virtual status_t Content(const char *data, int32_t size)     = 0;
  virtual status_t Comment(const char *data, int32_t size);
};

/**************************************************************************************/

}  // namespace xml
}  // namespace os

#endif /* XML2_XMLOSTR_H */
