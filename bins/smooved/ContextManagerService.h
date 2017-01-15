#pragma once

#include <binder/BinderService.h>
#include <os/support/IContextManager.h>
#include <os/support/BnContextManager.h>

namespace os {
namespace support {

using namespace android;
using namespace android::binder;

// ---------------------------------------------------------------------------
class ContextManagerService :
        public BinderService<ContextManagerService>,
        public BnContextManager
{
public:

private:
    friend class BinderService<ContextManagerService>;

    static char const* getServiceName() { return "context_manager"; }
    ContextManagerService();
    virtual ~ContextManagerService();

    // IContextManager interface
    virtual Status getContext(sp<IBinder>* ctx);

};

} } // namespace os::support
