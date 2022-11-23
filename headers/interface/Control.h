#ifndef _CONTROL_H
#define _CONTROL_H

#include <Invoker.h>
#include <View.h>

enum {
	B_CONTROL_OFF = 0,
	B_CONTROL_ON  = 1
};

class BControl : public BView, public BInvoker
{
   public:
	BControl(BRect		 frame,
			 const char *name,
			 const char *label,
			 BMessage	  *message,
			 uint32		 resizeMask,
			 uint32		 flags);
	virtual ~BControl();

	BControl(BMessage *data);
	static BArchivable *Instantiate(BMessage *data);
	virtual status_t	Archive(BMessage *data, bool deep = true) const;

	virtual void WindowActivated(bool state);
	virtual void AttachedToWindow();
	virtual void MessageReceived(BMessage *msg);
	virtual void MakeFocus(bool state = true);
	virtual void KeyDown(const char *bytes, int32 numBytes);
	virtual void MouseDown(BPoint pt);
	virtual void MouseUp(BPoint pt);
	virtual void MouseMoved(BPoint pt, uint32 code, const BMessage *msg);
	virtual void DetachedFromWindow();

	virtual void SetLabel(const char *text);
	const char  *Label() const;

	virtual void SetValue(int32 value);
	int32		 Value() const;

	virtual void SetEnabled(bool on);
	bool		 IsEnabled() const;

	virtual void GetPreferredSize(float *width, float *height);
	virtual void ResizeToPreferred();

	virtual status_t  Invoke(BMessage *msg = nullptr);
	virtual BHandler *ResolveSpecifier(BMessage	*msg,
									   int32	   index,
									   BMessage	*specifier,
									   int32	   form,
									   const char *property);
	virtual status_t  GetSupportedSuites(BMessage *data);

	virtual void AllAttached();
	virtual void AllDetached();

   protected:
	bool IsFocusChanging() const;
	bool IsTracking() const;
	void SetTracking(bool state);

	void SetValueNoUpdate(int32 value);

   private:
	BControl &operator=(const BControl &);

	const char *fLabel;
	int32		fValue;
	bool		fEnabled;
	// bool		fFocusChanging;
	// bool		fTracking;
	// bool		fWantsNav;
};

#endif /* _CONTROL_H */
