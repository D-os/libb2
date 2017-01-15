#include <signal.h>
#include "ContextManagerService.h"

using namespace os::support;

int main(int /*argc*/, char** /*argv*/) {
    signal(SIGPIPE, SIG_IGN);
    ContextManagerService::publishAndJoinThreadPool();
    return 0;
}
