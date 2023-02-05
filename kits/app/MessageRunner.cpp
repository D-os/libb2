#include "MessageRunner.h"

#include <pimpl.h>

class BMessageRunner::impl
{
};

BMessageRunner::BMessageRunner(BMessenger target, const BMessage *msg, bigtime_t interval, int32 count)
{
}

BMessageRunner::BMessageRunner(BMessenger target, const BMessage *msg, bigtime_t interval, int32 count, BMessenger reply_to)
{
}

BMessageRunner::~BMessageRunner()
{
}

status_t BMessageRunner::InitCheck() const
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

status_t BMessageRunner::SetInterval(bigtime_t interval)
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

status_t BMessageRunner::SetCount(int32 count)
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

status_t BMessageRunner::GetInfo(bigtime_t *interval, int32 *count) const
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}
