#include "ContextManagerService.h"
#include <support/Context.h>
#include <support/Catalog.h>
#include <support/Node.h>

#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

static os::support::Context g_rootContext;

namespace os { namespace support {

char const* ContextManagerService::getServiceName() {
    static char serviceName[32];
    if (!serviceName[0]) {
        snprintf(serviceName, sizeof(serviceName), "user.%u.context_manager", getuid());
    }
    return serviceName;
}

ContextManagerService::ContextManagerService()
{
    g_rootContext = Context(new Catalog());
}

ContextManagerService::~ContextManagerService()
{}

Status ContextManagerService::getContext(sp<IBinder>* ctx)
{
    auto ts = IPCThreadState::self();
    ALOGE("%s called from %d by %d", __PRETTY_FUNCTION__, ts->getCallingPid(), ts->getCallingUid());
    *ctx = IInterface::asBinder(g_rootContext.Root());
    return Status::ok();
}

} }; // namespace os::support

