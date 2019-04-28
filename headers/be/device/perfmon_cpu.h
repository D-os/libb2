/* 
	DO NOT use this driver  in the production code !!! 
	This interface WILL BE CHANGED in the next releases.	
*/



#ifndef _PERFMON_CPU_H
#define _PERFMON_CPU_H

#ifdef __cplusplus
extern "C" {
#endif

#if __INTEL__

#define PERFMON_CPU_DEVICE_NAME "perfmon/cpu"


/* 
	write this structure to the device to setup monitored events
	Use regX ==  0 if you do not want to write to the register. 
 */ 
typedef struct 
{
	uint32	reg0;		
	uint32	reg1;
	uint64	reg0_val;
	uint64	reg1_val;
} perfmon_cpu_control;	

/* 
	read this structure from the device to get Pentium CPU counters.
	On Pentium MMX, PentiumPro, Pentium || use perfmon_user.h  read_pmc() function.
*/
typedef struct
{
	uint64	counter0;
	uint64	counter1;
} perfmon_cpu_counters;	

#endif	/* __INTEL__ */

#ifdef __cplusplus
}
#endif

#endif	/* _PERFMON_CPU_H */
