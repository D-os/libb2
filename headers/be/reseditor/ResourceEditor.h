/***************************************************************************
//
//	File:			ResourceEditor.h
//
//	Description:	Interfaces for creating graphic resource editors.
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

#ifndef _RESOURCE_EDITOR_H
#define _RESOURCE_EDITOR_H

#include <View.h>

#include <ResourceAddon.h>

// Some new standard commands that views can handle.

#ifndef B_BEOS_VERSION_DANO
const uint32 B_CLEAR = 'CLER';
const uint32 B_REDO = 'REDO';
#endif

enum {
	B_DESELECT_ALL				= 'DSAL'
};

class BMiniItemEditor;
class BMenuItem;
class BMenu;

class BMiniItemView : public BView
{
public:
	/****
	***** NOTE
	*****
	***** This class is not yet finished or usable.
	*****
	****/
	
	BMiniItemView(BRect frame, const char* name, uint32 flags);
	virtual ~BMiniItemView();
	
	// Initialize this view to edit the data in 'forEditor'.
	virtual status_t BeginEdit(BMiniItemEditor* forEditor) = 0;
	
	// Return to editing this view -- i.e., make this view or one
	// of its appropriate children the focus.  (You should NOT
	// change focus in BeginEdit().)
	virtual status_t ContinueEdit() = 0;
	
	// Copy any changes back to the resource data.
	virtual status_t UpdateEdit() = 0;
	
	// Called when this editor is no longer being displayed.  Forget
	// any changes in it that have not been updated back to the resource.
	virtual status_t EndEdit() = 0;
	
private:
};

class BMiniItemEditor : public BResourceAddonBase
{
public:
	BMiniItemEditor(const BResourceAddonArgs& args,
					BResourceHandle primaryItem);
	virtual ~BMiniItemEditor();

	// This is the resource item that the editor was originally
	// invoked for.  Note that this does NOT automatically subscribe
	// to the item -- the container of the editor will either need
	// to do that itself, or manually call DataChanged() at the
	// appropriate times.  (I.e., you may not be subscribed to the
	// resource item, but don't worry about it because the person
	// using you will have to make sure you still get updates.)
	const BResourceHandle& PrimaryItem() const;
	BResourceHandle& PrimaryItem();
	
	// Invalidate yourself to display correct information when
	// next drawn.  Be sure that you also call your superclass.
	virtual void DataChanged(BResourceHandle& item);
	
	// If your editor can have intermediate changes that have not
	// yet been written to the resource item, override these.
	// HasOpenChanges() should return true if there are changes
	// the user has made that have not yet been written.
	// ApplyChanges() should write any existing changes into the
	// resource item, returning true if there were any.
	virtual bool HasOpenChanges() const;
	virtual bool ApplyChanges();
	
	// Allocate and return a BMiniItemView for editing this class's
	// data type.  If not implemented (or returns NULL), ReadData()
	// and WriteData() will instead be called for the default text
	// editing interface.
	virtual BMiniItemView* MakeEditView() const;
	
	// If MakeEditView() returns NULL, these are called for editing
	// the resource data as text.  If ReadData() does not return B_OK,
	// no editing is allowed.
	virtual status_t ReadData(BString* out) const;
	virtual status_t WriteData(const char* in);
	
	// Draw this editor's data into the given view.
	virtual void DrawData(BView* into, BRect frame, float baseline) const;
	
	// If you want to just display a line of text in DrawData(), this is
	// an easy way to do it.
	void DrawText(BView* into, BRect frame, float baseline,
				  const char* text, uint32 truncate,
				  bool force=false) const;
	
	// If 'text' arg is 0, DrawText() calls this function whenever its
	// cached string is invalid.
	virtual void MakeText(BString* out) const;
	
private:
	BResourceHandle fPrimaryItem;
	BString fClippedText;
	float fLastWidth;
};

class BFullItemEditor : public BResourceAddonBase
{
public:
	BFullItemEditor(const BResourceAddonArgs& args,
					BResourceHandle primaryItem,
					BHandler* owner = 0);
	BFullItemEditor(const BResourceAddonArgs& args,
					BResourceHandle primaryItem,
					BMessenger owner);
	virtual ~BFullItemEditor();

	// This is the resource item that the editor was originally
	// invoked for.
	const BResourceHandle& PrimaryItem() const;
	BResourceHandle& PrimaryItem();
	
	// The rest of these methods must be called in the item view's
	// window context -- i.e., with the window locked.  The editor
	// is then free to acquire any read or write locks on the
	// resource data, as needed.
	
	// Change the primary item for the editor.  This automatically
	// subscribes to the new item.
	void SetPrimaryItem(BResourceHandle new_item);
	
	// If your editor can have intermediate changes that have not
	// yet been written to the resource item, override these.
	// HasOpenChanges() should return true if there are changes
	// the user has made that have not yet been written.
	// ApplyChanges() should write any existing changes into the
	// resource item, returning true if there were any.
	virtual bool HasOpenChanges() const;
	virtual bool ApplyChanges();
	
	// Call this to change the item that is currently being
	// edited.  The default implementation returns B_ERROR,
	// indicating that the editor can not switch items.
	virtual status_t Retarget(BResourceHandle new_item);
	
	// Return the view for this editor.
	virtual BView* View() = 0;
	
	// Return custom menus to show for this editor.  The returned
	// menu is owned by your BFullItemEditor -- you should delete
	// any menus you created when you are destroyed.
	virtual BMenuItem* EditMenuItem(int32 which);
	virtual BMenu* CustomMenu(int32 which);
	
	// Get and set the settings for this editor.
	virtual status_t GetConfiguration(BMessage* into) const;
	virtual status_t SetConfiguration(const BMessage* from);
	
private:
	BResourceHandle fPrimaryItem;
	
	void InitObject();
};

#endif
