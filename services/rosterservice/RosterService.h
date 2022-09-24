#pragma once
#include <cutils/compiler.h>
#include <os/services/rosterservice/BnRosterService.h>

namespace os {
namespace services {
namespace rosterservice {

using namespace android;

class RosterService : public BnRosterService
{
   public:
	static const char* const SERVICE_NAME ANDROID_API;

	RosterService() ANDROID_API;

   protected:
	binder::Status add(int32_t a, int32_t b, int32_t* _aidl_return) override;
	binder::Status sub(int32_t a, int32_t b, int32_t* _aidl_return) override;

	status_t shellCommand(int in, int out, int err, Vector<String16>& args,
						  const sp<IShellCallback>&	 callback,
						  const sp<IResultReceiver>& resultReceiver) override;
};
}  // namespace rosterservice
}  // namespace services
}  // namespace os
