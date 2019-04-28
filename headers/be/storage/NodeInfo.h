/****************************************************************************
//
//	File:			NodeInfo.h
//
//	Description:	File type information
//
//	Copyright 1998, Be Incorporated, All Rights Reserved.
//
****************************************************************************/

#ifndef _NODE_INFO_H
#define _NODE_INFO_H

#ifndef _BE_BUILD_H
#include <BeBuild.h>
#endif
#include <SupportDefs.h>
#include <Mime.h>
#include <Message.h>
#include <File.h>
#include <Entry.h>

class BBitmap;
class BResources;


class BNodeInfo {
public:
  					BNodeInfo();
					BNodeInfo(BNode *node);
virtual				~BNodeInfo();

		status_t	SetTo(BNode *node);
		status_t	InitCheck() const;

virtual	status_t	GetType(char *type) const;
virtual	status_t	SetType(const char *type);
virtual	status_t	GetIcon(BBitmap *icon, icon_size k = B_LARGE_ICON) const;
virtual	status_t	SetIcon(const BBitmap *icon, icon_size k = B_LARGE_ICON);

		status_t	GetPreferredApp(char *signature,
									app_verb verb = B_OPEN) const;
		status_t	SetPreferredApp(const char *signature,
									app_verb verb = B_OPEN);
		status_t	GetAppHint(entry_ref *ref) const;
		status_t	SetAppHint(const entry_ref *ref);


		status_t	GetTrackerIcon(BBitmap *icon,
									icon_size k = B_LARGE_ICON) const;
static	status_t	GetTrackerIcon(const entry_ref *ref,
									BBitmap *icon,
									icon_size k = B_LARGE_ICON);


private:
friend class BAppFileInfo;

virtual	void		_ReservedNodeInfo1();
virtual	void		_ReservedNodeInfo2();
virtual	void		_ReservedNodeInfo3();

		BNodeInfo	&operator=(const BNodeInfo &);
					BNodeInfo(const BNodeInfo &);

		BNode		*fNode;
		uint32		_reserved[2];
		status_t	fCStatus;
};

#endif
