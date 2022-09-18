#ifndef _OS_H
#define _OS_H

#include <StorageDefs.h>
#include <SupportDefs.h>

#ifdef __cplusplus
extern "C" {
#endif

// system-wide constants; also see storage/StorageDefs.h.
#define B_OS_NAME_LENGTH 32
#define B_PAGE_SIZE 4096
#define B_INFINITE_TIMEOUT (9223372036854775807LL)

// types
typedef int32 area_id;
typedef int32 port_id;
typedef int32 sem_id;
typedef int32 thread_id;
typedef int32 team_id;

/// Areas
#define B_NO_LOCK 0
#define B_LAZY_LOCK 1
#define B_FULL_LOCK 2
#define B_CONTIGUOUS 3
#define B_LOMEM 4

#define B_ANY_ADDRESS 0
#define B_EXACT_ADDRESS 1
#define B_BASE_ADDRESS 2
#define B_CLONE_ADDRESS 3
#define B_ANY_KERNEL_ADDRESS 4

#define B_READ_AREA 1
#define B_WRITE_AREA 2

typedef struct area_info
{
	area_id area;
	char	name[B_OS_NAME_LENGTH];
	size_t	size;
	uint32	lock;
	uint32	protection;
	team_id team;
	uint32	ram_size;
	uint32	copy_count;
	uint32	in_count;
	uint32	out_count;
	void	 *address;
} area_info;

extern area_id create_area(const char *name, void **start_addr,
						   uint32 addr_spec, size_t size,
						   uint32 lock, uint32 protection);

extern area_id clone_area(const char *name, void **dest_addr,
						  uint32 addr_spec, uint32 protection,
						  area_id source);

extern area_id	find_area(const char *name);
extern area_id	area_for(void *addr);
extern status_t delete_area(area_id id);
extern status_t resize_area(area_id id, size_t new_size);
extern status_t set_area_protection(area_id id,
									uint32	new_protection);

extern status_t _get_area_info(area_id id, area_info *ainfo,
							   size_t size);
extern status_t _get_next_area_info(team_id team, int32 *cookie,
									area_info *ainfo, size_t size);

#define get_area_info(id, ainfo) \
	_get_area_info((id), (ainfo), sizeof(*(ainfo)))
#define get_next_area_info(team, cookie, ainfo) \
	_get_next_area_info((team), (cookie), (ainfo), sizeof(*(ainfo)))

/// Ports
typedef struct port_info
{
	port_id port;
	team_id team;
	char	name[B_OS_NAME_LENGTH];
	int32	capacity;	 /* queue depth */
	int32	queue_count; /* # msgs waiting to be read */
	int32	total_count; /* total # msgs read so far */
} port_info;

extern port_id create_port(int32 capacity, const char *name);
extern port_id find_port(const char *name);

extern status_t write_port(port_id port, int32 code,
						   const void *buf,
						   size_t	   buf_size);

extern status_t read_port(port_id port, int32 *code,
						  void *buf, size_t buf_size);

extern status_t write_port_etc(port_id port, int32 code,
							   const void *buf, size_t buf_size,
							   uint32 flags, bigtime_t timeout);

extern status_t read_port_etc(port_id port, int32 *code,
							  void *buf, size_t buf_size,
							  uint32 flags, bigtime_t timeout);

extern ssize_t port_buffer_size(port_id port);
extern ssize_t port_buffer_size_etc(port_id port,
									uint32 flags, bigtime_t timeout);

extern ssize_t	port_count(port_id port);
extern status_t set_port_owner(port_id port, team_id team);

extern status_t close_port(port_id port);

extern status_t delete_port(port_id port);

extern status_t _get_port_info(port_id port, port_info *info,
							   size_t size);
extern status_t _get_next_port_info(team_id team, int32 *cookie, port_info *info, size_t size);

#define get_port_info(port, info) \
	_get_port_info((port), (info), sizeof(*(info)))

#define get_next_port_info(team, cookie, info) \
	_get_next_port_info((team), (cookie), (info), sizeof(*(info)))

// Semaphores
typedef struct sem_info
{
	sem_id	  sem;
	team_id	  team;
	char	  name[B_OS_NAME_LENGTH];
	int32	  count;
	thread_id latest_holder;
} sem_info;

extern sem_id	create_sem(int32 count, const char *name);
extern status_t delete_sem(sem_id sem);
extern status_t acquire_sem(sem_id sem);
extern status_t acquire_sem_etc(sem_id sem, int32 count,
								uint32 flags, bigtime_t microsecond_timeout);
extern status_t release_sem(sem_id sem);
extern status_t release_sem_etc(sem_id sem, int32 count,
								uint32 flags);
extern status_t get_sem_count(sem_id sem, int32 *count); /* be careful! */

extern status_t set_sem_owner(sem_id sem, team_id team);

extern status_t _get_sem_info(sem_id sem, sem_info *info,
							  size_t size);
extern status_t _get_next_sem_info(team_id team, int32 *cookie,
								   sem_info *info, size_t size);

#define get_sem_info(sem, info) \
	_get_sem_info((sem), (info), sizeof(*(info)))

#define get_next_sem_info(team, cookie, info) \
	_get_next_sem_info((team), (cookie), (info), sizeof(*(info)))

/// flags for semaphore control
enum {
	B_CAN_INTERRUPT		= 1, /* semaphore can be interrupted by a signal */
	B_DO_NOT_RESCHEDULE = 2, /* release() without rescheduling */
	B_CHECK_PERMISSION	= 4, /* disallow users changing kernel semaphores */
	B_TIMEOUT			= 8, /* honor the (relative) timeout parameter */
	B_RELATIVE_TIMEOUT	= 8,
	B_ABSOLUTE_TIMEOUT	= 16 /* honor the (absolute) timeout parameter */
};

/// alarms
enum {
	B_ONE_SHOT_ABSOLUTE_ALARM = 1, /* alarm is one-shot and time is specified absolutely */
	B_ONE_SHOT_RELATIVE_ALARM = 2, /* alarm is one-shot and time is specified relatively */
	B_PERIODIC_ALARM		  = 3  /* alarm is periodic and time is the period */
};

extern bigtime_t set_alarm(bigtime_t when, uint32 flags);

/// Threads
typedef enum {
	B_THREAD_RUNNING = 1,
	B_THREAD_READY,
	B_THREAD_RECEIVING,
	B_THREAD_ASLEEP,
	B_THREAD_SUSPENDED,
	B_THREAD_WAITING
} thread_state;

#define B_LOW_PRIORITY 5
#define B_NORMAL_PRIORITY 10
#define B_DISPLAY_PRIORITY 15
#define B_URGENT_DISPLAY_PRIORITY 20
#define B_REAL_TIME_DISPLAY_PRIORITY 100
#define B_URGENT_PRIORITY 110
#define B_REAL_TIME_PRIORITY 120

typedef struct
{
	thread_id	 thread;
	team_id		 team;
	char		 name[B_OS_NAME_LENGTH];
	thread_state state;
	int32		 priority;
	sem_id		 sem;
	bigtime_t	 user_time;
	bigtime_t	 kernel_time;
	void		 *stack_base;
	void		 *stack_end;
} thread_info;

typedef struct
{
	bigtime_t user_time;
	bigtime_t kernel_time;
} team_usage_info;

typedef int32 (*thread_func)(void *);

/// thread_entry is obsolete ("entry" is reserved by the file system)
/// use thread_func instead.
#define thread_entry thread_func

extern thread_id spawn_thread(
	thread_func function_name,
	const char *thread_name,
	int32		priority,
	void		 *arg);

extern status_t kill_thread(thread_id thread);
extern status_t resume_thread(thread_id thread);
extern status_t suspend_thread(thread_id thread);
extern status_t rename_thread(thread_id thread, const char *new_name);
extern status_t set_thread_priority(thread_id thread, int32 new_priority);
extern void		exit_thread(status_t status);
extern status_t wait_for_thread(thread_id thread,
								status_t *thread_return_value);
extern status_t on_exit_thread(void (*callback)(void *), void *data);

extern status_t _get_thread_info(thread_id thread, thread_info *info, size_t size);
extern status_t _get_next_thread_info(team_id tmid, int32 *cookie, thread_info *info, size_t size);
extern status_t _get_team_usage_info(team_id tmid, int32 who, team_usage_info *ti, size_t size);

extern thread_id find_thread(const char *name);

#define get_thread_info(thread, info) \
	_get_thread_info((thread), (info), sizeof(*(info)))

#define get_next_thread_info(tmid, cookie, info) \
	_get_next_thread_info((tmid), (cookie), (info), sizeof(*(info)))

#define get_team_usage_info(tmid, who, info) \
	_get_team_usage_info((tmid), (who), (info), sizeof(*(info)))

extern status_t send_data(thread_id	  thread,
						  int32		  code,
						  const void *buf,
						  size_t	  buffer_size);

extern status_t receive_data(thread_id *sender,
							 void	  *buf,
							 size_t		buffer_size);

extern bool has_data(thread_id thread);

extern status_t snooze(bigtime_t microseconds);

extern status_t snooze_until(bigtime_t time, int timebase);
#define B_SYSTEM_TIMEBASE (0)

/// Teams
#define B_SYSTEM_TEAM 2

typedef struct
{
	team_id	  team;
	int32	  image_count;
	int32	  thread_count;
	int32	  area_count;
	thread_id debugger_nub_thread;
	port_id	  debugger_nub_port;

	int32 argc;		/* number of args on the command line */
	char  args[64]; /* abbreviated command line args */
	uid_t uid;
	gid_t gid;
} team_info;

extern status_t kill_team(team_id team); /* see also: send_signal() */

extern status_t _get_team_info(team_id team, team_info *info, size_t size);
extern status_t _get_next_team_info(int32 *cookie, team_info *info, size_t size);

#define get_team_info(team, info) \
	_get_team_info((team), (info), sizeof(*(info)))

#define get_next_team_info(cookie, info) \
	_get_next_team_info((cookie), (info), sizeof(*(info)))

extern uint32	 real_time_clock(void);
extern void		 set_real_time_clock(int32 secs_since_jan1_1970);
extern bigtime_t real_time_clock_usecs(void);
extern status_t	 set_timezone(char *str);

extern bigtime_t system_time(void); /* time since booting in microseconds */

/// debugging calls
extern void		 debugger(const char *message);
extern const int disable_debugger(int state);

#ifdef __cplusplus
}
#endif

#endif /* ifdef _OS_H */
