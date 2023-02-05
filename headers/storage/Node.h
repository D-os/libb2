#ifndef _NODE_H
#define _NODE_H

#include <Statable.h>
#include <SupportDefs.h>

struct node_ref
{
	node_ref();
	node_ref(int fd);
	node_ref(const node_ref &other);
	~node_ref();

	bool	  operator==(const node_ref &other) const;
	bool	  operator!=(const node_ref &other) const;
	node_ref &operator=(const node_ref &other);

	int fd;
};

class BString;
class BEntry;
class BDirectory;
struct entry_ref;

class BNode : public BStatable
{
   public:
	BNode();
	BNode(const entry_ref *ref);
	BNode(const BEntry *entry);
	BNode(const char *path);
	BNode(const BDirectory *dir, const char *path);
	BNode(const BNode &node);

	virtual ~BNode();

	status_t InitCheck() const;

	virtual status_t GetStat(struct stat *st) const;
	virtual status_t GetNodeRef(node_ref *ref) const;

	status_t SetTo(const entry_ref *ref);
	status_t SetTo(const BEntry *entry);
	status_t SetTo(const char *path);
	status_t SetTo(const BDirectory *dir, const char *path);
	void	 Unset();

	status_t Lock();
	status_t Unlock();

	status_t Sync();

	ssize_t WriteAttr(const char *attr, type_code type,
					  off_t off, const void *buf, size_t l);
	ssize_t ReadAttr(const char *attr, type_code type,
					 off_t off, void *buf, size_t l) const;

	status_t RemoveAttr(const char *attr);
	status_t RenameAttr(const char *oldname, const char *newname);
	status_t GetAttrInfo(const char		  *attr,
						 struct attr_info *buf) const;
	status_t GetNextAttrName(char *buf);
	status_t RewindAttrs();

	status_t WriteAttrString(const char *attr, const BString *);
	status_t ReadAttrString(const char *attr, BString *result) const;

	BNode &operator=(const BNode &node);
	bool   operator==(const BNode &node) const;
	bool   operator!=(const BNode &node) const;

	int Dup() const; /* don't forget to close() fd later */

   private:
	// friend class BEntry;
	// friend class BVolume;
	// friend class BFile;
	friend class BDirectory;
	// friend class BSymLink;

	status_t	 set_fd(int fd);
	virtual void close_fd();
	void		 set_status(status_t newStatus);
	// status_t	 clear_virtual();
	// status_t	 clear();

	virtual status_t set_stat(struct stat &st, uint32 what);

	// status_t set_to(const entry_ref *ref, bool traverse = false);
	// status_t set_to(const BEntry *entry, bool traverse = false);
	// status_t set_to(const char *path, bool traverse = false);
	// status_t set_to(const BDirectory *dir, const char *path, bool traverse = false);
	status_t _SetTo(int fd, const char *path, bool traverse);
	status_t _SetTo(const entry_ref *ref, bool traverse);

	int fFd;
	// int		 fAttrFd;
	status_t fCStatus;
};

#endif
