#include <support/Context.h>
#include <cutils/klog.h>

using namespace os::support;

int main(int argc, char* const argv[])
{
    klog_set_level(KLOG_DEBUG_LEVEL);
    auto ctx = Context::UserContext();
    return ctx.InitCheck();
}
