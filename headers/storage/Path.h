#ifndef _PATH_H
#define _PATH_H

#include <Flattenable.h>
#include <SupportDefs.h>

class BDirectory;
class BEntry;
struct entry_ref;

class BPath : public BFlattenable
{
   public:
	BPath();
	BPath(const char *dir, const char *leaf = nullptr, bool normalize = false);
	BPath(const BDirectory *dir, const char *leaf, bool normalize = false);
	BPath(const BPath &path);
	BPath(const BEntry *entry);
	BPath(const entry_ref *ref);

	virtual ~BPath();

	status_t InitCheck() const;

	status_t SetTo(const char *path, const char *leaf = nullptr, bool normalize = false);
	status_t SetTo(const BDirectory *dir, const char *path, bool normalize = false);
	status_t SetTo(const BEntry *entry);
	status_t SetTo(const entry_ref *ref);
	status_t Append(const char *path, bool normalize = false);
	void	 Unset();

	const char *Path() const;
	const char *Leaf() const;

	status_t GetParent(BPath *) const;

	bool   operator==(const BPath &item) const;
	bool   operator==(const char *path) const;
	bool   operator!=(const BPath &item) const;
	bool   operator!=(const char *path) const;
	BPath &operator=(const BPath &item);
	BPath &operator=(const char *path);

	virtual bool	  IsFixedSize() const;
	virtual type_code TypeCode() const;
	virtual ssize_t	  FlattenedSize() const;
	virtual status_t  Flatten(void *buffer, ssize_t size) const;
	virtual bool	  AllowsTypeCode(type_code code) const;
	virtual status_t  Unflatten(type_code c, const void *buf, ssize_t size);

   private:
	status_t _SetPath(const char *path);
	bool	 _MustNormalize(const char *path, status_t *_error);

	char	*fName;
	status_t fCStatus;
};
#endif
