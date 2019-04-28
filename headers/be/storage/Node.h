/***************************************************************************
//
//	File:			Node.h
//
//	Description:	node_ref struct and BNode class descriptions
//
//	Copyright 1992-98, Be Incorporated, All Rights Reserved.
//
***************************************************************************/

#ifndef _NODE_H
#define _NODE_H

#ifndef _BE_BUILD_H
#include <BeBuild.h>
#endif
#include <SupportDefs.h>
#include <StorageDefs.h>
#include <Statable.h>

struct node_ref {
						node_ref();
						node_ref(const node_ref &ref);

	bool				operator==(const node_ref &ref) const;
	bool				operator!=(const node_ref &ref) const;
	node_ref &			operator=(const node_ref &ref);

	dev_t				device;
	ino_t				node;
};


class BStatable;
class BEntry;
class BDirectory;
struct entry_ref;

class BNode : public BStatable {

public:
							BNode();
							BNode(const entry_ref *ref);
							BNode(const BEntry *entry);
							BNode(const char *path);
							BNode(const BDirectory *dir, const char *path);
							BNode(const BNode &node);

	virtual					~BNode();

			status_t		InitCheck() const;

	virtual status_t		GetStat(struct stat *st) const;

			status_t		SetTo(const entry_ref *ref);
			status_t		SetTo(const BEntry *entry);
			status_t		SetTo(const char *path);
			status_t		SetTo(const BDirectory *dir, const char *path);
			void			Unset();

			status_t		Lock();
			status_t		Unlock();

			status_t		Sync();

			ssize_t			WriteAttr(const char *attr, type_code type,
									off_t off, const void *buf, size_t l);
			ssize_t			ReadAttr(const char *attr, type_code type,
									off_t off, void *buf, size_t l) const;

			status_t		RemoveAttr(const char *attr);
			status_t		RenameAttr(const char *oldname, const char *newname);
			status_t		GetAttrInfo(const char *attr,
									struct attr_info *buf) const;
			status_t		GetNextAttrName(char *buf) ;
			status_t		RewindAttrs() ;

			status_t		WriteAttrString(const char *attr, const BString *);
			status_t		ReadAttrString(const char *attr, BString *result) const;

			BNode &			operator=(const BNode &node);
			bool			operator==(const BNode &node) const;
			bool			operator!=(const BNode &node) const;

			int Dup();		/* don't forget to close() fd later */

private:
friend class BEntry;
friend class BVolume;
friend class BFile;
friend class BDirectory;
friend class BSymLink;

		/* FBC */
virtual	void		_RudeNode1();
virtual	void		_RudeNode2();
virtual	void		_RudeNode3();
virtual	void		_RudeNode4();
virtual	void		_RudeNode5();
virtual	void		_RudeNode6();

		uint32		_rudeData[4];

			status_t		set_fd(int fd);
	virtual	void			close_fd();
			status_t		clear_virtual();
			status_t		clear();

	virtual	status_t		set_stat(struct stat &st, uint32 what);

			status_t		set_to(const entry_ref *ref, bool traverse = false);
			status_t		set_to(const BEntry *entry, bool traverse = false);
			status_t		set_to(const char *path, bool traverse = false);
			status_t		set_to(const BDirectory *dir, const char *path, bool traverse = false);

			int				fFd;
			int				fAttrFd;
			status_t		fCStatus;
};


#endif
