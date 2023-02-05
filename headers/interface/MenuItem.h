#ifndef _MENU_ITEM_H
#define _MENU_ITEM_H

#include <Archivable.h>
#include <Invoker.h>
#include <Menu.h>  // For convenience

class BMessage;
class BWindow;

class BMenuItem : public BArchivable, public BInvoker
{
   public:
	BMenuItem(const char *label,
			  BMessage	 *message,
			  char		  shortcut	= 0,
			  uint32	  modifiers = 0);
	BMenuItem(BMenu *menu, BMessage *message = nullptr);
	BMenuItem(BMessage *data);
	virtual ~BMenuItem();
	static BArchivable *Instantiate(BMessage *data);
	virtual status_t	Archive(BMessage *data, bool deep = true) const;

	virtual status_t Invoke(BMessage *msg = nullptr);

	virtual void SetLabel(const char *name);
	virtual void SetEnabled(bool state);
	virtual void SetMarked(bool state);
	virtual void SetTrigger(char ch);
	virtual void SetShortcut(char ch, uint32 modifiers);

	const char *Label() const;
	bool		IsEnabled() const;
	bool		IsMarked() const;
	char		Trigger() const;
	char		Shortcut(uint32 *modifiers = nullptr) const;

	BMenu *Submenu() const;
	BMenu *Menu() const;
	BRect  Frame() const;

   protected:
	virtual void GetContentSize(float *width, float *height);
	virtual void TruncateLabel(float max, char *new_label);
	virtual void DrawContent();
	virtual void Draw();
	virtual void Highlight(bool on);
	bool		 IsSelected() const;
	BPoint		 ContentLocation() const;

   private:
	friend class BMenu;
	// friend class BPopUpMenu;
	friend class BMenuBar;

	BMenuItem(const BMenuItem &);
	BMenuItem &operator=(const BMenuItem &);

	void Install(BWindow *window);
	void Uninstall();
	void SetSuper(BMenu *superMenu);
	void Select(bool select);

	bool	  _IsActivated();
	rgb_color _LowColor();
	rgb_color _HighColor();

	void _DrawMarkSymbol();
	void _DrawShortcutSymbol(bool);
	void _DrawSubmenuSymbol();
	void _DrawControlChar(char shortcut, BPoint where);

	char	*fLabel;
	BMenu	*fSubmenu;
	BWindow *fWindow;
	BMenu	*fSuper;
	BRect	 fBounds;
	uint32	 fModifiers;
	float	 fCachedWidth;
	int16	 fTriggerIndex;
	char	 fUserTrigger;
	char	 fSysTrigger;
	char	 fShortcutChar;
	bool	 fMark;
	bool	 fEnabled;
	bool	 fSelected;
};

class BSeparatorItem : public BMenuItem
{
   public:
	BSeparatorItem();
	BSeparatorItem(BMessage *data);
	virtual ~BSeparatorItem();
	virtual status_t	Archive(BMessage *data, bool deep = true) const;
	static BArchivable *Instantiate(BMessage *data);
	virtual void		SetEnabled(bool state);

   protected:
	virtual void GetContentSize(float *width, float *height);
	virtual void Draw();

   private:
	BSeparatorItem &operator=(const BSeparatorItem &);
};

#endif /* _MENU_ITEM_H */
