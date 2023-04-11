#include "Application.h"

#define LOG_TAG "BApplication"

#define DEFAULT_FONT_FAMILY "Inter"
#define FIXED_FONT_FAMILY "Cascadia Mono"

#include <AppFileInfo.h>
#include <Clipboard.h>
#include <Cursor.h>
#include <File.h>
#include <Font.h>
#include <Message.h>
#include <MessageRunner.h>
#include <Mime.h>
#include <Path.h>
#include <Roster.h>
#include <binder/IPCThreadState.h>
#include <log/log.h>
#include <signal.h>

#include <algorithm>
#include <fstream>
#include <vector>

#include "AppMisc.h"
#include "DosControlLook.h"
#include "RegistrarDefs.h"

BApplication  *be_app = nullptr;
BMessenger	   be_app_messenger;
const BRoster *be_roster = nullptr;

std::vector<unsigned char>	 __cmdline;
std::vector<unsigned char *> __arg_ptr;
int							 __argc = 0;
char					   **__argv = 0;

#ifndef RUN_WITHOUT_REGISTRAR
// Fills the passed BMessage with B_ARGV_RECEIVED infos.
static void fill_argv_message(BMessage &message)
{
	message.what = B_ARGV_RECEIVED;

	int32			   argc = __argc;
	const char *const *argv = __argv;
	ALOGV("argc: %d, argv: %p", argc, argv);

	// add argc
	message.AddInt32("argc", argc);

	// add argv
	for (int32 i = 0; i < argc; i++) {
		if (argv[i])
			message.AddString("argv", argv[i]);
	}

	// add current working directory
	char cwd[B_PATH_NAME_LENGTH];
	if (getcwd(cwd, B_PATH_NAME_LENGTH))
		message.AddString("cwd", cwd);
}
#endif

BApplication::BApplication(const char *signature)
	: BLooper(signature),
	  fCursorData{nullptr},
	  fCursorHidden{false},
	  fCursorObscured{false},
	  fPulseRate{0},
	  fPulseRunner{nullptr},
	  fInitError{B_NO_INIT},
	  fReadyToRunCalled{false}
{
	if (be_app != NULL)
		debugger("BApplication already created. Only one is allowed.");

	be_app = this;

	// Application cannot be suspended
	signal(SIGTSTP, SIG_IGN);

	// check signature
	BMimeType type(signature);
	if (!type.IsValid() || type.IsSupertypeOnly() || !BMimeType("application").Contains(&type)) {
		fInitError = B_BAD_VALUE;
		ALOGE(
			"bad signature (%s), must begin with \"application/\" and "
			"can't conflict with existing registered mime types inside "
			"the \"application\" media type.",
			signature);
	}
	else {
		fInitError = B_OK;
	}

	// default delivery via looper using preferred handler
	be_app_messenger = BMessenger(nullptr, this);

	be_roster = new BRoster();

	be_clipboard = new BClipboard(nullptr);

	// BString path;
	// if (get_control_look(path) && path.Length() > 0) {
	// 	BControlLook *(*instantiate)(image_id);

	// 	sControlLookAddon = load_add_on(path.String());
	// 	if (sControlLookAddon >= 0
	// 		&& get_image_symbol(sControlLookAddon, "instantiate_control_look", B_SYMBOL_TYPE_TEXT, (void **)&instantiate) == B_OK) {
	// 		be_control_look = instantiate(sControlLookAddon);
	// 		if (!be_control_look) {
	// 			unload_add_on(sControlLookAddon);
	// 			sControlLookAddon = -1;
	// 		}
	// 	}
	// }
	if (!be_control_look)
		be_control_look = new BPrivate::DosControlLook();

	status_t	ret;
	font_family default_family{DEFAULT_FONT_FAMILY};
	font_style	plain_style{"Semi Bold"};
	ret = const_cast<BFont *>(be_plain_font)->SetFamilyAndStyle(default_family, plain_style);
	ALOGE_IF(ret != B_OK, "Failed to initialize plain font");

	font_style bold_style{"Extra Bold"};
	ret = const_cast<BFont *>(be_bold_font)->SetFamilyAndStyle(default_family, bold_style);
	ALOGE_IF(ret != B_OK, "Failed to initialize bold font");
	const_cast<BFont *>(be_bold_font)->SetSize(be_bold_font->Size() + 1.f);

	font_family fixed_family{FIXED_FONT_FAMILY};
	font_style	fixed_style{"Regular"};
	ret = const_cast<BFont *>(be_fixed_font)->SetFamilyAndStyle(fixed_family, fixed_style);
	ALOGE_IF(ret != B_OK, "Failed to initialize fixed font");

	// Get __argc/__argv
	{
		std::ifstream reader("/proc/self/cmdline", std::ios::binary);
		__cmdline = std::vector<unsigned char>(std::istreambuf_iterator<char>(reader), {});

		__argc = std::count(__cmdline.begin(), __cmdline.end(), '\0');
		__arg_ptr.resize(__argc);

		auto cur = __cmdline.begin();
		int	 i	 = 0;
		while (i < __argc && cur != __cmdline.end()) {
			__arg_ptr[i] = &(*cur);
			i += 1;

			while (*cur != '\0' && cur != __cmdline.end()) {
				++cur;
			}
			++cur;
		}

		__argv = reinterpret_cast<char **>(__arg_ptr.data());
	}

	// get app executable ref
	entry_ref ref;
	if (fInitError == B_OK) {
		fInitError = BPrivate::get_app_ref(&ref);
		if (fInitError != B_OK) {
			ALOGE("Failed to get app ref: %s", strerror(B_TO_POSIX_ERROR(fInitError)));
		}
	}

	// get the BAppFileInfo and extract the information we need
	uint32 appFlags = B_REG_DEFAULT_APP_FLAGS;
	if (fInitError == B_OK) {
		BAppFileInfo fileInfo;
		BFile		 file(&ref, B_READ_ONLY);
		fInitError = fileInfo.SetTo(&file);
		if (fInitError == B_OK) {
			fileInfo.GetAppFlags(&appFlags);
			char appFileSignature[B_MIME_TYPE_LENGTH];
			// compare the file signature and the supplied signature
			if (fileInfo.GetSignature(appFileSignature) == B_OK
				&& strcasecmp(appFileSignature, signature) != 0) {
				ALOGE("Signature in rsrc doesn't match constructor arg. (%s, %s)",
					  signature, appFileSignature);
			}
		}
		else {
			ALOGE("Failed to get info from: BAppFileInfo: %s", strerror(B_TO_POSIX_ERROR(fInitError)));
		}
	}

#ifndef RUN_WITHOUT_REGISTRAR
	// check whether be_roster is valid
	if (fInitError == B_OK && !be_roster) {
		ALOGE("FATAL: be_roster is not valid. Is the registrar running?");
		fInitError = B_NO_INIT;
	}

	if (fInitError == B_OK) {
		// not pre-registered -- try to register the application
		team_id otherTeam = -1;
		fInitError		  = be_roster->_AddApplication(signature, &ref, 0, Team(), Thread(), -1, true);
		if (fInitError != B_OK) {
			ALOGE("Failed to add app to registry: %s", strerror(B_TO_POSIX_ERROR(fInitError)));
		}
		if (fInitError == B_ALREADY_RUNNING) {
			// An instance is already running and we asked for
			// single/exclusive launch. Send our argv to the running app.
			// Do that only, if the app is NOT B_ARGV_ONLY.
			if (otherTeam >= 0) {
				BMessenger otherApp(NULL, otherTeam);
				app_info   otherAppInfo;
				bool	   argvOnly = be_roster->GetRunningAppInfo(otherTeam,
																   &otherAppInfo)
									== B_OK
								&& (otherAppInfo.flags & B_ARGV_ONLY) != 0;

				if (__argc > 1 && !argvOnly) {
					// create an B_ARGV_RECEIVED message
					BMessage argvMessage(B_ARGV_RECEIVED);
					fill_argv_message(argvMessage);

					// replace the first argv string with the path of the
					// other application
					BPath path;
					if (path.SetTo(&otherAppInfo.ref) == B_OK)
						argvMessage.ReplaceString("argv", 0, path.Path());

					// send the message
					otherApp.SendMessage(&argvMessage);
				}
				else if (!argvOnly)
					otherApp.SendMessage(B_SILENT_RELAUNCH);
			}
		}
		else if (fInitError == B_OK) {
			// the registrations was successful
			// Create a B_ARGV_RECEIVED message and send it to ourselves.
			// Do that even, if we are B_ARGV_ONLY.
			// TODO: When BLooper::AddMessage() is done, use that instead of
			// PostMessage().

			ALOGI("BApplication successfully registered.");

			if (__argc > 1) {
				BMessage argvMessage(B_ARGV_RECEIVED);
				fill_argv_message(argvMessage);
				PostMessage(&argvMessage, this);
			}
			// send a B_READY_TO_RUN message as well
			fInitError = PostMessage(B_READY_TO_RUN, this);
		}
		else if (fInitError > B_ERRORS_END) {
			// Registrar internal errors shouldn't fall into the user's hands.
			fInitError = B_ERROR;
		}
	}
#else
	// We need to have ReadyToRun called even when we're not using the registrar
	PostMessage(B_READY_TO_RUN, this);
#endif	// ifndef RUN_WITHOUT_REGISTRAR
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

#ifndef RUN_WITHOUT_REGISTRAR
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
	// hook function
}

void BApplication::ReadyToRun()
{
	// hook function
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
	// hook function
}

void BApplication::AppActivated(bool active)
{
	// hook function
}

void BApplication::RefsReceived(BMessage *a_message)
{
	// hook function
}

void BApplication::AboutRequested()
{
	// hook function
}

BHandler *BApplication::ResolveSpecifier(BMessage *msg, int32 index, BMessage *specifier, int32 form, const char *property)
{
	debugger(__PRETTY_FUNCTION__);
	return nullptr;
}

void BApplication::ShowCursor()
{
	fCursorHidden = false;
}

void BApplication::HideCursor()
{
	fCursorHidden = true;
}

void BApplication::ObscureCursor()
{
	fCursorObscured = true;
}

bool BApplication::IsCursorHidden() const
{
	return fCursorHidden;
}

void BApplication::SetCursor(const void *cursorData)
{
	BCursor cursor(cursorData);
	SetCursor(&cursor, false);
}

void BApplication::SetCursor(const BCursor *cursor, bool sync)
{
}

bool BApplication::IsLaunching() const
{
	return !fReadyToRunCalled;
}

void BApplication::DispatchMessage(BMessage *message, BHandler *handler)
{
	ALOGV("DispatchMessage 0x%x: %.4s -> %s", message->what, (char *)&message->what, handler ? handler->Name() : nullptr);
	if (handler != this) {
		// it's not ours to dispatch
		BLooper::DispatchMessage(message, handler);
		return;
	}

	switch (message->what) {
		case B_ABOUT_REQUESTED:
			AboutRequested();
			break;

		case B_APP_ACTIVATED: {
			bool active;
			if (message->FindBool("active", &active) == B_OK)
				AppActivated(active);
			break;
		} break;

		case B_ARGV_RECEIVED: {
			// build the argv vector
			status_t error = B_OK;
			int32	 argc  = 0;
			char   **argv  = nullptr;
			if (message->FindInt32("argc", &argc) == B_OK && argc > 0) {
				// allocate a NULL terminated array
				argv = new (std::nothrow) char *[argc + 1];
				if (argv == NULL)
					return;

				// copy the arguments
				for (int32 i = 0; error == B_OK && i < argc; i++) {
					const char *arg = nullptr;
					error			= message->FindString("argv", i, &arg);
					if (error == B_OK && arg) {
						argv[i] = strdup(arg);
						if (argv[i] == NULL)
							error = B_NO_MEMORY;
					}
					else
						argc = i;
				}

				argv[argc] = nullptr;
			}

			// call the hook
			if (error == B_OK && argc > 0)
				ArgvReceived(argc, argv);

			if (error != B_OK) {
				ALOGE("Error parsing B_ARGV_RECEIVED message. Message:\n");
				message->PrintToStream();
			}

			// cleanup
			if (argv) {
				for (int32 i = 0; i < argc; i++)
					free(argv[i]);
				delete[] argv;
			}
		} break;

		case B_PULSE:
			Pulse();
			break;

		case B_READY_TO_RUN:
			if (!fReadyToRunCalled) {
				ReadyToRun();
				fReadyToRunCalled = true;
			}
			break;

		case B_REFS_RECEIVED: {
			// this adds the refs that are part of this message to the recent
			// lists, but only folders and documents are handled here
			debugger("BApplication B_REFS_RECEIVED");

			RefsReceived(message);
		} break;

		default:
			BLooper::DispatchMessage(message, handler);
			break;
	}
}

void BApplication::SetPulseRate(bigtime_t rate)
{
	if (rate < 0)
		rate = 0;

	if (!Lock())
		return;

	if (rate != 0) {
		// reset existing pulse runner, or create new one
		if (!fPulseRunner) {
			BMessage pulse(B_PULSE);
			fPulseRunner = new BMessageRunner(be_app_messenger, &pulse, rate);
		}
		else
			fPulseRunner->SetInterval(rate);
	}
	else {
		// turn off pulse messages
		delete fPulseRunner;
		fPulseRunner = nullptr;
	}

	fPulseRate = rate;
	Unlock();
}

status_t BApplication::GetSupportedSuites(BMessage *data)
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}
