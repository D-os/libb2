/***************************************************************************
//
//	File:			Path.h
//
//	Description:	Lightweight class that stores (and allocates
//					storage for) a string pathname.
//					The pathname is ALWAYS absolute; the 
//					file that's identified by the pathname needn't
//					actually exist:  You can create an "abstract"
//					pathname that you use to search for or create
//					a file (as two examples).
//
//	Copyright 1992-98, Be Incorporated, All Rights Reserved.
//
***************************************************************************/


#ifndef _PATH_H
#define _PATH_H

#ifndef _BE_BUILD_H
#include <BeBuild.h>
#endif
#include <sys/types.h>
#include <SupportDefs.h>
#include <StorageDefs.h>
#include <Message.h>

class BDirectory;
class BEntry;
struct entry_ref;

class BPath : public BFlattenable {
public:
						BPath();
						BPath(const char *dir, const char *leaf = NULL,
							bool normalize = false);
						BPath(const BDirectory *dir, const char *leaf,
							bool normalize = false);
						BPath(const BPath &path);
						BPath(const BEntry *entry);
						BPath(const entry_ref *ref);

virtual					~BPath();

			status_t	InitCheck() const;

			status_t	SetTo(const char *path, const char *leaf = NULL,
							bool normalize = false);
			status_t	SetTo(const BDirectory *dir, const char *path,
							bool normalize = false);
			status_t	SetTo(const BEntry *entry);
			status_t	SetTo(const entry_ref *ref);
			status_t	Append(const char *path, bool normalize = false);
			void		Unset();

			const char *Path() const;
			const char *Leaf() const;

			status_t	GetParent(BPath *) const;
			
			bool		operator==(const BPath &item) const;
			bool		operator==(const char *path) const;
			bool		operator!=(const BPath &item) const;
			bool		operator!=(const char *path) const;
			BPath &		operator=(const BPath &item);
			BPath &		operator=(const char *path);

virtual		bool		IsFixedSize() const;
virtual		type_code	TypeCode() const;
virtual		ssize_t		FlattenedSize() const;
virtual		status_t	Flatten(void *buffer, ssize_t size) const;
virtual		bool		AllowsTypeCode(type_code code) const;
virtual		status_t	Unflatten(type_code c, const void *buf, ssize_t size);

private:

		/* FBC */
virtual	void		_WarPath1();
virtual	void		_WarPath2();
virtual	void		_WarPath3();
		uint32		_warData[4];

			status_t	clear();
			char		*fName;
			status_t	fCStatus;
};
#endif
