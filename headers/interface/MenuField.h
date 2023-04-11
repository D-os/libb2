#ifndef _MENU_FIELD_H
#define _MENU_FIELD_H

#include <Menu.h>  // For convenience
#include <View.h>

class BMenuBar;

class BMenuField : public BView
{
   public:
	BMenuField(BRect	   frame,
			   const char *name,
			   const char *label,
			   BMenu	  *menu,
			   uint32	   resize = B_FOLLOW_LEFT | B_FOLLOW_TOP,
			   uint32	   flags  = B_WILL_DRAW | B_NAVIGABLE);
	BMenuField(BRect	   frame,
			   const char *name,
			   const char *label,
			   BMenu	  *menu,
			   bool		   fixed_size,
			   uint32	   resize = B_FOLLOW_LEFT | B_FOLLOW_TOP,
			   uint32	   flags  = B_WILL_DRAW | B_NAVIGABLE);
	BMenuField(BMessage *data);
	virtual ~BMenuField();
	static BArchivable *Instantiate(BMessage *data);
	virtual status_t	Archive(BMessage *data, bool deep = true) const;

	virtual void Draw(BRect update);
	virtual void AttachedToWindow();
	virtual void AllAttached();
	virtual void MouseDown(BPoint where);
	virtual void KeyDown(const char *bytes, int32 numBytes);
	virtual void MakeFocus(bool state);
	virtual void MessageReceived(BMessage *msg);
	virtual void WindowActivated(bool state);
	virtual void MouseUp(BPoint pt);
	virtual void MouseMoved(BPoint pt, uint32 code, const BMessage *msg);
	virtual void DetachedFromWindow();
	virtual void AllDetached();
	virtual void FrameMoved(BPoint new_position);
	virtual void FrameResized(float new_width, float new_height);

	BMenu	  *Menu() const;
	BMenuBar  *MenuBar() const;
	BMenuItem *MenuItem() const;

	virtual void SetLabel(const char *label);
	const char	*Label() const;

	virtual void SetEnabled(bool on);
	bool		 IsEnabled() const;

	virtual void SetAlignment(alignment label);
	alignment	 Alignment() const;
	virtual void SetDivider(float dividing_line);
	float		 Divider() const;

	void ShowPopUpMarker();
	void HidePopUpMarker();

	virtual BHandler *ResolveSpecifier(BMessage	  *msg,
									   int32	   index,
									   BMessage	  *specifier,
									   int32	   form,
									   const char *property);
	virtual status_t  GetSupportedSuites(BMessage *data);

	virtual void ResizeToPreferred();
	virtual void GetPreferredSize(float *width, float *height);

	virtual status_t Perform(perform_code d, void *arg);

   private:
	BMenuField &operator=(const BMenuField &);

	// char	 *fLabel;
	// BMenu	 *fMenu;
	// BMenuBar *fMenuBar;
	// alignment fAlign;
	// float	  fDivider;
	// float	  fStringWidth;
	// bool	  fEnabled;
	// bool	  fSelected;
	// bool	  fTransition;
	// bool	  fFixedSizeMB;
	// thread_id fMenuTaskID;
};

#endif /* _MENU_FIELD_H */
