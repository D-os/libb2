#include <OS.h>

/* Pause instruction to prevent excess processor bus usage */
#define cpu_relax() __builtin_ia32_pause()

#define cmpxchg(P, O, N) __sync_val_compare_and_swap((P), (O), (N))
#define atomic_add(P, V) __sync_add_and_fetch((P), (V))
#define atomic_sub(P, V) __sync_add_and_fetch((P), -(V))
#define atomic_xadd(P, V) __sync_fetch_and_add((P), (V))

#define likely(x)    __builtin_expect (!!(x), 1)
#define unlikely(x)  __builtin_expect (!!(x), 0)

typedef enum {
    TASK_NEW = 0,
    TASK_WAITING,
    TASK_RUNNING,
    TASK_EXITED
} _task_state;

typedef struct {
    pid_t           tid;
    char			name[B_OS_NAME_LENGTH];
    int32			priority;
    thread_state	state;
    sem_id			sem;
    void			*stack_base;
    void			*stack_end;
    thread_func		func;
    void			*data;
    status_t		exit;
    _task_state     task_state;
    int             has_data;
    thread_id       data_sender;
    int32           data_code;
    void            *data_buffer;
    size_t          data_buffer_size;
} _thread_info;

typedef struct {
    int             count; // futex
    team_id         team;
    char			name[B_OS_NAME_LENGTH];
    thread_id       latest_holder;
    // TODO: double-link list for get_next_sem_info()
} _sem_info;


_thread_info *_find_thread_info(thread_id thread);