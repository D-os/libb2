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

#ifndef _SUPPORT2_WINDOWSSTREAMS_H
#define _SUPPORT2_WINDOWSSTREAMS_H

#include <support/ByteStream.h>
#include <support/SupportDefs.h>
#include <sys/uio.h>

#if _SUPPORTS_NAMESPACE
namespace os {
namespace support {
#endif

/*---------------------------------------------------------------------*/

class BWindowsOutputStream : public BnByteOutput
{
 public:
  BWindowsOutputStream();
  virtual ~BWindowsOutputStream();

  virtual ssize_t  WriteV(const struct iovec *vector, ssize_t count, uint32_t flags = 0);
  virtual status_t Sync();

 private:
};

/*-------------------------------------------------------------*/

class BWindowsInputStream : public BnByteInput
{
 public:
  BWindowsInputStream();
  virtual ~BWindowsInputStream();

  virtual ssize_t ReadV(const struct iovec *vector, ssize_t count);

 private:
};

/*-------------------------------------------------------------*/

#if _SUPPORTS_NAMESPACE
}
}  // namespace os::support
#endif

#endif /* _SUPPORT2_WINDOWSSTREAMS_H */
