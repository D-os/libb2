#include "Looper.h"

BLooper::BLooper(const char *name, int32 priority, int32 port_capacity)
{
}

BLooper::~BLooper() = default;

status_t BLooper::Archive(BMessage *data, bool deep) const
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

void BLooper::DispatchMessage(BMessage *message, BHandler *handler)
{
	debugger(__PRETTY_FUNCTION__);
}

void BLooper::MessageReceived(BMessage *msg)
{
	debugger(__PRETTY_FUNCTION__);
}

thread_id BLooper::Run()
{
	debugger(__PRETTY_FUNCTION__);
	return -1;
}

void BLooper::Quit()
{
	debugger(__PRETTY_FUNCTION__);
}

bool BLooper::QuitRequested()
{
	debugger(__PRETTY_FUNCTION__);
	return false;
}

BHandler *BLooper::ResolveSpecifier(BMessage *msg, int32 index, BMessage *specifier, int32 form, const char *property)
{
	debugger(__PRETTY_FUNCTION__);
	return nullptr;
}

status_t BLooper::GetSupportedSuites(BMessage *data)
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

void BLooper::AddCommonFilter(BMessageFilter *filter)
{
	debugger(__PRETTY_FUNCTION__);
}

bool BLooper::RemoveCommonFilter(BMessageFilter *filter)
{
	debugger(__PRETTY_FUNCTION__);
	return false;
}

void BLooper::SetCommonFilterList(BList *filters)
{
	debugger(__PRETTY_FUNCTION__);
}
