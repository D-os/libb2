/******************************************************************************
/
/	File:			Application.h
/
/	Description:	BApplication class is the center of the application
/					universe.  The global be_app and be_app_messenger 
/					variables are defined here as well.
/
/	Copyright 1995-98, Be Incorporated, All Rights Reserved.
/
*******************************************************************************/

#ifndef _APPLICATION_H
#define _APPLICATION_H

#include <BeBuild.h>
#include <AppDefs.h>		/* For convenience */
#include <InterfaceDefs.h>
#include <Rect.h>
#include <Point.h>
#include <Looper.h>
#include <Messenger.h>

class BCursor;
class BList;
class BWindow;
class _BSession_;
class BResources;
class BMessageRunner;
struct _server_heap_;
struct _drag_data_;

/*----- BApplication class --------------------------------------------*/

class BApplication : public BLooper {

public:
						BApplication(const char *signature);
						BApplication(const char *signature, status_t *error);
virtual					~BApplication();

/* Archiving */
						BApplication(BMessage *data);
static	BArchivable		*Instantiate(BMessage *data);
virtual	status_t		Archive(BMessage *data, bool deep = true) const;

		status_t		InitCheck() const;

/* App control and System Message handling */
virtual	thread_id		Run();
virtual	void			Quit();
virtual bool			QuitRequested();
virtual	void			Pulse();
virtual	void			ReadyToRun();
virtual	void			MessageReceived(BMessage *msg);
virtual	void			ArgvReceived(int32 argc, char **argv);
virtual	void			AppActivated(bool active);
virtual	void			RefsReceived(BMessage *a_message);
virtual	void			AboutRequested();

/* Scripting */
virtual BHandler		*ResolveSpecifier(BMessage *msg,
										int32 index,
										BMessage *specifier,
										int32 form,
										const char *property);

/* Cursor control, window/looper list, and app info */
		void			ShowCursor();
		void			HideCursor();
		void			ObscureCursor();
		bool			IsCursorHidden() const;
		void			SetCursor(const void *cursor);
		void			SetCursor(const BCursor *cursor, bool sync=true);
		int32			CountWindows() const;
		BWindow			*WindowAt(int32 index) const;
		int32			CountLoopers() const;
		BLooper			*LooperAt(int32 index) const;
		bool			IsLaunching() const;
		status_t		GetAppInfo(app_info *info) const;
static		BResources		*AppResources();

virtual	void			DispatchMessage(BMessage *an_event,
										BHandler *handler);
		void			SetPulseRate(bigtime_t rate);

/* More scripting  */
virtual status_t		GetSupportedSuites(BMessage *data);


/*----- Private or reserved -----------------------------------------*/
virtual status_t		Perform(perform_code d, void *arg);

private:

typedef BLooper _inherited;

friend class BWindow;
friend class BView;
friend class BBitmap;
friend class BScrollBar;
friend class BPrivateScreen;
friend class _BAppServerLink_;
friend void _toggle_handles_(bool);
						
						BApplication(uint32 signature);
						BApplication(const BApplication &);
		BApplication	&operator=(const BApplication &);

virtual	void			_ReservedApplication1();
virtual	void			_ReservedApplication2();
virtual	void			_ReservedApplication3();
virtual	void			_ReservedApplication4();
virtual	void			_ReservedApplication5();
virtual	void			_ReservedApplication6();
virtual	void			_ReservedApplication7();
virtual	void			_ReservedApplication8();

virtual	bool			ScriptReceived(BMessage *msg,
										int32 index,
										BMessage *specifier,
										int32 form,
										const char *property);
		void			run_task();
		void			InitData(const char *signature, status_t *error);
		void			BeginRectTracking(BRect r, bool trackWhole);
		void			EndRectTracking();
		void			get_scs();
		void			setup_server_heaps();
		void *			rw_offs_to_ptr(uint32 offset);
		void *			ro_offs_to_ptr(uint32 offset);
		void *			global_ro_offs_to_ptr(uint32 offset);
		void			connect_to_app_server();
		void			send_drag(	BMessage *msg,
									int32 vs_token,
									BPoint offset,
									BRect drag_rect,
									BHandler *reply_to);
		void			send_drag(	BMessage *msg,
									int32 vs_token,
									BPoint offset,
									int32 bitmap_token,
									drawing_mode dragMode,
									BHandler *reply_to);
		void			write_drag(_BSession_ *session, BMessage *a_message);
		bool			quit_all_windows(bool force);
		bool			window_quit_loop(bool, bool);
		void			do_argv(BMessage *msg);
#ifndef FIX_FOR_4_6
		void			SetAppCursor();
#endif
		uint32			InitialWorkspace();
		int32			count_windows(bool incl_menus) const;
		BWindow			*window_at(uint32 index, bool incl_menus) const;
		status_t		get_window_list(BList *list, bool incl_menus) const;
static	int32			async_quit_entry(void *);
static	BResources		* _app_resources;
static	BLocker			_app_resources_lock;

		const char		*fAppName;
		int32			fServerFrom;
		int32			fServerTo;
#ifndef FIX_FOR_4_6
		void			*fCursorData;
#else
		void			*_unused1;
#endif
		_server_heap_ *	fServerHeap;
		bigtime_t		fPulseRate;
		uint32			fInitialWorkspace;
		_drag_data_	*	fDraggedMessage;
		BMessageRunner	*fPulseRunner;
		status_t		fInitError;
		uint32			_reserved[11];

		bool			fReadyToRunCalled;
};

/*----- Global Objects -----------------------------------------*/

extern _IMPEXP_BE BApplication	*be_app;
extern _IMPEXP_BE BMessenger	be_app_messenger;

/*-------------------------------------------------------------*/

#endif /* _APPLICATION_H */
