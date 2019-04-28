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

#ifndef SUPPORT_ERRORS_H
#define SUPPORT_ERRORS_H

/*!	@file support/Errors.h
	@ingroup CoreSupportUtilities
	@brief Common error codes (renaming from CmnError.h).
*/

#include <utils/Errors.h>

/*!	@addtogroup CoreSupportUtilities
	@{
*/

namespace os {
namespace support {

using status_t = ::android::status_t;

}  // namespace support
}  // namespace os

/*-------------------------------------------------------------*/

//!	General error codes.
enum general_error_codes_enum {
  B_NO_MEMORY = ::android::NO_MEMORY,
  B_BAD_VALUE = ::android::BAD_VALUE,
  //  B_NOT_ALLOWED            = sysErrNotAllowed, /*!< @copydoc sysErrNotAllowed */
  B_TIMED_OUT = ::android::TIMED_OUT,
  B_BAD_INDEX = ::android::BAD_INDEX,
  B_BAD_TYPE  = ::android::BAD_TYPE,
  //  B_MISMATCHED_VALUES      = sysErrMismatchedValues, /*!< @copydoc sysErrMismatchedValues */
  B_NAME_NOT_FOUND = ::android::NAME_NOT_FOUND,
  B_NAME_IN_USE    = ::android::ALREADY_EXISTS,
  //  B_CANCELED               = sysErrCanceled,  /*!< @copydoc sysErrCanceled */
  B_NO_INIT           = ::android::NO_INIT,
  B_PERMISSION_DENIED = ::android::PERMISSION_DENIED,
  //  B_BAD_DATA               = sysErrBadData,       /*!< @copydoc sysErrBadData */
  //  B_DATA_TRUNCATED         = sysErrDataTruncated, /*!< @copydoc sysErrDataTruncated */
  B_UNSUPPORTED = ::android::INVALID_OPERATION, /*!< @copydoc sysErrUnsupported */
  B_WOULD_BLOCK = ::android::WOULD_BLOCK,
  //  B_BUSY                   = sysErrBusy,                 /*!< @copydoc sysErrBusy */
  //  B_IO_ERROR               = sysErrIO,                   /*!< @copydoc sysErrIO */
  //  B_DONT_DO_THAT           = sysErrDontDoThat,           /*!< @copydoc sysErrDontDoThat */
  //  B_BAD_DESIGN_ENCOUNTERED = sysErrBadDesignEncountered, /*!< @copydoc sysErrBadDesignEncountered */
  //  B_WEAK_REF_GONE          = sysErrWeakRefGone,          /*!< @copydoc sysErrWeakRefGone */
  //  B_END_OF_DATA            = sysErrEndOfData,            /*!< @copydoc sysErrEndOfData */
  //  B_INTERRUPTED            = sysErrInterrupted,          /*!< @copydoc sysErrInterrupted */

  // Storage-like errors
  B_BROKEN_PIPE     = ::android::DEAD_OBJECT,
  //  B_ENTRY_NOT_FOUND = sysErrEntryNotFound, /*!< @copydoc sysErrEntryNotFound */
  //  B_ENTRY_EXISTS    = sysErrEntryExists,   /*!< @copydoc sysErrEntryExists */
  //  B_NAME_TOO_LONG   = sysErrNameTooLong,   /*!< @copydoc sysErrNameTooLong */
  //  B_OUT_OF_RANGE    = sysErrOutOfRange,    /*!< @copydoc sysErrOutOfRange */

  B_ERROR    = -1, /*!< Very generic error code.  Do not use! */
  B_OK       = ::android::OK,
  B_NO_ERROR = ::android::NO_ERROR
};

/*-------------------------------------------------------------*/

//!	Media Kit error codes.
enum media_error_codes_enum {
  //  B_MEDIA_FORMAT_MISMATCH      = mediaErrFormatMismatch,    /*!< @copydoc mediaErrFormatMismatch */
  //  B_MEDIA_ALREADY_VISITED      = mediaErrAlreadyVisited,    /*!< @copydoc mediaErrAlreadyVisited */
  //  B_MEDIA_STREAM_EXHAUSTED     = mediaErrStreamExhausted,   /*!< @copydoc mediaErrStreamExhausted */
  //  B_MEDIA_ALREADY_CONNECTED    = mediaErrAlreadyConnected,  /*!< @copydoc mediaErrAlreadyConnected */
  //  B_MEDIA_NOT_CONNECTED        = mediaErrNotConnected,      /*!< @copydoc mediaErrNotConnected */
  //  B_MEDIA_NO_BUFFER_SOURCE     = mediaErrNoBufferSource,    /*!< @copydoc mediaErrNoBufferSource */
  //  B_MEDIA_BUFFER_FLOW_MISMATCH = mediaErrBufferFlowMismatch /*!< @copydoc mediaErrBufferFlowMismatch */
};

/*-------------------------------------------------------------*/

//!	WWW error codes.
enum www_error_codes_enum {
  //  B_INVALID_URL    = exgErrInvalidURL,
  //  B_INVALID_SCHEME = exgErrInvalidScheme
};

/*-------------------------------------------------------------*/

//!	Regular Expression error codes.
//enum regexp_error_codes_enum {
  //  B_REGEXP_UNMATCHED_PARENTHESIS              = regexpErrUnmatchedParenthesis,           /*!< @copydoc regexpErrUnmatchedParenthesis */
  //  B_REGEXP_TOO_BIG                            = regexpErrTooBig,                         /*!< @copydoc regexpErrTooBig */
  //  B_REGEXP_TOO_MANY_PARENTHESIS               = regexpErrTooManyParenthesis,             /*!< @copydoc regexpErrTooManyParenthesis */
  //  B_REGEXP_JUNK_ON_END                        = regexpErrJunkOnEnd,                      /*!< @copydoc regexpErrJunkOnEnd */
  //  B_REGEXP_STAR_PLUS_OPERAND_EMPTY            = regexpErrStarPlusOneOperandEmpty,        /*!< @copydoc regexpErrStarPlusOneOperandEmpty */
  //  B_REGEXP_NESTED_STAR_QUESTION_PLUS          = regexpErrNestedStarQuestionPlus,         /*!< @copydoc regexpErrNestedStarQuestionPlus */
  //  B_REGEXP_INVALID_BRACKET_RANGE              = regexpErrInvalidBracketRange,            /*!< @copydoc regexpErrInvalidBracketRange */
  //  B_REGEXP_UNMATCHED_BRACKET                  = regexpErrUnmatchedBracket,               /*!< @copydoc regexpErrUnmatchedBracket */
  //  B_REGEXP_INTERNAL_ERROR                     = regexpErrInternalError,                  /*!< @copydoc regexpErrInternalError */
  //  B_REGEXP_QUESTION_PLUS_STAR_FOLLOWS_NOTHING = regexpErrQuestionPlusStarFollowsNothing, /*!< @copydoc regexpErrQuestionPlusStarFollowsNothing */
  //  B_REGEXP_TRAILING_BACKSLASH                 = regexpErrTrailingBackslash,              /*!< @copydoc regexpErrTrailingBackslash */
  //  B_REGEXP_CORRUPTED_PROGRAM                  = regexpErrCorruptedProgram,               /*!< @copydoc regexpErrCorruptedProgram */
  //  B_REGEXP_MEMORY_CORRUPTION                  = regexpErrMemoryCorruption,               /*!< @copydoc regexpErrMemoryCorruption */
  //  B_REGEXP_CORRUPTED_POINTERS                 = regexpErrCorruptedPointers,              /*!< @copydoc regexpErrCorruptedPointers */
  //  B_REGEXP_CORRUPTED_OPCODE                   = regexpErrCorruptedOpcode                 /*!< @copydoc regexpErrCorruptedOpcode */
//};

/*-------------------------------------------------------------*/

//!	Binder error codes.
enum binder_error_codes_enum {
  //  B_BINDER_MISSING_ARG       = bndErrMissingArg,      /*!< @copydoc bndErrMissingArg */
  //  B_BINDER_BAD_TYPE          = bndErrBadType,         /*!< @copydoc bndErrBadType */
  //  B_BINDER_DEAD              = bndErrDead,            /*!< @copydoc bndErrDead */
  B_BINDER_UNKNOWN_TRANSACT = ::android::UNKNOWN_TRANSACTION,
  //  B_BINDER_BAD_TRANSACT      = bndErrBadTransact,     /*!< @copydoc bndErrBadTransact */
  //  B_BINDER_TOO_MANY_LOOPERS  = bndErrTooManyLoopers,  /*!< @copydoc bndErrTooManyLoopers */
  //  B_BINDER_BAD_INTERFACE     = bndErrBadInterface,    /*!< @copydoc bndErrBadInterface */
  //  B_BINDER_UNKNOWN_METHOD    = bndErrUnknownMethod,   /*!< @copydoc bndErrUnknownMethod */
  //  B_BINDER_UNKNOWN_PROPERTY  = bndErrUnknownProperty, /*!< @copydoc bndErrUnknownProperty */
  //  B_BINDER_OUT_OF_STACK      = bndErrOutOfStack,      /*!< @copydoc bndErrOutOfStack */
  //  B_BINDER_INC_STRONG_FAILED = bndErrIncStrongFailed, /*!< @copydoc bndErrIncStrongFailed */
  //  B_BINDER_READ_NULL_VALUE   = bndErrReadNullValue    /*!< @copydoc bndErrReadNullValue */
};

#define B_JPARKS_BROKE_IT B_BROKEN_PIPE

/*-------------------------------------------------------------*/
/*-------------------------------------------------------------*/

/*!	@} */

#endif  // SUPPORT_ERRORS_H
