#ifndef _DOC_APPLICATION_H
#define _DOC_APPLICATION_H

#include <Application.h>
#include <Entry.h>
#include <Window.h>
#include <List.h>
#include <String.h>

#include <experimental/WindowRoster.h>

class BFilePanel;

namespace BExperimental {

class DocWindow;

typedef DocWindow *(*DocWindowFactory)(WindowRoster *wr, entry_ref *ref,
		const char *title, window_look look, window_feel feel,
		uint32 flags, uint32 workspace);

class DocApplication : public BApplication, public WindowRoster
{
public:
					DocApplication(const char *signature, DocWindowFactory factory);

	virtual bool	QuitRequested();
	virtual void	MessageReceived(BMessage *msg);
	virtual void	ReadyToRun();
	virtual void	RefsReceived(BMessage *msg);
	virtual void	ArgvReceived(int32 argc, char **argv);
	virtual void	FileOpen();
	virtual void	New();
	virtual void	WindowFrame(BRect *proposed);

private:
	typedef BApplication inherited;
	
	// WindowRoster implementation
	struct	window_info
	{
		entry_ref	*ref;
		node_ref	nref;
		DocWindow	*w;
		BMenu		*windowmenu;
		int32		number;
	};
	BList				window_list;
	DocWindowFactory	factory;
	BRect				lastframe;
	int32				untitledcount;
	BFilePanel			*openpanel;
	BString				fSignature;
	
	// WindowRoster methods
	void	AddWindow(DocWindow *w, entry_ref *ref);
	void	ChangeWindow(DocWindow *w, entry_ref *ref);
	void	RemoveWindow(DocWindow *w);
	BMenu	*WindowMenu(DocWindow *w);
	void	UpdateWindowMenu(DocWindow *w);
	void	LoadRefs(BMessage *message);

};

#define DOC_APP_NEW_WINDOW		'Dnew'
#define DOC_APP_OPEN			'Dopn'
#define DOC_APP_ACTIVATE_WINDOW	'Dact'

}	// namespace BExperimental

using namespace BExperimental;

#endif
