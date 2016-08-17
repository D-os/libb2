#include <OS.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/mman.h>
#include <linux/mman.h>
#include <string.h>
#include <stdlib.h>

#include "private.h"
#include "utlist.h"
#include "rwlock.h"

typedef struct _area_info_struct {
    int         shmid;
    char        name[B_OS_NAME_LENGTH];
    uint32		lock;
    uint32      protection;
    void        *address;
    struct _area_info_struct *next;
    struct _area_info_struct *prev;
} _area_info;

static _area_info *_areas = NULL;
RWLOCK(_areas)

/* WARNING! you need to lock _areas in caller function! */
static _area_info *_find_area_info(area_id id)
{
    _area_info *info;
    int shmid = (int)id;
    DL_SEARCH_SCALAR(_areas, info, shmid, shmid);
    if (info) return info;

    void *address = (void *)id;
    DL_SEARCH_SCALAR(_areas, info, address, address);
    return info;
}
static _area_info *_find_area_info_name(const char *name)
{
    _area_info *info;
    DL_SEARCH_SCALAR(_areas, info, name, name);
    return info;
}

static area_id _attach_shm_area(int shmid, const char *name,
                                void **startAddress, uint32 addressSpec,
                                size_t size, uint32 lock, uint32 protection)
{
    void *shmaddr = NULL;
    int shmflg = 0;

    if (!startAddress) return B_BAD_VALUE;

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

    _area_info *info = calloc(1, sizeof(_area_info));
    info->shmid = shmid;
    COPY_OS_NAME_LENGTH(info->name, name);
    info->address = shmaddr;
    info->lock = lock;
    info->protection = protection;
    _areas_wlock();
    DL_APPEND(_areas, info);
    _areas_unlock();

    *startAddress = shmaddr;
    return shmid;
}

area_id create_area(const char *name, void **startAddress, uint32 addressSpec,
                    size_t size, uint32 lock, uint32 protection)
{
    int shmid = shmget(IPC_PRIVATE, size, IPC_CREAT | 0600);
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
    return _attach_shm_area(shmid, name, startAddress, addressSpec, size, lock, protection);
}

status_t delete_area(area_id id)
{
    _areas_rlock();
    _area_info *info = _find_area_info(id);
    _areas_unlock();
    if(info) {
        _areas_wlock();
        DL_DELETE(_areas, info);
        _areas_unlock();
        free(info);
    }
    if (shmctl(id, IPC_RMID, NULL) != 0) {
        switch (errno) {
        case EIDRM:
        case EINVAL:
            return B_ERROR;
        case EPERM:
            return B_PERMISSION_DENIED;
        default:
            return B_ERROR;
        }
    }
    return B_OK;
}

status_t set_area_protection(area_id id, uint32 newProtection)
{
    int shmid = -1;
    void *shmaddr = NULL;
    _areas_wlock();
    _area_info *info = _find_area_info(id);
    if (info) {
        info->protection = newProtection;
        shmaddr = info->address;
        shmid = info->shmid;
    }
    _areas_unlock();
    if (!info || !shmaddr) return B_BAD_VALUE;

    int shmflg = SHM_REMAP;
    if (!(newProtection & B_WRITE_AREA)) shmflg |= SHM_RDONLY;
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
    return B_OK;
}

area_id find_area(const char *name)
{
    area_id id = -1;
    _areas_rlock();
    _area_info *info = _find_area_info_name(name);
    if (info) id = info->shmid;
    _areas_unlock();
    return info ? id : B_NAME_NOT_FOUND;
}

area_id clone_area(const char *name, void **destAddress,
                   uint32 addressSpec, uint32 protection, area_id source)
{
    int shmid = -1;
    _areas_rlock();
    _area_info *info = _find_area_info(source);
    if (info) shmid = info->shmid;
    _areas_unlock();
    if (!info) shmid = (int)source;
    struct shmid_ds ds = {};
    if (shmctl(shmid, IPC_STAT, &ds) != 0) {
        switch (errno) {
        case EIDRM:
        case EINVAL:
            return B_BAD_VALUE;
        case EPERM:
            return B_PERMISSION_DENIED;
        default:
            return B_ERROR;
        }
    }
    area_id clone = _attach_shm_area(shmid, name, destAddress, addressSpec, ds.shm_segsz, B_NO_LOCK, protection);
    return info ? (intptr_t)*destAddress : clone;
}

status_t _get_area_info(area_id id, area_info *areaInfo, size_t size)
{
    _area_info *info = NULL;
    int shmid = -1;

    memset(areaInfo, 0, size);
    areaInfo->area = id;

    _areas_rlock();
    info = _find_area_info(id);
    if (info) {
        shmid = info->shmid;
        COPY_OS_NAME_LENGTH(areaInfo->name, info->name);
        areaInfo->lock = info->lock;
        areaInfo->protection = info->protection;
        areaInfo->address = info->address;
    }
    _areas_unlock();
    if (!info) return B_ERROR;

    struct shmid_ds ds = {};
    if (shmctl(shmid, IPC_STAT, &ds) != 0) {
        goto error;
    }
    areaInfo->size = ds.shm_segsz;
    areaInfo->team = ds.shm_cpid;
    areaInfo->copy_count = ds.shm_nattch;

    return B_OK;
error:
    switch (errno) {
    case EIDRM:
    case EINVAL:
        return B_ERROR;
    case EPERM:
        return B_PERMISSION_DENIED;
    default:
        return B_ERROR;
    }
}
