#include "Application.h"

BApplication *be_app = NULL;
// BMessenger be_app_messenger;

BApplication::BApplication(const char *signature)
	: BLooper(signature)
{
	if (be_app != NULL)
		debugger("BApplication already created. Only one is allowed.");

	be_app = this;
	// be_app_messenger = BMessenger(this);
}

BApplication::~BApplication()
{
}

thread_id BApplication::Run()
{
	return 0;
}
