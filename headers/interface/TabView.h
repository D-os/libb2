#ifndef _TAB_VIEW_H
#define _TAB_VIEW_H

#include <View.h>

class BMessage;

/// tab definitions
enum tab_position {
	B_TAB_FIRST = 999,
	B_TAB_FRONT,
	B_TAB_ANY
};

class BTab : public BArchivable
{
   public:
	BTab(BView* contents = nullptr);
	virtual ~BTab();

	BTab(BMessage* data);
	static BArchivable* Instantiate(BMessage* data);
	virtual status_t	Archive(BMessage* data, bool deep = true) const;
	virtual status_t	Perform(uint32 d, void* arg);

	const char*	 Label() const;
	virtual void SetLabel(const char* label);

	bool		 IsSelected() const;
	virtual void Select(BView* owner);
	virtual void Deselect();

	virtual void SetEnabled(bool on);
	bool		 IsEnabled() const;

	void MakeFocus(bool infocus = true);
	bool IsFocus() const;

	virtual void SetView(BView* contents);
	BView*		 View() const;

	virtual void DrawFocusMark(BView* owner, BRect tabFrame);
	virtual void DrawLabel(BView* owner, BRect tabFrame);
	virtual void DrawTab(BView* owner, BRect tabFrame, tab_position, bool full = true);

   private:
	BTab& operator=(const BTab&);

	bool   fEnabled;
	bool   fSelected;
	bool   fFocus;
	BView* fView;
};

class BTabView : public BView
{
   public:
	BTabView(BRect frame, const char* name,
			 button_width width		   = B_WIDTH_AS_USUAL,
			 uint32		  resizingMode = B_FOLLOW_ALL,
			 uint32		  flags		   = B_FULL_UPDATE_ON_RESIZE | B_WILL_DRAW | B_NAVIGABLE_JUMP | B_FRAME_EVENTS | B_NAVIGABLE);
	virtual ~BTabView();

	BTabView(BMessage*);
	static BArchivable* Instantiate(BMessage*);
	virtual status_t	Archive(BMessage*, bool deep = true) const;
	virtual status_t	Perform(perform_code d, void* arg);

	virtual void WindowActivated(bool state);
	virtual void AttachedToWindow();
	virtual void AllAttached();
	virtual void AllDetached();
	virtual void DetachedFromWindow();

	virtual void MessageReceived(BMessage* msg);
	virtual void FrameMoved(BPoint new_position);
	virtual void FrameResized(float w, float h);
	virtual void KeyDown(const char* bytes, int32 n);
	virtual void MouseDown(BPoint);
	virtual void MouseUp(BPoint);
	virtual void MouseMoved(BPoint pt, uint32 transit, const BMessage* dnd);
	virtual void Pulse();

	virtual void Select(int32 tabIndex);
	int32		 Selection() const;

	virtual void MakeFocus(bool focusState = true);
	virtual void SetFocusTab(int32 tabIndex, bool focusState);
	int32		 FocusTab() const;

	virtual void  Draw(BRect);
	virtual void  DrawAfterChildren(BRect);
	virtual BRect DrawTabs();
	virtual void  DrawBox(BRect selectedTabFrame);
	virtual BRect TabFrame(int32 tabIndex) const;

	virtual void SetFlags(uint32 flags);
	virtual void SetResizingMode(uint32 mode);

	virtual void GetPreferredSize(float* width, float* height);
	virtual void ResizeToPreferred();

	virtual BHandler* ResolveSpecifier(BMessage* msg, int32 index, BMessage* specifier, int32 form, const char* property);
	virtual status_t  GetSupportedSuites(BMessage* data);

	virtual void  AddTab(BView* tabContents, BTab* tab = nullptr);
	virtual BTab* RemoveTab(int32 tabIndex);
	virtual BTab* TabAt(int32 tabIndex) const;

	virtual void SetTabWidth(button_width s);
	button_width TabWidth() const;

	virtual void SetTabHeight(float height);
	float		 TabHeight() const;

	BView* ContainerView() const;

	int32  CountTabs() const;
	BView* ViewForTab(int32 tabIndex) const;

   private:
	BTabView(const BTabView&);
	BTabView& operator=(const BTabView&);

	BList*		 fTabList;
	BView*		 fContainerView;
	button_width fTabWidthSetting;
	// float		 fTabWidth;
	float fTabHeight;
	int32 fSelection;
	// int32		 fInitialSelection;
	int32 fFocus;
};

#endif /* _TAB_VIEW_H */
