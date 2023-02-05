#include "Path.h"

#include <Debug.h>
#include <Directory.h>
#include <Entry.h>
#include <StorageDefs.h>
#include <syscalls.h>

#include <filesystem>

#include "Errors.h"

BPath::BPath()
	: fName{nullptr}, fCStatus{B_NO_INIT} {}

BPath::BPath(const char *dir, const char *leaf, bool normalize)
	: BPath()
{
	SetTo(dir, leaf, normalize);
}

BPath::BPath(const BDirectory *dir, const char *leaf, bool normalize)
	: BPath()
{
	SetTo(dir, leaf, normalize);
}

BPath::BPath(const BPath &path)
	: BPath()
{
	*this = path;
}

BPath::BPath(const BEntry *entry)
	: BPath()
{
	SetTo(entry);
}

BPath::BPath(const entry_ref *ref)
	: BPath()
{
	SetTo(ref);
}

BPath::~BPath()
{
	Unset();
}

status_t BPath::InitCheck() const
{
	return fCStatus;
}

status_t BPath::SetTo(const char *path, const char *leaf, bool normalize)
{
	status_t error = (path ? B_OK : B_BAD_VALUE);
	if (error == B_OK && leaf && std::filesystem::path(leaf).is_absolute())
		error = B_BAD_VALUE;

	char newPath[B_PATH_NAME_LENGTH];
	if (error == B_OK) {
		// we always normalize relative paths
		normalize |= !std::filesystem::path(path).is_absolute();

		// build a new path from path and leaf

		// copy path first
		uint32 pathLen = strlen(path);
		if (pathLen >= sizeof(newPath))
			error = B_NAME_TOO_LONG;

		if (error == B_OK)
			strcpy(newPath, path);

		// append leaf, if supplied
		if (error == B_OK && leaf) {
			bool   needsSeparator = (pathLen > 0 && path[pathLen - 1] != '/');
			uint32 wholeLen		  = pathLen + (needsSeparator ? 1 : 0) + strlen(leaf);
			if (wholeLen >= sizeof(newPath))
				error = B_NAME_TOO_LONG;

			if (error == B_OK) {
				if (needsSeparator) {
					newPath[pathLen] = '/';
					pathLen++;
				}
				strcpy(newPath + pathLen, leaf);
			}
		}
		// check, if necessary to normalize
		if (error == B_OK && !normalize)
			normalize = normalize || _MustNormalize(newPath, &error);

		// normalize the path, if necessary, otherwise just set it
		if (error == B_OK) {
			if (normalize) {
				// create a BEntry and initialize us with this entry
				BEntry entry;
				error = entry.SetTo(newPath, false);
				if (error == B_OK)
					return SetTo(&entry);
			}
			else
				error = _SetPath(newPath);
		}
	}

	// cleanup, if something went wrong
	if (error != B_OK)
		Unset();

	fCStatus = error;
	return error;
}

status_t BPath::SetTo(const BDirectory *dir, const char *path, bool normalize)
{
	status_t error = (dir && dir->InitCheck() == B_OK ? B_OK : B_BAD_VALUE);

	// get the path of the BDirectory
	BEntry entry;
	if (error == B_OK)
		error = dir->GetEntry(&entry);

	BPath dirPath;
	if (error == B_OK)
		error = dirPath.SetTo(&entry);

	// let the other version do the work
	if (error == B_OK)
		error = SetTo(dirPath.Path(), path, normalize);
	if (error != B_OK)
		Unset();

	fCStatus = error;
	return error;
}

status_t BPath::SetTo(const BEntry *entry)
{
	Unset();

	if (!entry)
		return B_BAD_VALUE;

	entry_ref ref;
	fCStatus = entry->GetRef(&ref);
	if (fCStatus == B_OK)
		fCStatus = SetTo(&ref);

	return fCStatus;
}

status_t BPath::SetTo(const entry_ref *ref)
{
	Unset();

	if (!ref)
		return fCStatus = B_BAD_VALUE;

	char path[B_PATH_NAME_LENGTH];
	fCStatus = _kern_entry_ref_to_path(ref->dirfd, ref->name, path, sizeof(path));
	if (fCStatus != B_OK)
		return fCStatus;

	fCStatus = _SetPath(path);	// the path is already normalized

	return fCStatus;
}

status_t BPath::Append(const char *path, bool normalize)
{
	status_t error = (InitCheck() == B_OK ? B_OK : B_BAD_VALUE);
	if (error == B_OK)
		error = SetTo(Path(), path, normalize);

	if (error != B_OK)
		Unset();

	fCStatus = error;
	return error;
}

void BPath::Unset()
{
	_SetPath(nullptr);
	fCStatus = B_NO_INIT;
}

const char *BPath::Path() const
{
	return fName;
}

const char *BPath::Leaf() const
{
	if (InitCheck() != B_OK)
		return nullptr;

	const char *result = fName + strlen(fName);

	// There should be no need for the second condition, since we deal
	// with absolute paths only and those contain at least one '/'.
	// However, it doesn't harm.
	while (*result != '/' && result > fName)
		result--;
	result++;

	return result;
}

status_t BPath::GetParent(BPath *path) const
{
	if (!path)
		return B_BAD_VALUE;

	status_t error = InitCheck();
	if (error != B_OK)
		return error;

	int32 length = strlen(fName);
	if (length == 1) {
		// handle "/" (path is supposed to be absolute)
		return B_ENTRY_NOT_FOUND;
	}

	char parentPath[B_PATH_NAME_LENGTH];
	length--;
	while (fName[length] != '/' && length > 0)
		length--;
	if (length == 0) {
		// parent dir is "/"
		length++;
	}
	memcpy(parentPath, fName, length);
	parentPath[length] = '\0';

	return path->SetTo(parentPath);
}

bool BPath::operator==(const BPath &item) const
{
	return *this == item.Path();
}

bool BPath::operator==(const char *path) const
{
	return (InitCheck() != B_OK && !path)
		   || (fName && path && strcmp(fName, path) == 0);
}

bool BPath::operator!=(const BPath &item) const
{
	return !(*this == item);
}

bool BPath::operator!=(const char *path) const
{
	return !(*this == path);
}

BPath &BPath::operator=(const BPath &item)
{
	if (this != &item)
		*this = item.Path();

	return *this;
}

BPath &BPath::operator=(const char *path)
{
	if (!path)
		Unset();
	else
		SetTo(path);

	return *this;
}

status_t BPath::_SetPath(const char *path)
{
	status_t	error	= B_OK;
	const char *oldPath = fName;

	// set the new path
	if (path) {
		fName = new (std::nothrow) char[strlen(path) + 1];
		if (fName)
			strcpy(fName, path);
		else
			error = B_NO_MEMORY;
	}
	else
		fName = nullptr;

	// delete the old one
	delete[] oldPath;
	return error;
}

/*!     Checks a path to see if normalization is required.

		The following items require normalization:
		- Relative pathnames (after concatenation; e.g. "boot/ltj")
		- The presence of "." or ".." ("/boot/ltj/../ltj/./gwar")
		- Redundant slashes ("/boot//ltj")
		- A trailing slash ("/boot/ltj/")

		\param _error A pointer to an error variable that will be set if the input
				is not a valid path.

		\return \c true if \a path requires normalization, \c false otherwise.
*/
bool BPath::_MustNormalize(const char *path, status_t *_error)
{
	// Check for useless input
	if (!path || path[0] == '\0') {
		if (_error)
			*_error = B_BAD_VALUE;
		return false;
	}

	int len = strlen(path);

	/* Look for anything in the string that forces us to normalize:
		+ No leading /
		+ any occurence of /./ or /../ or //, or a trailing /. or /..
		+ a trailing /
	*/
	;
	if (path[0] != '/')
		return true;  //      not "/*"
	else if (len == 1)
		return false;  //     "/"
	else if (len > 1 && path[len - 1] == '/')
		return true;  //      "*/"
	else {
		enum ParseState {
			NoMatch,
			InitialSlash,
			OneDot,
			TwoDots
		} state
			= NoMatch;

		for (int i = 0; path[i] != 0; i++) {
			switch (state) {
				case NoMatch:
					if (path[i] == '/')
						state = InitialSlash;
					break;

				case InitialSlash:
					if (path[i] == '/')
						return true;  // "*//*"

					if (path[i] == '.')
						state = OneDot;
					else
						state = NoMatch;
					break;

				case OneDot:
					if (path[i] == '/')
						return true;  // "*/./*"

					if (path[i] == '.')
						state = TwoDots;
					else
						state = NoMatch;
					break;

				case TwoDots:
					if (path[i] == '/')
						return true;  // "*/../*"

					state = NoMatch;
					break;
			}
		}

		// If we hit the end of the string while in either
		// of these two states, there was a trailing /. or /..
		if (state == OneDot || state == TwoDots)
			return true;

		return false;
	}
}

#pragma mark - BFlattenable functionality

bool BPath::IsFixedSize() const
{
	debugger(__PRETTY_FUNCTION__);
	return false;
}

type_code BPath::TypeCode() const
{
	debugger(__PRETTY_FUNCTION__);
	return 0;
}

ssize_t BPath::FlattenedSize() const
{
	debugger(__PRETTY_FUNCTION__);
	return -1;
}

status_t BPath::Flatten(void *buffer, ssize_t size) const
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

bool BPath::AllowsTypeCode(type_code code) const
{
	debugger(__PRETTY_FUNCTION__);
	return false;
}

status_t BPath::Unflatten(type_code c, const void *buf, ssize_t size)
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}
