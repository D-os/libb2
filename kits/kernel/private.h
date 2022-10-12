#include <OS.h>

/* Pause instruction to prevent excess processor bus usage */
#define cpu_relax() __builtin_ia32_pause()

#define likely(x)    __builtin_expect (!!(x), 1)
#define unlikely(x)  __builtin_expect (!!(x), 0)

typedef enum {
    TASK_NEW = 0,
    TASK_PAUSED,
    TASK_RUNNING,
    TASK_EXITED
} _task_state;

typedef struct _thread_info_struct {
    pid_t           tid; // futex
    pthread_t       pthread;
    team_id         team;
    char			name[B_OS_NAME_LENGTH];
    int32			priority;
    thread_state	state;
    sem_id			sem;
    void			*stack_base;
    void			*stack_end;
    thread_func		func;
    void			*data;
    _task_state     task_state;
    int             has_data;
    thread_id       data_sender;
    int32           data_code;
    void            *data_buffer;
    size_t          data_buffer_size;
    _task_state     *task_state_copy;
    struct _thread_info_struct *next;
    struct _thread_info_struct *prev;
} _thread_info;

typedef struct {
    int             count; // futex
    team_id         team;
    char			name[B_OS_NAME_LENGTH];
    thread_id       latest_holder;
    // TODO: double-link list for get_next_sem_info()
} _sem_info;


extern __thread _thread_info *_info; // current thread info
_thread_info *_find_thread_info(thread_id thread);

#include <string.h>
#define COPY_OS_NAME_LENGTH(dest, src) \
    if (src) strncpy(dest, src, B_OS_NAME_LENGTH); else dest[0] = '\0';
