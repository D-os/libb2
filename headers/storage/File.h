#ifndef _FILE_H
#define _FILE_H

#include <DataIO.h>
#include <Node.h>
#include <SupportDefs.h>

class BEntry;
class BDirectory;
struct entry_ref;

class BFile : public BNode, public BPositionIO
{
   public:
	BFile();
	BFile(const entry_ref *ref, uint32 open_mode);
	BFile(const BEntry *entry, uint32 open_mode);
	BFile(const char *path, uint32 open_mode);
	BFile(const BDirectory *dir, const char *path, uint32 open_mode);
	BFile(const BFile &file);

	virtual ~BFile();

	status_t SetTo(const entry_ref *ref, uint32 open_mode);
	status_t SetTo(const BEntry *entry, uint32 open_mode);
	status_t SetTo(const char *path, uint32 open_mode);
	status_t SetTo(const BDirectory *dir, const char *path, uint32 open_mode);

	bool IsReadable() const;
	bool IsWritable() const;

	virtual ssize_t Read(void *buffer, size_t size);
	virtual ssize_t ReadAt(off_t pos, void *buffer, size_t size);
	virtual ssize_t Write(const void *buffer, size_t size);
	virtual ssize_t WriteAt(off_t pos, const void *buffer, size_t size);

	virtual off_t	 Seek(off_t position, uint32 seek_mode);
	virtual off_t	 Position() const;
	virtual status_t SetSize(off_t size);

	BFile &operator=(const BFile &file);

   private:
	// virtual void close_fd();
	uint32 fMode;
};

#endif
