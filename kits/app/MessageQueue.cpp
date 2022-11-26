#include "MessageQueue.h"

#include <Autolock.h>
#include <Message.h>
#include <doctest/doctest.h>

#include <list>
#include <system_error>

class BMessageQueue::impl : public std::list<BMessage *>
{
};

BMessageQueue::BMessageQueue() : _impl(new impl{}),
								 locker("BMessageQueue Lock")
{
}

BMessageQueue::~BMessageQueue()
{
	if (!Lock()) return;  // someone else just deleted us

	for (auto message : *_impl) {
		delete message;
	}
}

void BMessageQueue::AddMessage(BMessage *an_event)
{
	if (an_event == nullptr) return;

	BAutolock _(locker);
	_impl->push_back(an_event);
}

void BMessageQueue::RemoveMessage(BMessage *an_event)
{
	if (an_event == nullptr) return;

	BAutolock _(locker);
	_impl->remove(an_event);

	// delete an_event;
	// NOTE: BeOS documentation states:
	// > RemoveMessage() removes a particular message from the queue and deletes it.
	// but Haiku implementation does not do it and its documentation states:
	// > you regain ownership of the message.
	// For the sake of compatibility with existing Haiku code
	// we assume BeOS doc is incorrect, until proven otherwise.
}

BMessage *BMessageQueue::NextMessage()
{
	if (IsEmpty()) return nullptr;

	BAutolock _(locker);
	BMessage *next = _impl->front();
	_impl->pop_front();
	return next;
}

BMessage *BMessageQueue::FindMessage(int32 index) const
{
	if (IsEmpty()) return nullptr;

	BAutolock _(locker);
	for (auto message : *_impl) {
		if (index == 0) return message;
		--index;
	}
	return nullptr;
}

BMessage *BMessageQueue::FindMessage(uint32 what, int32 index) const
{
	if (IsEmpty()) return nullptr;

	BAutolock _(locker);
	for (auto message : *_impl) {
		if (message->what == what) {
			if (index == 0) return message;
			--index;
		}
	}
	return nullptr;
}

int32 BMessageQueue::CountMessages() const
{
	return _impl->size();
}

bool BMessageQueue::IsEmpty() const
{
	return _impl->empty();
}

inline bool BMessageQueue::Lock()
{
	return locker.Lock();
	return true;
}

inline void BMessageQueue::Unlock()
{
	return locker.Unlock();
}

TEST_SUITE("BMessageQueue")
{
	TEST_CASE("Add/Remove/Find")
	{
		BMessageQueue queue;

		CHECK(queue.IsEmpty());
		CHECK(queue.CountMessages() == 0);
		CHECK(queue.FindMessage((int32)0) == nullptr);

		auto message = new BMessage(123);
		queue.AddMessage(message);
		CHECK_FALSE(queue.IsEmpty());
		CHECK(queue.CountMessages() == 1);
		queue.AddMessage(message);
		CHECK(queue.CountMessages() == 2);

		CHECK(queue.FindMessage((int32)0) != nullptr);
		CHECK(queue.FindMessage((uint32)111) == nullptr);

		auto msg_in_queue = queue.FindMessage(123, 0);
		CHECK(msg_in_queue != nullptr);
		CHECK(msg_in_queue == message);
		CHECK(queue.FindMessage(123, 1) != nullptr);
		CHECK(queue.FindMessage(123, 2) == nullptr);

		auto message2 = new BMessage(111);
		queue.AddMessage(message2);
		CHECK(queue.CountMessages() == 3);
		queue.RemoveMessage(message);
		CHECK(queue.CountMessages() == 1);
		CHECK(queue.FindMessage(111, 0) != nullptr);
		CHECK(queue.FindMessage(111, 1) == nullptr);

		CHECK(queue.NextMessage() != nullptr);
		CHECK(queue.IsEmpty());
	}
}
