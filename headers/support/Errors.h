#ifndef _ERRORS_H
#define _ERRORS_H

#include <errno.h>
#include <limits.h>

/// Error baselines
#define B_GENERAL_ERROR_BASE INT_MIN
#define B_OS_ERROR_BASE B_GENERAL_ERROR_BASE + 0x1000
#define B_APP_ERROR_BASE B_GENERAL_ERROR_BASE + 0x2000
#define B_INTERFACE_ERROR_BASE B_GENERAL_ERROR_BASE + 0x3000
#define B_MEDIA_ERROR_BASE B_GENERAL_ERROR_BASE + 0x4000	   /* - 0x41ff */
#define B_TRANSLATION_ERROR_BASE B_GENERAL_ERROR_BASE + 0x4800 /* - 0x48ff */
#define B_MIDI_ERROR_BASE B_GENERAL_ERROR_BASE + 0x5000
#define B_STORAGE_ERROR_BASE B_GENERAL_ERROR_BASE + 0x6000
#define B_POSIX_ERROR_BASE B_GENERAL_ERROR_BASE + 0x7000
#define B_MAIL_ERROR_BASE B_GENERAL_ERROR_BASE + 0x8000
#define B_PRINT_ERROR_BASE B_GENERAL_ERROR_BASE + 0x9000
#define B_DEVICE_ERROR_BASE B_GENERAL_ERROR_BASE + 0xa000

/// Developer-defined errors start at (B_ERRORS_END+1)
#define B_ERRORS_END (B_GENERAL_ERROR_BASE + 0xffff)

/// POSIX Errors
#define B_TO_POSIX_ERROR(error) (-(error))
#define B_FROM_POSIX_ERROR(error) (-(error))
/* POSIX errors that can be mapped to BeOS error codes */
#define B_PERMISSION_DENIED B_FROM_POSIX_ERROR(EACCES)
#define B_INTERRUPTED B_FROM_POSIX_ERROR(EINTR)
#define B_IO_ERROR B_FROM_POSIX_ERROR(EIO)
#define B_BUSY B_FROM_POSIX_ERROR(EBUSY)
#define B_BAD_ADDRESS B_FROM_POSIX_ERROR(EFAULT)
#define B_TIMED_OUT B_FROM_POSIX_ERROR(ETIMEDOUT)
#define B_WOULD_BLOCK B_FROM_POSIX_ERROR(EWOULDBLOCK)
#define B_FILE_ERROR B_FROM_POSIX_ERROR(EBADF)
#define B_FILE_EXISTS B_FROM_POSIX_ERROR(EEXIST)
#define B_BAD_VALUE B_FROM_POSIX_ERROR(EINVAL)
#define B_NAME_TOO_LONG B_FROM_POSIX_ERROR(ENAMETOOLONG)
#define B_ENTRY_NOT_FOUND B_FROM_POSIX_ERROR(ENOENT)
#define B_NOT_ALLOWED B_FROM_POSIX_ERROR(EPERM)
#define B_NOT_A_DIRECTORY B_FROM_POSIX_ERROR(ENOTDIR)
#define B_IS_A_DIRECTORY B_FROM_POSIX_ERROR(EISDIR)
#define B_DIRECTORY_NOT_EMPTY B_FROM_POSIX_ERROR(ENOTEMPTY)
#define B_DEVICE_FULL B_FROM_POSIX_ERROR(ENOSPC)
#define B_READ_ONLY_DEVICE B_FROM_POSIX_ERROR(EROFS)
#define B_NO_MORE_FDS B_FROM_POSIX_ERROR(EMFILE)
#define B_CROSS_DEVICE_LINK B_FROM_POSIX_ERROR(EXDEV)
#define B_LINK_LIMIT B_FROM_POSIX_ERROR(ELOOP)
#define B_NOT_AN_EXECUTABLE B_FROM_POSIX_ERROR(ENOEXEC)
#define B_BUSTED_PIPE B_FROM_POSIX_ERROR(EPIPE)
/* new error codes that can be mapped to POSIX errors */
#define B_BUFFER_OVERFLOW B_FROM_POSIX_ERROR(EOVERFLOW)
#define B_TOO_MANY_ARGS B_FROM_POSIX_ERROR(E2BIG)
#define B_FILE_TOO_LARGE B_FROM_POSIX_ERROR(EFBIG)
#define B_RESULT_NOT_REPRESENTABLE B_FROM_POSIX_ERROR(ERANGE)
#define B_DEVICE_NOT_FOUND B_FROM_POSIX_ERROR(ENODEV)
#define B_NOT_SUPPORTED B_FROM_POSIX_ERROR(EOPNOTSUPP)

/// General Errors
enum {
	B_NO_MEMORY = B_GENERAL_ERROR_BASE,
	// B_IO_ERROR,
	// B_PERMISSION_DENIED,
	B_BAD_INDEX,
	B_BAD_TYPE,
	// B_BAD_VALUE,
	B_MISMATCHED_VALUES,
	B_NAME_NOT_FOUND,
	B_NAME_IN_USE,
	// B_TIMED_OUT,
	// B_INTERRUPTED,
	// B_WOULD_BLOCK,
	B_CANCELED,
	B_NO_INIT,
	// B_BUSY,
	// B_NOT_ALLOWED,

	B_ERROR	   = -1,
	B_OK	   = 0,
	B_NO_ERROR = 0
};

/// Kernel Kit Errors
enum {
	B_BAD_SEM_ID = B_OS_ERROR_BASE,
	B_NO_MORE_SEMS,

	B_BAD_THREAD_ID = B_OS_ERROR_BASE + 0x100,
	B_NO_MORE_THREADS,
	B_BAD_THREAD_STATE,
	B_BAD_TEAM_ID,
	B_NO_MORE_TEAMS,

	B_BAD_PORT_ID = B_OS_ERROR_BASE + 0x200,
	B_NO_MORE_PORTS,

	B_BAD_IMAGE_ID = B_OS_ERROR_BASE + 0x300,
	// B_BAD_ADDRESS,
	// B_NOT_AN_EXECUTABLE,
	B_MISSING_LIBRARY,
	B_MISSING_SYMBOL,

	B_DEBUGGER_ALREADY_INSTALLED = B_OS_ERROR_BASE + 0x400
};

/// Application Kit Errors
enum {
	B_BAD_REPLY = B_APP_ERROR_BASE,
	B_DUPLICATE_REPLY,
	B_MESSAGE_TO_SELF,
	B_BAD_HANDLER,
	B_ALREADY_RUNNING,
	B_LAUNCH_FAILED,
	B_AMBIGUOUS_APP_LAUNCH,
	B_UNKNOWN_MIME_TYPE,
	B_BAD_SCRIPT_SYNTAX,
	B_LAUNCH_FAILED_NO_RESOLVE_LINK,
	B_LAUNCH_FAILED_EXECUTABLE,
	B_LAUNCH_FAILED_APP_NOT_FOUND,
	B_LAUNCH_FAILED_APP_IN_TRASH,
	B_LAUNCH_FAILED_NO_PREFERRED_APP,
	B_LAUNCH_FAILED_FILES_APP_NOT_FOUND,
	B_BAD_MIME_SNIFFER_RULE
};

/// Storage Kit/File System Errors
enum {
	// B_FILE_ERROR = B_STORAGE_ERROR_BASE,
	B_FILE_NOT_FOUND = B_STORAGE_ERROR_BASE,  /// deprecated: use B_ENTRY_NOT_FOUND
	// B_FILE_EXISTS,
	// B_ENTRY_NOT_FOUND,
	// B_NAME_TOO_LONG,
	// B_NOT_A_DIRECTORY,
	// B_DIRECTORY_NOT_EMPTY,
	// B_DEVICE_FULL,
	// B_READ_ONLY_DEVICE,
	// B_IS_A_DIRECTORY,
	// B_NO_MORE_FDS,
	// B_CROSS_DEVICE_LINK,
	// B_LINK_LIMIT,
	// B_BUSTED_PIPE,
	B_UNSUPPORTED,
	B_PARTITION_TOO_SMALL
};

#endif /* _DERRORS_H */
