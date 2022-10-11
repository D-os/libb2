#pragma once
#include <cutils/compiler.h>
#include <os/services/BnRegistrarService.h>

namespace os {
namespace services {
namespace registrar {

using namespace android;

class RegistrarService : public BnRegistrarService
{
   public:
	static const char* const SERVICE_NAME ANDROID_API;

	RegistrarService() ANDROID_API;

   protected:
	virtual binder::Status addApplication() override;
	virtual binder::Status listApplications() override;
	virtual binder::Status getApplication() override;

	status_t shellCommand(int in, int out, int err, Vector<String16>& args,
						  const sp<IShellCallback>&	 callback,
						  const sp<IResultReceiver>& resultReceiver) override;
};
}  // namespace registrar
}  // namespace services
}  // namespace os
