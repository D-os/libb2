#include "Looper.h"

#include <AppDefs.h>
#include <Message.h>
#include <MessageQueue.h>
#include <OS.h>
#include <doctest/doctest.h>

#include <cstdio>
#include <map>

#define LOG_TAG "Looper"
#include <log/log.h>

static std::map<thread_id, BLooper *> g_Loopers;
static std::mutex					  g_LoopersMutex;

BLooper::BLooper(const char *name, int32 priority, int32 _port_capacity)
	: fQueue{new BMessageQueue()},
	  fLastMessage{nullptr},
	  fLockSem(create_sem(1, "BLooper Lock")),
	  fOwnerCount{0},
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
	return _PostMessage(new BMessage(command), this, nullptr);
}

status_t BLooper::PostMessage(BMessage *message)
{
	return _PostMessage(new BMessage(*message), this, nullptr);
}

status_t BLooper::PostMessage(uint32 command, BHandler *handler, BHandler *reply_to)
{
	return _PostMessage(new BMessage(command), handler, reply_to);
}

status_t BLooper::PostMessage(BMessage *message, BHandler *handler, BHandler *reply_to)
{
	return _PostMessage(new BMessage(*message), handler, reply_to);
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
	ALOGD("MessageReceived: %.4s", (char *)&(msg->what));
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
		ALOGD("Run() has not been called yet");
		fTerminating = true;
		delete this;
		return;
	}

	auto current_thread = find_thread(NULL);
	if (fThread != current_thread) {
		fTerminating = true;
		if (fThread >= 0) {
			// Unlock fully to release task_looper to process messages and shutdown
			while (IsLocked()) Unlock();

			ALOGD("Terminating. Waiting for thread: %d", fThread);

			status_t status_code = B_NO_ERROR;

			if (!has_data(fThread)) {
				// wake up the thread to process termination request
				status_code = send_data(fThread, 'QUIT', NULL, 0);
			}

			if (status_code != B_BAD_THREAD_ID) {
				wait_for_thread(fThread, &status_code);
			}
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
	return true;
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
	// ALOGD("Try to lock by ", current_thread, ", Current ", fOwner);

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

	// ALOGD("Locked by ", current_thread, ", Current ", fOwner, " count ", fOwnerCount);
	return B_NO_ERROR;
}

thread_id BLooper::Thread() const
{
	return fThread;
}

team_id BLooper::Team() const
{
	thread_id	tid = fThread >= 0 ? fThread : find_thread(NULL);
	thread_info tinfo;
	status_t	ret = _get_thread_info(tid, &tinfo, sizeof(tinfo));
	return ret == B_OK ? tinfo.team : ret;
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
		debugger("Looper must be locked before proceeding");
		return false;
	}

	return true;
}

status_t BLooper::_PostMessage(BMessage *msg, BHandler *handler, BHandler *reply_to)
{
	if ((handler && handler != this) || reply_to) {
		// FIXME: add handler and reply support
		debugger(__PRETTY_FUNCTION__);
	}

	fQueue->AddMessage(msg);

	if (fThread != B_ERROR && !has_data(fThread))
		return send_data(fThread, '_MSG', nullptr, 0);

	return B_OK;
}

status_t BLooper::_task0_(void *arg)
{
	BLooper *looper = (BLooper *)arg;

	ALOGD("_task0_()");

	if (looper->Lock()) {
		ALOGV("_task0_() looper locked");
		looper->task_looper();

		delete looper;
	}

	ALOGD("_task0_() done: thread %d", find_thread(NULL));
	return B_OK;
}

void BLooper::task_looper()
{
	ALOGD("BLooper::task_looper()");
	// Check that looper is locked (should be)
	AssertLocked();
	// Unlock the looper
	Unlock();

	if (IsLocked())
		debugger("task_looper() cannot unlock Looper");

	// loop: As long as we are not terminating.
	while (!fTerminating) {
		ALOGV("outer loop");

		if (fQueue->IsEmpty()) {
			ALOGV("waiting for data");
			thread_id sender;
			uint32	  code = receive_data(&sender, NULL, 0);
			ALOGD("received data from %d: %.4s", sender, (char *)&code);
		}

		// loop: As long as there are messages in the queue and the port is
		//               empty... and we are not terminating, of course.
		bool dispatchNextMessage = true;
		while (!fTerminating && dispatchNextMessage) {
			ALOGV("inner loop");
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
				ALOGV("fLastMessage: 0x%x: %.4s", fLastMessage->what,
					  (char *)&fLastMessage->what);
				INFO(*fLastMessage);

				// Get the target handler
				BHandler *handler = nullptr;
				// BMessage::Private messagePrivate(fLastMessage);
				bool usePreferred = true;  // messagePrivate.UsePreferredTarget();

				if (usePreferred) {
					ALOGV("use preferred target");
					handler = fPreferred;
					if (handler == nullptr)
						handler = this;
				}
				else {
					// if this handler doesn't belong to us, we drop the message
					if (handler != nullptr && handler->Looper() != this)
						handler = nullptr;
				}

				// // Is this a scripting message? (BMessage::HasSpecifiers())
				// if (handler != nullptr && fLastMessage->HasSpecifiers()) {
				// 	int32 index = 0;
				// 	// Make sure the current specifier is kosher
				// 	if (fLastMessage->GetCurrentSpecifier(&index) == B_OK)
				// 		handler = resolve_specifier(handler, fLastMessage);
				// }

				if (handler) {
					// Do filtering
					// handler = _TopLevelFilter(fLastMessage, handler);
					// ALOGV("_TopLevelFilter(): %p", handler);
					if (handler && handler->Looper() == this)
						DispatchMessage(fLastMessage, handler);
				}
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
	ALOGD("BLooper::task_looper() done");
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

	TEST_CASE("Messaging")
	{
		CHECK(count_running_threads() == 1);
		BLooper *loop = new BLooper();
		loop->Run();
		snooze(1000);
		CHECK(count_running_threads() == 2);

		loop->PostMessage(B_QUIT_REQUESTED);
		snooze(1000);
		CHECK(count_running_threads() == 1);
	}
}
