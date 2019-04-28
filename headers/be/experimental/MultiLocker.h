/*******************************************************************************
/
/	File:			MultiLocker.h
/
/   Description:    Experimental multiple-reader single-writer locking class.
/
/	Copyright 2000, Be Incorporated, All Rights Reserved
/
*******************************************************************************/


#ifndef _MULTI_LOCKER_H
#define _MULTI_LOCKER_H

#include <OS.h>

namespace BExperimentalPrivate {
struct multilocker_timing;
}

namespace BExperimental {

using namespace ::BExperimentalPrivate;

class BMultiLocker
{
	public:
		//create a new lock; if 'debug' is true, then this lock with
		//verify correctness of its use (i.e., report if a thread tries
		//to acquire multiple read locks).
								BMultiLocker(const char* name = "some MultiLocker",
											 bool debug = true);
		virtual					~BMultiLocker();
		
		status_t				InitCheck();
		
		//locking for reading or writing
		bool					ReadLock();
		bool					WriteLock();

		//unlocking after reading or writing
		bool					ReadUnlock();
		bool					WriteUnlock();

		//does the current thread hold a write lock ?
		bool					IsWriteLocked(uint32 *stack_base = NULL, thread_id *thread = NULL) const;
		//in DEBUG mode returns whether the lock is held
		//in non-debug mode returns true
		bool					IsReadLocked() const;

		enum {
			MAX_READERS = 1000000000
		};

	private:
		//functions for managing the DEBUG reader array
		void					register_thread();
		void					unregister_thread();
		
		status_t				fInit;
		//readers adjust count and block on fReadSem when a writer
		//hold the lock
		int32					fReadCount;
		sem_id					fReadSem;
		//writers adjust the count and block on fWriteSem
		//when readers hold the lock
		int32					fWriteCount;
		sem_id 					fWriteSem;
		//writers must acquire fWriterLock when acquiring a write lock
		int32					fLockCount;
		sem_id					fWriterLock;
		int32					fWriterNest;
	
		thread_id				fWriterThread;
		uint32					fWriterStackBase;
				
		int32 *					fDebugArray;
		int32					fMaxThreads;
		
		multilocker_timing*		fTiming;
};

}	// namespace BExperimental

using namespace BExperimental;

#endif
