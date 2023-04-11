#include <binder/IResultReceiver.h>
#include <binder/IShellCallback.h>
#include <utils/Trace.h>

#include "RegistrarService.h"

using namespace os::services::registrar;

namespace {
status_t cmdHelp(int out);
}

const char* const RegistrarService::SERVICE_NAME = "registrar";

RegistrarService::RegistrarService()
{
}

binder::Status RegistrarService::addApplication(const ::std::string&			mime_sig,
												const ::os::storage::entry_ref& ref,
												int32_t							flags,
												int32_t							team,
												int32_t							thread,
												int32_t*						_aidl_return)
{
	ATRACE_CALL();
	ALOGV("RegistrarService::addApplication: '%s' %d/%s 0x%x %d:%d", mime_sig.c_str(), ref.dirfd, ref.name, flags, team, thread);

	if (!_aidl_return)
		return binder::Status::fromExceptionCode(binder::Status::EX_ILLEGAL_ARGUMENT);

	*_aidl_return = B_OK;
	return binder::Status::ok();
}

binder::Status RegistrarService::listApplications()
{
	return binder::Status::ok();
}

binder::Status RegistrarService::getApplication()
{
	return binder::Status::ok();
}

status_t RegistrarService::shellCommand(int in, int out, int err, Vector<String16>& args,
										const sp<IShellCallback>& callback, const sp<IResultReceiver>& resultReceiver)
{
	ATRACE_CALL();

	ALOGV("shellCommand");
	for (auto arg : args)
		ALOGV("  : %s", String8(arg).string());

	int32_t result = BAD_VALUE;
	if (args.size() > 0) {
		if (args[0] == String16("help")) {
			result = cmdHelp(out);
			goto out;
		}
	}
	// no command, or unrecognized command
	cmdHelp(err);

out:
	if (resultReceiver != nullptr) {
		resultReceiver->send(result);
	}
	return NO_ERROR;
}

namespace {

status_t cmdHelp(int out)
{
	FILE* outs = fdopen(out, "w");
	if (!outs) {
		ALOGE("RegistrarService: failed to create out stream: %s (%d)", strerror(errno), errno);
		return BAD_VALUE;
	}

	fprintf(outs,
			"RegistrarService commands:\n"
			"  help   print this help message\n");

	fclose(outs);
	return NO_ERROR;
}

}  // anonymous namespace
