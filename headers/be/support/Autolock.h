/******************************************************************************
/
/	File:			Autolock.h
/
/	Description:	BAutolock is a stack-based locking mechanism.
/
/	Copyright 1993-98, Be Incorporated
/
******************************************************************************/

#ifndef	_AUTOLOCK_H
#define	_AUTOLOCK_H

#include <BeBuild.h>
#include <Locker.h>
#include <Looper.h>

/*-----------------------------------------------------------------*/
/*----- BAutolock class --------------------------------------------*/

class BAutolock {
public:
					BAutolock(BLocker *lock);
					BAutolock(BLocker &lock);
					BAutolock(BLooper *looper);
					~BAutolock();
		
		bool		IsLocked();

/*----- Private or reserved ---------------*/
private:
		BLocker		*fLock;
		BLooper		*fLooper;
		bool		fLocked;
};

/*-------------------------------------------------------------*/
/*----- inline implementations --------------------------------*/

inline BAutolock::BAutolock(BLooper *looper)
{
	fLooper = looper;
	fLock = NULL;
	fLocked = fLooper->Lock();
}

inline BAutolock::BAutolock(BLocker *target)
{
	fLooper = NULL;
	fLock = target;
	fLocked = fLock->Lock();
}

inline BAutolock::BAutolock(BLocker &target)
{
	fLooper = NULL;
	fLock = &target;
	fLocked = fLock->Lock();
}

inline BAutolock::~BAutolock()
{
	if (fLocked) {
		if (fLock)
			fLock->Unlock();
		else
			fLooper->Unlock();
	}
}

inline bool BAutolock::IsLocked()
{
	return fLocked;
}

/*-------------------------------------------------------------*/
/*-------------------------------------------------------------*/

#endif /* _AUTOLOCK_H */
