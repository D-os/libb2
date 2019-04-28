/***************************************************************************
//
//	File:			EntryList.h
//
//	Description:	BEntryList class and entry_ref struct
//
//	Copyright 1992-98, Be Incorporated, All Rights Reserved.
//
***************************************************************************/

#ifndef _ENTRY_LIST_H
#define _ENTRY_LIST_H

#ifndef _BE_BUILD_H
#include <BeBuild.h>
#endif
#include <dirent.h>
#include <limits.h>

#include <SupportDefs.h>

class BEntry;
struct entry_ref;

class BEntryList
{
public:

						BEntryList();
#if !_PR3_COMPATIBLE_
virtual					~BEntryList();
#else
						~BEntryList();
#endif

virtual status_t		GetNextEntry(BEntry *entry, 
									 bool traverse=false) = 0;
virtual status_t		GetNextRef(entry_ref *ref) = 0;
virtual int32			GetNextDirents(struct dirent *buf, 
						   		size_t length, int32 count = INT_MAX) = 0;

virtual status_t		Rewind() = 0;
virtual int32			CountEntries() = 0;

private:

#if !_PR3_COMPATIBLE_
virtual	void			_ReservedEntryList1();
virtual	void			_ReservedEntryList2();
virtual	void			_ReservedEntryList3();
virtual	void			_ReservedEntryList4();
virtual	void			_ReservedEntryList5();
virtual	void			_ReservedEntryList6();
virtual	void			_ReservedEntryList7();
virtual	void			_ReservedEntryList8();
#endif

};

#endif
