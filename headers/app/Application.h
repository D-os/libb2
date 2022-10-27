#ifndef _APPLICATION_H
#define _APPLICATION_H

#include <Looper.h>
#include <Messenger.h>

/// For convenience
#include <AppDefs.h>

class BCursor;
class BList;
class BWindow;
class BResources;
class BMessageRunner;
struct app_info;

class BApplication : public BLooper
{
   public:
	BApplication(const char *signature);
	BApplication(const char *signature, status_t *error);
	virtual ~BApplication();

	/// Archiving
	BApplication(BMessage *data);
	static BArchivable *Instantiate(BMessage *data);
	virtual status_t	Archive(BMessage *data, bool deep = true) const override;

	status_t InitCheck() const;

	/// App control and System Message handling
	virtual thread_id Run() override;
	virtual void	  Quit() override;
	virtual bool	  QuitRequested() override;
	virtual void	  Pulse();
	virtual void	  ReadyToRun();
	virtual void	  MessageReceived(BMessage *msg) override;
	virtual void	  ArgvReceived(int32 argc, char **argv);
	virtual void	  AppActivated(bool active);
	virtual void	  RefsReceived(BMessage *a_message);
	virtual void	  AboutRequested();

	/// Scripting
	virtual BHandler *ResolveSpecifier(BMessage	*msg,
									   int32	   index,
									   BMessage	*specifier,
									   int32	   form,
									   const char *property) override;

	/// Cursor control, window/looper list, and app info
	void			   ShowCursor();
	void			   HideCursor();
	void			   ObscureCursor();
	bool			   IsCursorHidden() const;
	void			   SetCursor(const void *cursor);
	void			   SetCursor(const BCursor *cursor, bool sync = true);
	int32			   CountWindows() const;
	BWindow			*WindowAt(int32 index) const;
	int32			   CountLoopers() const;
	BLooper			*LooperAt(int32 index) const;
	bool			   IsLaunching() const;
	status_t		   GetAppInfo(app_info *info) const;
	static BResources *AppResources();

	virtual void DispatchMessage(BMessage *an_event, BHandler *handler) override;
	void		 SetPulseRate(bigtime_t rate);

	/// More scripting
	virtual status_t GetSupportedSuites(BMessage *data) override;

   private:
	BApplication(uint32 signature);
	BApplication(const BApplication &);
	BApplication &operator=(const BApplication &);

	status_t fInitError;
	bool	 fReadyToRunCalled;
};

/// Global Objects

extern BApplication *be_app;
extern BMessenger	 be_app_messenger;

#endif /* _APPLICATION_H */
