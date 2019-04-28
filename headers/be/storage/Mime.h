/*****************************************************************************
//
//	File:			Mime.h
//
//	Description:	MIME string functions
//
//	Copyright 1998, Be Incorporated, All Rights Reserved.
//
*****************************************************************************/

#ifndef _MIME_H
#define _MIME_H

#ifndef _BE_BUILD_H
#include <BeBuild.h>
#endif
#include <sys/types.h>
#include <SupportDefs.h>
#include <StorageDefs.h>

#ifdef __cplusplus
extern "C" {
#endif

_IMPEXP_BE 	int	update_mime_info(const char *path,
						int recursive,
						int synchronous,
						int force);
_IMPEXP_BE 	status_t create_app_meta_mime(const char *path,
									int recursive,
									int synchronous,
									int force);
_IMPEXP_BE 	status_t get_device_icon(const char *dev, void *icon, int32 size);

	static const uint32 B_MIME_STRING_TYPE	= 'MIMS';

#ifdef __cplusplus
}
#endif

enum icon_size {
	B_LARGE_ICON = 32,
	B_MINI_ICON = 16
};

#ifdef __cplusplus

#include <Message.h>
#include <File.h>
#include <Entry.h>

class BBitmap;
class BResources;
class BAppFileInfo;
class BMessenger;

enum app_verb {
	B_OPEN
};

extern _IMPEXP_BE const char *B_APP_MIME_TYPE;		/* platform dependent*/
extern _IMPEXP_BE const char *B_PEF_APP_MIME_TYPE;	/* "application/x-be-executable"*/
extern _IMPEXP_BE const char *B_PE_APP_MIME_TYPE;	/* "application/x-vnd.be-peexecutable"*/
extern _IMPEXP_BE const char *B_ELF_APP_MIME_TYPE;	/* "application/x-vnd.be-elfexecutable"*/
extern _IMPEXP_BE const char *B_RESOURCE_MIME_TYPE;	/* "application/x-be-resource"*/
extern _IMPEXP_BE const char *B_FILE_MIME_TYPE;		/* "application/octet-stream"*/

/* ------------------------------------------------------------- */

enum {
	B_META_MIME_CHANGED = 'MMCH'
};

enum {
	B_ICON_CHANGED					= 0x00000001,
	B_PREFERRED_APP_CHANGED			= 0x00000002,
	B_ATTR_INFO_CHANGED				= 0x00000004,
	B_FILE_EXTENSIONS_CHANGED		= 0x00000008,
	B_SHORT_DESCRIPTION_CHANGED		= 0x00000010,
	B_LONG_DESCRIPTION_CHANGED		= 0x00000020,
	B_ICON_FOR_TYPE_CHANGED			= 0x00000040,
	B_APP_HINT_CHANGED				= 0x00000080,
	B_MIME_TYPE_CREATED				= 0x00000100,
	B_MIME_TYPE_DELETED				= 0x00000200,
	B_SNIFFER_RULE_CHANGED			= 0x00000400,

	B_EVERYTHING_CHANGED			= (int)0xFFFFFFFF
};

/* ------------------------------------------------------------- */

class BMimeType	{

public:
					BMimeType();
					BMimeType(const char *MIME_type);
virtual				~BMimeType();

		status_t	SetTo(const char *MIME_type);
		void		Unset();
		status_t 	InitCheck() const;

		/* these functions simply perform string manipulations*/
		const char	*Type() const;
		bool		IsValid() const;
		bool		IsSupertypeOnly() const;
		bool		IsInstalled() const;
		status_t	GetSupertype(BMimeType *super_type) const;
		bool		operator==(const BMimeType &type) const;
		bool		operator==(const char *type) const;
		bool		Contains(const BMimeType *type) const;

		/* These functions are for managing data in the meta mime file*/
		status_t	Install();
		status_t	Delete();
		status_t	GetIcon(BBitmap *icon, icon_size) const;
		status_t	GetPreferredApp(char *signature,
									app_verb verb = B_OPEN) const;
		status_t	GetAttrInfo(BMessage *info) const;
		status_t	GetFileExtensions(BMessage *extensions) const;
		status_t	GetShortDescription(char *description) const;
		status_t	GetLongDescription(char *description) const;
		status_t	GetSupportingApps(BMessage *signatures) const;

		status_t	SetIcon(const BBitmap *icon, icon_size);
		status_t	SetPreferredApp(const char *signature,
									app_verb verb = B_OPEN);
		status_t	SetAttrInfo(const BMessage *info);
		status_t	SetFileExtensions(const BMessage *extensions);
		status_t	SetShortDescription(const char *description);
		status_t	SetLongDescription(const char *description);

static	status_t	GetInstalledSupertypes(BMessage *super_types);
static	status_t	GetInstalledTypes(BMessage *types);
static	status_t	GetInstalledTypes(const char *super_type,
									BMessage *subtypes);
static	status_t	GetWildcardApps(BMessage *wild_ones);
static	bool		IsValid(const char *string);

		status_t	GetAppHint(entry_ref *ref) const;
		status_t	SetAppHint(const entry_ref *ref);

		/* for application signatures only.*/
		status_t	GetIconForType(const char *type,
								BBitmap *icon,
								icon_size which) const;
		status_t	SetIconForType(const char *type,
								const BBitmap *icon,
								icon_size which);

		/* sniffer rule manipulation */
		status_t	GetSnifferRule(BString *result) const;
		status_t	SetSnifferRule(const char *);
static	status_t	CheckSnifferRule(const char *rule, BString *parseError);

		/* calls to ask the sniffer to identify the MIME type of a file or data in memory */
static	status_t	GuessMimeType(const entry_ref *file, BMimeType *result);
static	status_t	GuessMimeType(const void *buffer, int32 length, BMimeType *result);
static	status_t	GuessMimeType(const char *filename, BMimeType *result);

static	status_t	StartWatching(BMessenger target);
static	status_t	StopWatching(BMessenger target);

		/* Deprecated  Use SetTo instead. */
		status_t	SetType(const char *MIME_type);		

private:

friend	class BAppFileInfo;
friend	class BRoster;
friend  class TRosterApp;
friend  class TMimeWorker;

friend	status_t	_update_mime_info_(const char *, int32);
friend	status_t	_real_update_app_(BAppFileInfo *, const char *, bool);

static  void		_set_local_dispatch_target_(BMessenger *, void (*)(BMessage *));
		void        _touch_(); 

virtual	void		_ReservedMimeType1();
virtual	void		_ReservedMimeType2();
virtual	void		_ReservedMimeType3();

		BMimeType	&operator=(const BMimeType &);
					BMimeType(const BMimeType &);

		void		InitData(const char *type);
		status_t	OpenFile(bool create_file = false,
							dev_t dev = -1) const;
		status_t	CloseFile() const;
		status_t	GetSupportedTypes(BMessage *types);
		status_t	SetSupportedTypes(const BMessage *types);
		void		MimeChanged(int32 w, const char *type = NULL,
						bool large = true) const;

		char		*fType;
		BFile		*fMeta;
		void		*_unused;
		entry_ref	fRef;
		int			fWhere;
		status_t	fCStatus;
		uint32		_reserved[3];
};

#endif
#endif
