/* ++++++++++
	File:			perfmon_user.h
	Description:	user mode interface to performance counters and time stamp 
	registers of 586 and 686 CPUs

	DO NOT use these functions in the production code !!! 
	This interface WILL BE CHANGED in the next releases.	

	User mode read_pmc()  is enabled only on Pentium || because of
	Pentium errata #74 and Pentium Pro errata #26.
	For details see http://developer.intel.com 

	Copyright (c) 1998 by Be Incorporated.  All Rights Reserved.
+++++ */



#ifndef _PERFMON_USER_H
#define	_PERFMON_USER_H

#include <SupportDefs.h>

#ifdef __cplusplus
extern "C" {
#endif

#if __INTEL__

extern _IMPEXP_ROOT		uint64	read_pmc(uint32 pmc); 
extern _IMPEXP_ROOT		uint64	read_tsc(void); 

#endif

#ifdef __cplusplus
}
#endif

#endif
