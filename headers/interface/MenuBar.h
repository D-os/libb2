#ifndef _MENU_BAR_H
#define _MENU_BAR_H

#include <Menu.h>

enum menu_bar_border {
	B_BORDER_FRAME,
	B_BORDER_CONTENTS,
	B_BORDER_EACH_ITEM
};

class BMenu;
class BWindow;
class BMenuItem;
class BMenuField;

class BMenuBar : public BMenu
{
   public:
	BMenuBar(BRect		 frame,
			 const char *title,
			 uint32		 resizeMask	 = B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP,
			 menu_layout layout		 = B_ITEMS_IN_ROW,
			 bool		 resizeToFit = true);
	BMenuBar(BMessage *data);
	virtual ~BMenuBar();
	static BArchivable *Instantiate(BMessage *data);
	virtual status_t	Archive(BMessage *data, bool deep = true) const;

	virtual void	SetBorder(menu_bar_border border);
	menu_bar_border Border() const;
	virtual void	Draw(BRect updateRect);
	virtual void	AttachedToWindow();
	virtual void	DetachedFromWindow();
	virtual void	MessageReceived(BMessage *msg);
	virtual void	MouseDown(BPoint where);
	virtual void	WindowActivated(bool state);
	virtual void	MouseUp(BPoint where);
	virtual void	FrameMoved(BPoint new_position);
	virtual void	FrameResized(float new_width, float new_height);

	virtual void Show();
	virtual void Hide();

	virtual BHandler *ResolveSpecifier(BMessage	  *msg,
									   int32	   index,
									   BMessage	  *specifier,
									   int32	   form,
									   const char *property);
	virtual status_t  GetSupportedSuites(BMessage *data);

	virtual void ResizeToPreferred();
	virtual void GetPreferredSize(float *width, float *height);
	virtual void MakeFocus(bool state = true);
	virtual void AllAttached();
	virtual void AllDetached();

   private:
	friend BWindow;
	friend BMenuItem;
	friend BMenuField;
	friend BMenu;

	BMenuBar &operator=(const BMenuBar &);

	menu_bar_border fBorder;
	// thread_id		fTrackingPID;
	// int32			fPrevFocusToken;
	// sem_id			fMenuSem;
	BRect *fLastBounds;

	// bool fTracking;
};

#endif /* _MENU_BAR_H */
