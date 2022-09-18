#ifndef _APPLICATION_H
#define _APPLICATION_H

#include <Looper.h>

class BMessenger;

class BApplication : public BLooper
{
   public:
	BApplication(const char *signature);
	BApplication(const char *signature, status_t *error);
	virtual ~BApplication();

	/// Archiving
	// BApplication(BMessage *data);
	// static BArchivable *Instantiate(BMessage *data);
	// virtual status_t	Archive(BMessage *data, bool deep = true) const;

	// status_t InitCheck() const;

	/// App control and System Message handling
	virtual thread_id Run();
	// virtual void	  Quit();
	// virtual bool	  QuitRequested();
	// virtual void	  Pulse();
	// virtual void	  ReadyToRun();
	// virtual void	  MessageReceived(BMessage *msg);
	// virtual void	  ArgvReceived(int32 argc, char **argv);
	// virtual void	  AppActivated(bool active);
	// virtual void	  RefsReceived(BMessage *a_message);
	// virtual void	  AboutRequested();

	// virtual void DispatchMessage(BMessage *an_event,
	// 							 BHandler *handler);
};

extern BApplication *be_app;
extern BMessenger	 be_app_messenger;

#endif /* _APPLICATION_H */
