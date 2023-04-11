/*
 * Based on Haiku implementation.
 * Authors:
 *              Tyler Dauwalder
 *              Ingo Weinhold, bonefish@users.sf.net
 */

#include "File.h"

#include <Debug.h>
#include <Entry.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <syscalls.h>

#include <filesystem>

#define DEFFILEMODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)

BFile::BFile()
	: BNode(),
	  BPositionIO(),
	  fMode{0} {}

BFile::BFile(const entry_ref *ref, uint32 open_mode)
	: BFile()
{
	SetTo(ref, open_mode);
}

BFile::BFile(const BEntry *entry, uint32 open_mode)
	: BFile()
{
	SetTo(entry, open_mode);
}

BFile::BFile(const char *path, uint32 open_mode)
	: BFile()
{
	SetTo(path, open_mode);
}

BFile::BFile(const BDirectory *dir, const char *path, uint32 open_mode)
	: BFile()
{
	SetTo(dir, path, open_mode);
}

BFile::BFile(const BFile &file)
	: BFile()
{
	*this = file;
}

BFile::~BFile()
{
	// Also called by the BNode destructor, but we rather try to avoid
	// problems with calling virtual functions in the base class destructor.
	// Depending on the compiler implementation an object may be degraded to
	// an object of the base class after the destructor of the derived class
	// has been executed.
	close_fd();
}

status_t BFile::SetTo(const entry_ref *ref, uint32 open_mode)
{
	Unset();

	if (!ref)
		return (fCStatus = B_BAD_VALUE);

	// if ref->name is absolute, let the path-only SetTo() do the job
	if (std::filesystem::path(ref->name).is_absolute())
		return SetTo(ref->name, open_mode);

	open_mode |= O_CLOEXEC;

	int fd = _kern_open_entry_ref(ref->dirfd, ref->name, open_mode, DEFFILEMODE);
	if (fd >= 0) {
		set_fd(fd);
		fMode	 = open_mode;
		fCStatus = B_OK;
	}
	else
		fCStatus = fd;

	return fCStatus;
}

status_t BFile::SetTo(const BEntry *entry, uint32 open_mode)
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

status_t BFile::SetTo(const char *path, uint32 open_mode)
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

status_t BFile::SetTo(const BDirectory *dir, const char *path, uint32 open_mode)
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

bool BFile::IsReadable() const
{
	return InitCheck() == B_OK
		   && ((fMode & O_ACCMODE) == O_RDONLY || (fMode & O_ACCMODE) == O_RDWR);
}

bool BFile::IsWritable() const
{
	return InitCheck() == B_OK
		   && ((fMode & O_ACCMODE) == O_WRONLY || (fMode & O_ACCMODE) == O_RDWR);
}

ssize_t BFile::Read(void *buffer, size_t size)
{
	debugger(__PRETTY_FUNCTION__);
	return -1;
}

ssize_t BFile::ReadAt(off_t pos, void *buffer, size_t size)
{
	debugger(__PRETTY_FUNCTION__);
	return -1;
}

ssize_t BFile::Write(const void *buffer, size_t size)
{
	debugger(__PRETTY_FUNCTION__);
	return -1;
}

ssize_t BFile::WriteAt(off_t pos, const void *buffer, size_t size)
{
	debugger(__PRETTY_FUNCTION__);
	return -1;
}

off_t BFile::Seek(off_t position, uint32 seek_mode)
{
	debugger(__PRETTY_FUNCTION__);
	return -1;
}

off_t BFile::Position() const
{
	debugger(__PRETTY_FUNCTION__);
	return -1;
}

status_t BFile::SetSize(off_t size)
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

BFile &BFile::operator=(const BFile &file)
{
	debugger(__PRETTY_FUNCTION__);
	return *this;
}
