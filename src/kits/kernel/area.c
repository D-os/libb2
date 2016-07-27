#include <OS.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/mman.h>
#include <linux/mman.h>

area_id create_area(const char *name, void **startAddress, uint32 addressSpec,
                    size_t size, uint32 lock, uint32 protection)
{
    int shmid = shmget(IPC_PRIVATE, size, IPC_CREAT | 0666);
    if (shmid < 0) {
        switch (errno) {
        case EINVAL:
            return B_BAD_VALUE;
        case ENFILE:
        case ENOMEM:
        case ENOSPC:
            return B_NO_MEMORY;
        default:
            return B_ERROR;
        }
    }

    void *shmaddr = NULL;
    int shmflg = 0;

    switch (addressSpec) {
    case B_BASE_ADDRESS:
        shmflg |= SHM_RND;
        // fallthrough
    case B_EXACT_ADDRESS:
        shmaddr = *startAddress;
        break;
    case B_ANY_ADDRESS:
        shmaddr = NULL;
        break;
    case B_CLONE_ADDRESS:
        return B_BAD_VALUE;
    case B_ANY_KERNEL_ADDRESS:
    default:
        return B_NOT_SUPPORTED;
    }

    if (!(protection & B_WRITE_AREA)) shmflg |= SHM_RDONLY;

    shmaddr = shmat(shmid, shmaddr, shmflg);
    if (shmaddr == (void *) -1) {
        switch (errno) {
        case EIDRM:
        case EINVAL:
            return B_BAD_VALUE;
        case ENOMEM:
            return B_NO_MEMORY;
        default:
            return B_ERROR;
        }
    }

    int flags = 0;
    switch (lock) {
    case B_FULL_LOCK:
        flags = 0;
        break;
    case B_LAZY_LOCK:
        flags = MLOCK_ONFAULT;
        break;
    case B_CONTIGUOUS:
    case B_LOMEM:
        shmdt(shmaddr);
        return B_NOT_SUPPORTED;
    }

    if (lock != B_NO_LOCK) {
        if (syscall(SYS_mlock2, shmaddr, size, flags) != 0) {
            int e = errno;
            shmdt(shmaddr);
            switch (e) {
            case ENOMEM:
                return B_NO_MEMORY;
            case EPERM:
            case EAGAIN:
            case EINVAL:
                return B_BAD_VALUE;
            default:
                return B_ERROR;
            }
        }
    }

    //FIXME: track all areas by name, id, address, size so other functions can work

    *startAddress = shmaddr;
    return shmid;
}
