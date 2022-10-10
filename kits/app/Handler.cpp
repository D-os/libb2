#include "Handler.h"

BHandler::BHandler(const char *name)
{
}

BHandler::~BHandler() = default;

status_t BHandler::Archive(BMessage *data, bool deep) const
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

void BHandler::MessageReceived(BMessage *message)
{
	debugger(__PRETTY_FUNCTION__);
}

void BHandler::SetNextHandler(BHandler *handler)
{
	debugger(__PRETTY_FUNCTION__);
}

void BHandler::AddFilter(BMessageFilter *filter)
{
	debugger(__PRETTY_FUNCTION__);
}

bool BHandler::RemoveFilter(BMessageFilter *filter)
{
	debugger(__PRETTY_FUNCTION__);
	return false;
}

void BHandler::SetFilterList(BList *filters)
{
	debugger(__PRETTY_FUNCTION__);
}

BHandler *BHandler::ResolveSpecifier(BMessage *msg, int32 index, BMessage *specifier, int32 form, const char *property)
{
	debugger(__PRETTY_FUNCTION__);
	return nullptr;
}

status_t BHandler::GetSupportedSuites(BMessage *data)
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

void BHandler::SendNotices(uint32 what, const BMessage *)
{
	debugger(__PRETTY_FUNCTION__);
}
