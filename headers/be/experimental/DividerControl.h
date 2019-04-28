/*******************************************************************************
/
/	File:			DividerControl.h
/
/   Description:    Experimental divider control.  It allows moving
/					the divider and provides a twisty for hiding a view.
/
/	Copyright 2000, Be Incorporated, All Rights Reserved
/
*******************************************************************************/


#ifndef _DIVIDER_CONTROL_H
#define _DIVIDER_CONTROL_H

#include <Control.h>

class BMessageRunner;

namespace BExperimental {

class DividerControl : public BControl
{
public:
	// Create the divider.  The divider will control its previous
	// and next sibling views.  When the user changes the twisty,
	// you will get a report from the control but must hide the
	// appropriate view yourself.
	// B_VERTICAL orientation is currently not supported.
	DividerControl(BPoint where, const char* name,
				   BMessage* message,
				   orientation o = B_HORIZONTAL,
				   uint32 resizeMask = B_FOLLOW_NONE);
	~DividerControl();
	
	virtual	void			AttachedToWindow();
	//virtual	void			AllAttached();
	virtual	void			DetachedFromWindow();
	//virtual	void			AllDetached();

	virtual	void			FrameMoved(BPoint new_position);
	virtual	void			MessageReceived(BMessage *msg);

	virtual	void			Draw(BRect updateRect);
	virtual	void			MouseDown(BPoint where);
	virtual	void			MouseUp(BPoint where);
	virtual	void			MouseMoved(	BPoint where,
										uint32 code,
										const BMessage *a_message);
	
	virtual	void			GetPreferredSize(float *width, float *height);
	
	virtual	void			SetValue(int32 value);
	void					AnimateValue(int32 value);
	
private:
	typedef BControl inherited;
	
	orientation fOrientation;
	bool fFillToEdge;
	
	// Tracking the movement bar.
	bool fTracking;
	BPoint fInitPoint;
	float fInitPos;
	rgb_color fBackColor;
	
	// Twisting the visibility knob.
	float TargetTwist() const;
	void StartTwisting();
	bool fTwisting;
	bool fOverKnob;
	float fTwistAmount;
	float fTwistSpeed;
	BMessageRunner* fTwistPulse;
};

}	// namespace BExperimental

using namespace BExperimental;

#endif
