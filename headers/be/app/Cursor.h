/*******************************************************************************
/
/	File:			Cursor.h
/
/   Description:    BCursor describes a view-wide or application-wide cursor
/
/	Copyright 1992-98, Be Incorporated, All Rights Reserved
/
*******************************************************************************/

#ifndef _CURSOR_H
#define _CURSOR_H

#include <BeBuild.h>
#include <InterfaceDefs.h>
#include <Archivable.h>

/*----------------------------------------------------------------*/
/*----- BCursor class --------------------------------------------*/

class BCursor : BArchivable {

public:
						BCursor(const void *cursorData);
						BCursor(BMessage *data);
virtual					~BCursor();

virtual	status_t		Archive(BMessage *into, bool deep = true) const;
static	BArchivable		*Instantiate(BMessage *data);

/*----- Private or reserved ---------------*/
virtual status_t		Perform(perform_code d, void *arg);

private:

virtual	void			_ReservedCursor1();
virtual	void			_ReservedCursor2();
virtual	void			_ReservedCursor3();
virtual	void			_ReservedCursor4();

		friend class	BApplication;
		friend class	BView;

		int32			m_serverToken;
		int32			m_needToFree;
		uint32			_reserved[6];
};

#endif
