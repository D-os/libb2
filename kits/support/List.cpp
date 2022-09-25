#include "List.h"

#include <algorithm>
#include <vector>

#define DATA_PTR(data) reinterpret_cast<std::vector<void *> *>(data)
#define DATA DATA_PTR(data)

BList::BList(int32 initialAllocSize)
{
	data = new std::vector<void *>(initialAllocSize);
}

BList::BList(const BList &other)
{
	data = new std::vector<void *>(*DATA_PTR(other.data));
}

BList::~BList()
{
	delete DATA;
}

BList &BList::operator=(const BList &from)
{
	*DATA = *DATA_PTR(from.data);
	return *this;
}

bool BList::AddItem(void *item)
{
	DATA->push_back(item);
	return true;
}

bool BList::AddItem(void *item, int32 atIndex)
{
	auto pos = DATA->begin() + atIndex;
	return !(pos == DATA->end() || DATA->insert(pos, item) == DATA->end());
}

bool BList::AddList(BList *newItems)
{
	auto newSize = DATA->size() + DATA_PTR(newItems)->size();
	DATA->resize(newSize);
	for (size_t source = 0, dest = DATA->size(); dest < newSize; source++, dest++) {
		(*DATA)[dest] = (*DATA_PTR(newItems))[source];
	}
	return true;
}

bool BList::AddList(BList *newItems, int32 atIndex)
{
	auto destSize	= DATA->size();
	auto sourceSize = DATA_PTR(newItems)->size();
	if (sourceSize == 0) return true;  // nothing to copy

	if (atIndex > destSize) return false;

	auto newSize = destSize + sourceSize;
	DATA->resize(newSize);

	if (atIndex < destSize) {
		// move existing items to make place
		for (size_t source = sourceSize - 1, dest = newSize - 1; source >= 0; source--, dest--) {
			(*DATA)[dest] = (*DATA)[source];
		}
	}

	for (size_t source = 0, dest = destSize; dest < newSize; source++, dest++) {
		(*DATA)[dest] = (*DATA_PTR(newItems))[source];
	}
	return true;
}

bool BList::RemoveItem(void *item)
{
	auto pos = std::find(DATA->begin(), DATA->end(), item);
	if (pos == DATA->end()) return false;

	DATA->erase(pos);
	return true;
}

void *BList::RemoveItem(int32 index)
{
	auto pos = DATA->begin() + index;
	if (pos == DATA->end()) return nullptr;

	auto value = *pos;
	DATA->erase(pos);
	return value;
}

bool BList::RemoveItems(int32 index, int32 count)
{
	auto begin = DATA->begin() + index;
	if (begin == DATA->end()) return false;

	DATA->erase(begin, begin + count);
	return true;
}

void BList::MakeEmpty()
{
	DATA->clear();
}

void BList::SortItems(int (*cmp)(const void *, const void *))
{
	std::sort(DATA->begin(), DATA->end(), cmp);
}

void *BList::ItemAt(int32 index) const
{
	try {
		return DATA->at(index);
	}
	catch (std::out_of_range const &) {
		return nullptr;
	}
}

void *BList::FirstItem() const
{
	return DATA->empty() ? nullptr : DATA->front();
}

void *BList::LastItem() const
{
	return DATA->empty() ? nullptr : DATA->back();
}

void **BList::Items() const
{
	return DATA->data();
}

bool BList::HasItem(void *item) const
{
	auto pos = std::find(DATA->begin(), DATA->end(), item);
	return pos != DATA->end();
}

int32 BList::IndexOf(void *item) const
{
	auto pos = std::find(DATA->begin(), DATA->end(), item);
	return pos == DATA->end() ? -1 : pos - DATA->begin();
}

int32 BList::CountItems() const
{
	return DATA->size();
}

bool BList::IsEmpty() const
{
	return DATA->empty();
}

void BList::DoForEach(bool (*func)(void *))
{
	for (auto item : *DATA) {
		func(item);
	}
}

void BList::DoForEach(bool (*func)(void *, void *), void *arg)
{
	for (auto item : *DATA) {
		func(item, arg);
	}
}
