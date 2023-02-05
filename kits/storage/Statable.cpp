#include "Statable.h"

#include <sys/stat.h>

bool BStatable::IsFile() const
{
	struct stat stat;
	if (GetStat(&stat) == B_OK)
		return S_ISREG(stat.st_mode);

	return false;
}

bool BStatable::IsDirectory() const
{
	struct stat stat;
	if (GetStat(&stat) == B_OK)
		return S_ISDIR(stat.st_mode);

	return false;
}

bool BStatable::IsSymLink() const
{
	struct stat stat;
	if (GetStat(&stat) == B_OK)
		return S_ISLNK(stat.st_mode);

	return false;
}
