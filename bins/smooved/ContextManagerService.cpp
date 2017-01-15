#include "ContextManagerService.h"

namespace os {
namespace support {

ContextManagerService::ContextManagerService()
{}

ContextManagerService::~ContextManagerService()
{}

Status ContextManagerService::getContext(sp<IBinder>* ctx)
{
    ctx = nullptr;
    return Status::ok();
}

} }; // namespace os::support

