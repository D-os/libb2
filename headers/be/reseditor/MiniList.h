/***************************************************************************
//
//	File:			MiniList.h
//
//	Description:	A BList that is more efficient with 0 or 1 item.
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

#ifndef _MINI_LIST_H
#define _MINI_LIST_H

#include <SupportDefs.h>

// A BMiniList looks and behaves much like a BList, but it is much more
// efficient for small numbers of items in the list.  If there is zero
// or just one item in the list, it only requires 8 bytes for the class
// itself; for more than 1 item, it requires a full BList allocated on
// the heap.

class BList;

class BMiniList
{
public:
		BMiniList();
		~BMiniList();
	
/* Adding and removing items. */
		bool	AddItem(void *item);
		bool	AddItem(void *item, int32 atIndex);
		bool	RemoveItem(void *item);
		void	*RemoveItem(int32 index);
		bool	ReplaceItem(int32 index, void *newItem);
		void	MakeEmpty();

/* Retrieving items. */
		void	*ItemAt(int32) const;
		void	*FirstItem() const;
		void	*LastItem() const;

/* Querying the list. */
		bool	HasItem(void *item) const;
		int32	IndexOf(void *item) const;
		int32	CountItems() const;
		bool	IsEmpty() const;

/* Iterating over the list. */
		void	DoForEach(bool (*func)(void *));
		void	DoForEach(bool (*func)(void *, void *), void *);

private:
	BList* Rest() const;
	void UnRest();
		
	void* fFirst;
	BList* fRest;
};

#endif
