#include "Directory.h"

#include <Debug.h>
#include <syscalls.h>

BDirectory::BDirectory()
	: BNode(), BEntryList(), fDirFd{-1} {}

BDirectory::BDirectory(const BEntry *entry)
	: BDirectory()
{
	SetTo(entry);
}

BDirectory::BDirectory(const entry_ref *ref)
	: BDirectory()
{
	SetTo(ref);
}

BDirectory::BDirectory(const char *path)
	: BDirectory()
{
	SetTo(path);
}

BDirectory::BDirectory(const BDirectory *dir, const char *path)
	: BDirectory()
{
	SetTo(dir, path);
}

BDirectory::BDirectory(const node_ref *ref)
	: BDirectory()
{
	SetTo(ref);
}

BDirectory::BDirectory(const BDirectory &dir)
	: BDirectory()
{
	*this = dir;
}

BDirectory::~BDirectory()
{
	close_fd();
}

status_t BDirectory::SetTo(const entry_ref *ref)
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

status_t BDirectory::SetTo(const BEntry *entry)
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

status_t BDirectory::SetTo(const char *path)
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

status_t BDirectory::SetTo(const BDirectory *dir, const char *path)
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

status_t BDirectory::SetTo(const node_ref *ref)
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

status_t BDirectory::GetEntry(BEntry *entry) const
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

status_t BDirectory::GetNextEntry(BEntry *entry, bool traverse)
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

status_t BDirectory::GetNextRef(entry_ref *ref)
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

int32 BDirectory::GetNextDirents(struct dirent *buf, size_t length, int32 count)
{
	debugger(__PRETTY_FUNCTION__);
	return -1;
}

status_t BDirectory::Rewind()
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

int32 BDirectory::CountEntries()
{
	debugger(__PRETTY_FUNCTION__);
	return -1;
}

BDirectory &BDirectory::operator=(const BDirectory &dir)
{
	if (&dir != this) {	 // no need to assign us to ourselves
		Unset();
		if (dir.InitCheck() == B_OK)
			SetTo(&dir, ".");
	}
	return *this;
}

void BDirectory::close_fd()
{
	if (fDirFd >= 0) {
		_kern_close(fDirFd);
		fDirFd = -1;
	}
	BNode::close_fd();
}

int BDirectory::get_fd() const
{
	return fDirFd;
}
