#include "Messenger.h"

#define LOG_TAG "BMessenger"

#include <Application.h>
#include <Handler.h>
#include <Looper.h>
#include <Message.h>
#include <OS.h>
#include <log/log.h>

BMessenger::BMessenger()
	: fHandler{nullptr}, fLooper{nullptr} {}

BMessenger::BMessenger(const BHandler *handler, const BLooper *looper, status_t *err)
	: fStatus{B_NO_INIT}, fHandler{handler}, fLooper{looper}, fTeam{-1}
{
	if (!handler && !looper) {
		fStatus = B_BAD_VALUE;
	}

	fStatus = B_OK;

	if (handler) {
		if (looper && handler->Looper() != looper) {
			fStatus = B_MISMATCHED_VALUES;
		}
	}

	if (err) *err = fStatus;
}

BMessenger::BMessenger(const BMessenger &from) = default;

BMessenger::BMessenger(BMessenger &&from) = default;

BMessenger::~BMessenger() = default;

BMessenger &BMessenger::operator=(const BMessenger &from) = default;

bool BMessenger::operator==(const BMessenger &other) const = default;

bool BMessenger::IsValid() const
{
	return fStatus == B_OK;
}

bool BMessenger::IsTargetLocal() const
{
	return fTeam < 0;
}

BHandler *BMessenger::Target(BLooper **looper) const
{
	if (!IsValid() || !IsTargetLocal()) {
		if (looper) *looper = nullptr;
		return nullptr;
	}

	if (looper) *looper = const_cast<BLooper *>(fLooper);
	return const_cast<BHandler *>(fHandler);
}

bool BMessenger::LockTarget() const
{
	if (IsValid() && IsTargetLocal()) {
		BLooper *looper = const_cast<BLooper *>(fLooper);
		if (!looper) looper = fHandler->Looper();
		return looper && looper->Lock();
	}

	return false;
}

status_t BMessenger::LockTargetWithTimeout(bigtime_t timeout) const
{
	if (IsValid() && IsTargetLocal()) {
		BLooper *looper = const_cast<BLooper *>(fLooper);
		if (!looper) looper = fHandler->Looper();
		return looper ? looper->LockWithTimeout(timeout) : B_BAD_VALUE;
	}

	return B_BAD_VALUE;
}

status_t BMessenger::SendMessage(uint32 command, BHandler *reply_to) const
{
	BMessage message(command);
	return SendMessage(&message, reply_to);
}

status_t BMessenger::SendMessage(BMessage *message, BHandler *reply_to, bigtime_t timeout) const
{
	status_t result = message != nullptr ? B_OK : B_BAD_VALUE;
	if (result == B_OK) {
		BMessenger replyMessenger(reply_to);
		result = SendMessage(message, replyMessenger, timeout);
	}

	return result;
}

status_t BMessenger::SendMessage(BMessage *message, BMessenger reply_to, bigtime_t timeout) const
{
	if (fStatus != B_OK)
		return fStatus;

	if (!message)
		return B_BAD_VALUE;

	if (IsTargetLocal()) {
		BLooper	*looper;
		BHandler *handler = Target(&looper);
		if (handler) {
			ALOGV("handler delivery");
			handler->MessageReceived(message);
			return B_OK;
		}
		else if (looper) {
			ALOGV("looper delivery");
			BHandler *reply_handler = reply_to.Target(nullptr);
			if (!reply_handler) reply_handler = be_app_messenger.Target(nullptr);
			ALOGV("BMessenger::SendMessage %p %p", reply_handler, looper);
			return looper->PostMessage(message, nullptr, reply_handler);
		}
		else {
			return B_BAD_PORT_ID;
		}
	}
	else {
		debugger(__PRETTY_FUNCTION__);
		return B_ERROR;
	}
}

status_t BMessenger::SendMessage(uint32 command, BMessage *reply) const
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

status_t BMessenger::SendMessage(BMessage *message, BMessage *reply, bigtime_t send_timeout, bigtime_t reply_timeout) const
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}
