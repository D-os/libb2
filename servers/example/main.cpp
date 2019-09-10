#include <binder/BinderService.h>
#include <log/log.h>
#include <support/Errors.h>

#include "example/BnExample.h"

//using namespace ::os::support;
using namespace ::android;

class ExampleService : public BinderService<ExampleService>, public example::BnExample
{
 public:
  static status_t    start();
  static char const* getServiceName() { return "example.service"; }

  // IExample
  virtual binder::Status Concat(const std::string& name, String16* _aidl_return);
};

status_t ExampleService::start()
{
  IPCThreadState::self()->disableBackgroundScheduling(true);
  status_t ret = BinderService<ExampleService>::publish();
  if (ret != OK) {
    return ret;
  }
  sp<ProcessState> ps(ProcessState::self());
  ps->startThreadPool();
  ps->giveThreadPoolName();
  return OK;
}

binder::Status ExampleService::Concat(const ::std::string& name, String16* _aidl_return)
{
  _aidl_return->setTo(String16((name + name).c_str()));
  return binder::Status::ok();
}

int main(int argc, char* argv[])
{
  int ret;

  if ((ret = ExampleService::start()) != OK) {
    SLOGE("Unable to start ExampleService: %d", ret);
    exit(EXIT_FAILURE);
  }

  IPCThreadState::self()->joinThreadPool();

  return EXIT_SUCCESS;
}
