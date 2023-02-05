#ifndef _ENTRY_H
#define _ENTRY_H

#include <Statable.h>
#include <SupportDefs.h>

class BDirectory;
class BPath;
struct entry_ref;

status_t get_ref_for_path(const char *path, entry_ref *ref);
bool	 operator<(const entry_ref &a, const entry_ref &b);

struct entry_ref
{
	entry_ref();
	entry_ref(int dirfd, const char *name);
	entry_ref(const entry_ref &ref);
	~entry_ref();

	status_t set_name(const char *name);

	bool	   operator==(const entry_ref &ref) const;
	bool	   operator!=(const entry_ref &ref) const;
	entry_ref &operator=(const entry_ref &ref);

	int	  dirfd;
	char *name;
};

class BEntry : public BStatable
{
   public:
	BEntry();

	/// BEntry(dir, nullptr) gets the entry for dir.
	BEntry(const BDirectory *dir, const char *path, bool traverse = false);

	BEntry(const entry_ref *ref, bool traverse = false);
	BEntry(const char *path, bool traverse = false);
	BEntry(const BEntry &entry);

	virtual ~BEntry();

	status_t InitCheck() const;

	bool Exists() const;

	const char *Name() const;

	virtual status_t GetStat(struct stat *st) const;
	virtual status_t GetNodeRef(node_ref *ref) const;

	status_t SetTo(const BDirectory *dir, const char *path, bool traverse = false);
	status_t SetTo(const entry_ref *ref, bool traverse = false);
	status_t SetTo(const char *path, bool traverse = false);

	void Unset();

	status_t GetRef(entry_ref *ref) const;
	status_t GetPath(BPath *path) const;

	status_t GetParent(BEntry *entry) const;
	status_t GetParent(BDirectory *dir) const;
	status_t GetName(char *buffer) const;

	status_t Rename(const char *path, bool clobber = false);
	status_t MoveTo(BDirectory *dir, const char *path = nullptr, bool clobber = false);
	status_t Remove();

	bool	operator==(const BEntry &item) const;
	bool	operator!=(const BEntry &item) const;
	BEntry &operator=(const BEntry &item);

   private:
	friend class BNode;
	// friend class BDirectory;
	// friend class BFile;
	// friend class BSymLink;

	// virtual status_t set_stat(struct stat &st, uint32 what);
	// status_t		 move(int fd, const char *path);
	// status_t		 set(int fd, const char *path, bool traverse);
	// status_t		 clear();
	status_t _SetTo(int dir, const char *path, bool traverse);
	status_t _SetName(const char *name);
	status_t _Rename(BEntry &target, bool clobber);

	int		 fDirFd;
	char	*fName;
	status_t fCStatus;
};

#endif
