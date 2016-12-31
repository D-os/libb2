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

#ifndef _SUPPORT_STATIC_VALUE_H_
#define _SUPPORT_STATIC_VALUE_H_

/*!	@file support/StaticValue.h
    @ingroup CoreSupportUtilities
    @brief Optimized representations of static Value constants.
*/

#include <stdint.h>
#include <support/Debug.h>
#include <support/SupportDefs.h>
#include <utils/SharedBuffer.h>
#include <support/TypeConstants.h>

namespace os {
namespace support {

/*!	@addtogroup CoreSupportUtilities
@{
*/

class SString;
class SValue;

// the extra cast to void* stops the stu-pid ADS warning:
// Warning: C2354W: cast from ptr/ref to 'class Value' to ptr/ref to 'struct static_large_value'; one is undefined, assuming unrelated

//	--------------------------------------------------------------------

/*!	This is a static representation of a Value containing 4 or fewer bytes
of data (with this data placed in-line), which can be used to create
constant values for placement in the text section of an executable. */
struct static_small_value
{
    uint32_t		type;
    char			data[4];

    inline operator const SValue&() const { return *(const SValue*)(void*)this; }
};

/*!	This is a static representation of a Value containing more than 4 bytes
of data (pointing to its data out-of-line), which can be used to create
constant values for placement in the text section of an executable. */
struct static_large_value
{
    uint32_t		type;
    const void*	data;

    inline operator const SValue&() const { return *(const SValue*)(void*)this; }
};

/*!	Special version of static_small_value for a value holding a string. */
struct static_small_string_value
{
    uint32_t		type;
    char			data[4];
    const void*	data_str;

    inline operator const SString&() const { return *(const SString*)(void*)&data_str; }
    inline operator const SValue&() const { return *(const SValue*)(void*)this; }
};

/*!	Special version of static_small_value for a value holding a string. */
struct static_large_string_value
{
    uint32_t		type;
    const void*	data;
    const void*	data_str;		// XXX could change SString to point to the SSharedBuffer

    inline operator const SString&() const { return *(const SString*)(void*)&data_str; }
    inline operator const SValue&() const { return *(const SValue*)(void*)this; }
};

/*!	This is a static representation of a Value containing a 32-bit
integer, which can be used to create constant values for placement
in the text section of an executable. */
struct static_int32_value
{
    uint32_t	type;
    int32_t		value;

    inline operator const SValue&() const { return *(const SValue*)(void*)this; }
};

/*!	This is a static representation of a Value containing a 32-bit
float, which can be used to create constant values for placement
in the text section of an executable. */
struct static_float_value
{
    uint32_t	type;
    float		value;

    inline operator const SValue&() const { return *(const SValue*)(void*)this; }
};

/*!	This is a static representation of a Value containing a boolean,
which can be used to create constant values for placement
in the text section of an executable. */
struct static_bool_value
{
    uint32_t	type;
    // The data is not a real bool type because the size of bool
    // changes between compilers.  (For example, in ADS it is
    // 4 bytes.)
    int8_t		value;

    inline operator const SValue&() const { return *(const SValue*)(void*)this; }
};

//	--------------------------------------------------------------------

#define STRING_ASSERT(x) inline void string_assert() { STATIC_ASSERT(x); }
#define PADDED_STRING_LENGTH(string) sizeof(string)+((4-(sizeof(string)%4)) & 0x3)

//!	Convenience macro for making a static Value containing a string of 4 or fewer (including the terminating \\0) characters.
#define B_CONST_STRING_VALUE_SMALL(ident, string, prefix)				\
const struct {														\
STRING_ASSERT(sizeof(string)<=4);								\
struct static_shared_buffer buff;								\
char    data[PADDED_STRING_LENGTH(string)];						\
} ident##str = {													\
static_shared_buffer(sizeof(string)), string						\
};																	\
const static_small_string_value prefix##ident = {					\
B_PACK_SMALL_TYPE(B_STRING_TYPE, sizeof(string)), string, ident##str.buff.data()		\
};																	\

//!	Convenience macro for making a static Value containing a string of	4 or fewer (including the terminating \\0) characters.
#define B_STATIC_STRING_VALUE_SMALL(ident, string, prefix)				\
static const struct {												\
STRING_ASSERT(sizeof(string)<=4);								\
struct static_shared_buffer buff;								\
char    data[PADDED_STRING_LENGTH(string)];						\
} ident##str = {													\
static_shared_buffer(sizeof(string)), string						\
} ident##str = {													\
B_STATIC_USERS,													\
sizeof(string)<<B_BUFFER_LENGTH_SHIFT, string					\
};																	\
static const static_small_string_value prefix##ident = {			\
B_PACK_SMALL_TYPE(B_STRING_TYPE, sizeof(string)), string, ident##str.buff.data()		\
};																	\

//!	Convenience macro for making a static Value containing a string of	5 or more (including the terminating \\0) characters.
#define B_CONST_STRING_VALUE_LARGE(ident, string, prefix)				\
const struct {														\
STRING_ASSERT(sizeof(string)>4);								\
struct static_shared_buffer buff;								\
char    data[PADDED_STRING_LENGTH(string)];						\
} ident##str = {													\
static_shared_buffer(sizeof(string)), string						\
};																	\
const static_large_string_value prefix##ident = {					\
B_PACK_LARGE_TYPE(B_STRING_TYPE), string, ident##str.buff.data()		\
};																	\

//!	Convenience macro for making a static Value containing a string of	5 or more (including the terminating \\0) characters.
#define B_STATIC_STRING_VALUE_LARGE(ident, string, prefix)				\
static const struct {												\
STRING_ASSERT(sizeof(string)>4);								\
struct static_shared_buffer buff;								\
char    data[PADDED_STRING_LENGTH(string)];						\
} ident##str = {													\
static_shared_buffer(sizeof(string)), string						\
};																	\
static const static_large_string_value prefix##ident = {			\
B_PACK_LARGE_TYPE(B_STRING_TYPE), string, ident##str.buff.data()		\
};																	\

//!	Convenience macro for making a static Value containing an int32_t.
#define B_CONST_INT32_VALUE(ident, val, prefix)							\
const static_int32_value prefix##ident = {							\
B_PACK_SMALL_TYPE(B_INT32_TYPE, sizeof(int32_t)), val			\
};																	\

//!	Convenience macro for making a static Value containing an int32_t.
#define B_STATIC_INT32_VALUE(ident, val, prefix)						\
static const static_int32_value prefix##ident = {					\
B_PACK_SMALL_TYPE(B_INT32_TYPE, sizeof(int32_t)), val			\
};																	\

//!	Convenience macro for making a static Value containing a float.
#define B_CONST_FLOAT_VALUE(ident, val, prefix)							\
const static_float_value prefix##ident = {							\
B_PACK_SMALL_TYPE(B_FLOAT_TYPE, sizeof(float)), val			\
};																	\

//!	Convenience macro for making a static Value containing a float.
#define B_STATIC_FLOAT_VALUE(ident, val, prefix)						\
static const static_float_value prefix##ident = {					\
B_PACK_SMALL_TYPE(B_FLOAT_TYPE, sizeof(float)), val			\
};																	\

/*!	@} */

// Compatibility.  Don't use!
#define B_CONST_STRING_VALUE_4 B_CONST_STRING_VALUE_SMALL
#define B_STATIC_STRING_VALUE_4 B_STATIC_STRING_VALUE_SMALL
#define B_CONST_STRING_VALUE_8 B_CONST_STRING_VALUE_LARGE
#define B_STATIC_STRING_VALUE_8 B_STATIC_STRING_VALUE_LARGE
#define B_CONST_STRING_VALUE_12 B_CONST_STRING_VALUE_LARGE
#define B_STATIC_STRING_VALUE_12 B_STATIC_STRING_VALUE_LARGE
#define B_CONST_STRING_VALUE_16 B_CONST_STRING_VALUE_LARGE
#define B_STATIC_STRING_VALUE_16 B_STATIC_STRING_VALUE_LARGE
#define B_CONST_STRING_VALUE_20 B_CONST_STRING_VALUE_LARGE
#define B_STATIC_STRING_VALUE_20 B_STATIC_STRING_VALUE_LARGE
#define B_CONST_STRING_VALUE_24 B_CONST_STRING_VALUE_LARGE
#define B_STATIC_STRING_VALUE_24 B_STATIC_STRING_VALUE_LARGE
#define B_CONST_STRING_VALUE_28 B_CONST_STRING_VALUE_LARGE
#define B_STATIC_STRING_VALUE_28 B_STATIC_STRING_VALUE_LARGE
#define B_CONST_STRING_VALUE_32 B_CONST_STRING_VALUE_LARGE
#define B_STATIC_STRING_VALUE_32 B_STATIC_STRING_VALUE_LARGE
#define B_CONST_STRING_VALUE_36 B_CONST_STRING_VALUE_LARGE
#define B_STATIC_STRING_VALUE_36 B_STATIC_STRING_VALUE_LARGE
#define B_CONST_STRING_VALUE_40 B_CONST_STRING_VALUE_LARGE
#define B_STATIC_STRING_VALUE_40 B_STATIC_STRING_VALUE_LARGE
#define B_CONST_STRING_VALUE_44 B_CONST_STRING_VALUE_LARGE
#define B_STATIC_STRING_VALUE_44 B_STATIC_STRING_VALUE_LARGE
#define B_STATIC_STRING	B_STATIC_STRING_VALUE_LARGE
#define B_CONST_STRING	B_CONST_STRING_VALUE_LARGE

//	--------------------------------------------------------------------

} }	// namespace palmos::support

#endif	/* _SUPPORT_STATIC_VALUE_H_ */
