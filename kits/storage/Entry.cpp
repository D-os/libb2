/*
 * Based on Haiku implementation.
 * Authors:
 *              Tyler Dauwalder
 *              Ingo Weinhold, bonefish@users.sf.net
 */

#include "Entry.h"

#include <Directory.h>
#include <Path.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <syscalls.h>
#include <unistd.h>

#include <filesystem>

class FDCloser
{
   public:
	FDCloser(int fd)
		: fFD{fd}
	{
	}

	~FDCloser()
	{
		Close();
	}

	void SetTo(int fd)
	{
		Close();
		fFD = fd;
	}

	void Close()
	{
		if (fFD >= 0)
			_kern_close(fFD);
		fFD = -1;
	}

	int Detach()
	{
		int fd = fFD;
		fFD	   = -1;
		return fd;
	}

   private:
	int fFD;
};

static status_t parse_path(const char* fullPath, char* dirPath, char* leafName)
{
	auto path = std::filesystem::path(fullPath);
	if (path.empty())
		return B_BAD_VALUE;
	auto dir  = path.parent_path().string();
	auto leaf = path.filename().string();

	if (dir.length() >= B_PATH_NAME_LENGTH
		|| leaf.length() >= B_FILE_NAME_LENGTH) {
		return B_NAME_TOO_LONG;
	}
	// copy the result strings
	if (dirPath)
		strlcpy(dirPath, dir.c_str(), dir.length());
	if (leafName)
		strlcpy(leafName, leaf.c_str(), leaf.length());
	return B_OK;
}

/*!     An entry name is considered valid, if its length doesn't exceed
		\c B_FILE_NAME_LENGTH (including the terminating null) and it doesn't
		contain any \c "/".
		\param entry the entry name
		\return
		- \c B_OK, if \a entry is valid,
		- \c B_BAD_VALUE, if \a entry is \c NULL or contains a "/",
		- \c B_NAME_TOO_LONG, if \a entry is too long
		\note \c "" is considered a valid entry name.
*/
status_t check_entry_name(const char* entry)
{
	status_t error = (entry ? B_OK : B_BAD_VALUE);
	if (error == B_OK) {
		if (strlen(entry) >= B_FILE_NAME_LENGTH)
			error = B_NAME_TOO_LONG;
	}
	if (error == B_OK) {
		for (int32 i = 0; error == B_OK && entry[i] != '\0'; i++) {
			if (entry[i] == '/')
				error = B_BAD_VALUE;
		}
	}
	return error;
}

#pragma mark - struct entry_ref

entry_ref::entry_ref()
	: dirfd{-1},
	  name{nullptr}
{
}

entry_ref::entry_ref(int dirfd, const char* name)
	: dirfd(dirfd),
	  name{nullptr}
{
	set_name(name);
}

entry_ref::entry_ref(const entry_ref& ref)
	: entry_ref()
{
	dirfd = dup(ref.dirfd);
	set_name(ref.name);
}

entry_ref::~entry_ref()
{
	close(dirfd);
	free(name);
}

status_t entry_ref::set_name(const char* name)
{
	free(this->name);

	if (name) {
		this->name = strdup(name);
		if (!this->name)
			return B_NO_MEMORY;
	}
	else {
		this->name = nullptr;
	}

	return B_OK;
}

bool entry_ref::operator==(const entry_ref& other) const
{
	struct stat this_stat, other_stat;
	fstat(dirfd, &this_stat);
	fstat(other.dirfd, &other_stat);
	return (this_stat.st_dev == other_stat.st_dev
			&& this_stat.st_ino == other_stat.st_ino
			&& (name == other.name
				|| (name && other.name && strcmp(name, other.name) == 0)));
}

bool entry_ref::operator!=(const entry_ref& other) const
{
	return !(*this == other);
}

entry_ref& entry_ref::operator=(const entry_ref& other)
{
	if (this == &other)
		return *this;

	close(dirfd);
	dirfd = dup(other.dirfd);
	set_name(other.name);
	return *this;
}

#pragma mark - BEntry

BEntry::BEntry()
	: fDirFd{-1},
	  fName{nullptr},
	  fCStatus{B_NO_INIT}
{
}

BEntry::BEntry(const BDirectory* dir, const char* path, bool traverse)
	: BEntry()
{
	SetTo(dir, path, traverse);
}

BEntry::BEntry(const entry_ref* ref, bool traverse)
	: BEntry()
{
	SetTo(ref, traverse);
}

BEntry::BEntry(const char* path, bool traverse)
	: BEntry()
{
	SetTo(path, traverse);
}

BEntry::BEntry(const BEntry& entry)
	: BEntry()
{
	*this = entry;
}

BEntry::~BEntry()
{
	Unset();
}

status_t BEntry::InitCheck() const
{
	return fCStatus;
}

bool BEntry::Exists() const
{
	// just stat the beast
	struct stat st;
	return GetStat(&st) == B_OK;
}

const char* BEntry::Name() const
{
	if (fCStatus != B_OK)
		return nullptr;

	return fName;
}

status_t BEntry::GetStat(struct stat* st) const
{
	if (!st || fDirFd < 0 || !fName || !fName[0])
		return B_BAD_VALUE;

	if (fstatat(fDirFd, fName, st, AT_NO_AUTOMOUNT) != 0) {
		int e = errno;
		switch (e) {
			case EBADF:
			case ENOENT:
				return B_BAD_VALUE;
			case ENOMEM:
				return B_NO_MEMORY;
		}
		return B_FROM_POSIX_ERROR(errno);
	}
	return B_OK;
}

status_t BEntry::GetNodeRef(node_ref* ref) const
{
	if (!ref)
		return B_BAD_VALUE;

	int fd = openat(fDirFd, fName, O_CLOEXEC);
	if (fd < 0) {
		int e = errno;
		switch (e) {
			case EBADF:
			case ENOENT:
				return B_BAD_VALUE;
			case ENOMEM:
				return B_NO_MEMORY;
		}
		return B_FROM_POSIX_ERROR(errno);
	}

	*ref = std::move(node_ref(fd));
	return B_OK;
}

status_t BEntry::SetTo(const BDirectory* dir, const char* path, bool traverse)
{
	// check params
	if (!dir)
		return (fCStatus = B_BAD_VALUE);
	if (path && path[0] == '\0')  // R5 behaviour
		path = nullptr;

	// if path is absolute, let the path-only SetTo() do the job
	if (std::filesystem::path(path).is_absolute())
		return SetTo(path, traverse);

	Unset();

	if (dir->InitCheck() != B_OK)
		fCStatus = B_BAD_VALUE;

	// dup() the dir's FD and let set() do the rest
	int dirFD = _kern_dup(dir->get_fd());
	if (dirFD < 0)
		return (fCStatus = dirFD);
	return (fCStatus = _SetTo(dirFD, path, traverse));
}

status_t BEntry::SetTo(const entry_ref* ref, bool traverse)
{
	Unset();
	if (ref == NULL)
		return (fCStatus = B_BAD_VALUE);

	// if ref-name is absolute, let the path-only SetTo() do the job
	if (std::filesystem::path(ref->name).is_absolute())
		return SetTo(ref->name, traverse);

	if (ref->dirfd < 0)
		return (fCStatus = ref->dirfd);
	return (fCStatus = _SetTo(ref->dirfd, ref->name, traverse));
}

status_t BEntry::SetTo(const char* path, bool traverse)
{
	Unset();
	// check the argument
	if (!path)
		return (fCStatus = B_BAD_VALUE);
	return (fCStatus = _SetTo(-1, path, traverse));
}

void BEntry::Unset()
{
	// Close the directory
	if (fDirFd >= 0)
		_kern_close(fDirFd);

	// Free our leaf name
	free(fName);

	fDirFd	 = -1;
	fName	 = nullptr;
	fCStatus = B_NO_INIT;
}

status_t BEntry::GetRef(entry_ref* ref) const
{
	if (fCStatus != B_OK)
		return B_NO_INIT;

	if (ref == NULL)
		return B_BAD_VALUE;

	status_t error;
	close(ref->dirfd);
	ref->dirfd = dup(fDirFd);
	if (ref->dirfd < 0) {
		error = B_FROM_POSIX_ERROR(errno);
	}
	else {
		error = ref->set_name(fName);
	}
	return error;
}

status_t BEntry::GetPath(BPath* path) const
{
	if (fCStatus != B_OK)
		return B_NO_INIT;

	if (path == NULL)
		return B_BAD_VALUE;

	return path->SetTo(this);
}

status_t BEntry::GetParent(BEntry* entry) const
{
	// check parameter and initialization
	if (fCStatus != B_OK)
		return B_NO_INIT;
	if (entry == NULL)
		return B_BAD_VALUE;

	// check whether we are the root directory
	// It is sufficient to check whether our leaf name is ".".
	if (strcmp(fName, ".") == 0)
		return B_ENTRY_NOT_FOUND;

	// open the parent directory
	char leafName[B_FILE_NAME_LENGTH];
	int	 parentFD = _kern_open_parent_dir(fDirFd, leafName, B_FILE_NAME_LENGTH);
	if (parentFD < 0)
		return parentFD;

	// set close on exec flag on dir FD
	fcntl(parentFD, F_SETFD, FD_CLOEXEC);

	// init the entry
	entry->Unset();
	entry->fDirFd	= parentFD;
	entry->fCStatus = entry->_SetName(leafName);
	if (entry->fCStatus != B_OK)
		entry->Unset();
	return entry->fCStatus;
}

status_t BEntry::GetParent(BDirectory* dir) const
{
	// check initialization and parameter
	if (fCStatus != B_OK)
		return B_NO_INIT;
	if (dir == NULL)
		return B_BAD_VALUE;
	// check whether we are the root directory
	// It is sufficient to check whether our leaf name is ".".
	if (strcmp(fName, ".") == 0)
		return B_ENTRY_NOT_FOUND;
	// get a node ref for the directory and init it
	entry_ref entry;
	status_t  error = GetRef(&entry);
	if (error != B_OK)
		return error;
	node_ref ref;
	ref.fd = dup(entry.dirfd);
	return dir->SetTo(&ref);
}

status_t BEntry::GetName(char* buffer) const
{
	if (fCStatus != B_OK)
		return B_NO_INIT;
	if (buffer == NULL)
		return B_BAD_VALUE;

	strcpy(buffer, fName);
	return B_OK;
}

status_t BEntry::Rename(const char* path, bool clobber)
{
	// check parameter and initialization
	if (path == NULL)
		return B_BAD_VALUE;
	if (fCStatus != B_OK)
		return B_NO_INIT;
	// get an entry representing the target location
	BEntry	 target;
	status_t error;
	if (std::filesystem::path(path).is_absolute()) {
		error = target.SetTo(path);
	}
	else {
		int dirFD = _kern_dup(fDirFd);
		if (dirFD < 0)
			return dirFD;
		// init the entry
		error = target.fCStatus = target._SetTo(dirFD, path, false);
	}
	if (error != B_OK)
		return error;
	return _Rename(target, clobber);
}

status_t BEntry::MoveTo(BDirectory* dir, const char* path, bool clobber)
{
	// check parameters and initialization
	if (fCStatus != B_OK)
		return B_NO_INIT;
	if (!dir)
		return B_BAD_VALUE;
	if (dir->InitCheck() != B_OK)
		return B_BAD_VALUE;
	// NULL path simply means move without renaming
	if (!path)
		path = fName;
	// get an entry representing the target location
	BEntry	 target;
	status_t error = target.SetTo(dir, path);
	if (error != B_OK)
		return error;
	return _Rename(target, clobber);
}

status_t BEntry::Remove()
{
	if (fCStatus != B_OK)
		return B_NO_INIT;

	if (IsDirectory())
		return _kern_remove_dir(fDirFd, fName);

	return _kern_unlink(fDirFd, fName);
}

bool BEntry::operator==(const BEntry& item) const
{
	// First check statuses
	if (this->InitCheck() != B_OK && item.InitCheck() != B_OK) {
		return true;
	}
	else if (this->InitCheck() == B_OK && item.InitCheck() == B_OK) {
		// Directories don't compare well directly, so we'll
		// compare entry_refs instead
		entry_ref ref1, ref2;
		if (this->GetRef(&ref1) != B_OK)
			return false;
		if (item.GetRef(&ref2) != B_OK)
			return false;
		return (ref1 == ref2);
	}
	else {
		return false;
	}
}

bool BEntry::operator!=(const BEntry& item) const
{
	return !(*this == item);
}

BEntry& BEntry::operator=(const BEntry& item)
{
	if (this == &item)
		return *this;

	Unset();
	if (item.fCStatus == B_OK) {
		fDirFd = _kern_dup(item.fDirFd);
		if (fDirFd >= 0)
			fCStatus = _SetName(item.fName);
		else
			fCStatus = fDirFd;

		if (fCStatus != B_OK)
			Unset();
	}

	return *this;
}

status_t BEntry::_SetTo(int dirFD, const char* path, bool traverse)
{
	bool	 requireConcrete = false;
	FDCloser fdCloser(dirFD);
	char	 tmpPath[B_PATH_NAME_LENGTH];
	char	 leafName[B_FILE_NAME_LENGTH];
	int32	 linkLimit = B_MAX_SYMLINKS;
	while (true) {
		if (!path || strcmp(path, ".") == 0) {
			// "."
			// if no dir FD is supplied, we need to open the current directory
			// first
			if (dirFD < 0) {
				dirFD = _kern_open_dir(-1, ".");
				if (dirFD < 0)
					return dirFD;
				fdCloser.SetTo(dirFD);
			}
			// get the parent directory
			int parentFD = _kern_open_parent_dir(dirFD, leafName, B_FILE_NAME_LENGTH);
			if (parentFD < 0)
				return parentFD;
			dirFD = parentFD;
			fdCloser.SetTo(dirFD);
			break;
		}
		else if (strcmp(path, "..") == 0) {
			// ".."
			// open the parent directory
			int parentFD = _kern_open_dir(dirFD, "..");
			if (parentFD < 0)
				return parentFD;
			dirFD = parentFD;
			fdCloser.SetTo(dirFD);
			// get the parent's parent directory
			parentFD = _kern_open_parent_dir(dirFD, leafName, B_FILE_NAME_LENGTH);
			if (parentFD < 0)
				return parentFD;
			dirFD = parentFD;
			fdCloser.SetTo(dirFD);
			break;
		}
		else {
			// an ordinary path; analyze it
			char	 dirPath[B_PATH_NAME_LENGTH];
			status_t error = parse_path(path, dirPath, leafName);
			if (error != B_OK)
				return error;
			// special case: root directory ("/")
			if (leafName[0] == '\0' && dirPath[0] == '/')
				strcpy(leafName, ".");
			if (leafName[0] == '\0') {
				// the supplied path is already a leaf
				error = check_entry_name(dirPath);
				if (error != B_OK)
					return error;
				strcpy(leafName, dirPath);
				// if no directory was given, we need to open the current dir
				// now
				if (dirFD < 0) {
					char* cwd = getcwd(tmpPath, B_PATH_NAME_LENGTH);
					if (!cwd)
						return B_ERROR;
					dirFD = _kern_open_dir(-1, cwd);
					if (dirFD < 0)
						return dirFD;
					fdCloser.SetTo(dirFD);
				}
			}
			else if (strcmp(leafName, ".") == 0
					 || strcmp(leafName, "..") == 0) {
				// We have to resolve this to get the entry name. Just open
				// the dir and let the next iteration deal with it.
				dirFD = _kern_open_dir(-1, path);
				if (dirFD < 0)
					return dirFD;
				fdCloser.SetTo(dirFD);
				path = nullptr;
				continue;
			}
			else {
				int parentFD = _kern_open_dir(dirFD, dirPath);
				if (parentFD < 0)
					return parentFD;
				dirFD = parentFD;
				fdCloser.SetTo(dirFD);
			}
			// traverse symlinks, if desired
			if (!traverse)
				break;
			struct stat st;
			error = _kern_read_stat(dirFD, leafName, false, &st, sizeof(struct stat));
			if (error == B_ENTRY_NOT_FOUND && !requireConcrete) {
				// that's fine -- the entry is abstract and was not target of
				// a symlink we resolved
				break;
			}
			if (error != B_OK)
				return error;
			// the entry is concrete
			if (!S_ISLNK(st.st_mode))
				break;
			requireConcrete = true;
			// we need to traverse the symlink
			if (--linkLimit < 0)
				return B_LINK_LIMIT;
			size_t bufferSize = B_PATH_NAME_LENGTH - 1;
			error			  = _kern_read_link(dirFD, leafName, tmpPath, &bufferSize);
			if (error < 0)
				return error;
			tmpPath[bufferSize] = '\0';
			path				= tmpPath;
			// next round...
		}
	}

	// set close on exec flag on dir FD
	fcntl(dirFD, F_SETFD, FD_CLOEXEC);

	// set the result
	status_t error = _SetName(leafName);
	if (error != B_OK)
		return error;
	fdCloser.Detach();
	fDirFd = dirFD;
	return B_OK;
}

status_t BEntry::_SetName(const char* name)
{
	if (name == NULL)
		return B_BAD_VALUE;

	free(fName);

	fName = strdup(name);
	if (fName == NULL)
		return B_NO_MEMORY;

	return B_OK;
}

status_t BEntry::_Rename(BEntry& target, bool clobber)
{
	// check, if there's an entry in the way
	if (!clobber && target.Exists())
		return B_FILE_EXISTS;
	// rename
	status_t error = _kern_rename(fDirFd, fName, target.fDirFd, target.fName);
	if (error == B_OK) {
		Unset();
		fCStatus		= target.fCStatus;
		fDirFd			= target.fDirFd;
		fName			= target.fName;
		target.fCStatus = B_NO_INIT;
		target.fDirFd	= -1;
		target.fName	= nullptr;
	}
	return error;
}

#pragma mark -

status_t get_ref_for_path(const char* path, entry_ref* ref)
{
	status_t error = path && ref ? B_OK : B_BAD_VALUE;
	if (error == B_OK) {
		BEntry entry(path);
		error = entry.InitCheck();
		if (error == B_OK)
			error = entry.GetRef(ref);
	}
	return error;
}
