#include "File.h"

#include <Debug.h>

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

BFile::~BFile() {}

status_t BFile::SetTo(const entry_ref *ref, uint32 open_mode)
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
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
