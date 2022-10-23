#include "Looper.h"

#include <AppDefs.h>
#include <Message.h>
#include <MessageQueue.h>
#include <OS.h>
#include <doctest/doctest.h>

#include <cstdio>
#include <map>

static std::map<thread_id, BLooper *> g_Loopers;
static std::mutex					  g_LoopersMutex;

BLooper::BLooper(const char *name, int32 priority, int32 _port_capacity)
	: fQueue{new BMessageQueue()},
	  fLastMessage{nullptr},
	  fLockSem(create_sem(1, "BLooper Lock")),
	  fOwnerCount(0),
	  fOwner{B_ERROR},
	  fThread{B_ERROR},
	  fInitPriority{priority},
	  fTerminating{false},
	  fRunCalled{false}
{
	Lock();

	std::lock_guard<std::mutex> guard(g_LoopersMutex);
	g_Loopers[find_thread(NULL)] = this;
	AddHandler(this);
}

BLooper::~BLooper()
{
	if (fRunCalled && !fTerminating) {
		debugger("You can't call delete on a BLooper object once it is running.");
	}

	Lock();

	// In case the looper thread calls Quit() fLastMessage is not deleted.
	if (fLastMessage) {
		delete fLastMessage;
		fLastMessage = nullptr;
	}

	delete fQueue;

	RemoveHandler(this);

	// Remove all the "child" handlers
	int32 count = fHandlers.CountItems();
	for (int32 i = 0; i < count; i++) {
		BHandler *handler = (BHandler *)fHandlers.ItemAt(i);
		handler->SetNextHandler(NULL);
		handler->SetLooper(NULL);
	}
	fHandlers.MakeEmpty();

	std::lock_guard<std::mutex> guard(g_LoopersMutex);
	g_Loopers.erase(find_thread(NULL));

	Unlock();

	delete_sem(fLockSem);
};

status_t BLooper::Archive(BMessage *data, bool deep) const
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

status_t BLooper::PostMessage(uint32 command)
{
	fQueue->AddMessage(new BMessage(command));
	return B_OK;
}

status_t BLooper::PostMessage(BMessage *message)
{
	fQueue->AddMessage(new BMessage(*message));
	return B_OK;
}

status_t BLooper::PostMessage(uint32 command, BHandler *handler, BHandler *reply_to)
{
	return B_ERROR;
}

status_t BLooper::PostMessage(BMessage *message, BHandler *handler, BHandler *reply_to)
{
	return B_ERROR;
}

void BLooper::DispatchMessage(BMessage *message, BHandler *target)
{
	if (!message || !target) return;

	if (message->what == B_QUIT_REQUESTED && target == this) {
		this->fTerminating |= this->QuitRequested();
	}
	else {
		target->MessageReceived(message);
	}
}

void BLooper::MessageReceived(BMessage *msg)
{
	return BHandler::MessageReceived(msg);
}

BMessageQueue *BLooper::MessageQueue() const
{
	return fQueue;
}

void BLooper::AddHandler(BHandler *handler)
{
	if (!handler) return;

	AssertLocked();

	if (!handler->Looper()) {
		fHandlers.AddItem(handler);
		handler->SetLooper(this);
		handler->SetNextHandler(this);
	}
}

bool BLooper::RemoveHandler(BHandler *handler)
{
	if (!handler) return false;

	AssertLocked();

	if (handler->Looper() == this && fHandlers.RemoveItem(handler)) {
		if (handler == fPreferred) {
			fPreferred = nullptr;
		}
		handler->SetNextHandler(nullptr);
		handler->SetLooper(nullptr);

		return true;
	}

	return false;
}

inline int32 BLooper::CountHandlers() const
{
	AssertLocked();

	return fHandlers.CountItems();
}

inline BHandler *BLooper::HandlerAt(int32 index) const
{
	AssertLocked();

	return static_cast<BLooper *>(fHandlers.ItemAt(index));
}

inline int32 BLooper::IndexOf(BHandler *handler) const
{
	AssertLocked();

	return fHandlers.IndexOf(handler);
}

inline BHandler *BLooper::PreferredHandler() const
{
	return fPreferred;
}

void BLooper::SetPreferredHandler(BHandler *handler)
{
	if (handler && handler->Looper() == this && IndexOf(handler) >= 0) {
		fPreferred = handler;
	}
	else {
		fPreferred = nullptr;
	}
}

thread_id BLooper::Run()
{
	AssertLocked();

	if (fRunCalled) {
		// Not allowed to call Run() more than once
		debugger("can't call BLooper::Run twice!");
		return fThread;
	}

	fThread = spawn_thread(_task0_, Name(), fInitPriority, this);
	if (fThread < B_OK)
		return fThread;

	fRunCalled = true;
	Unlock();

	status_t err = resume_thread(fThread);
	if (err < B_OK)
		return err;

	return fThread;
}

void BLooper::Quit()
{
	if (!IsLocked()) {
		dprintf(2, "Quit called on not locked Looper: \"%s\" %p, team=%d", Name(), this, Team());
		return;
	}

	// Try to lock
	if (!Lock()) {
		// We're toast already
		return;
	}

	if (!fRunCalled) {
		MESSAGE("Run() has not been called yet");
		fTerminating = true;
		delete this;
		return;
	}

	auto current_thread = find_thread(NULL);
	if (fThread != current_thread) {
		fTerminating = true;
		if (fThread >= 0) {
			MESSAGE("fTerminating. Waiting for fThread ", fThread);
			// Unlock fully to release task_looper to process messages and shutdown
			while (IsLocked()) Unlock();

			status_t exit_code;
			wait_for_thread(fThread, &exit_code);
			// delete is done by Looper _task0_
		}
		else {
			delete this;
		}
	}
	else {
		// When Quit() is called from the BLooper's thread, the message loop is immediately stopped
		// and any messages in the message queue are deleted (without being processed).
		delete this;
		// Note that, in this case, Quit() doesn't return since the calling thread is dead.
		exit_thread(B_QUIT_REQUESTED);
	}
}

bool BLooper::QuitRequested()
{
	return fTerminating;
}

inline bool BLooper::Lock()
{
	return LockWithTimeout(B_INFINITE_TIMEOUT) == B_OK;
}

void BLooper::Unlock()
{
	thread_id current_thread = find_thread(NULL);

	std::lock_guard<std::mutex> guard(fLockMutex);

	if (fOwner == current_thread) {
		fOwnerCount -= 1;
		if (fOwnerCount == 0) {
			fOwner = B_ERROR;
			release_sem(fLockSem);
		}
	}
}

bool BLooper::IsLocked() const
{
	return fOwnerCount > 0;
}

status_t BLooper::LockWithTimeout(bigtime_t timeout)
{
	thread_id current_thread = find_thread(NULL);
	// MESSAGE("Try to lock by ", current_thread, ", Current ", fOwner);

	fLockMutex.lock();

	if (fOwner == current_thread) {
		fOwnerCount += 1;
		fLockMutex.unlock();

		// check if current_thread still owns the looper
		if (fOwner != current_thread) return B_INTERRUPTED;

		return B_OK;
	}

	fLockMutex.unlock();
	status_t ack = timeout == B_INFINITE_TIMEOUT
					   ? acquire_sem(fLockSem)
					   : acquire_sem_etc(fLockSem, 1, B_RELATIVE_TIMEOUT, timeout);
	if (ack != B_NO_ERROR) return ack;

	fOwner		= current_thread;
	fOwnerCount = 1;

	// MESSAGE("Locked by ", current_thread, ", Current ", fOwner, " count ", fOwnerCount);
	return B_NO_ERROR;
}

thread_id BLooper::Thread() const
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

team_id BLooper::Team() const
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

BLooper *BLooper::LooperForThread(thread_id thread)
{
	return g_Loopers[thread];
}

thread_id BLooper::LockingThread() const
{
	return fOwner;
}

int32 BLooper::CountLocks() const
{
	return fOwnerCount;
}

int32 BLooper::CountLockRequests() const
{
	int32 thread_count = 0;
	get_sem_count(fLockSem, &thread_count);
	return fOwnerCount + thread_count;
}

sem_id BLooper::Sem() const
{
	return fLockSem;
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

bool BLooper::AssertLocked() const
{
	if (!IsLocked()) {
		debugger("looper must be locked before proceeding");
		return false;
	}

	return true;
}

status_t BLooper::_task0_(void *arg)
{
	BLooper *looper = (BLooper *)arg;

	MESSAGE("LOOPER: _task0_()");

	if (looper->Lock()) {
		MESSAGE("LOOPER: looper locked");
		looper->task_looper();

		delete looper;
	}

	MESSAGE("LOOPER: _task0_() done: thread ", find_thread(NULL));
	return B_OK;
}

void BLooper::task_looper()
{
	MESSAGE("BLooper::task_looper()");
	// Check that looper is locked (should be)
	AssertLocked();
	// Unlock the looper
	Unlock();

	if (IsLocked())
		debugger("looper must not be locked!");

	// loop: As long as we are not terminating.
	while (!fTerminating) {
		MESSAGE("LOOPER: outer loop ", fTerminating);

		// loop: As long as there are messages in the queue and the port is
		//               empty... and we are not terminating, of course.
		bool dispatchNextMessage = true;
		while (!fTerminating && dispatchNextMessage) {
			MESSAGE("LOOPER: inner loop ", fTerminating);
			// Get next message from queue (assign to fLastMessage after
			// locking)
			BMessage *message = fQueue->NextMessage();

			Lock();

			fLastMessage = message;

			if (!fLastMessage) {
				// No more messages: Unlock the looper and terminate the
				// dispatch loop.
				dispatchNextMessage = false;
			}
			else {
				MESSAGE("LOOPER: fLastMessage: 0x%x: %.4s\n", fLastMessage->what,
						(char *)&fLastMessage->what);
				INFO(*fLastMessage);
			}

			if (fTerminating) {
				// we leave the looper locked when we quit
				return;
			}

			message		 = fLastMessage;
			fLastMessage = nullptr;

			// Unlock the looper
			Unlock();

			// Delete the current message (fLastMessage)
			if (message)
				delete message;
		}
	}
	MESSAGE("BLooper::task_looper() done");
}

static size_t count_running_threads()
{
	size_t		count = 0;
	thread_info info;
	int32		cookie = 0;
	while (get_next_thread_info(0, &cookie, &info) == B_OK) {
		count += 1;
	}
	return count;
}
TEST_SUITE("BLooper")
{
	// Be Book:
	//     Because they delete themselves when told to quit,
	//     BLoopers can't be allocated on the stack;
	//     you have to construct them with new.

	TEST_CASE("Locking")
	{
		CHECK(count_running_threads() == 1);
		BLooper *loop = new BLooper();
		CHECK(loop->IsLocked());
		CHECK(count_running_threads() == 1);

		loop->Run();
		CHECK_FALSE(loop->IsLocked());
		snooze(1000);
		CHECK(count_running_threads() == 2);

		loop->Lock();
		CHECK(loop->IsLocked());
		snooze(100000);
		loop->Quit();
		CHECK(count_running_threads() == 1);
	}
}
