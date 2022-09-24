#include "RosterService.h"

#include <binder/IResultReceiver.h>
#include <binder/IShellCallback.h>
#include <utils/Trace.h>

using namespace os::services::rosterservice;

namespace {
status_t cmdHelp(int out);
}

const char* const RosterService::SERVICE_NAME = "samp";

RosterService::RosterService()
{
}

binder::Status RosterService::add(int32_t a, int32_t b, int32_t* _aidl_return)
{
	*_aidl_return = a + b;
	return binder::Status::ok();
}

binder::Status RosterService::sub(int32_t a, int32_t b, int32_t* _aidl_return)
{
	*_aidl_return = a - b;
	return binder::Status::ok();
}

status_t RosterService::shellCommand(int in, int out, int err, Vector<String16>& args,
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
		ALOGE("RosterService: failed to create out stream: %s (%d)", strerror(errno), errno);
		return BAD_VALUE;
	}

	fprintf(outs,
			"RosterService commands:\n"
			"  help   print this help message\n");

	fclose(outs);
	return NO_ERROR;
}

}  // anonymous namespace
