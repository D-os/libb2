/*****************************************************************************
//
//	File:		Volume.h
//
//	Description:	BVolume class
//
//	Copyright 1992-98, Be Incorporated
//
*****************************************************************************/

#ifndef _VOLUME_H
#define _VOLUME_H

#ifndef _BE_BUILD_H
#include <BeBuild.h>
#endif
#include <sys/types.h>

#ifndef _MIME_H
#include <Mime.h>
#endif
#ifndef _SUPPORT_DEFS_H
#include <SupportDefs.h>
#endif

class	BDirectory;
class	BBitmap;

class BVolume {
public:
							BVolume();
							BVolume(dev_t dev);
							BVolume(const BVolume &vol);

virtual						~BVolume();

			status_t		InitCheck() const;

			status_t		SetTo(dev_t dev);
			void			Unset(void);

			dev_t			Device() const;

			status_t		GetRootDirectory(BDirectory *dir) const;

			off_t			Capacity() const;
			off_t			FreeBytes() const;

			status_t		GetName(char *name) const;
			status_t		SetName(const char *name);

			status_t		GetIcon(BBitmap *icon, icon_size which) const;
		
			bool			IsRemovable() const;
			bool			IsReadOnly() const;
			bool			IsPersistent() const;
			bool			IsShared() const;
			bool			KnowsMime() const;
			bool			KnowsAttr() const;
			bool			KnowsQuery() const;
		
			bool			operator==(const BVolume &vol) const;
			bool			operator!=(const BVolume &vol) const;
			BVolume &		operator=(const BVolume &vol);

private:

friend class BVolumeRoster;

virtual	void		_TurnUpTheVolume1();
virtual	void		_TurnUpTheVolume2();

#if !_PR3_COMPATIBLE_
virtual	void		_TurnUpTheVolume3();
virtual	void		_TurnUpTheVolume4();
virtual	void		_TurnUpTheVolume5();
virtual	void		_TurnUpTheVolume6();
virtual	void		_TurnUpTheVolume7();
virtual	void		_TurnUpTheVolume8();
#endif

		dev_t			fDev;
		status_t		fCStatus;

#if !_PR3_COMPATIBLE_
		int32			_reserved[8];
#endif

};



#endif
