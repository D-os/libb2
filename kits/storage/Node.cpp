/*
 * Based on Haiku implementation.
 * Authors:
 *		Tyler Dauwalder
 *		Ingo Weinhold, bonefish@users.sf.net
 */
#include "Node.h"

#include <Directory.h>
#include <Entry.h>
#include <sys/stat.h>
#include <syscalls.h>
#include <unistd.h>

#include <filesystem>

#pragma mark - node_ref

node_ref::node_ref()
	: fd{-1}
{
}

node_ref::node_ref(int fd)
	: fd{fd}
{
}

node_ref::node_ref(const node_ref& other)
	: node_ref()
{
	*this = other;
}

node_ref::~node_ref()
{
	close(fd);
}

bool node_ref::operator==(const node_ref& other) const
{
	struct stat this_stat, other_stat;
	fstat(fd, &this_stat);
	fstat(other.fd, &other_stat);
	return (this_stat.st_dev == other_stat.st_dev
			&& this_stat.st_ino == other_stat.st_ino);
}

bool node_ref::operator!=(const node_ref& other) const
{
	return !(*this == other);
}

node_ref& node_ref::operator=(const node_ref& other)
{
	if (this == &other)
		return *this;

	close(fd);
	fd = dup(other.fd);
	return *this;
}

#pragma mark - BNode

BNode::BNode()
	: fFd{-1},
	  fCStatus{B_NO_INIT}
{
}

BNode::BNode(const entry_ref* ref)
	: BNode()
{
	SetTo(ref);
}

BNode::BNode(const BEntry* entry)
	: BNode()
{
	SetTo(entry);
}

BNode::BNode(const char* path)
	: BNode()
{
	SetTo(path);
}

BNode::BNode(const BDirectory* dir, const char* path)
	: BNode()
{
	SetTo(dir, path);
}

BNode::BNode(const BNode& node)
	: BNode()
{
	*this = node;
}

BNode::~BNode()
{
	Unset();
}

status_t BNode::InitCheck() const
{
	return fCStatus;
}

status_t BNode::SetTo(const entry_ref* ref)
{
	return _SetTo(ref, false);
}

status_t BNode::SetTo(const BEntry* entry)
{
	if (!entry) {
		Unset();
		return (fCStatus = B_BAD_VALUE);
	}

	return _SetTo(entry->fDirFd, entry->fName, false);
}

status_t BNode::SetTo(const char* path)
{
	return _SetTo(-1, path, false);
}

status_t BNode::SetTo(const BDirectory* dir, const char* path)
{
	if (!dir || !path || std::filesystem::path(path).is_absolute()) {
		Unset();
		return (fCStatus = B_BAD_VALUE);
	}

	return _SetTo(dir->fDirFd, path, false);
}

void BNode::Unset()
{
	close_fd();
	fCStatus = B_NO_INIT;
}

status_t BNode::GetStat(struct stat* st) const
{
	if (!st || fFd < 0)
		return B_BAD_VALUE;

	if (fstat(fFd, st) != 0) {
		int e = errno;
		switch (e) {
			case EBADF:
				return B_BAD_VALUE;
			case ENOMEM:
				return B_NO_MEMORY;
		}
		return B_FROM_POSIX_ERROR(errno);
	}
	return B_OK;
}

status_t BNode::GetNodeRef(node_ref* ref) const
{
	if (!ref)
		return B_BAD_VALUE;

	*ref = node_ref(fFd);
	return B_OK;
}

status_t BNode::Lock()
{
	if (fCStatus != B_OK)
		return fCStatus;

	return _kern_lock_node(fFd);
}

status_t BNode::Unlock()
{
	if (fCStatus != B_OK)
		return fCStatus;

	return _kern_unlock_node(fFd);
}

status_t BNode::Sync()
{
	return (fCStatus != B_OK) ? B_FILE_ERROR : _kern_fsync(fFd);
}

BNode& BNode::operator=(const BNode& node)
{
	// No need to do any assignment if already equal
	if (*this == node)
		return *this;

	// Close down out current state
	Unset();
	// We have to manually dup the node, because R5::BNode::Dup()
	// is not declared to be const (which IMO is retarded).
	fFd		 = _kern_dup(node.fFd);
	fCStatus = (fFd < 0) ? B_NO_INIT : B_OK;

	return *this;
}

bool BNode::operator==(const BNode& node) const
{
	if (fCStatus == B_NO_INIT && node.InitCheck() == B_NO_INIT)
		return true;

	if (fCStatus == B_OK && node.InitCheck() == B_OK) {
		// compare the node_refs
		node_ref ref1, ref2;
		if (GetNodeRef(&ref1) != B_OK)
			return false;

		if (node.GetNodeRef(&ref2) != B_OK)
			return false;

		return (ref1 == ref2);
	}

	return false;
}

bool BNode::operator!=(const BNode& node) const
{
	return !(*this == node);
}

int BNode::Dup() const
{
	int fd = _kern_dup(fFd);

	return (fd >= 0 ? fd : -1);
	// comply with R5 return value
}

/*!	Sets the node's file descriptor.

	Used by each implementation (i.e. BNode, BFile, BDirectory, etc.) to set
	the node's file descriptor. This allows each subclass to use the various
	file-type specific system calls for opening file descriptors.

	\note This method calls close_fd() to close previously opened FDs. Thus
		derived classes should take care to first call set_fd() and set
		class specific resources freed in their close_fd() version
		thereafter.

	\param fd the file descriptor this BNode should be set to (may be -1).

	\returns \c B_OK if everything went fine, or an error code if something
		went wrong.
*/
status_t BNode::set_fd(int fd)
{
	if (fFd != -1)
		close_fd();

	fFd = fd;

	return B_OK;
}

/*!	Closes the node's file descriptor(s).

	To be implemented by subclasses to close the file descriptor using the
	proper system call for the given file-type. This implementation calls
	_kern_close(fFd) and also _kern_close(fAttrDir) if necessary.
*/
void BNode::close_fd()
{
	if (fFd >= 0) {
		_kern_close(fFd);
		fFd = -1;
	}
}

/*!	Sets the BNode's status.

	To be used by derived classes instead of accessing the BNode's private
	\c fCStatus member directly.

	\param newStatus the new value for the status variable.
*/
void BNode::set_status(status_t newStatus)
{
	fCStatus = newStatus;
}

/*!	Initializes the BNode's file descriptor to the node referred to
	by the given FD and path combo.

	\a path must either be \c NULL, an absolute or a relative path.
	In the first case, \a fd must not be \c NULL; the node it refers to will
	be opened. If absolute, \a fd is ignored. If relative and \a fd is >= 0,
	it will be reckoned off the directory identified by \a fd, otherwise off
	the current working directory.

	The method will first try to open the node with read and write permission.
	If that fails due to a read-only FS or because the user has no write
	permission for the node, it will re-try opening the node read-only.

	The \a fCStatus member will be set to the return value of this method.

	\param fd Either a directory FD or a value < 0. In the latter case \a path
		   must be specified.
	\param path Either \a NULL in which case \a fd must be given, absolute, or
		   relative to the directory specified by \a fd (if given) or to the
		   current working directory.
	\param traverse If the node identified by \a fd and \a path is a symlink
		   and \a traverse is \c true, the symlink will be resolved recursively.

	\returns \c B_OK if everything went fine, or an error code otherwise.
*/
status_t BNode::_SetTo(int fd, const char* path, bool traverse)
{
	Unset();

	status_t error = (fd >= 0 || path ? B_OK : B_BAD_VALUE);
	if (error == B_OK) {
		int traverseFlag = (traverse ? 0 : O_NOFOLLOW);
		fFd				 = _kern_open(fd, path, O_RDWR | O_CLOEXEC | traverseFlag, 0);
		if (fFd < B_OK && fFd != B_ENTRY_NOT_FOUND) {
			// opening read-write failed, re-try read-only
			fFd = _kern_open(fd, path, O_RDONLY | O_CLOEXEC | traverseFlag, 0);
		}
		if (fFd < 0)
			error = fFd;
	}

	return fCStatus = error;
}

/*!	Initializes the BNode's file descriptor to the node referred to
	by the given entry_ref.

	The method will first try to open the node with read and write permission.
	If that fails due to a read-only FS or because the user has no write
	permission for the node, it will re-try opening the node read-only.

	The \a fCStatus member will be set to the return value of this method.

	\param ref An entry_ref identifying the node to be opened.
	\param traverse If the node identified by \a ref is a symlink and
		   \a traverse is \c true, the symlink will be resolved recursively.

	\returns \c B_OK if everything went fine, or an error code otherwise.
*/
status_t BNode::_SetTo(const entry_ref* ref, bool traverse)
{
	return _SetTo(ref->dirfd, ref->name, traverse);
	Unset();

	status_t result = (ref ? B_OK : B_BAD_VALUE);
	if (result == B_OK) {
		int traverseFlag = (traverse ? 0 : O_NOFOLLOW);
		fFd				 = _kern_open_entry_ref(ref->dirfd, ref->name, O_RDWR | O_CLOEXEC | traverseFlag, 0);
		if (fFd < B_OK && fFd != B_ENTRY_NOT_FOUND) {
			// opening read-write failed, re-try read-only
			fFd = _kern_open_entry_ref(ref->dirfd, ref->name, O_RDONLY | O_CLOEXEC | traverseFlag, 0);
		}
		if (fFd < 0)
			result = fFd;
	}

	return fCStatus = result;
}

/*!	Modifies a certain setting for this node based on \a what and the
	corresponding value in \a st.

	Inherited from and called by BStatable.

	\param st a stat structure containing the value to be set.
	\param what specifies what setting to be modified.

	\returns \c B_OK if everything went fine, or an error code otherwise.
*/
status_t BNode::set_stat(struct stat& stat, uint32 what)
{
	if (fCStatus != B_OK)
		return B_FILE_ERROR;

	return _kern_write_stat(fFd, nullptr, false, &stat, sizeof(struct stat), what);
}
