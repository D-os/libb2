/*
 * Copyright (C) 2005 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define __STDC_LIMIT_MACROS
#include <stdint.h>

#include <utils/String.h>

#include <utils/Log.h>
#include <utils/Unicode.h>
#include <utils/SharedBuffer.h>
#include <utils/threads.h>

#include <ctype.h>

/*
 * Functions outside android is below the namespace android, since they use
 * functions and constants in android namespace.
 */

// ---------------------------------------------------------------------------

namespace android {

// Separator used by resource paths. This is not platform dependent contrary
// to OS_PATH_SEPARATOR.
#define RES_PATH_SEPARATOR '/'

static SharedBuffer* gEmptyStringBuf = NULL;
static char* gEmptyString = NULL;

extern int gDarwinCantLoadAllObjects;
int gDarwinIsReallyAnnoying;

void initialize_string();

static inline char* getEmptyString()
{
    gEmptyStringBuf->acquire();
    return gEmptyString;
}

void initialize_string()
{
    // HACK: This dummy dependency forces linking libutils Static.cpp,
    // which is needed to initialize String class.
    // These variables are named for Darwin, but are needed elsewhere too,
    // including static linking on any platform.
    gDarwinIsReallyAnnoying = gDarwinCantLoadAllObjects;

    SharedBuffer* buf = SharedBuffer::alloc(1);
    char* str = (char*)buf->data();
    *str = 0;
    gEmptyStringBuf = buf;
    gEmptyString = str;
}

void terminate_string()
{
    SharedBuffer::bufferFromData(gEmptyString)->release();
    gEmptyStringBuf = NULL;
    gEmptyString = NULL;
}

// ---------------------------------------------------------------------------

static char* allocFromUTF8(const char* in, size_t len)
{
    if (len > 0) {
        if (len == SIZE_MAX) {
            return NULL;
        }
        SharedBuffer* buf = SharedBuffer::alloc(len+1);
        ALOG_ASSERT(buf, "Unable to allocate shared buffer");
        if (buf) {
            char* str = (char*)buf->data();
            memcpy(str, in, len);
            str[len] = 0;
            return str;
        }
        return NULL;
    }

    return getEmptyString();
}

static char* allocFromUTF16(const char16_t* in, size_t len)
{
    if (len == 0) return getEmptyString();

    const ssize_t bytes = utf16_to_utf8_length(in, len);
    if (bytes < 0) {
        return getEmptyString();
    }

    SharedBuffer* buf = SharedBuffer::alloc(bytes+1);
    ALOG_ASSERT(buf, "Unable to allocate shared buffer");
    if (!buf) {
        return getEmptyString();
    }

    char* str = (char*)buf->data();
    utf16_to_utf8(in, len, str);
    return str;
}

static char* allocFromUTF32(const char32_t* in, size_t len)
{
    if (len == 0) {
        return getEmptyString();
    }

    const ssize_t bytes = utf32_to_utf8_length(in, len);
    if (bytes < 0) {
        return getEmptyString();
    }

    SharedBuffer* buf = SharedBuffer::alloc(bytes+1);
    ALOG_ASSERT(buf, "Unable to allocate shared buffer");
    if (!buf) {
        return getEmptyString();
    }

    char* str = (char*) buf->data();
    utf32_to_utf8(in, len, str);

    return str;
}

// ---------------------------------------------------------------------------

String::String()
    : mString(getEmptyString())
{
}

String::String(StaticLinkage)
    : mString(0)
{
    // this constructor is used when we can't rely on the static-initializers
    // having run. In this case we always allocate an empty string. It's less
    // efficient than using getEmptyString(), but we assume it's uncommon.

    char* data = static_cast<char*>(
            SharedBuffer::alloc(sizeof(char))->data());
    data[0] = 0;
    mString = data;
}

String::String(const String& o)
    : mString(o.mString)
{
    SharedBuffer::bufferFromData(mString)->acquire();
}

String::String(const char* o)
    : mString(allocFromUTF8(o, strlen(o)))
{
    if (mString == NULL) {
        mString = getEmptyString();
    }
}

String::String(const char* o, size_t len)
    : mString(allocFromUTF8(o, len))
{
    if (mString == NULL) {
        mString = getEmptyString();
    }
}

String::String(const char16_t* o)
    : mString(allocFromUTF16(o, strlen16(o)))
{
}

String::String(const char16_t* o, size_t len)
    : mString(allocFromUTF16(o, len))
{
}

String::String(const char32_t* o)
    : mString(allocFromUTF32(o, strlen32(o)))
{
}

String::String(const char32_t* o, size_t len)
    : mString(allocFromUTF32(o, len))
{
}

String::~String()
{
    SharedBuffer::bufferFromData(mString)->release();
}

String String::format(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    String result(formatV(fmt, args));

    va_end(args);
    return result;
}

String String::formatV(const char* fmt, va_list args)
{
    String result;
    result.appendFormatV(fmt, args);
    return result;
}

void String::clear() {
    SharedBuffer::bufferFromData(mString)->release();
    mString = getEmptyString();
}

void String::setTo(const String& other)
{
    SharedBuffer::bufferFromData(other.mString)->acquire();
    SharedBuffer::bufferFromData(mString)->release();
    mString = other.mString;
}

status_t String::setTo(const char* other)
{
    const char *newString = allocFromUTF8(other, strlen(other));
    SharedBuffer::bufferFromData(mString)->release();
    mString = newString;
    if (mString) return NO_ERROR;

    mString = getEmptyString();
    return NO_MEMORY;
}

status_t String::setTo(const char* other, size_t len)
{
    const char *newString = allocFromUTF8(other, len);
    SharedBuffer::bufferFromData(mString)->release();
    mString = newString;
    if (mString) return NO_ERROR;

    mString = getEmptyString();
    return NO_MEMORY;
}

status_t String::setTo(const char16_t* other, size_t len)
{
    const char *newString = allocFromUTF16(other, len);
    SharedBuffer::bufferFromData(mString)->release();
    mString = newString;
    if (mString) return NO_ERROR;

    mString = getEmptyString();
    return NO_MEMORY;
}

status_t String::setTo(const char32_t* other, size_t len)
{
    const char *newString = allocFromUTF32(other, len);
    SharedBuffer::bufferFromData(mString)->release();
    mString = newString;
    if (mString) return NO_ERROR;

    mString = getEmptyString();
    return NO_MEMORY;
}

status_t String::append(const String& other)
{
    const size_t otherLen = other.bytes();
    if (bytes() == 0) {
        setTo(other);
        return NO_ERROR;
    } else if (otherLen == 0) {
        return NO_ERROR;
    }

    return real_append(other.string(), otherLen);
}

status_t String::append(const char* other)
{
    return append(other, strlen(other));
}

status_t String::append(const char* other, size_t otherLen)
{
    if (bytes() == 0) {
        return setTo(other, otherLen);
    } else if (otherLen == 0) {
        return NO_ERROR;
    }

    return real_append(other, otherLen);
}

status_t String::appendFormat(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    status_t result = appendFormatV(fmt, args);

    va_end(args);
    return result;
}

status_t String::appendFormatV(const char* fmt, va_list args)
{
    int n, result = NO_ERROR;
    va_list tmp_args;

    /* args is undefined after vsnprintf.
     * So we need a copy here to avoid the
     * second vsnprintf access undefined args.
     */
    va_copy(tmp_args, args);
    n = vsnprintf(NULL, 0, fmt, tmp_args);
    va_end(tmp_args);

    if (n != 0) {
        size_t oldLength = length();
        char* buf = lockBuffer(oldLength + n);
        if (buf) {
            vsnprintf(buf + oldLength, n + 1, fmt, args);
        } else {
            result = NO_MEMORY;
        }
    }
    return result;
}

status_t String::real_append(const char* other, size_t otherLen)
{
    const size_t myLen = bytes();

    SharedBuffer* buf = SharedBuffer::bufferFromData(mString)
        ->editResize(myLen+otherLen+1);
    if (buf) {
        char* str = (char*)buf->data();
        mString = str;
        str += myLen;
        memcpy(str, other, otherLen);
        str[otherLen] = '\0';
        return NO_ERROR;
    }
    return NO_MEMORY;
}

char* String::lockBuffer(size_t size)
{
    SharedBuffer* buf = SharedBuffer::bufferFromData(mString)
        ->editResize(size+1);
    if (buf) {
        char* str = (char*)buf->data();
        mString = str;
        return str;
    }
    return NULL;
}

void String::unlockBuffer()
{
    unlockBuffer(strlen(mString));
}

status_t String::unlockBuffer(size_t size)
{
    if (size != this->size()) {
        SharedBuffer* buf = SharedBuffer::bufferFromData(mString)
            ->editResize(size+1);
        if (! buf) {
            return NO_MEMORY;
        }

        char* str = (char*)buf->data();
        str[size] = 0;
        mString = str;
    }

    return NO_ERROR;
}

ssize_t String::find(const char* other, size_t start) const
{
    size_t len = size();
    if (start >= len) {
        return -1;
    }
    const char* s = mString+start;
    const char* p = strstr(s, other);
    return p ? p-mString : -1;
}

bool String::removeAll(const char* other) {
    ssize_t index = find(other);
    if (index < 0) return false;

    char* buf = lockBuffer(size());
    if (!buf) return false; // out of memory

    size_t skip = strlen(other);
    size_t len = size();
    size_t tail = index;
    while (size_t(index) < len) {
        ssize_t next = find(other, index + skip);
        if (next < 0) {
            next = len;
        }

        memmove(buf + tail, buf + index + skip, next - index - skip);
        tail += next - index - skip;
        index = next;
    }
    unlockBuffer(tail);
    return true;
}

void String::toLower()
{
    toLower(0, size());
}

void String::toLower(size_t start, size_t length)
{
    const size_t len = size();
    if (start >= len) {
        return;
    }
    if (start+length > len) {
        length = len-start;
    }
    char* buf = lockBuffer(len);
    buf += start;
    while (length > 0) {
        *buf = tolower(*buf);
        buf++;
        length--;
    }
    unlockBuffer(len);
}

void String::toUpper()
{
    toUpper(0, size());
}

void String::toUpper(size_t start, size_t length)
{
    const size_t len = size();
    if (start >= len) {
        return;
    }
    if (start+length > len) {
        length = len-start;
    }
    char* buf = lockBuffer(len);
    buf += start;
    while (length > 0) {
        *buf = toupper(*buf);
        buf++;
        length--;
    }
    unlockBuffer(len);
}

// ---------------------------------------------------------------------------
// Path functions

void String::setPathName(const char* name)
{
    setPathName(name, strlen(name));
}

void String::setPathName(const char* name, size_t len)
{
    char* buf = lockBuffer(len);

    memcpy(buf, name, len);

    // remove trailing path separator, if present
    if (len > 0 && buf[len-1] == OS_PATH_SEPARATOR)
        len--;

    buf[len] = '\0';

    unlockBuffer(len);
}

String String::getPathLeaf(void) const
{
    const char* cp;
    const char*const buf = mString;

    cp = strrchr(buf, OS_PATH_SEPARATOR);
    if (cp == NULL)
        return String(*this);
    else
        return String(cp+1);
}

String String::getPathDir(void) const
{
    const char* cp;
    const char*const str = mString;

    cp = strrchr(str, OS_PATH_SEPARATOR);
    if (cp == NULL)
        return String("");
    else
        return String(str, cp - str);
}

String String::walkPath(String* outRemains) const
{
    const char* cp;
    const char*const str = mString;
    const char* buf = str;

    cp = strchr(buf, OS_PATH_SEPARATOR);
    if (cp == buf) {
        // don't include a leading '/'.
        buf = buf+1;
        cp = strchr(buf, OS_PATH_SEPARATOR);
    }

    if (cp == NULL) {
        String res = buf != str ? String(buf) : *this;
        if (outRemains) *outRemains = String("");
        return res;
    }

    String res(buf, cp-buf);
    if (outRemains) *outRemains = String(cp+1);
    return res;
}

/*
 * Helper function for finding the start of an extension in a pathname.
 *
 * Returns a pointer inside mString, or NULL if no extension was found.
 */
char* String::find_extension(void) const
{
    const char* lastSlash;
    const char* lastDot;
    const char* const str = mString;

    // only look at the filename
    lastSlash = strrchr(str, OS_PATH_SEPARATOR);
    if (lastSlash == NULL)
        lastSlash = str;
    else
        lastSlash++;

    // find the last dot
    lastDot = strrchr(lastSlash, '.');
    if (lastDot == NULL)
        return NULL;

    // looks good, ship it
    return const_cast<char*>(lastDot);
}

String String::getPathExtension(void) const
{
    char* ext;

    ext = find_extension();
    if (ext != NULL)
        return String(ext);
    else
        return String("");
}

String String::getBasePath(void) const
{
    char* ext;
    const char* const str = mString;

    ext = find_extension();
    if (ext == NULL)
        return String(*this);
    else
        return String(str, ext - str);
}

String& String::appendPath(const char* name)
{
    // TODO: The test below will fail for Win32 paths. Fix later or ignore.
    if (name[0] != OS_PATH_SEPARATOR) {
        if (*name == '\0') {
            // nothing to do
            return *this;
        }

        size_t len = length();
        if (len == 0) {
            // no existing filename, just use the new one
            setPathName(name);
            return *this;
        }

        // make room for oldPath + '/' + newPath
        int newlen = strlen(name);

        char* buf = lockBuffer(len+1+newlen);

        // insert a '/' if needed
        if (buf[len-1] != OS_PATH_SEPARATOR)
            buf[len++] = OS_PATH_SEPARATOR;

        memcpy(buf+len, name, newlen+1);
        len += newlen;

        unlockBuffer(len);

        return *this;
    } else {
        setPathName(name);
        return *this;
    }
}

String& String::convertToResPath()
{
#if OS_PATH_SEPARATOR != RES_PATH_SEPARATOR
    size_t len = length();
    if (len > 0) {
        char * buf = lockBuffer(len);
        for (char * end = buf + len; buf < end; ++buf) {
            if (*buf == OS_PATH_SEPARATOR)
                *buf = RES_PATH_SEPARATOR;
        }
        unlockBuffer(len);
    }
#endif
    return *this;
}

}; // namespace android
