#ifndef _BE_LIST_H
#define _BE_LIST_H

#include <SupportDefs.h>

class BList
{
   public:
	BList(int32 initialAllocSize = 0);
	BList(const BList &);
	virtual ~BList();

	BList &operator=(const BList &from);

	/// Adding and removing items
	bool  AddItem(void *item);
	bool  AddItem(void *item, int32 atIndex);
	bool  AddList(BList *newItems);
	bool  AddList(BList *newItems, int32 atIndex);
	bool  RemoveItem(void *item);
	void *RemoveItem(int32 index);
	bool  RemoveItems(int32 index, int32 count);
	void  MakeEmpty();

	/// Reordering items
	void SortItems(int (*cmp)(const void *, const void *));

	/// Retrieving items
	void	 *ItemAt(int32) const;
	void	 *FirstItem() const;
	void	 *LastItem() const;
	void **Items() const;

	/// Querying the list
	bool  HasItem(void *item) const;
	int32 IndexOf(void *item) const;
	int32 CountItems() const;
	bool  IsEmpty() const;

	/// Iterating over the list
	void DoForEach(bool (*func)(void *));
	void DoForEach(bool (*func)(void *, void *), void *);

   private:
	void *data;
};

#endif /* _BE_LIST_H */
