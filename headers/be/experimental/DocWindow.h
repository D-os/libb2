#ifndef _BITMAP_TOOLS_H
#define _BITMAP_TOOLS_H

#include <Window.h>
#include <Entry.h>

#include <experimental/WindowRoster.h>

class BFilePanel;

namespace BExperimental {

class DocWindow : public BWindow
{
	WindowRoster	*windowroster;
	void			AddMe(entry_ref *ref);
	void			RemoveMe();
	entry_ref		fileref;
	bool			dirty;
	bool			untitled;
	BFilePanel		*savepanel;
	bool			waitforsave;

public:
			DocWindow(WindowRoster *wr, entry_ref *ref, BRect frame, const char *title, window_look look, window_feel feel,
				uint32 flags, uint32 workspace = B_CURRENT_WORKSPACE);
			~DocWindow();
	bool	QuitRequested();
	void	MessageReceived(BMessage *msg);
	void	MenusBeginning();

	// hooks
	virtual bool		IsDirty();
	virtual void		EntryChanged(BMessage *msg);
	virtual status_t	Load(BEntry *e);
	virtual status_t	Save(BEntry *e, const BMessage* args = 0);
	virtual void		SaveAs();
	virtual BFilePanel*	CreateSavePanel() const;
	virtual void		WindowFrame(BRect *proposed);

	// accessors
	void				SetDirty(bool flag = true);
	entry_ref			FileRef() const;
};

#define DOC_WIN_SAVE	'Dsav'
#define DOC_WIN_SAVE_AS	'Dsas'

}	// namespace BExperimental

using namespace BExperimental;

#endif
