#ifndef _WINDOW_ROSTER_H
#define _WINDOW_ROSTER_H

struct entry_ref;
class BMenu;

namespace BExperimental {

class DocWindow;

class WindowRoster
{
public:
	// window counting
	virtual void	AddWindow(DocWindow *w, entry_ref *ref) = 0;
	virtual void	ChangeWindow(DocWindow *w, entry_ref *ref) = 0;
	virtual void	RemoveWindow(DocWindow *w) = 0;

	// "Window" menu management
	virtual BMenu	*WindowMenu(DocWindow *w) = 0;
	virtual void	UpdateWindowMenu(DocWindow *w) = 0;
};

}	// namespace BExperimental

using namespace BExperimental;

#endif
