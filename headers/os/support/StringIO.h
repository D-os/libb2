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

#ifndef SUPPORT_STRINGIO_H
#define SUPPORT_STRINGIO_H

/*!	@file support/StringIO.h
	@ingroup CoreSupportDataModel
	@brief Binder IO stream for creating C strings.
*/

#include <support/ByteStream.h>
#include <support/MemoryStore.h>
#include <support/TextStream.h>

namespace os {
namespace support {

/*!	@addtogroup CoreSupportDataModel
	@{
*/

/*-------------------------------------------------------------------*/
/*------- BStringIO Class -------------------------------------------*/

class BStringIO : public BMallocStore, public BTextOutput
{
 public:
  BStringIO();
  virtual ~BStringIO();

  const char* String();
  size_t      StringLength() const;
  void        Clear(off_t to);
  void        Reset();
  void        PrintAndReset(const sptr<ITextOutput>& io);
};

/*-------------------------------------------------------------*/
/*-------------------------------------------------------------*/

/*!	@} */

}  // namespace support
}  // namespace os

#endif /* SUPPORT_STRINGIO_H */
