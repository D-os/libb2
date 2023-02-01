#include "Invoker.h"

#include <Handler.h>
#include <Message.h>

BInvoker::BInvoker()
	: fMessage(nullptr),
	  fMessenger(),
	  fReplyTo{nullptr},
	  fTimeout{B_INFINITE_TIMEOUT},
	  fNotifyKind{0}
{
}

BInvoker::BInvoker(BMessage *message, const BHandler *handler, const BLooper *looper)
	: fMessage{message},
	  fMessenger(handler, looper),
	  fReplyTo{nullptr},
	  fTimeout{B_INFINITE_TIMEOUT},
	  fNotifyKind{0}
{
}

BInvoker::BInvoker(BMessage *message, BMessenger target)
	: fMessage{message},
	  fMessenger{target},
	  fReplyTo{nullptr},
	  fTimeout{B_INFINITE_TIMEOUT},
	  fNotifyKind{0}
{
}

BInvoker::~BInvoker()
{
	delete fMessage;
}

status_t BInvoker::SetMessage(BMessage *message)
{
	auto current_message = fMessage;

	fMessage = message;

	if (current_message) delete current_message;

	return B_OK;
}

BMessage *BInvoker::Message() const
{
	return fMessage;
}

uint32 BInvoker::Command() const
{
	return fMessage ? fMessage->what : 0;
}

status_t BInvoker::SetTarget(const BHandler *handler, const BLooper *looper)
{
	if (!handler || !handler->Looper())
		return B_BAD_VALUE;

	if (looper && handler->Looper() != looper)
		return B_MISMATCHED_VALUES;

	fMessenger = BMessenger(handler, looper);
	return B_OK;
}

status_t BInvoker::SetTarget(BMessenger messenger)
{
	fMessenger = messenger;
	return B_OK;
}

bool BInvoker::IsTargetLocal() const
{
	return fMessenger.IsTargetLocal();
}

BHandler *BInvoker::Target(BLooper **looper) const
{
	return fMessenger.Target(looper);
}

BMessenger BInvoker::Messenger() const
{
	return fMessenger;
}

status_t BInvoker::SetHandlerForReply(BHandler *handler)
{
	fReplyTo = handler;
	return B_OK;
}

BHandler *BInvoker::HandlerForReply() const
{
	return fReplyTo;
}

status_t BInvoker::Invoke(BMessage *msg)
{
	if (!msg) msg = fMessage;
	if (!msg) return B_BAD_VALUE;

	return fMessenger.SendMessage(fMessage, fReplyTo, fTimeout);
}

status_t BInvoker::InvokeNotify(BMessage *msg, uint32 kind)
{
	if (fNotifyKind != 0)
		return B_WOULD_BLOCK;

	status_t err = B_BAD_VALUE;

	BeginInvokeNotify(kind);

	if (msg)
		err = Invoke(msg);

	if (fMessenger.IsValid()) {
		BHandler *handler = fMessenger.Target(nullptr);
		if (handler && handler->IsWatched())
			handler->SendNotices(kind, msg);
	}

	EndInvokeNotify();

	return err;
}

status_t BInvoker::SetTimeout(bigtime_t timeout)
{
	fTimeout = timeout;
	return B_OK;
}

bigtime_t BInvoker::Timeout() const
{
	return fTimeout;
}

uint32 BInvoker::InvokeKind(bool *notify)
{
	if (notify)
		*notify = fNotifyKind != 0;

	if (fNotifyKind != 0)
		return fNotifyKind;

	return B_CONTROL_INVOKED;
}

void BInvoker::BeginInvokeNotify(uint32 kind)
{
	fNotifyKind = kind;
}

void BInvoker::EndInvokeNotify()
{
	fNotifyKind = 0;
}
