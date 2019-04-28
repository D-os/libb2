/*******************************************************************************
/
/	File:		fs_attr.h
/
/	Description:	Interface to extended file attributes.
/
/	Copyright 1993-98, Be Incorporated, All Rights Reserved.
/
*******************************************************************************/


#ifndef _FS_ATTR_H
#define _FS_ATTR_H

#include <BeBuild.h>
#include <SupportDefs.h>
#include <dirent.h>


typedef struct attr_info
{
	uint32     type;
	off_t      size;
} attr_info;



#ifdef  __cplusplus
extern "C" {
#endif

_IMPEXP_ROOT ssize_t	fs_read_attr(int fd, const char *attribute,
						uint32 type, off_t pos, void *buf, size_t count);
_IMPEXP_ROOT ssize_t fs_write_attr(int fd, const char *attribute,
						uint32 type, off_t pos, const void *buf, size_t count);
_IMPEXP_ROOT int		fs_remove_attr(int fd, const char *attr);



_IMPEXP_ROOT DIR *	fs_open_attr_dir(const char *path);
_IMPEXP_ROOT DIR *	fs_fopen_attr_dir(int fd);
_IMPEXP_ROOT int		fs_close_attr_dir(DIR *dirp);
_IMPEXP_ROOT struct dirent *fs_read_attr_dir(DIR *dirp);
_IMPEXP_ROOT void	fs_rewind_attr_dir(DIR *dirp);

_IMPEXP_ROOT int		fs_stat_attr(int fd, const char *name,
								struct attr_info *ai);


#ifdef  __cplusplus
}
#endif


#endif /* _FS_ATTR_H */
