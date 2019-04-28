/***************************************************************************
//
//	File:			ToolTip.h
//
//	Description:	Support classes for showing tool tips.
//
//	Copyright 2000, Be Incorporated, All Rights Reserved.
//
***************************************************************************/

/**************************************************************************
// WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
//
// This code is experimental.  Please do not use it in your own
// applications, as future versions WILL break this interface.
//
***************************************************************************/

#ifndef _TOOL_TIP_H
#define _TOOL_TIP_H

#include <MessageFilter.h>

#include <Font.h>
#include <GraphicsDefs.h>
#include <Rect.h>
#include <Point.h>

#include <Locker.h>
#include <String.h>

class BView;

// To be able to show a tool tip, have your BView-derived class also
// inherit from BToolTipable.  Note that these classes are only
// temporarily in the resource editor kit.

enum {
	B_REQUEST_TOOL_INFO			= 'RQTI'
};

// Information about how to display the tool tip.  Not needed unless
// you want to do something special besides "show my text outside of
// the view frame".

class BToolTipInfo
{
public:
	BToolTipInfo();
	virtual ~BToolTipInfo();
	
	void SetText(const char* text);
	const char* Text() const;
	
	void SetFont(const BFont* font);
	const BFont* Font() const;
	
	void SetFillColor(rgb_color color);
	rgb_color FillColor() const;
	
	void SetTextColor(rgb_color color);
	rgb_color TextColor() const;
	
	void SetInline(bool state);
	bool Inline() const;
	
	void SetBaseline(float region_y);
	float Baseline() const;
	
	void SetView(BView* view);
	BView* View() const;

	BView* DetachView();
	
private:
	BString fText;
	BFont fFont;
	rgb_color fFillColor;
	rgb_color fTextColor;
	float fBaseline;
	bool fInline;
	BView* fView;
};

// Derive from this class in your BView-subclass to be able to
// show tool tips.  If all you want is some static text shown outside
// of the view frame, just call SetText() to provide it.  Otherwise,
// override GetToolTipInfo() to give more information.

class BToolTipable
{
public:
	BToolTipable(BView& owner, const char* text = 0);
	virtual ~BToolTipable();
	
	void SetText(const char* text);
	const char* Text() const;
	
	// This method is called in two sitations --
	// * To figure out how the pointer is moving around tipable
	//   objects, GetToolTipInfo() will be called with out_info
	//   set to zero.  In this case, you should just quickly return
	//   the rectangle corresponding to the tipable area containing
	//   the point 'where'.  All of this is in your view coordinates.
	// * To actually display a tool tip, GetToolTipInfo() is called
	//   with a non-zero out_info object.  You should then fill in
	//   the object with the information to display.
	//
	// The default implementation of this simply returns the view's
	// frame rectangle (offset to (0,0)) as the region, and plugs
	// the current Text() into out_info.
	
	virtual status_t GetToolTipInfo(BPoint where, BRect* out_region,
									BToolTipInfo* out_info = 0);

private:
	BView& fOwner;
	BString fText;
};

namespace BPrivate {
	class TipWindow;
}

class BToolTipFilter;

class BToolTip : public BLocker
{
public:
	BToolTip();
	virtual ~BToolTip();
	
	virtual status_t ShowTip(BHandler* who);
	virtual status_t CursorMoved(BHandler* who,
								 BPoint where, BPoint delta);
	virtual status_t HideTip(BHandler* who);
	virtual status_t KillTip(BHandler* who);
	
	virtual BToolTipInfo* NewToolTipInfo() const;
	virtual status_t SetToolTipInfo(BHandler* who,
									BRect region, BToolTipInfo* info);
	
	virtual status_t RemoveOwner(BHandler* who);
	
private:
	friend class BPrivate::TipWindow;
	
	BPrivate::TipWindow* Tip();
	void WindowGone(BPrivate::TipWindow* w);
	
	BPrivate::TipWindow* fTip;
	BHandler* fShower;
};

extern BToolTip* be_tip;

class BToolTipFilter : public BMessageFilter
{
public:
	BToolTipFilter(BToolTip& tip);
	BToolTipFilter();
	virtual ~BToolTipFilter();
	virtual	filter_result Filter(BMessage *message, BHandler **target);

	virtual status_t SendToolTipInfo();
	
	void MoveCursor(BView* v, BPoint screen_loc);
	
	void HideTip();
	void KillTip();
	
private:
	static bool find_view(BView* root, BView* which);
	
	BToolTip& fTip;
	BRect fRegion;
	int32 fButtons;
	BPoint fCursor;
	BView* fShower;
};

#endif
