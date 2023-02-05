#ifndef _MENU_H
#define _MENU_H

#include <List.h>
#include <Messenger.h>
#include <View.h>

class BMenu;
class BMenuBar;
class BMenuItem;

namespace BPrivate {
class BMenuWindow;
}  // namespace BPrivate

enum menu_layout {
	B_ITEMS_IN_ROW = 0,
	B_ITEMS_IN_COLUMN,
	B_ITEMS_IN_MATRIX
};

struct menu_info
{
	float		font_size;
	font_family f_family;
	font_style	f_style;
	rgb_color	background_color;
	int32		separator;
	bool		click_to_open;
	bool		triggers_always_shown;
};

typedef bool (*menu_tracking_hook)(BMenu *, void *);

class BMenu : public BView
{
   public:
	BMenu(const char *title, menu_layout layout = B_ITEMS_IN_COLUMN);
	BMenu(const char *title, float width, float height);
	virtual ~BMenu();

	BMenu(BMessage *data);
	static BArchivable *Instantiate(BMessage *data);
	virtual status_t	Archive(BMessage *data, bool deep = true) const;

	virtual void AttachedToWindow();
	virtual void DetachedFromWindow();

	bool	   AddItem(BMenuItem *item);
	bool	   AddItem(BMenuItem *item, int32 index);
	bool	   AddItem(BMenuItem *item, BRect frame);
	bool	   AddItem(BMenu *menu);
	bool	   AddItem(BMenu *menu, int32 index);
	bool	   AddItem(BMenu *menu, BRect frame);
	bool	   AddList(BList *list, int32 index);
	bool	   AddSeparatorItem();
	bool	   RemoveItem(BMenuItem *item);
	BMenuItem *RemoveItem(int32 index);
	bool	   RemoveItems(int32 index, int32 count, bool del = false);
	bool	   RemoveItem(BMenu *menu);

	BMenuItem *ItemAt(int32 index) const;
	BMenu	  *SubmenuAt(int32 index) const;
	int32	   CountItems() const;
	int32	   IndexOf(BMenuItem *item) const;
	int32	   IndexOf(BMenu *menu) const;
	BMenuItem *FindItem(uint32 command) const;
	BMenuItem *FindItem(const char *name) const;

	virtual status_t SetTargetForItems(BHandler *target);
	virtual status_t SetTargetForItems(BMessenger messenger);
	virtual void	 SetEnabled(bool state);
	virtual void	 SetRadioMode(bool state);
	virtual void	 SetTriggersEnabled(bool state);
	virtual void	 SetMaxContentWidth(float max);

	void  SetLabelFromMarked(bool on);
	bool  IsLabelFromMarked();
	bool  IsEnabled() const;
	bool  IsRadioMode() const;
	bool  AreTriggersEnabled() const;
	bool  IsRedrawAfterSticky() const;
	float MaxContentWidth() const;

	BMenuItem *FindMarked();

	BMenu	  *Supermenu() const;
	BMenuItem *Superitem() const;

	virtual void MessageReceived(BMessage *msg);
	virtual void KeyDown(const char *bytes, int32 numBytes);
	virtual void Draw(BRect updateRect);
	virtual void GetPreferredSize(float *width, float *height);
	virtual void ResizeToPreferred();
	virtual void FrameMoved(BPoint new_position);
	virtual void FrameResized(float new_width, float new_height);
	void		 InvalidateLayout();

	virtual BHandler *ResolveSpecifier(BMessage	  *msg,
									   int32	   index,
									   BMessage	  *specifier,
									   int32	   form,
									   const char *property);
	virtual status_t  GetSupportedSuites(BMessage *data);

	virtual void MakeFocus(bool state = true);
	virtual void AllAttached();
	virtual void AllDetached();

   protected:
	BMenu(BRect		  frame,
		  const char *viewName,
		  uint32	  resizeMask,
		  uint32	  flags,
		  menu_layout layout,
		  bool		  resizeToFit);

	virtual BPoint ScreenLocation();

	void SetItemMargins(float left,
						float top,
						float right,
						float bottom);
	void GetItemMargins(float *left,
						float *top,
						float *right,
						float *bottom) const;

	menu_layout Layout() const;

	virtual void Show();
	void		 Show(bool selectFirstItem);
	void		 Hide();
	BMenuItem	*Track(bool	  start_opened = false,
					   BRect *special_rect = nullptr);

   public:
	enum add_state {
		B_INITIAL_ADD,
		B_PROCESSING,
		B_ABORT
	};
	virtual bool AddDynamicItem(add_state s);
	virtual void DrawBackground(BRect updateRect);

	void SetTrackingHook(menu_tracking_hook func, void *state);

   private:
	class TriggerList;

	friend class BWindow;
	friend class BMenuBar;
	friend class BMenuItem;
	friend class BSeparatorItem;

	BMenu &operator=(const BMenu &);

	bool	   _Show(bool selectFirstItem = false, bool keyDown = false);
	void	   _Hide();
	BMenuItem *_Track(int *action, long start = -1);

	void _UpdateNavigationArea(BPoint position,
							   BRect &navAreaRectAbove,
							   BRect &navAreaBelow);

	void _UpdateStateOpenSelect(BMenuItem *item,
								BPoint position, BRect &navAreaRectAbove,
								BRect	  &navAreaBelow,
								bigtime_t &selectedTime,
								bigtime_t &navigationAreaTime);
	void _UpdateStateClose(BMenuItem	*item,
						   const BPoint &where,
						   const uint32 &buttons);

	bool  _AddItem(BMenuItem *item, int32 index);
	bool  _RemoveItems(int32 index, int32 count, BMenuItem *item, bool del = false);
	bool  _RelayoutIfNeeded();
	void  _LayoutItems(int32 index);
	BSize _ValidatePreferredSize();
	void  _ComputeLayout(int32 index, bool bestFit, bool moveItems, float *width, float *height);

	BRect _CalcFrame(BPoint where, bool *scrollOn);

   protected:
	void DrawItems(BRect updateRect);

   private:
	bool				   _OverSuper(BPoint loc);
	bool				   _OverSubmenu(BMenuItem *item, BPoint loc);
	BPrivate::BMenuWindow *_MenuWindow();
	void				   _DeleteMenuWindow();
	BMenuItem			  *_HitTestItems(BPoint where, BPoint slop = B_ORIGIN) const;
	void				   _CacheFontInfo();

	void	   _ItemMarked(BMenuItem *item);
	void	   _Install(BWindow *target);
	void	   _Uninstall();
	void	   _SelectItem(BMenuItem *item,
						   bool		  showSubmenu	  = true,
						   bool		  selectFirstItem = false,
						   bool		  keyDown		  = false);
	bool	   _SelectNextItem(BMenuItem *item, bool forward);
	BMenuItem *_NextItem(BMenuItem *item, bool forward) const;
	void	   _SetStickyMode(bool on);
	bool	   _IsStickyMode() const;

	void		_CalcTriggers();
	const char *_ChooseTrigger(const char *title, BList *chars);
	void		_UpdateWindowViewSize(const bool &updatePosition);
	bool		_AddDynamicItems(bool keyDown = false);
	bool		_OkToProceed(BMenuItem *item, bool keyDown = false);

	bool _CustomTrackingWantsToQuit();

	// int	 _State(BMenuItem **_item = nullptr) const;
	void _InvokeItem(BMenuItem *item, bool now = false);
	void _QuitTracking(bool onlyThis = true);

	BMenuItem			  *fChosenItem;
	BList				   fItems;
	BRect				   fPad;
	BMenuItem			  *fSelected;
	BPrivate::BMenuWindow *fCachedMenuWindow;
	BMenu				  *fSuper;
	BMenuItem			  *fSuperitem;
	BRect				   fSuperbounds;
	float				   fAscent;
	float				   fDescent;
	float				   fFontHeight;
	uint32				   fState;
	menu_layout			   fLayout;
	BRect				  *fExtraRect;
	float				   fMaxContentWidth;
	// BPoint		*fInitMatrixSize;

	class impl;
	impl *fExtraMenuData;

	// char fTrigger;
	bool fResizeToFit;
	bool fUseCachedMenuLayout;
	bool fEnabled;
	bool fDynamicName;
	bool fRadioMode;
	// bool fTrackNewBounds;
	bool fStickyMode;
	// bool fIgnoreHidden;
	bool fTriggerEnabled;
	bool fHasSubmenus;	// bool fRedrawAfterSticky;
	bool fAttachAborted;
};

#endif /* _MENU_H */
