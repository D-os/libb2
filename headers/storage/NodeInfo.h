#ifndef _NODE_INFO_H
#define _NODE_INFO_H

#include <Mime.h>
#include <SupportDefs.h>

class BBitmap;
class BNode;

class BNodeInfo
{
   public:
	BNodeInfo();
	BNodeInfo(BNode *node);
	virtual ~BNodeInfo();

	status_t SetTo(BNode *node);
	status_t InitCheck() const;

	virtual status_t GetType(char *type) const;
	virtual status_t SetType(const char *type);
	virtual status_t GetIcon(BBitmap *icon, icon_size k = B_LARGE_ICON) const;
	virtual status_t SetIcon(const BBitmap *icon, icon_size k = B_LARGE_ICON);

	status_t GetPreferredApp(char *signature, app_verb verb = B_OPEN) const;
	status_t SetPreferredApp(const char *signature, app_verb verb = B_OPEN);
	status_t GetAppHint(entry_ref *ref) const;
	status_t SetAppHint(const entry_ref *ref);

	status_t		GetTrackerIcon(BBitmap	*icon,
								   icon_size k = B_LARGE_ICON) const;
	static status_t GetTrackerIcon(const entry_ref *ref,
								   BBitmap		   *icon,
								   icon_size		k = B_LARGE_ICON);

   private:
	friend class BAppFileInfo;

	BNodeInfo &operator=(const BNodeInfo &);
	BNodeInfo(const BNodeInfo &);

	BNode	*fNode;
	status_t fCStatus;
};

#endif
