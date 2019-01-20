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

#include <support/Debug.h>
#include <xml/XMLOStr.h>

namespace os {
namespace xml {

class BpXMLOStr : public BpInterface<IXMLOStr>
{
 public:
  BpXMLOStr(const sptr<IBinder> &o)
      : BpInterface<IXMLOStr>(o)
  {
  }

  virtual status_t StartTag(SString &, SValue &)
  {
    ErrFatalError("Shoot me (geh)!");
    return B_UNSUPPORTED;
  }
  virtual status_t EndTag(SString &)
  {
    ErrFatalError("Shoot me (geh)!");
    return B_UNSUPPORTED;
  }
  virtual status_t Content(const char *, int32_t)
  {
    ErrFatalError("Shoot me (geh)!");
    return B_UNSUPPORTED;
  }
};

IMPLEMENT_META_INTERFACE(XMLOStr, "org.openbinder.xml.IXMLOStr")

status_t
IXMLOStr::Comment(const char *, int32_t)
{
  return B_OK;
}

CXMLOStr::CXMLOStr()
{
}

CXMLOStr::~CXMLOStr()
{
}

status_t
CXMLOStr::Told(SValue &)
{
  return B_UNSUPPORTED;
}

};  // namespace xml
};  // namespace os
