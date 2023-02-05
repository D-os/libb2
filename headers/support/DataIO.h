#ifndef _DATA_IO_H
#define _DATA_IO_H

#include <SupportDefs.h>

class BDataIO
{
   public:
	BDataIO();
	virtual ~BDataIO();

	virtual ssize_t Read(void *buffer, size_t size)		   = 0;
	virtual ssize_t Write(const void *buffer, size_t size) = 0;

   private:
	BDataIO(const BDataIO &)			= delete;
	BDataIO &operator=(const BDataIO &) = delete;
};

class BPositionIO : public BDataIO
{
   public:
	BPositionIO();
	virtual ~BPositionIO();

	virtual ssize_t Read(void *buffer, size_t size);
	virtual ssize_t Write(const void *buffer, size_t size);

	virtual ssize_t ReadAt(off_t pos, void *buffer, size_t size)		= 0;
	virtual ssize_t WriteAt(off_t pos, const void *buffer, size_t size) = 0;

	virtual off_t Seek(off_t position, uint32 seek_mode) = 0;
	virtual off_t Position() const						 = 0;

	virtual status_t SetSize(off_t size);
};

class BMallocIO : public BPositionIO
{
   public:
	BMallocIO();
	virtual ~BMallocIO();

	virtual ssize_t ReadAt(off_t pos, void *buffer, size_t size);
	virtual ssize_t WriteAt(off_t pos, const void *buffer, size_t size);

	virtual off_t	 Seek(off_t pos, uint32 seek_mode);
	virtual off_t	 Position() const;
	virtual status_t SetSize(off_t size);

	void SetBlockSize(size_t blocksize);

	const void *Buffer() const;
	size_t		BufferLength() const;

   private:
	BMallocIO(const BMallocIO &)			= delete;
	BMallocIO &operator=(const BMallocIO &) = delete;

	size_t fBlockSize;
	size_t fMallocSize;
	size_t fLength;
	char  *fData;
	off_t  fPosition;
};

class BMemoryIO : public BPositionIO
{
   public:
	BMemoryIO(void *p, size_t len);
	BMemoryIO(const void *p, size_t len);
	virtual ~BMemoryIO();

	virtual ssize_t ReadAt(off_t pos, void *buffer, size_t size);
	virtual ssize_t WriteAt(off_t pos, const void *buffer, size_t size);

	virtual off_t Seek(off_t pos, uint32 seek_mode);
	virtual off_t Position() const;

	virtual status_t SetSize(off_t size);

   private:
	BMemoryIO(const BMemoryIO &)			= delete;
	BMemoryIO &operator=(const BMemoryIO &) = delete;

	bool   fReadOnly;
	char  *fBuffer;
	size_t fLength;
	size_t fBufferSize;
	size_t fPosition;
};

#endif /* _DATA_IO_H */
