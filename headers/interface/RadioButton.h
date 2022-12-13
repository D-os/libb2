#ifndef _RADIO_BUTTON_H
#define _RADIO_BUTTON_H

#include <Control.h>

class BRadioButton : public BControl
{
   public:
	BRadioButton(BRect		 frame,
				 const char *name,
				 const char *label,
				 BMessage	*message,
				 uint32		 resizeMask = B_FOLLOW_LEFT | B_FOLLOW_TOP,
				 uint32		 flags		= B_WILL_DRAW | B_NAVIGABLE);

	BRadioButton(BMessage *data);
	virtual ~BRadioButton();

	static BArchivable *Instantiate(BMessage *data);
	virtual status_t	Archive(BMessage *data, bool deep = true) const;

	virtual void	 Draw(BRect updateRect);
	virtual void	 MouseDown(BPoint where);
	virtual void	 AttachedToWindow();
	virtual void	 KeyDown(const char *bytes, int32 numBytes);
	virtual void	 SetValue(int32 value);
	virtual void	 GetPreferredSize(float *width, float *height);
	virtual void	 ResizeToPreferred();
	virtual status_t Invoke(BMessage *msg = nullptr);

	virtual void MessageReceived(BMessage *msg);
	virtual void WindowActivated(bool state);
	virtual void MouseUp(BPoint where);
	virtual void MouseMoved(BPoint where, uint32 transit, const BMessage *dnd);
	virtual void DetachedFromWindow();
	virtual void FrameMoved(BPoint new_position);
	virtual void FrameResized(float new_width, float new_height);

	virtual BHandler *ResolveSpecifier(BMessage	  *msg,
									   int32	   index,
									   BMessage	  *specifier,
									   int32	   form,
									   const char *property);

	virtual void	 MakeFocus(bool state = true);
	virtual void	 AllAttached();
	virtual void	 AllDetached();
	virtual status_t GetSupportedSuites(BMessage *data);

   private:
	BRadioButton &operator=(const BRadioButton &);
};

#endif /* _RADIO_BUTTON_H */
