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

#include <support/IByteStream.h>
#include <xml/Parser.h>
#include <xml/Writer.h>

#if _SUPPORTS_NAMESPACE
namespace os {
namespace xml {
#endif

/****************************************************************************/

const char *indents         = "\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t";
const char *markup          = "&<>'\"";
int32_t     entityLengths[] = {5, 4, 4, 6, 6};
const char *entities[]      = {"&amp;", "&lt;", "&gt;", "&apos;", "&quot;"};

status_t write_xml_data(const sptr<IByteOutput> &stream, const char *data, int32_t size)
{
  const char *start = data;
  const char *end   = start;
  const char *p     = markup;
  char        c;

  while (size > 0) {
    while (size--) {
      c = *end;
      p = markup;
      while (*p) {
        if (c == *p) goto gotSegment;
        p++;
      };
      end++;
    }

  gotSegment:

    if (end - start) stream->Write(start, end - start);
    if (*p) {
      stream->Write(entities[p - markup], entityLengths[p - markup]);
      end++;
    }
    start = end;
  }

  return B_OK;
}

BOutputStream::BOutputStream(const sptr<IByteOutput> &stream, bool writeHeader)
    : m_stream(stream)
{
  m_depth           = writeHeader ? -1 : 0;
  m_lastPrettyDepth = 0;
  m_isLeaf          = false;
  m_openStartTag    = false;
}

BOutputStream::~BOutputStream()
{
  m_stream->Write("\n", 1);
}

void BOutputStream::Indent()
{
  if (m_lastPrettyDepth == m_depth) {
    const char *s       = indents;
    int32_t     onetime = 1, size, depth = m_depth;
    do {
      size = depth;
      if (size > 16) size = 16;
      depth -= size;
      m_stream->Write(indents, size + onetime);
      s += onetime;
      onetime = 0;
    } while (depth);
  }
}

const char *xmlHeader = "<?xml version=\"1.0\"?>";

status_t
BOutputStream::StartTag(SString &name, SValueMap &attr)
{
  SValue key, value;

  if (m_openStartTag) {
    m_stream->Write(">", 1);
    m_openStartTag = false;
  }

  if (m_depth == -1) {
    m_stream->Write(xmlHeader, strlen(xmlHeader));
    m_depth = 0;
  };

  Indent();

  m_stream->Write("<", 1);
  m_stream->Write(name.string(), name.length());

  for (auto const &[key, value] : attr) {
    SString k(key.c_str());
    SString v;
    value.getString(&v);
    m_stream->Write(" ", 1);
    m_stream->Write(k.string(), k.length());
    m_stream->Write("=\"", 2);
    write_xml_data(m_stream, v.string(), v.length());
    m_stream->Write("\"", 1);
  }

  m_openStartTag = true;
  //	m_stream.Write(">",1);
  //	m_isLeaf = (formattingHints & stfLeaf);
  m_isLeaf = false;
  //	if ((m_lastPrettyDepth == m_depth) && (formattingHints & stfCanAddWS))
  //		m_lastPrettyDepth = m_depth+1;
  m_depth++;

  return B_OK;
}

status_t
BOutputStream::EndTag(SString &name)
{
  m_depth--;
  if (m_lastPrettyDepth > m_depth) m_lastPrettyDepth = m_depth;

  if (m_openStartTag) {
    m_stream->Write("/>", 2);
    m_openStartTag = false;
    m_isLeaf       = false;
    return B_OK;
  }

  if (!m_isLeaf) Indent();

  m_stream->Write("</", 2);
  m_stream->Write(name.string(), name.length());
  m_stream->Write(">", 1);

  m_isLeaf = false;

  return B_OK;
}

status_t
BOutputStream::TextData(const char *data, int32_t size)
{
  if (m_openStartTag) {
    m_stream->Write(">", 1);
    m_openStartTag = false;
  }

  return write_xml_data(m_stream, data, size);
}

status_t
BOutputStream::Comment(const char *data, int32_t size)
{
  (void)data;
  (void)size;
  return B_OK;
}

#if _SUPPORTS_NAMESPACE
};  // namespace xml
};  // namespace os
#endif
