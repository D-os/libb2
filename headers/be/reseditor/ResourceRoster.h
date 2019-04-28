/***************************************************************************
//
//	File:			ResourceRoster.h
//
//	Description:	Keeps track of all available editor add-ons.
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

#ifndef _RESOURCE_ROSTER_H
#define _RESOURCE_ROSTER_H

#include <ListItem.h>

#include <List.h>
#include <Locker.h>

#include <ResourceAddon.h>

class BMessage;
class BString;

enum {
	B_GENERATE_RESOURCE		= 'BGRS'
};

class BGenerateItem : public BStringItem
{
public:
	BGenerateItem(BMessage* info,
				  uint32 outlineLevel = 0, bool expanded = true);
	BGenerateItem(const char* text, BMessage* info,
				  uint32 outlineLevel = 0, bool expanded = true);
	virtual ~BGenerateItem();
	
	BMessage* Info() const;
	void SetInfo(BMessage* info);

private:
	BMessage* fInfo;
};

class BResourceRoster
{
public:
	BResourceRoster(const BResourceAddonArgs& args);
	~BResourceRoster();

	BResourceAddon* AddonForResource(const BResourceItem *it, BString* out_name);
	status_t UpdateConfiguration(BResourceAddon* who, const BMessage* config);
	status_t GetConfiguration(BResourceAddon* who, BMessage* into) const;
	
	status_t GetAllConfigurations(BMessage* into) const;
	status_t SetAllConfigurations(const BMessage* from);
	
	// Returns a list of BMessages filled in with all add-on's generate
	// info.
	status_t GetGenerateList(BList* out) const;
	static void FreeGenerateList(BList* in);
	
	status_t GetGenerateMenuItems(BList* out) const;
	status_t GetGenerateListItems(BList* out) const;
	
	status_t GenerateResource(BResourceCollection& c,
							  BResourceHandle* out_item,
							  const BMessage* which,
							  int32 id = 1, const char* name = 0,
							  bool make_selected = true,
							  BResourceCollection::conflict_resolution
							  resol = BResourceCollection::B_RENAME_NEW_ITEM);
	
	status_t MessagePaste(BResourceCollection& c, const BMessage* data);
	status_t MessageDrop(BResourceCollection& c, const BMessage* drop);
	status_t ReadBuffer(BResourceCollection& c, BPositionIO& buf,
						const char* mime_type = 0, entry_ref* ref = 0);

private:
	void get_addons(BDirectory& dir);
	
	BLocker fAccess;
	BResourceAddonArgs fAddonArgs;
	BList fAddons;
};

#endif
