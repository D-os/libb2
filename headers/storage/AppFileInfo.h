#ifndef _APP_FILE_INFO_H
#define _APP_FILE_INFO_H

#include <Mime.h>
#include <NodeInfo.h>
#include <SupportDefs.h>

class BBitmap;
class BMessage;
class BFile;

struct version_info
{
	uint32 major;
	uint32 middle;
	uint32 minor;
	uint32 variety;
	uint32 internal;
	char   short_info[64];
	char   long_info[256];
};

enum info_location {
	B_USE_ATTRIBUTES	 = 0x1,
	B_USE_RESOURCES		 = 0x2,
	B_USE_BOTH_LOCATIONS = 0x3
};

enum version_kind {
	B_APP_VERSION_KIND,
	B_SYSTEM_VERSION_KIND
};

class BAppFileInfo : public BNodeInfo
{
   public:
	BAppFileInfo();
	BAppFileInfo(BFile *file);
	virtual ~BAppFileInfo();

	status_t SetTo(BFile *file);

	virtual status_t GetType(char *type) const;
	status_t		 GetSignature(char *sig) const;
	status_t		 GetAppFlags(uint32 *flags) const;
	status_t		 GetSupportedTypes(BMessage *types) const;
	status_t		 GetIcon(BBitmap *icon, icon_size which) const;
	status_t		 GetVersionInfo(version_info *vinfo, version_kind k) const;
	status_t		 GetIconForType(const char *type,
									BBitmap	   *icon,
									icon_size	which) const;

	bool IsSupportedType(const char *type) const;

	virtual status_t SetType(const char *type);
	status_t		 SetSignature(const char *sig);
	status_t		 SetAppFlags(uint32 flags);
	status_t		 SetSupportedTypes(const BMessage *types, bool sync_all);
	status_t		 SetSupportedTypes(const BMessage *types);
	status_t		 SetIcon(const BBitmap *icon, icon_size which);
	status_t		 SetVersionInfo(const version_info *vinfo, version_kind k);
	status_t		 SetIconForType(const char	  *type,
									const BBitmap *icon,
									icon_size	   which);

	void SetInfoLocation(info_location loc);
	bool IsUsingAttributes() const;
	bool IsUsingResources() const;

	bool Supports(BMimeType *mt) const;

   private:
	// typedef BNodeInfo inherited;
	// friend status_t	  _update_mime_info_(const char *, int32);
	// friend status_t	  _real_update_app_(BAppFileInfo *, const char *, bool);
	// friend status_t	  _query_for_app_(BMimeType *, const char *, entry_ref *, version_info *);
	// friend class BRoster;

	// static status_t SetSupTypesForAll(BMimeType *, const BMessage *);

	BAppFileInfo &operator=(const BAppFileInfo &);
	BAppFileInfo(const BAppFileInfo &);

	// status_t _SetSupportedTypes(const BMessage *types);
	// status_t UpdateFromRsrc();
	// status_t RealUpdateRsrcToAttr();
	// status_t UpdateMetaMime(const char *path, bool force, uint32 *changes_mask) const;
	// bool	 IsApp();
	// status_t GetMetaMime(BMimeType *meta) const;

	status_t _ReadData(const char *name, int32 id,
					   type_code type, void *buffer,
					   size_t bufferSize, size_t &bytesRead,
					   void **allocatedBuffer = nullptr) const;

	// BResources	 *fResources;
	info_location fWhere;
};

#endif
