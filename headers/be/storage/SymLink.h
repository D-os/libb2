/***************************************************************************
//
//	File:			SymLink.h
//
//	Description:	The BSymLink class; a tiny bit of eye shadow
//					on the POSIX readlink() func.
//
//	Copyright 1992-98, Be Incorporated, All Rights Reserved.
//
***************************************************************************/


#ifndef _SYM_LINK_H
#define _SYM_LINK_H

#ifndef _BE_BUILD_H
#include <BeBuild.h>
#endif
#include <sys/types.h>
#include <Node.h>

class	BEntry;
class 	BDirectory;
class	BPath;
struct	entry_ref;

class BSymLink : public BNode {
public:
						BSymLink();
						BSymLink(const entry_ref *ref);
						BSymLink(const BEntry *entry);
						BSymLink(const char *path);
						BSymLink(const BDirectory *dir, const char *path);
						BSymLink(const BSymLink &link);
virtual					~BSymLink();

/* The ReadLink() functions return the contents of THIS symlink --
 * they don't traverse to the end of the "link chain".  Because
 * the linked-to path might be relative, we can't return a BPath here.
 */
		ssize_t			ReadLink(char *path, size_t length);

/* MakeLinkedPath reads the link, appends it to the first arg and returns
 * the whole thing in path.  It's up to the caller to pass the 
 * correct dir--the function does not check to see if the returned
 * path is "correct" (indeed, it CAN'T check since the BSymLink object
 * doesn't know its own location).
 * If the linked-to path is absolute, then the dir is ignored.
 */
		ssize_t			MakeLinkedPath(const char *dir, BPath *path);
		ssize_t			MakeLinkedPath(const BDirectory *dir, BPath *path);

		bool			IsAbsolute();

private:

		/* FBC */
virtual	void		_MissingSymLink1();
virtual	void		_MissingSymLink2();
virtual	void		_MissingSymLink3();
virtual	void		_MissingSymLink4();
virtual	void		_MissingSymLink5();
virtual	void		_MissingSymLink6();
		uint32		_missingData[4];

};

#endif
