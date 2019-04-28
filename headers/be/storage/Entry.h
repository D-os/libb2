/***************************************************************************
//
//	File:			Entry.h
//
//	Description:	BEntry class and entry_ref struct
//
//	Copyright 1992-98, Be Incorporated, All Rights Reserved.
//
***************************************************************************/


#ifndef _ENTRY_H
#define _ENTRY_H

#ifndef _BE_BUILD_H
#include <BeBuild.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <OS.h>
#include <StorageDefs.h>
#include <Statable.h>
#include <Node.h>

class	BDirectory;
class	BPath;
struct	entry_ref;

_IMPEXP_BE status_t	get_ref_for_path(const char *path, entry_ref *ref);
_IMPEXP_BE bool operator<(const entry_ref & a, const entry_ref & b);


struct entry_ref {
						entry_ref();
  						entry_ref(dev_t dev, ino_t dir, const char *name);
						entry_ref(const entry_ref &ref);
						~entry_ref();
	
  	status_t 			set_name(const char *name);

	bool				operator==(const entry_ref &ref) const;
	bool				operator!=(const entry_ref &ref) const;
	entry_ref &			operator=(const entry_ref &ref);

	dev_t				device;
	ino_t				directory;
	char				*name;
};

class BEntry : public BStatable {
public:
						BEntry();

						/* BEntry(dir, NULL) gets the entry for dir. */
						BEntry(const BDirectory *dir, const char *path, 
							   bool traverse = false);

						BEntry(const entry_ref *ref, bool traverse = false);
						BEntry(const char *path, bool traverse = false);
						BEntry(const BEntry &entry);

	virtual				~BEntry();

			status_t 	InitCheck() const;

			bool		Exists() const;

	virtual status_t	GetStat(struct stat *st) const;

			status_t	SetTo(const BDirectory *dir, const char *path,
							  bool traverse = false);
			status_t	SetTo(const entry_ref *ref, bool traverse = false);
			status_t	SetTo(const char *path, bool traverse = false);

			void		Unset();

			status_t	GetRef(entry_ref *ref) const;
			status_t	GetPath(BPath *path) const;

			status_t	GetParent(BEntry *entry) const;
			status_t	GetParent(BDirectory *dir) const;
			status_t	GetName(char *buffer) const;

			status_t	Rename(const char *path, bool clobber = false);
			status_t	MoveTo(BDirectory *dir, const char *path = NULL, bool clobber = false);
			status_t	Remove();

			bool		operator==(const BEntry &item) const;
			bool		operator!=(const BEntry &item) const;
			BEntry &	operator=(const BEntry &item);


private:
friend class BNode;
friend class BDirectory;
friend class BFile;
friend class BSymLink;

		/* FBC */
virtual	void		_PennyEntry1();
virtual	void		_PennyEntry2();
virtual	void		_PennyEntry3();
virtual	void		_PennyEntry4();
virtual	void		_PennyEntry5();
virtual	void		_PennyEntry6();

		uint32		_pennyData[4];

virtual		status_t		set_stat(struct stat &st, uint32 what);
			status_t		move(int fd, const char *path);
			status_t		set(int fd, const char *path, bool traverse);
			status_t		clear();

			int			fDfd;
			char		*fName;
			status_t	fCStatus;
};


#endif
