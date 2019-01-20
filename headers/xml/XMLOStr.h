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

#ifndef XML2_XMLOSTR_H
#define XML2_XMLOSTR_H

#include <support/Binder.h>
#include <xml/IXMLOStr.h>

namespace os {
namespace xml {
using namespace support;

/**************************************************************************************/

class CXMLOStr : public BnInterface<IXMLOStr>
{
 public:
  CXMLOStr();
  virtual ~CXMLOStr();

  virtual status_t Told(SValue& in);

 private:
  CXMLOStr(const CXMLOStr& o);
};

/**************************************************************************************/

}  // namespace xml
}  // namespace os

#endif /* B_XML2_PARSER_H */
