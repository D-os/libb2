/*******************************************************************************
/
/	File:			Order.h
/
/   Description:    Experimental class for sorting arrays.
/
/	Copyright 2000, Be Incorporated, All Rights Reserved
/
*******************************************************************************/


#ifndef _ORDER_H
#define _ORDER_H

#include <Locker.h>

namespace BExperimental {

// Base class for ordering.

class TOrderBase
{
public:
	TOrderBase(size_t size);
	virtual ~TOrderBase();
	
	size_t operator[](int32 i) const;
	
	// You must implement this.  When called, return the result
	// of comparing the entries at index e1 and index e2 in your array.
	virtual int Compare(size_t e1, size_t e2) const = 0;

private:
	static BLocker fGlobalLock;
	static TOrderBase* fOrderObj;
	
	static int compare_elements(const void* e1, const void* e2);
	size_t* Order() const;
	
	size_t fSize;
	size_t* fOrder;
};

// Template class for ordering any array-like container.

template<class Container, class Value> class TArrayOrder : public TOrderBase
{
public:
	TArrayOrder(const Container& container, size_t size,
				int (*compare)(const Value& v1, const Value& v2))
		: TOrderBase(size),
		  fContainer(container),
		  fCompare(compare)
	{
	}
	
	virtual ~TArrayOrder()
	{
	}
	
	// Return the ordered value in the original list.
	const Value& ValueAt(int32 i) const
	{
		return fContainer[(*this)[i]];
	}
	
	virtual int Compare(size_t e1, size_t e2) const
	{
		return (*fCompare)(fContainer[e1], fContainer[e2]);
	}

private:
	const Container& fContainer;
	int (*fCompare)(const Value& v1, const Value& v2);
};

}	// namespace BExperimental

using namespace BExperimental;

#endif
