#include "Handler.h"

#include <Looper.h>
#include <Message.h>

#include <cstdlib>
#include <cstring>

BHandler::BHandler(const char *name) : fName(nullptr),
									   fLooper(nullptr)
{
	SetName(name);
}

BHandler::~BHandler() = default;

status_t BHandler::Archive(BMessage *data, bool deep) const
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

void BHandler::MessageReceived(BMessage *message)
{
	if (fNextHandler) {
		// we need to apply the next handler's filters here, too
		// FIXME: BHandler *target = Looper()->_HandlerFilter(message, fNextHandler);
		BHandler *target = fNextHandler;  // FIXME: ^^^
		if (target != NULL && target != this) {
			// TODO: we also need to make sure that "target" is not before
			//      us in the handler chain - at least in case it wasn't before
			//      the handler actually targeted with this message - this could
			//      get ugly, though.
			target->MessageReceived(message);
		}
	}
	else if (message->what != B_MESSAGE_NOT_UNDERSTOOD
			 && (message->WasDropped() /*|| message->HasSpecifiers()*/)) {
		printf("BHandler %s: MessageReceived() couldn't understand the message:\n", Name());
		message->PrintToStream();
		message->SendReply(B_MESSAGE_NOT_UNDERSTOOD);
	}
}

void BHandler::SetLooper(BLooper *looper)
{
	fLooper = looper;
}

BLooper *BHandler::Looper() const
{
	return fLooper;
}

void BHandler::SetName(const char *name)
{
	if (fName) free(fName);
	fName = name ? strdup(name) : nullptr;
}

const char *BHandler::Name() const
{
	return fName;
}

void BHandler::SetNextHandler(BHandler *handler)
{
	if (!fLooper) {
		debugger("handler must belong to looper before setting NextHandler");
		return;
	}

	if (!fLooper->IsLocked()) {
		debugger("The handler's looper must be locked before setting NextHandler");
		return;
	}

	if (handler && fLooper != handler->Looper()) {
		debugger("The handler and its NextHandler must have the same looper");
		return;
	}

	// avoid a cycle
	if (handler == this) return;

	fNextHandler = handler;
}

BHandler *BHandler::NextHandler() const
{
	return fNextHandler;
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

bool BHandler::LockLooper()
{
	BLooper *looper = fLooper;
	// Locking the looper also makes sure that the looper is valid
	if (looper != NULL && looper->Lock()) {
		// Have we locked the right looper? That's as far as the
		// "pseudo-atomic" operation mentioned in the BeBook.
		if (fLooper == looper)
			return true;

		// we locked the wrong looper, bail out
		looper->Unlock();
	}

	return false;
}

status_t BHandler::LockLooperWithTimeout(bigtime_t timeout)
{
	BLooper *looper = fLooper;
	if (looper == NULL)
		return B_BAD_VALUE;

	status_t status = looper->LockWithTimeout(timeout);
	if (status != B_OK)
		return status;

	if (fLooper != looper) {
		// we locked the wrong looper, bail out
		looper->Unlock();
		return B_MISMATCHED_VALUES;
	}

	return B_OK;
}

void BHandler::UnlockLooper()
{
	fLooper->Unlock();
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
