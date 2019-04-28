/******************************************************************************
/
/	File:			Locker.h
/
/	Description:	BLocker class defines a nestable locking mechanism.
/
/	Copyright 1993-98, Be Incorporated
/
******************************************************************************/

#ifndef	_LOCKER_H
#define	_LOCKER_H

#include <BeBuild.h>
#include <OS.h>
#include <SupportDefs.h>

/*-------------------------------------------------------------*/
/*----- BLocker class -----------------------------------------*/

class BLocker {
public:
					BLocker(bool benaphore_style);
					BLocker(const char *name, bool benaphore_style);
					BLocker(const char *name, bool benaphore_style, bool for_IPC);

					BLocker();
					BLocker(const char *name);
virtual				~BLocker();	

		bool		Lock();
		void		Unlock();
		bool		IsLocked() const;
		status_t	LockWithTimeout(bigtime_t timeout);

/* -- For debugging (only!) -- */
		thread_id	LockingThread() const;
		int32		CountLocks() const;
		int32		CountLockRequests() const;
		sem_id		Sem() const;

/*----- Private or reserved ---------------*/
private:
		void		InitData(const char *name, bool benaphore, bool ipc);
		bool		_Lock(bigtime_t timeout, status_t *error);

		int32		fCount;
		sem_id		fSem;
		int32		fOwner;
		int32		fOwnerCount;
#if !_PR3_COMPATIBLE_
mutable uint32		fOwnerStack;
		int32		fCanUseStack;
		int32		fReserved[2];
#endif /* not _PR3_COMPATIBLE_ */
};

/*-------------------------------------------------------------*/
/*-------------------------------------------------------------*/

#endif /* _LOCKER_H */
