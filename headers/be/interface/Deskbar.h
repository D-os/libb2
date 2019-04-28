#ifndef _DESKBAR_H
#define _DESKBAR_H

#include <BeBuild.h>

#include <Rect.h>

class BMessenger;
class BView;

enum deskbar_location {
	B_DESKBAR_TOP,
	B_DESKBAR_BOTTOM,
	B_DESKBAR_LEFT_TOP,
	B_DESKBAR_RIGHT_TOP,
	B_DESKBAR_LEFT_BOTTOM,
	B_DESKBAR_RIGHT_BOTTOM
};

class BDeskbar {
public:
							BDeskbar();
							~BDeskbar();
					
		BRect				Frame() const;
			
		deskbar_location	Location(bool* isExpanded=NULL) const;
		status_t			SetLocation(deskbar_location location, bool expanded=false);
			
		bool				IsExpanded() const;
		status_t			Expand(bool yn);

		status_t			GetItemInfo(int32 id, const char **name) const; 
		status_t			GetItemInfo(const char* name, int32 *id) const; 

	 	bool				HasItem(int32 id) const;
	 	bool				HasItem(const char* name) const;
								
		uint32				CountItems() const; 
			
		status_t			AddItem(BView* archivableView, int32* id=NULL);
		status_t			AddItem(entry_ref* addon, int32* id=NULL);
	
		status_t			RemoveItem(int32 id); 
		status_t			RemoveItem(const char* name);

private:
		BMessenger*			fMessenger;
		uint32				_reserved[12];
};

#endif
