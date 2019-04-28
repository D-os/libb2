/******************************************************************************
/
/	File:			StopWatch.h
/
/	Description:	BStopWatch class defines a handy code timing debug tool.
/
/	Copyright 1993-98, Be Incorporated
/
******************************************************************************/

#ifndef _STOP_WATCH_H
#define _STOP_WATCH_H

#include <BeBuild.h>
#include <SupportDefs.h>

/*-------------------------------------------------------------*/
/*----- BStopWatch class --------------------------------------*/

class BStopWatch {
public:
					BStopWatch(const char *name, bool silent = false);
virtual				~BStopWatch();

		void		Suspend();
		void		Resume();
		bigtime_t	Lap();
		bigtime_t	ElapsedTime() const;
		void		Reset();
		const char	*Name() const;

/*----- Private or reserved ---------------*/
private:

virtual	void		_ReservedStopWatch1();
virtual	void		_ReservedStopWatch2();

					BStopWatch(const BStopWatch &);
		BStopWatch	&operator=(const BStopWatch &);

		bigtime_t	fStart;
		bigtime_t	fSuspendTime;
		bigtime_t	fLaps[10];
		int32		fLap;
		const char	*fName;
		uint32		_reserved[2];
		bool		fSilent;
};

/*-------------------------------------------------------------*/
/*-------------------------------------------------------------*/

#endif /* _STOP_WATCH_H */
