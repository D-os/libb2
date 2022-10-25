#include "Messenger.h"

#include <Handler.h>

BMessenger::BMessenger()
	: fHandler{nullptr}, fLooper{nullptr} {}

BMessenger::BMessenger(const BHandler *handler, const BLooper *looper, status_t *error)
	: fHandler{handler}, fLooper{looper}
{
	if (!handler && !looper) {
		if (error) *error = B_BAD_VALUE;
		return;
	}

	if (handler) {
		if (looper && handler->Looper() != looper) {
			if (error) *error = B_MISMATCHED_VALUES;
		}

		return;
	}

	if (error) *error = B_OK;
}

BMessenger::BMessenger(const BMessenger &from) = default;

BMessenger::BMessenger(BMessenger &&from) = default;

BMessenger::~BMessenger() = default;

BMessenger &BMessenger::operator=(const BMessenger &from) = default;

bool BMessenger::operator==(const BMessenger &other) const = default;
