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
	virtual binder::Status addApplication(const ::std::string&			  mime_sig,
										  const ::os::storage::entry_ref& ref,
										  int32_t						  flags,
										  int32_t						  team,
										  int32_t						  thread,
										  int32_t*						  _aidl_return) override;
	virtual binder::Status listApplications() override;
	virtual binder::Status getApplication() override;

	status_t shellCommand(int in, int out, int err, Vector<String16>& args,
						  const sp<IShellCallback>&	 callback,
						  const sp<IResultReceiver>& resultReceiver) override;
};
}  // namespace registrar
}  // namespace services
}  // namespace os
