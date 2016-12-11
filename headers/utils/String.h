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

#ifndef ANDROID_STRING_H
#define ANDROID_STRING_H

#include <utils/Errors.h>
#include <utils/Unicode.h>
#include <utils/TypeHelpers.h>

#include <string.h> // for strcmp
#include <stdarg.h>

// ---------------------------------------------------------------------------

namespace android {

class TextOutput;

//! This is a string holding UTF-8 characters. Does not allow the value more
// than 0x10FFFF, which is not valid unicode codepoint.
class String
{
public:
    /* use String(StaticLinkage) if you're statically linking against
     * libutils and declaring an empty static String, e.g.:
     *
     *   static String sAStaticEmptyString(String::kEmptyString);
     *   static String sAnotherStaticEmptyString(sAStaticEmptyString);
     */
    enum StaticLinkage { kEmptyString };

                                String();
    explicit                    String(StaticLinkage);
                                String(const String& o);
    explicit                    String(const char* o);
    explicit                    String(const char* o, size_t numChars);

    explicit                    String(const char16_t* o);
    explicit                    String(const char16_t* o, size_t numChars);
    explicit                    String(const char32_t* o);
    explicit                    String(const char32_t* o, size_t numChars);
                                ~String();

    static inline const String  empty();

    static String               format(const char* fmt, ...) __attribute__((format (printf, 1, 2)));
    static String               formatV(const char* fmt, va_list args);

    inline  const char*         string() const;
    inline  size_t              size() const;
    inline  size_t              bytes() const;
    inline  bool                isEmpty() const;
    
            size_t              length() const;
    
            void                clear();

            void                setTo(const String& other);
            status_t            setTo(const char* other);
            status_t            setTo(const char* other, size_t numChars);
            status_t            setTo(const char16_t* other, size_t numChars);
            status_t            setTo(const char32_t* other,
                                      size_t length);

            status_t            append(const String& other);
            status_t            append(const char* other);
            status_t            append(const char* other, size_t numChars);

            status_t            appendFormat(const char* fmt, ...)
                    __attribute__((format (printf, 2, 3)));
            status_t            appendFormatV(const char* fmt, va_list args);

    inline  String&             operator=(const String& other);
    inline  String&             operator=(const char* other);

    inline  String&             operator+=(const String& other);
    inline  String              operator+(const String& other) const;

    inline  String&             operator+=(const char* other);
    inline  String              operator+(const char* other) const;

    inline  int                 compare(const String& other) const;

    inline  bool                operator<(const String& other) const;
    inline  bool                operator<=(const String& other) const;
    inline  bool                operator==(const String& other) const;
    inline  bool                operator!=(const String& other) const;
    inline  bool                operator>=(const String& other) const;
    inline  bool                operator>(const String& other) const;

    inline  bool                operator<(const char* other) const;
    inline  bool                operator<=(const char* other) const;
    inline  bool                operator==(const char* other) const;
    inline  bool                operator!=(const char* other) const;
    inline  bool                operator>=(const char* other) const;
    inline  bool                operator>(const char* other) const;
    
    inline                      operator const char*() const;
    
            char*               lockBuffer(size_t size);
            void                unlockBuffer();
            status_t            unlockBuffer(size_t size);
            
            // return the index of the first byte of other in this at or after
            // start, or -1 if not found
            ssize_t             find(const char* other, size_t start = 0) const;

            // return true if this string contains the specified substring
    inline  bool                contains(const char* other) const;

            // removes all occurrence of the specified substring
            // returns true if any were found and removed
            bool                removeAll(const char* other);

            void                toLower();
            void                toLower(size_t start, size_t numChars);
            void                toUpper();
            void                toUpper(size_t start, size_t numChars);


    /*
     * These methods operate on the string as if it were a path name.
     */

    /*
     * Set the filename field to a specific value.
     *
     * Normalizes the filename, removing a trailing '/' if present.
     */
    void setPathName(const char* name);
    void setPathName(const char* name, size_t numChars);

    /*
     * Get just the filename component.
     *
     * "/tmp/foo/bar.c" --> "bar.c"
     */
    String getPathLeaf(void) const;

    /*
     * Remove the last (file name) component, leaving just the directory
     * name.
     *
     * "/tmp/foo/bar.c" --> "/tmp/foo"
     * "/tmp" --> "" // ????? shouldn't this be "/" ???? XXX
     * "bar.c" --> ""
     */
    String getPathDir(void) const;

    /*
     * Retrieve the front (root dir) component.  Optionally also return the
     * remaining components.
     *
     * "/tmp/foo/bar.c" --> "tmp" (remain = "foo/bar.c")
     * "/tmp" --> "tmp" (remain = "")
     * "bar.c" --> "bar.c" (remain = "")
     */
    String walkPath(String* outRemains = NULL) const;

    /*
     * Return the filename extension.  This is the last '.' and any number
     * of characters that follow it.  The '.' is included in case we
     * decide to expand our definition of what constitutes an extension.
     *
     * "/tmp/foo/bar.c" --> ".c"
     * "/tmp" --> ""
     * "/tmp/foo.bar/baz" --> ""
     * "foo.jpeg" --> ".jpeg"
     * "foo." --> ""
     */
    String getPathExtension(void) const;

    /*
     * Return the path without the extension.  Rules for what constitutes
     * an extension are described in the comment for getPathExtension().
     *
     * "/tmp/foo/bar.c" --> "/tmp/foo/bar"
     */
    String getBasePath(void) const;

    /*
     * Add a component to the pathname.  We guarantee that there is
     * exactly one path separator between the old path and the new.
     * If there is no existing name, we just copy the new name in.
     *
     * If leaf is a fully qualified path (i.e. starts with '/', it
     * replaces whatever was there before.
     */
    String& appendPath(const char* leaf);
    String& appendPath(const String& leaf)  { return appendPath(leaf.string()); }

    /*
     * Like appendPath(), but does not affect this string.  Returns a new one instead.
     */
    String appendPathCopy(const char* leaf) const
                                             { String p(*this); p.appendPath(leaf); return p; }
    String appendPathCopy(const String& leaf) const { return appendPathCopy(leaf.string()); }

    /*
     * Converts all separators in this string to /, the default path separator.
     *
     * If the default OS separator is backslash, this converts all
     * backslashes to slashes, in-place. Otherwise it does nothing.
     * Returns self.
     */
    String& convertToResPath();

private:
            status_t            real_append(const char* other, size_t numChars);
            char*               find_extension(void) const;

            const char* mString;
};

// String can be trivially moved using memcpy() because moving does not
// require any change to the underlying SharedBuffer contents or reference count.
ANDROID_TRIVIAL_MOVE_TRAIT(String)

// ---------------------------------------------------------------------------
// No user servicable parts below.

inline int compare_type(const String& lhs, const String& rhs)
{
    return lhs.compare(rhs);
}

inline int strictly_order_type(const String& lhs, const String& rhs)
{
    return compare_type(lhs, rhs) < 0;
}

inline const String String::empty() {
    return String();
}

inline const char* String::string() const
{
    return mString;
}

inline size_t String::size() const
{
    return length();
}

inline bool String::isEmpty() const
{
    return length() == 0;
}

inline size_t String::bytes() const
{
    return length();
}

inline bool String::contains(const char* other) const
{
    return find(other) >= 0;
}

inline String& String::operator=(const String& other)
{
    setTo(other);
    return *this;
}

inline String& String::operator=(const char* other)
{
    setTo(other);
    return *this;
}

inline String& String::operator+=(const String& other)
{
    append(other);
    return *this;
}

inline String String::operator+(const String& other) const
{
    String tmp(*this);
    tmp += other;
    return tmp;
}

inline String& String::operator+=(const char* other)
{
    append(other);
    return *this;
}

inline String String::operator+(const char* other) const
{
    String tmp(*this);
    tmp += other;
    return tmp;
}

inline int String::compare(const String& other) const
{
    return strcmp(mString, other.mString);
}

inline bool String::operator<(const String& other) const
{
    return strcmp(mString, other.mString) < 0;
}

inline bool String::operator<=(const String& other) const
{
    return strcmp(mString, other.mString) <= 0;
}

inline bool String::operator==(const String& other) const
{
    return strcmp(mString, other.mString) == 0;
}

inline bool String::operator!=(const String& other) const
{
    return strcmp(mString, other.mString) != 0;
}

inline bool String::operator>=(const String& other) const
{
    return strcmp(mString, other.mString) >= 0;
}

inline bool String::operator>(const String& other) const
{
    return strcmp(mString, other.mString) > 0;
}

inline bool String::operator<(const char* other) const
{
    return strcmp(mString, other) < 0;
}

inline bool String::operator<=(const char* other) const
{
    return strcmp(mString, other) <= 0;
}

inline bool String::operator==(const char* other) const
{
    return strcmp(mString, other) == 0;
}

inline bool String::operator!=(const char* other) const
{
    return strcmp(mString, other) != 0;
}

inline bool String::operator>=(const char* other) const
{
    return strcmp(mString, other) >= 0;
}

inline bool String::operator>(const char* other) const
{
    return strcmp(mString, other) > 0;
}

inline String::operator const char*() const
{
    return mString;
}

}  // namespace android

// ---------------------------------------------------------------------------

#endif // ANDROID_STRING_H
