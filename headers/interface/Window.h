#ifndef _WINDOW_H
#define _WINDOW_H

#include <InterfaceDefs.h>
#include <Looper.h>
#include <Rect.h>

// window definitions
enum window_type {
	B_UNTYPED_WINDOW  = 0,
	B_TITLED_WINDOW	  = 1,
	B_MODAL_WINDOW	  = 3,
	B_DOCUMENT_WINDOW = 11,
	B_BORDERED_WINDOW = 20,
	B_FLOATING_WINDOW = 21
};

enum window_look {
	B_BORDERED_WINDOW_LOOK	= 20,
	B_NO_BORDER_WINDOW_LOOK = 19,
	B_TITLED_WINDOW_LOOK	= 1,
	B_DOCUMENT_WINDOW_LOOK	= 11,
	B_MODAL_WINDOW_LOOK		= 3,
	B_FLOATING_WINDOW_LOOK	= 7
};

enum window_feel {
	B_NORMAL_WINDOW_FEEL		  = 0,
	B_MODAL_SUBSET_WINDOW_FEEL	  = 2,
	B_MODAL_APP_WINDOW_FEEL		  = 1,
	B_MODAL_ALL_WINDOW_FEEL		  = 3,
	B_FLOATING_SUBSET_WINDOW_FEEL = 5,
	B_FLOATING_APP_WINDOW_FEEL	  = 4,
	B_FLOATING_ALL_WINDOW_FEEL	  = 6
};

enum window_alignment {
	B_BYTE_ALIGNMENT  = 0,
	B_PIXEL_ALIGNMENT = 1
};

enum {
	B_NOT_MOVABLE			   = 0x00000001,
	B_NOT_CLOSABLE			   = 0x00000020,
	B_NOT_ZOOMABLE			   = 0x00000040,
	B_NOT_MINIMIZABLE		   = 0x00004000,
	B_NOT_RESIZABLE			   = 0x00000002,
	B_NOT_H_RESIZABLE		   = 0x00000004,
	B_NOT_V_RESIZABLE		   = 0x00000008,
	B_AVOID_FRONT			   = 0x00000080,
	B_AVOID_FOCUS			   = 0x00002000,
	B_WILL_ACCEPT_FIRST_CLICK  = 0x00000010,
	B_OUTLINE_RESIZE		   = 0x00001000,
	B_NO_WORKSPACE_ACTIVATION  = 0x00000100,
	B_NOT_ANCHORED_ON_ACTIVATE = 0x00020000,
	B_ASYNCHRONOUS_CONTROLS	   = 0x00080000,
	B_QUIT_ON_WINDOW_CLOSE	   = 0x00100000
};

#define B_CURRENT_WORKSPACE 0
#define B_ALL_WORKSPACES 0xffffffff

class BButton;
class BMenuBar;
class BView;

class SkCanvas;

class BWindow : public BLooper
{
   public:
	BWindow(BRect		frame,
			const char *title,
			window_type type,
			uint32		flags,
			uint32		workspace = B_CURRENT_WORKSPACE);
	BWindow(BRect		frame,
			const char *title,
			window_look look,
			window_feel feel,
			uint32		flags,
			uint32		workspace = B_CURRENT_WORKSPACE);
	virtual ~BWindow();

	BWindow(BMessage *data);
	static BArchivable *Instantiate(BMessage *data);
	virtual status_t	Archive(BMessage *data, bool deep = true) const;

	virtual void Quit();
	void		 Close(); /* Synonym of Quit() */

	void   AddChild(BView *child, BView *before = NULL);
	bool   RemoveChild(BView *child);
	int32  CountChildren() const;
	BView *ChildAt(int32 index) const;

	virtual void DispatchMessage(BMessage *message, BHandler *handler);
	virtual void MessageReceived(BMessage *message);
	virtual void FrameMoved(BPoint new_position);
	virtual void WorkspacesChanged(uint32 old_ws, uint32 new_ws);
	virtual void WorkspaceActivated(int32 ws, bool state);
	virtual void FrameResized(float new_width, float new_height);
	virtual void Minimize(bool minimize);
	virtual void Zoom(BPoint rec_position,
					  float	 rec_width,
					  float	 rec_height);
	void		 Zoom();
	void		 SetZoomLimits(float max_h, float max_v);
	virtual void ScreenChanged(BRect screen_size, color_space depth);
	void		 SetPulseRate(bigtime_t rate);
	bigtime_t	 PulseRate() const;
	void		 AddShortcut(uint32	   key,
							 uint32	   modifiers,
							 BMessage *msg);
	void		 AddShortcut(uint32	   key,
							 uint32	   modifiers,
							 BMessage *msg,
							 BHandler *target);
	void		 RemoveShortcut(uint32 key, uint32 modifiers);
	void		 SetDefaultButton(BButton *button);
	BButton		*DefaultButton() const;
	virtual void MenusBeginning();
	virtual void MenusEnded();
	bool		 NeedsUpdate() const;
	void		 UpdateIfNeeded();
	BView		  *FindView(const char *view_name) const;
	BView		  *FindView(BPoint) const;
	BView		  *CurrentFocus() const;
	void		 Activate(bool = true);
	virtual void WindowActivated(bool state);
	void		 ConvertToScreen(BPoint *pt) const;
	BPoint		 ConvertToScreen(BPoint pt) const;
	void		 ConvertFromScreen(BPoint *pt) const;
	BPoint		 ConvertFromScreen(BPoint pt) const;
	void		 ConvertToScreen(BRect *rect) const;
	BRect		 ConvertToScreen(BRect rect) const;
	void		 ConvertFromScreen(BRect *rect) const;
	BRect		 ConvertFromScreen(BRect rect) const;
	void		 MoveBy(float dx, float dy);
	void		 MoveTo(BPoint);
	void		 MoveTo(float x, float y);
	void		 ResizeBy(float dx, float dy);
	void		 ResizeTo(float width, float height);
	virtual void Show();
	virtual void Hide();
	bool		 IsHidden() const;
	bool		 IsMinimized() const;

	void Flush() const;
	void Sync() const;

	status_t SendBehind(const BWindow *window);

	void DisableUpdates();
	void EnableUpdates();

	void BeginViewTransaction();
	void EndViewTransaction();

	BRect		Bounds() const;
	BRect		Frame() const;
	const char *Title() const;
	void		SetTitle(const char *title);
	bool		IsFront() const;
	bool		IsActive() const;
	void		SetKeyMenuBar(BMenuBar *bar);
	BMenuBar	 *KeyMenuBar() const;
	void		SetSizeLimits(float min_h,
							  float max_h,
							  float min_v,
							  float max_v);
	void		GetSizeLimits(float *min_h,
							  float *max_h,
							  float *min_v,
							  float *max_v);
	uint32		Workspaces() const;
	void		SetWorkspaces(uint32);
	BView	  *LastMouseMovedView() const;

	virtual BHandler *ResolveSpecifier(BMessage	*msg,
									   int32	   index,
									   BMessage	*specifier,
									   int32	   form,
									   const char *property);
	virtual status_t  GetSupportedSuites(BMessage *data);

	status_t AddToSubset(BWindow *window);
	status_t RemoveFromSubset(BWindow *window);

	status_t	SetType(window_type type);
	window_type Type() const;

	status_t	SetLook(window_look look);
	window_look Look() const;

	status_t	SetFeel(window_feel feel);
	window_feel Feel() const;

	status_t SetFlags(uint32);
	uint32	 Flags() const;

	bool IsModal() const;
	bool IsFloating() const;

	status_t SetWindowAlignment(window_alignment mode,
								int32			 h,
								int32			 hOffset	  = 0,
								int32			 width		  = 0,
								int32			 widthOffset  = 0,
								int32			 v			  = 0,
								int32			 vOffset	  = 0,
								int32			 height		  = 0,
								int32			 heightOffset = 0);
	status_t GetWindowAlignment(window_alignment *mode		   = NULL,
								int32			  *h			   = NULL,
								int32			  *hOffset	   = NULL,
								int32			  *width		   = NULL,
								int32			  *widthOffset  = NULL,
								int32			  *v			   = NULL,
								int32			  *vOffset	   = NULL,
								int32			  *height	   = NULL,
								int32			  *heightOffset = NULL) const;

	virtual bool	  QuitRequested();
	virtual thread_id Run();

   private:
	friend class BApplication;
	friend class BBitmap;
	friend class BScrollBar;
	friend class BView;
	friend class BMenuItem;
	friend class BWindowScreen;
	friend class BDirectWindow;
	friend class BFilePanel;

	BWindow();
	BWindow(BWindow &);
	BWindow &operator=(BWindow &);

	virtual void task_looper();

	short		fShowLevel;
	uint32		fFlags;
	BView	  *fFocus;
	BView	  *fLastMouseMovedView;
	window_look fLook;
	window_feel fFeel;

	class impl;
	pimpl<impl> m;
	SkCanvas	 *_get_canvas();
};

inline void BWindow::Close()
{
	Quit();
}

#endif /* _WINDOW_H */
