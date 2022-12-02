#ifndef _BUTTON_H
#define _BUTTON_H

#include <Control.h>

class BButton : public BControl
{
   public:
	BButton(BRect		frame,
			const char *name,
			const char *label,
			BMessage	 *message,
			uint32		resizeMask = B_FOLLOW_LEFT | B_FOLLOW_TOP,
			uint32		flags	   = B_WILL_DRAW | B_NAVIGABLE);
	virtual ~BButton();

	/// Archiving
	BButton(BMessage *data);
	static BArchivable *Instantiate(BMessage *data);
	virtual status_t	Archive(BMessage *data, bool deep = true) const;

	virtual void Draw(BRect updateRect);
	virtual void MouseDown(BPoint where);
	virtual void AttachedToWindow();
	virtual void KeyDown(const char *bytes, int32 numBytes);
	virtual void MakeDefault(bool state);
	virtual void SetLabel(const char *text);
	bool		 IsDefault() const;

	virtual void	 MessageReceived(BMessage *msg);
	virtual void	 WindowActivated(bool state);
	virtual void	 MouseUp(BPoint pt);
	virtual void	 MouseMoved(BPoint pt, uint32 transit, const BMessage *dnd);
	virtual void	 DetachedFromWindow();
	virtual void	 SetValue(int32 value);
	virtual void	 GetPreferredSize(float *width, float *height);
	virtual void	 ResizeToPreferred();
	virtual status_t Invoke(BMessage *msg = nullptr);
	virtual void	 FrameMoved(BPoint new_position);
	virtual void	 FrameResized(float new_width, float new_height);

	virtual void MakeFocus(bool state = true);
	virtual void AllAttached();
	virtual void AllDetached();

	virtual BHandler *ResolveSpecifier(BMessage	*msg,
									   int32	   index,
									   BMessage	*specifier,
									   int32	   form,
									   const char *property);
	virtual status_t  GetSupportedSuites(BMessage *data);

   private:
	BButton &operator=(const BButton &);

	bool fDrawAsDefault;
};

#endif /* _BUTTON_H */
