#include "ContextManagerService.h"

#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

namespace os { namespace support {

char const* ContextManagerService::getServiceName() {
    static char serviceName[32];
    if (!serviceName[0]) {
        snprintf(serviceName, sizeof(serviceName), "user.%u.context_manager", getuid());
    }
    return serviceName;
}

ContextManagerService::ContextManagerService()
{}

ContextManagerService::~ContextManagerService()
{}

Status ContextManagerService::getContext(sp<IBinder>* ctx)
{
    auto ts = IPCThreadState::self();
    ALOGE("%s called from %d by %d", __PRETTY_FUNCTION__, ts->getCallingPid(), ts->getCallingUid());
    ctx->force_set(this->onAsBinder());
    return Status::ok();
}

} }; // namespace os::support

