/******************************************************************************
/
/	File:			Button.h
/
/   Description:    BButton displays and controls a button in a window.
/
/	Copyright 1992-98, Be Incorporated, All Rights Reserved
/
*******************************************************************************/

#ifndef	_BUTTON_H
#define	_BUTTON_H

#include <BeBuild.h>
#include <Control.h>

/*----------------------------------------------------------------*/
/*----- BButton class --------------------------------------------*/

class BButton : public BControl {

public:
					BButton(BRect frame,
							const char *name,
							const char *label, 
							BMessage *message,
							uint32 resizeMask = B_FOLLOW_LEFT | B_FOLLOW_TOP, 
							uint32 flags = B_WILL_DRAW | B_NAVIGABLE); 
virtual				~BButton();

/* Archiving */
					BButton(BMessage *data);
static	BArchivable	*Instantiate(BMessage *data);
virtual	status_t	Archive(BMessage *data, bool deep = true) const;

virtual	void		Draw(BRect updateRect);
virtual	void		MouseDown(BPoint where);
virtual	void		AttachedToWindow();
virtual	void		KeyDown(const char *bytes, int32 numBytes);
virtual void		MakeDefault(bool state);
virtual	void		SetLabel(const char *text);
		bool		IsDefault() const;

virtual void		MessageReceived(BMessage *msg);
virtual void		WindowActivated(bool state);
virtual	void		MouseUp(BPoint pt);
virtual	void		MouseMoved(BPoint pt, uint32 code, const BMessage *msg);
virtual	void		DetachedFromWindow();
virtual	void		SetValue(int32 value);
virtual	void		GetPreferredSize(float *width, float *height);
virtual void		ResizeToPreferred();
virtual	status_t	Invoke(BMessage *msg = NULL);
virtual	void		FrameMoved(BPoint new_position);
virtual	void		FrameResized(float new_width, float new_height);

virtual void		MakeFocus(bool state = true);
virtual void		AllAttached();
virtual void		AllDetached();


virtual BHandler	*ResolveSpecifier(BMessage *msg,
									int32 index,
									BMessage *specifier,
									int32 form,
									const char *property);
virtual	status_t	GetSupportedSuites(BMessage *data);
virtual status_t	Perform(perform_code d, void *arg);

private:

virtual	void		_ReservedButton1();
virtual	void		_ReservedButton2();
virtual	void		_ReservedButton3();

		BButton		&operator=(const BButton &);

		BRect		DrawDefault(BRect bounds, bool enabled);
		status_t	Execute();

		float		fCachedWidth;
		bool		fDrawAsDefault;
		uint32		_reserved[3];
};

/*-------------------------------------------------------------*/
/*-------------------------------------------------------------*/

#endif /* _BUTTON_H */
