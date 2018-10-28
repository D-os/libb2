/*
 * Copyright (c) 2005 Palmsource, Inc.
 * 
 * This software is licensed as described in the file LICENSE, which
 * you should have received as part of this distribution. The terms
 * are also available at http://www.openbinder.org/license.html.
 * 
 * This software consists of voluntary contributions made by many
 * individuals. For the exact contribution history, see the revision
 * history and logs, available at http://www.openbinder.org
 */

#ifndef _SUPPORT2_HEAP_H_
#define _SUPPORT2_HEAP_H_

#include <support/Atom.h>
#include <support/Locker.h>
#include <support/SupportDefs.h>
#include <support/Vector.h>
#include <support/Memory.h>

#if _SUPPORTS_NAMESPACE
using palmos::support::sptr;
using palmos::support::BMemoryHeap;
using palmos::support::SAtom;
using palmos::support::SLocker;
using palmos::support::SReadWriteLocker;
using palmos::support::SVector;
#endif

// ---------------------------------------------------------

#define SIMUL_AREA_LOCKS	6
#define NONZERO_NULL		0xFFFFFFFF
#define PAGE_SIZE			m_pageSize

struct ThreadInfo {
	uint8_t		counts[SIMUL_AREA_LOCKS];
	uint8_t		_reserved[2];
	area_id		ids[SIMUL_AREA_LOCKS];
	ThreadInfo *next;
};

struct Chunk {
	uintptr_t start;
	size_t size;
	Chunk *next;
};

// ---------------------------------------------------------

#if 1 

typedef SReadWriteLocker RWLock;

#else

/*!	RWLock
	This class implements a one writer/many readers lock which is
	very fast for readers (a single atomic_add() when contested by 
	nobody or only other readers) and somewhat slower for writers
	(two atomic_adds when uncontested by other writers).  Starvation
	should never occur for either readers or writers.
*/

#define MAX_READERS		100000

class RWLock {
	public:
					RWLock(const char *name = "some RWLock");
					~RWLock();
	
		void		WriteLock();
		void		WriteUnlock();
		void		ReadLock();
		void		ReadUnlock();
		void		DowngradeWriteToRead();

	private:
	
		int32_t		m_count;
		sem_id		m_readerSem;
		sem_id		m_writerSem;
		SLocker		m_writeLock;
};

inline void RWLock::WriteLock()
{
	m_writeLock.Lock();
	int32_t readers = atomic_fetch_add(&m_count,-MAX_READERS);
	if (readers > 0) 
	   SysSemaphoreWaitCount(m_writerSem, B_WAIT_FOREVER, 0, readers);
};

inline void RWLock::WriteUnlock()
{
	int32_t readersWaiting = atomic_fetch_add(&m_count,MAX_READERS) + MAX_READERS;
	if (readersWaiting) SysSemaphoreSignalCount(m_readerSem, readersWaiting);
	m_writeLock.Unlock();
};

inline void RWLock::ReadLock()
{
	if (atomic_fetch_add(&m_count,1) < 0)
	   SysSemaphoreWait(m_readerSem, B_WAIT_FOREVER, 0);
};

inline void RWLock::ReadUnlock()
{
	if (atomic_fetch_add(&m_count,-1) < 0)
		SysSemaphoreSignalCount(m_writerSem, 1);
};

inline void RWLock::DowngradeWriteToRead()
{
	int32_t readersWaiting = atomic_fetch_add(&m_count,MAX_READERS-1) + MAX_READERS;
	if (readersWaiting) SysSemaphoreSignalCount(m_readerSem, readersWaiting);
	m_writeLock.Unlock();
}

#endif

// ---------------------------------------------------------

class HeapArea;

template <class t>
class Stash {

	public:

							Stash() { m_theStash = NULL; }
							~Stash()
							{
								for (int32_t n = m_freedom.CountItems()-1;n>=0;n--)
									free(m_freedom[n]);
							}
							
		t * 				Alloc()
		{
			t *n = m_theStash;
			if (n) {
				m_theStash = n->next;
				return n;
			};
			
			int32_t count = 1 << ((m_freedom.CountItems()+2)<<1);
			n = (t*)malloc(sizeof(t) * count);
			m_freedom.AddItem(n);
			for (;count;count--) {
				n->next = m_theStash;
				m_theStash = n;
				n++;
			};
		
			n = m_theStash;
			m_theStash = n->next;
			return n;
		}

		void				Free(t *n)
		{
			n->next = m_theStash;
			m_theStash = n;
		}

	private:
		t *					m_theStash;
		SVector<t*>			m_freedom;
};

class Chunker {
	public:
							Chunker(Stash<Chunk> *stash, uint32_t size=0, int32_t chunkQuantum=1);
							~Chunker();
		uintptr_t			Alloc(size_t &size, bool takeBest=false, uint32_t atLeast=1);
		bool				Alloc(uint32_t start, uint32_t size);
		Chunk const * const	Free(uintptr_t start, size_t size);
		
		uintptr_t			PreAbuttance(uintptr_t addr);
		uintptr_t			PostAbuttance(uintptr_t addr);
		size_t				TotalChunkSpace();
		size_t				LargestChunk();
		void				CheckSanity();
		void				SpewChunks();

		void				Reset(size_t size);

	private:
		friend				class HeapArea;

		Chunk *				m_freeList;
		Stash<Chunk> *		m_stash;
		uintptr_t			m_quantum;
		uintptr_t			m_quantumMask;
};

struct HashEntry {
	void *car;
	uint32_t key;
	HashEntry *next;
};

class HashTable {
	public:
							HashTable(int32_t buckets=64);
							~HashTable();
							
		void				Insert(uint32_t key, void *ptr);
		void *				Retrieve(uint32_t key);
		void				Remove(uint32_t key);
	
	private:

		uintptr_t			Hash(uintptr_t address);

		int32_t				m_buckets;
		HashEntry **		m_hashTable;
		Stash<HashEntry>	m_stash;
};

class Hasher {
	public:
							Hasher(Stash<Chunk> *stash);
							~Hasher();
							
		void				Insert(uint32_t address, uint32_t size);
		void				Retrieve(uintptr_t key, size_t **ptrToSize=NULL);
		uint32_t			Remove(uint32_t key);
		void				RemoveAll();
	
	private:

		uint32_t				Hash(uint32_t address);

		Chunk **			m_hashTable;
		Stash<Chunk> *		m_stash;
};

// ---------------------------------------------------------

class Area : virtual public SAtom {

	public:

								Area(const char *name, uint32_t size, bool mapped = true);
		virtual					~Area();

		inline uintptr_t		Ptr2Offset(void *ptr) const
								{ return (((uintptr_t)ptr) - ((uintptr_t)m_basePtr)); }
		inline void *			Offset2Ptr(uintptr_t offset) const
								{ return (void*)(((uintptr_t)m_basePtr) + offset); }
		inline	void *			BasePtr() const { return m_basePtr; }
		inline	area_id			ID() const { return m_area_public_id; }
		inline	size_t			Size()  const { return m_size * PAGE_SIZE; }
		size_t					ResizeTo(size_t newSize, bool acceptBest=false);
		status_t				Map(uintptr_t ptr, size_t size);
		status_t				Unmap(uintptr_t ptr, size_t size);

				void			BasePtrLock();
				void			BasePtrUnlock();
		
		inline	sptr<BMemoryHeap>	MemoryHeap() { return m_memoryHeap; }

	private:
				area_id				m_area;
				area_id				m_area_public_id;
				void *				m_basePtr;
				size_t				m_size;
				RWLock				m_basePtrLock;
				sptr<BMemoryHeap>	m_memoryHeap;
		static	SLocker				m_areaAllocLock;
	protected:
				size_t				m_pageSize;
};

class Heap : virtual public SAtom {
	public:


							Heap() {}
		virtual				~Heap() {}

		virtual	uintptr_t	Alloc(size_t size);
		virtual	uintptr_t	Realloc(uintptr_t ptr, size_t newSize);
		virtual	void		Free(uintptr_t ptr);
		inline	void		Free(void *ptr) { Free(Offset(ptr)); }

		virtual	void		Lock();
		virtual	void		Unlock();

		virtual uintptr_t	Offset(void *ptr);
		virtual void *		Ptr(uintptr_t offset);
};

class HeapArea : public Area, public Heap {

	public:


							HeapArea(const char *name, size_t size, uint32_t quantum=32, bool mapped = true);
		virtual				~HeapArea();

				void		Reset();
		virtual	uintptr_t	Alloc(size_t size);
		virtual	uintptr_t	Realloc(uintptr_t ptr, size_t newSize);
		virtual	void		Free(uintptr_t ptr);
		inline	void		Free(void *ptr) { Free(Offset(ptr)); }

		virtual	void		Lock();
		virtual	void		Unlock();

		virtual uintptr_t	Offset(void *ptr);
		virtual void *		Ptr(uintptr_t offset);

				size_t		FreeSpace();
				size_t		LargestFree();
				void		CheckSanity();
				void		DumpFreeList();
	
	private:

				status_t	MapRequired(uintptr_t ptr, size_t size);
				status_t	UnmapRequired(uintptr_t ptr, size_t size);

		Stash<Chunk>		m_stash;
		Chunker				m_freeList;
		Hasher				m_allocs;
		SLocker				m_heapLock;
		size_t				m_size;
		uintptr_t			*m_mapped;
};

#endif // _SUPPORT2_HEAP_H_
