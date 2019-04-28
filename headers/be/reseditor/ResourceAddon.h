/***************************************************************************
//
//	File:			ResourceAddon.h
//
//	Description:	Core classes of the resource add-on architecture.
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

#ifndef _RESOURCE_ADD_ON_H
#define _RESOURCE_ADD_ON_H

#include <image.h>
#include <Messenger.h>
#include <String.h>

#include <ResourceItem.h>
#include <MiniList.h>

namespace BPrivate {
	class ResourceEntry;
}

using namespace BPrivate;

enum {
	B_RESOURCE_DATA_CHANGED		= 'RDCG'
};

// Forward references.
class BResourceCollection;
class BResourceAddonBase;
class BMiniItemEditor;
class BFullItemEditor;
class BUndoContext;

// This class represents a "reference" to a particular resource item.
// You use this instead of raw pointers to items -- then, when you need
// to actually access the item in a holder, you use BResourceCollection
// to retrieve it.  This ensures that the item will always be valid
// while you are holding it (through reference counting), and enforces
// correct read/write access protection to its data.
//
// In addition, this class can also represent a "connection" between a
// resource item and an object that is viewing it.  Again through
// BResourceCollection, you can ask the handle to subscribe a
// BResourceAddonBase to its resource item, so that its DataChanged()
// is called whenever a change is made to the item.

class BResourceHandle
{
public:
	BResourceHandle();
	BResourceHandle(const BResourceHandle& o);
	BResourceHandle(ResourceEntry* entry);
	~BResourceHandle();
	
	BResourceHandle& operator=(const BResourceHandle& o);
	bool operator==(const BResourceHandle& o);
	bool operator!=(const BResourceHandle& o);
	
	bool IsValid() const;
	
	const void* ItemAddress() const;
	
private:
	friend class BResourceCollection;
	friend class BResourceAddonBase;
	friend class ResourceEntry;
	
	status_t Subscribe(BResourceAddonBase* editor, bool highlight);
	status_t Unsubscribe();
	
	BResourceAddonBase* Subscriber() const;
	void ClearSubscriber();
	ResourceEntry* Entry() const;
	
	ResourceEntry* fEntry;
	BResourceAddonBase* fSubscriber;
};

// This class provides access to the set of available resource items.
// Because this data may be accessed by multiple threads, it must be
// access protected.  This is accomplished by only making the class
// instance available through calls to read or write lock the data --
// the returned reference can be used for the type of access requested.
//
// A read-access returns a const BResourceCollection, only allowing
// read access to the class data.  A write-access returns a non-const
// instance, allowing both read and write access.

class BResourceCollection
{
public:
	// Iterator through all resource items.  'inout_cookie' should be
	// initialized to 0 when first calling, B_OK will be returned up to
	// and including the last item.
	virtual status_t	GetNextItem(size_t *inout_cookie, BResourceHandle* out_item) const = 0;
	
	enum conflict_resolution {
		B_ASK_USER = 0,
		B_RENAME_NEW_ITEM,
		B_DELETE_OLD_ITEM
	};
	
	// Add a new resource item with the given attribuates.
	virtual status_t 	AddItem(BResourceHandle* out_item,
								type_code type, int32 id, const char* name = 0,
								const void *data = 0, size_t len = 0,
								bool make_selected = true,
								conflict_resolution resolution = B_ASK_USER) = 0;
								
	// Find an existing resource item with the given attributes.
	virtual status_t	FindItem(BResourceHandle* out_item,
								 type_code type, int32 id, const char* name = 0) const = 0;

	// Remove the given resource item.
	virtual status_t	RemoveItem(BResourceHandle& item) = 0;
	
	// Get and set the currently selected resource item.
	virtual status_t	SetCurrentItem(const BResourceHandle& item) = 0;
	virtual BResourceHandle CurrentItem() const = 0;
	
	// Return a unique ID for all items or items with the given
	// type, which is >= the selected ID.
	virtual int32		UniqueID(int32 smallest = 1) const = 0;
	virtual int32		UniqueIDForType(type_code type, int32 smallest = 1) const = 0;
	
	// Determine next ID value for a new resource item.
	// NOTE: This needs to be made more general to work with resources
	// that are messages, etc.
	virtual int32		NextID(bool fromCurrent=true) const = 0;
	virtual int32		NextIDForType(type_code type, bool fromCurrent = true) const = 0;
	
	// Return the undo context for this collection.  If undo/redo is not
	// supported, returns NULL.
	virtual BUndoContext* UndoContext();
	virtual const BUndoContext* UndoContext() const;
	
	// Access the data for a selected resource item.
	virtual const BResourceItem* ReadItem(const BResourceHandle& handle) const;
	virtual BResourceItem* WriteItem(BResourceHandle& handle,
									 BResourceAddonBase* who = 0);

	// Start and stop getting reports of changes to a resource item.
	virtual status_t	Subscribe(BResourceHandle& item,
								  BResourceAddonBase* subcriber,
								  bool highlight = true);
	virtual status_t	Unsubscribe(BResourceHandle& item);
	
	// Convenience function to find a resource item and start watching it.
	virtual status_t	FindAndSubscribe(BResourceHandle* out_item,
										 BResourceAddonBase* subcriber,
										 type_code type, int32 id, const char* name = 0,
										 bool highlight = true);

	// Retrieve the oldest change seen by the 'from' object.
	BResourceHandle		GetNextChange(BResourceAddonBase* from,
									  uint32* changes = 0) const;
	
	// Retrieve items being watched by the 'from' object.  Returns B_OK
	// if there is an item, B_BAD_INDEX if the index is out of range.
	status_t			GetSubscriptionAt(BResourceHandle* out_item,
										  BResourceAddonBase* watcher,
										  size_t index) const;
	
protected:
	friend class ResourceEntry;
	friend class BResourceAddonBase;
	
	// Hooks to implement thread protection protocol.
	virtual status_t	ReadLockCollection() const = 0;
	virtual status_t	ReadUnlockCollection() const = 0;
	virtual status_t	WriteLockCollection(const char* name=0) = 0;
	virtual status_t	WriteUnlockCollection() = 0;
	
	// For implementors -- this is called whenever a ResourceEntry in your
	// collection reports a change.
	virtual void		EntryChanged(ResourceEntry* e);
	
protected:
	BResourceCollection()				{ }
	virtual ~BResourceCollection() 		{ }
};

// This contains the private arguments passed down to BResourceAddonBase
// instances.

class BResourceAddonArgs
{
public:
	BResourceAddonArgs(const BResourceAddonBase& o);
	BResourceAddonArgs(const BResourceAddonArgs& o);
	BResourceAddonArgs(BResourceCollection& collection);

	~BResourceAddonArgs();
	
private:
	friend class BResourceAddonBase;
	
	BResourceAddonArgs& operator=(const BResourceAddonArgs& o);
	
	BResourceCollection& fCollection;
};

// This is the base class for all classes implemented by an
// addon.  It provides the core functionality for accessing
// and subscribing to resource data items.

class BResourceAddonBase
{
public:
	BResourceAddonBase(const BResourceAddonArgs& args,
					   BHandler* owner = 0);
	BResourceAddonBase(const BResourceAddonArgs& args,
					   BMessenger owner);
	virtual ~BResourceAddonBase();

	// Read/write access to the collection this addon lives in.
	// NOTE: You must not cache the BResourceCollection object
	// after calling *Unlock(), nor change the const-ness of
	// the returned object.
	const BResourceCollection* ReadLock() const;
	void ReadUnlock(const BResourceCollection* locked) const;
	BResourceCollection* WriteLock(const char* name = 0) const;
	void WriteUnlock(BResourceCollection* locked) const;
	
	// This hook is called whenever a BResourceHandle you
	// have subcribed to is changed.  If you don't override the
	// method, these changes are added to an internal list and
	// B_RESOURCE_DATA_CHANGED is sent to the 'receiver' you
	// supplied in the constructor.  You can then use
	// BResourceCollection::GetNextChange() to see what happened.
	//
	// Note well: This method must guarantee that it is called
	// without any locks on the resource data being held; the
	// implementor of DataChange() should acquire any locks it
	// needs.
	//
	// Note weller: Calls to this method may cross thread/window
	// boundaries, so if you are manipulating a view you have
	// made then this method can be called when your window isn't
	// locked.  In this situation, you should not use this
	// function -- instead, call SetChangeTarget() to get
	// notifications sent to your view's MessageReceived() when
	// its data has changed.
	virtual void DataChanged(BResourceHandle& item);

	// Get and set the destination for B_RESOURCE_DATA_CHANGED
	// reports sent from DataChanged().
	void SetChangeTarget(BHandler* owner);
	void SetChangeTarget(BMessenger owner);
	BMessenger ChangeTarget() const;
	
private:
	friend class BResourceHandle;
	friend class BResourceAddonArgs;
	friend class BResourceCollection;
	
	BResourceHandle GetNextChange(uint32* changes = 0);
	void StartWatching(BResourceHandle* who);
	void StopWatching(BResourceHandle* who);
	
	BResourceCollection& fCollection;
	BMessenger fReceiver;				// object receiving updates.
	BHandler* fHandler;					// how to make fReceiver.
	BMiniList fWatching;				// list of BResourceHandle objs.
	BMiniList fChangeItems;				// ResourceEntry objects that have changed.
	BMiniList fChangeFlags;				// flags of what has changed in above.
};

extern const char* B_GENERATE_ADDON;
extern const char* B_GENERATE_NAME;
extern const char* B_GENERATE_TYPE;

class BResourceAddon : public BResourceAddonBase
{
public:
	BResourceAddon(const BResourceAddonArgs& args);
	~BResourceAddon();
	
	// Return information about the types of resources that this
	// add-on can generate.  You should add at least B_GENERATE_NAME
	// and B_GENERATE_TYPE fields to the message.
	virtual status_t GetNthGenerateInfo(int32 n, BMessage* out_info) const;
	
	// Create a new resource item that is the type of the given
	// generate info.
	virtual status_t GenerateResource(BResourceHandle* out_item,
									  const BMessage* info,
									  int32 id, const char* name,
									  bool make_selected = true,
									  BResourceCollection::conflict_resolution
									  resol = BResourceCollection::B_RENAME_NEW_ITEM);
	
	// These two methods are used to report the quality of editor
	// you are for a particular resource item.  The range of values
	// you can return are:
	//
	//     < 0.0 : Can't edit this resource.
	// 0.0 - 0.99: Can edit this resource, but with little or
	//             no semantic information.  For example, a hex
	//             editor can show anything as raw data.
	// 0.1 - 0.49: Can edit this resource as a general class of
	//             data.  For example, a BMessage editor can show
	//             any message as raw fields.
	// 0.5 - 1.0 : Can edit this resource as a specific type of
	//             data.  For example, a bitmap editor can edit
	//             a BMessage bitmap archive.
	//
	// Determining the editor to use for a particular resource
	// goes through two phases.  First, QuickQuality() is called
	// for each addon, and the best quality addon is selected.  If
	// this addon has a quality >= 0.3, then that addon is
	// immediately used.  Otherwise, PreciseQuality() is then
	// called for every add-on, and the best of those is used.
	//
	// Thus the rules are: QuickQuality() should return a quality
	// that can be determined without a lot of work.  This means
	// looking at the resource's type code and perhaps the first
	// few bytes of data.  PreciseQuality() should return the
	// most accurate quality that can be determined, no matter how
	// long that might take.
	
	virtual float QuickQuality(const BResourceItem* item) const;
	virtual float PreciseQuality(const BResourceItem* item) const;
	
	// Return a new mini editor implemented by this addon for the
	// given resource item.  The default implementation returns NULL,
	// indicating there is no mini editor.
	
	virtual BMiniItemEditor* MakeMiniEditor(const BResourceAddonArgs& args,
											BResourceHandle primaryItem,
											const BMessage* configuration=0);
	
	// Return a new full editor implemented by this addon for the
	// given resource item.  The default implementation returns NULL,
	// indicating there is no full editor.
	
	virtual BFullItemEditor* MakeFullEditor(const BResourceAddonArgs& args,
											BResourceHandle primaryItem,
											const BMessage* configuration=0);
	
	// Handle data pastes.  PasteQuality() should return how good the addon
	// thinks it is at handling the data; values should be the same as for
	// QuickQuality() and PreciseQuality().  The default implementation returns
	// -1, indicating the addon can not handle the data.  When HandlePaste()
	// is called, the addon should create a new resource item containing the
	// data in the message.
	
	virtual float PasteQuality(const BMessage* data) const;
	virtual status_t HandlePaste(const BMessage* data);

	// Handle message drops.  DropQuality() should return how good the addon
	// thinks it is at handling the message; values should be the same as for
	// QuickQuality() and PreciseQuality().  The default implementation returns
	// -1, indicating the addon can not handle the message.  When HandleDrop()
	// is called, the addon should create a new resource item containing the
	// data in the message.
	
	virtual float DropQuality(const BMessage* drop) const;
	virtual status_t HandleDrop(const BMessage* drop);

	// Handle data buffers, e.g., files.  BufferQuality() should return how good
	// the addon thinks it is at handling the data buffer; values should be the
	// same as for QuickQuality() and PreciseQuality().  The default
	// implementation returns -1, indicating the addon can not handle the buffer.
	// Optional parameters may give you more information about the buffer:
	// mime_type is the MIME type of its contents, and entry_ref is the actual
	// file it came from.  When HandleBuffer() is called, the addon should create
	// a new resource item containing the data in the buffer.
	
	virtual float BufferQuality(BPositionIO& buf, const char* mime_type = 0,
								entry_ref* ref = 0) const;
	virtual status_t HandleBuffer(BPositionIO& buf, const char* mime_type = 0,
								  entry_ref* ref = 0);
	
private:
};

// interface
typedef BResourceAddon* (*make_nth_resourcer_type)(int32 n, image_id you, const BResourceAddonArgs& args, uint32 flags, ...);
extern "C" _EXPORT BResourceAddon* make_nth_resourcer(int32 n, image_id you, const BResourceAddonArgs& args, uint32 flags, ...);

#endif
