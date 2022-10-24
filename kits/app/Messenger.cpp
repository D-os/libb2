#include "Messenger.h"

BMessenger::BMessenger() {}

BMessenger::BMessenger(const BHandler *handler, const BLooper *looper, status_t *perr)
{
	debugger(__PRETTY_FUNCTION__);
}

BMessenger::BMessenger(const BMessenger &from)
{
	debugger(__PRETTY_FUNCTION__);
}

BMessenger::~BMessenger() = default;

BMessenger &BMessenger::operator=(const BMessenger &from)
{
	debugger(__PRETTY_FUNCTION__);
	return *this;
}
bool BMessenger::operator==(const BMessenger &other) const
{
	debugger(__PRETTY_FUNCTION__);
	return false;
}
