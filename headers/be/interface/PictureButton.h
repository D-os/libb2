/*******************************************************************************
/
/	File:			PictureButton.h
/
/   Description:    BPictureButton creates a button that displays a bitmap.
/
/	Copyright 1994-98, Be Incorporated, All Rights Reserved
/
*******************************************************************************/

#ifndef	_PICTURE_BUTTON_H
#define	_PICTURE_BUTTON_H

#include <BeBuild.h>
#include <Control.h>
#include <Picture.h>	/* For convenience */

enum {
	B_ONE_STATE_BUTTON,
	B_TWO_STATE_BUTTON
};

/*----------------------------------------------------------------*/
/*----- BPictureButton class -------------------------------------*/

class BPictureButton : public BControl {

public:
						BPictureButton(BRect frame, 
									   const char* name, 
									   BPicture *off, 			   
									   BPicture *on,
									   BMessage *message, 
									   uint32 behavior = B_ONE_STATE_BUTTON,
									   uint32 resizeMask =
											B_FOLLOW_LEFT | B_FOLLOW_TOP, 
									   uint32 flgs = B_WILL_DRAW | B_NAVIGABLE); 
virtual					~BPictureButton();
	
						BPictureButton(BMessage *data);
static	BArchivable		*Instantiate(BMessage *data);
virtual	status_t		Archive(BMessage *data, bool deep = true) const;

virtual	void			Draw(BRect updateRect);
virtual	void			MouseDown(BPoint where);
virtual	void			KeyDown(const char *bytes, int32 numBytes);

virtual	void			SetEnabledOn(BPicture *on);
virtual	void			SetEnabledOff(BPicture *off);
virtual	void			SetDisabledOn(BPicture *on);
virtual	void			SetDisabledOff(BPicture *off);
		
		BPicture		*EnabledOn() const;
		BPicture		*EnabledOff() const;
		BPicture		*DisabledOn() const;
		BPicture		*DisabledOff() const;

virtual	void			SetBehavior(uint32 behavior);
		uint32			Behavior() const;

virtual void			MessageReceived(BMessage *msg);
virtual	void			MouseUp(BPoint pt);
virtual	void			WindowActivated(bool state);
virtual	void			MouseMoved(BPoint pt, uint32 code, const BMessage *msg);
virtual	void			AttachedToWindow();
virtual	void			DetachedFromWindow();
virtual	void			SetValue(int32 value);
virtual	status_t		Invoke(BMessage *msg = NULL);
virtual	void			FrameMoved(BPoint new_position);
virtual	void			FrameResized(float new_width, float new_height);

virtual BHandler		*ResolveSpecifier(BMessage *msg,
										int32 index,
										BMessage *specifier,
										int32 form,
										const char *property);
virtual status_t		GetSupportedSuites(BMessage *data);

virtual void			ResizeToPreferred();
virtual void			GetPreferredSize(float *width, float *height);
virtual void			MakeFocus(bool state = true);
virtual void			AllAttached();
virtual void			AllDetached();


/*----- Private or reserved -----------------------------------------*/
virtual status_t		Perform(perform_code d, void *arg);

private:

virtual	void			_ReservedPictureButton1();
virtual	void			_ReservedPictureButton2();
virtual	void			_ReservedPictureButton3();

		BPictureButton		&operator=(const BPictureButton &);

		void			Redraw();
		void			InitData();

		BPicture		*fEnabledOff;
		BPicture		*fEnabledOn;
		BPicture		*fDisabledOff;
		BPicture		*fDisabledOn;
		bool			fOutlined;
		uint32			fBehavior;
		uint32			_reserved[4];
};

/*-------------------------------------------------------------*/
/*-------------------------------------------------------------*/

#endif /* _PICTURE_BUTTON_H */
