#include "Application.h"

#define LOG_TAG "Application"

#include <Message.h>
#include <Roster.h>
#include <binder/IPCThreadState.h>
#include <log/log.h>

#include <cstdio>

BApplication *be_app = nullptr;
BMessenger	   be_app_messenger;
const BRoster *be_roster = nullptr;

BApplication::BApplication(const char *signature)
	: BLooper(signature),
	  fInitError{B_NO_INIT},
	  fReadyToRunCalled{false}
{
	if (be_app != NULL)
		debugger("BApplication already created. Only one is allowed.");

	be_app = this;
	be_app_messenger = BMessenger(nullptr, this);
	be_roster = new BRoster();

	PostMessage(B_READY_TO_RUN, this);

	fInitError = B_NO_ERROR;
}

BApplication::BApplication(const char *signature, status_t *error) : BApplication(signature)
{
	if (error) *error = fInitError;
}

BApplication::~BApplication()
{
	Lock();

	delete be_roster;
	be_roster		 = nullptr;
	be_app_messenger = BMessenger();
	be_app			 = nullptr;
}

status_t BApplication::Archive(BMessage *data, bool deep) const
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

thread_id BApplication::Run()
{
	AssertLocked();

	if (fRunCalled)
		debugger("BApplication::Run was already called. Can only be called once.");

	fThread	   = find_thread(NULL);
	fRunCalled = true;

#ifndef RUN_WITHOUT_APP_SERVER
	// start the thread pool
	android::sp<android::ProcessState> ps(android::ProcessState::self());
	ps->setThreadPoolMaxThreadCount(1);
	ps->startThreadPool();
	ps->giveThreadPoolName();
#endif

	// take over main thread
	task_looper();

	// delete fPulseRunner;
	return fThread;
}

void BApplication::Quit()
{
	bool unlock = false;
	if (!IsLocked()) {
		dprintf(2,
				"🐛 You must Lock the application object "
				"before calling Quit(), team=%d, looper=%s\n",
				Team(), Name());

		unlock = true;
		if (!Lock())
			return;
	}

	if (fRunCalled) {
		fTerminating = true;
	}
	else {
		// Quit() does delete the object if it's called before the message loop starts
		delete this;
	}

	// If we had to lock, unlock now.
	if (unlock)
		Unlock();
}

bool BApplication::QuitRequested()
{
	// TODO: Be Book:
	// BApplication sends BWindow::QuitRequested() to each of its BWindow objects.
	// If they all agree to quit, the windows are all destroyed (through BWindow::Quit())
	// and this QuitRequested() returns true. But if any BWindow refuses to quit,
	// that window and all surviving windows are saved, and this QuitRequested() returns false.

	return true;
}

void BApplication::Pulse()
{
	debugger(__PRETTY_FUNCTION__);
}

void BApplication::ReadyToRun()
{
	// The default version of ReadyToRun() is empty.
}

void BApplication::MessageReceived(BMessage *message)
{
	ALOGV("BApplication::MessageReceived 0x%x: %.4s", message->what, (char *)&message->what);
	switch (message->what) {
		default:
			BLooper::MessageReceived(message);
	}
}

void BApplication::ArgvReceived(int32 argc, char **argv)
{
	debugger(__PRETTY_FUNCTION__);
}

void BApplication::AppActivated(bool active)
{
	debugger(__PRETTY_FUNCTION__);
}

void BApplication::RefsReceived(BMessage *a_message)
{
	debugger(__PRETTY_FUNCTION__);
}

void BApplication::AboutRequested()
{
	debugger(__PRETTY_FUNCTION__);
}

BHandler *BApplication::ResolveSpecifier(BMessage *msg, int32 index, BMessage *specifier, int32 form, const char *property)
{
	debugger(__PRETTY_FUNCTION__);
	return nullptr;
}

bool BApplication::IsLaunching() const
{
	return !fReadyToRunCalled;
}

void BApplication::DispatchMessage(BMessage *message, BHandler *handler)
{
	ALOGV("BApplication::DispatchMessage %p %p", message, handler);
	if (handler != this) {
		// it's not ours to dispatch
		BLooper::DispatchMessage(message, handler);
		return;
	}

	switch (message->what) {
		case B_ARGV_RECEIVED:
			debugger("BApplication B_ARGV_RECEIVED");
			break;

		case B_REFS_RECEIVED:
			debugger("BApplication B_REFS_RECEIVED");
			break;

		case B_READY_TO_RUN:
			if (!fReadyToRunCalled) {
				ReadyToRun();
				fReadyToRunCalled = true;
			}
			break;

		case B_ABOUT_REQUESTED:
			AboutRequested();
			break;

		case B_PULSE:
			Pulse();
			break;

		case B_APP_ACTIVATED: {
			bool active;
			if (message->FindBool("active", &active) == B_OK)
				AppActivated(active);
			break;
		}

		default:
			BLooper::DispatchMessage(message, handler);
			break;
	}
}

status_t BApplication::GetSupportedSuites(BMessage *data)
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}
