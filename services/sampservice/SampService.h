#pragma once
#include <cutils/compiler.h>
#include <os/services/sampservice/BnSampService.h>

namespace os {
namespace services {
namespace sampservice {

using namespace android;

class SampService : public BnSampService
{
   public:
	static const char* const SERVICE_NAME ANDROID_API;

	SampService() ANDROID_API;

   protected:
	binder::Status add(int32_t a, int32_t b, int32_t* _aidl_return) override;
	binder::Status sub(int32_t a, int32_t b, int32_t* _aidl_return) override;

	status_t shellCommand(int in, int out, int err, Vector<String16>& args,
						  const sp<IShellCallback>&	 callback,
						  const sp<IResultReceiver>& resultReceiver) override;
};
}  // namespace sampservice
}  // namespace services
}  // namespace os
