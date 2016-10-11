#include <OS.h>
#include <syscalls.h>

status_t _kern_stop_notifying(port_id port, uint32 token)
{
    STUB;return 0;
}

status_t _kern_start_watching(int nodefd, uint32 flags,
                              port_id port, uint32 token)
{
    STUB;return 0;
}

status_t _kern_stop_watching(int nodefd, port_id port, uint32 token)
{
    STUB;return 0;
}
