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

status_t BApplication::Archive(BMessage *data, bool deep) const
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

thread_id BApplication::Run()
{
	debugger(__PRETTY_FUNCTION__);
	return -1;
}

void BApplication::Quit()
{
	debugger(__PRETTY_FUNCTION__);
}

bool BApplication::QuitRequested()
{
	debugger(__PRETTY_FUNCTION__);
	return false;
}

void BApplication::Pulse()
{
	debugger(__PRETTY_FUNCTION__);
}

void BApplication::ReadyToRun()
{
	debugger(__PRETTY_FUNCTION__);
}

void BApplication::MessageReceived(BMessage *msg)
{
	debugger(__PRETTY_FUNCTION__);
}

void BApplication::ArgvReceived(int32 argc, char **argv)
{
	debugger(__PRETTY_FUNCTION__);
}

void BApplication::AppActivated(bool active)
{
	debugger(__PRETTY_FUNCTION__);
}

void BApplication::RefsReceived(BMessage *a_message)
{
	debugger(__PRETTY_FUNCTION__);
}

void BApplication::AboutRequested()
{
	debugger(__PRETTY_FUNCTION__);
}

BHandler *BApplication::ResolveSpecifier(BMessage *msg, int32 index, BMessage *specifier, int32 form, const char *property)
{
	debugger(__PRETTY_FUNCTION__);
	return nullptr;
}

void BApplication::DispatchMessage(BMessage *an_event, BHandler *handler)
{
	debugger(__PRETTY_FUNCTION__);
}

status_t BApplication::GetSupportedSuites(BMessage *data)
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}
