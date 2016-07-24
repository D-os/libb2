/*
 * Copyright 2011-2013, Ingo Weinhold, ingo_weinhold@gmx.de
 * Copyright 2011, Clemens Zeidler <haiku@clemens-zeidler.de>
 *
 * Distributed under the terms of the MIT License.
 */


#include <StringList.h>

#include <algorithm>

#include <StringPrivate.h>
#include <TypeConstants.h>


static int
compare_private_data(const void* a, const void* b)
{
	return BString::Private::StringFromData(*(char**)a).Compare(
		BString::Private::StringFromData(*(char**)b));
}


static int
compare_private_data_ignore_case(const void* a, const void* b)
{
	return BString::Private::StringFromData(*(char**)a).ICompare(
		BString::Private::StringFromData(*(char**)b));
}


// #pragma mark - BStringList


BStringList::BStringList(int32 count)
	:
	fStrings(count)
{
}


BStringList::BStringList(const BStringList& other)
	:
	fStrings(other.fStrings)
{
	_IncrementRefCounts();
}


BStringList::~BStringList()
{
	_DecrementRefCounts();
}


bool
BStringList::Add(const BString& _string, int32 index)
{
	BString string(_string);
		// makes sure the string is shareable
	if (string.Length() != _string.Length())
		return false;

	char* privateData = BString::Private(string).Data();
	if (!fStrings.AddItem(privateData, index))
		return false;

	BString::Private::IncrementDataRefCount(privateData);
	return true;
}


bool
BStringList::Add(const BString& _string)
{
	BString string(_string);
		// makes sure the string is shareable
	if (string.Length() != _string.Length())
		return false;

	char* privateData = BString::Private(string).Data();
	if (!fStrings.AddItem(privateData))
		return false;

	BString::Private::IncrementDataRefCount(privateData);
	return true;
}


bool
BStringList::Add(const BStringList& list, int32 index)
{
	if (!fStrings.AddList(&list.fStrings, index))
		return false;

	list._IncrementRefCounts();
	return true;
}


bool
BStringList::Add(const BStringList& list)
{
	if (!fStrings.AddList(&list.fStrings))
		return false;

	list._IncrementRefCounts();
	return true;
}


bool
BStringList::Remove(const BString& string, bool ignoreCase)
{
	bool result = false;
	int32 count = fStrings.CountItems();

	if (ignoreCase) {
		int32 length = string.Length();

		for (int32 i = count - 1; i >= 0; i--) {
			BString element(StringAt(i));
			if (length == element.Length() && string.ICompare(element) == 0) {
				Remove(i);
				result = true;
			}
		}
	} else {
		for (int32 i = count - 1; i >= 0; i--) {
			if (string == StringAt(i)) {
				Remove(i);
				result = true;
			}
		}
	}

	return result;
}


bool
BStringList::Remove(const BStringList& list, bool ignoreCase)
{
	bool removedAnything = false;
	int32 stringCount = list.CountStrings();
	for (int32 i = 0; i < stringCount; i++)
		removedAnything |= Remove(list.StringAt(i), ignoreCase);

	return removedAnything;
}


BString
BStringList::Remove(int32 index)
{
	if (index < 0 || index >= fStrings.CountItems())
		return BString();

	char* privateData = (char*)fStrings.RemoveItem(index);
	BString string(BString::Private::StringFromData(privateData));
	BString::Private::DecrementDataRefCount(privateData);
	return string;
}


bool
BStringList::Remove(int32 index, int32 count)
{
	int32 stringCount = fStrings.CountItems();
	if (index < 0 || index > stringCount)
		return false;

	int32 end = index + std::min(stringCount - index, count);
	for (int32 i = index; i < end; i++)
		BString::Private::DecrementDataRefCount((char*)fStrings.ItemAt(i));

	fStrings.RemoveItems(index, end - index);
	return true;
}


bool
BStringList::Replace(int32 index, const BString& string)
{
	if (index < 0 || index >= fStrings.CountItems())
		return false;

	BString::Private::DecrementDataRefCount((char*)fStrings.ItemAt(index));

	char* privateData = BString::Private(string).Data();
	BString::Private::IncrementDataRefCount(privateData);
	fStrings.ReplaceItem(index, privateData);

	return true;
}


void
BStringList::MakeEmpty()
{
	_DecrementRefCounts();
	fStrings.MakeEmpty();
}


void
BStringList::Sort(bool ignoreCase)
{
	fStrings.SortItems(ignoreCase
		? compare_private_data_ignore_case : compare_private_data);
}


bool
BStringList::Swap(int32 indexA, int32 indexB)
{
	return fStrings.SwapItems(indexA, indexB);
}


bool
BStringList::Move(int32 fromIndex, int32 toIndex)
{
	return fStrings.MoveItem(fromIndex, toIndex);
}


BString
BStringList::StringAt(int32 index) const
{
	return BString::Private::StringFromData((char*)fStrings.ItemAt(index));
}


BString
BStringList::First() const
{
	return BString::Private::StringFromData((char*)fStrings.FirstItem());
}


BString
BStringList::Last() const
{
	return BString::Private::StringFromData((char*)fStrings.LastItem());
}


int32
BStringList::IndexOf(const BString& string, bool ignoreCase) const
{
	int32 count = fStrings.CountItems();

	if (ignoreCase) {
		int32 length = string.Length();

		for (int32 i = 0; i < count; i++) {
			BString element(StringAt(i));
			if (length == element.Length() && string.ICompare(element) == 0)
				return i;
		}
	} else {
		for (int32 i = 0; i < count; i++) {
			if (string == StringAt(i))
				return i;
		}
	}

	return -1;
}


int32
BStringList::CountStrings() const
{
	return fStrings.CountItems();
}


bool
BStringList::IsEmpty() const
{
	return fStrings.IsEmpty();
}


BString
BStringList::Join(const char* separator, int32 length) const
{
	return _Join(separator,
		length >= 0 ? strnlen(separator, length) : strlen(separator));
}


void
BStringList::DoForEach(bool (*func)(const BString& string))
{
	int32 count = fStrings.CountItems();
	for (int32 i = 0; i < count; i++)
		func(StringAt(i));
}


void
BStringList::DoForEach(bool (*func)(const BString& string, void* arg2),
	void* arg2)
{
	int32 count = fStrings.CountItems();
	for (int32 i = 0; i < count; i++)
		func(StringAt(i), arg2);
}


BStringList&
BStringList::operator=(const BStringList& other)
{
	if (this != &other) {
		_DecrementRefCounts();
		fStrings = other.fStrings;
		_IncrementRefCounts();
	}

	return *this;
}


bool
BStringList::operator==(const BStringList& other) const
{
	if (this == &other)
		return true;

	int32 count = fStrings.CountItems();
	if (count != other.fStrings.CountItems())
		return false;

	for (int32 i = 0; i < count; i++) {
		if (StringAt(i) != other.StringAt(i))
			return false;
	}

	return true;
}


bool
BStringList::IsFixedSize() const
{
	return false;
}


type_code
BStringList::TypeCode() const
{
	return B_STRING_LIST_TYPE;
}



bool
BStringList::AllowsTypeCode(type_code code) const
{
	return code == B_STRING_LIST_TYPE;
}


ssize_t
BStringList::FlattenedSize() const
{
	ssize_t size = 0;
	int32 count = CountStrings();
	for (int32 i = 0; i < count; i++)
		size += StringAt(i).Length() + 1;

	return size;
}


status_t
BStringList::Flatten(void* buf, ssize_t size) const
{
	const char* buffer = (const char*)buf;

	if (size < FlattenedSize())
		return B_NO_MEMORY;

	int32 count = CountStrings();
	for (int32 i = 0; i < count; i++) {
		BString item = StringAt(i);
		ssize_t storeSize = item.Length() + 1;
		memcpy((void*)buffer, (const void*)item.String(), storeSize);
		buffer += storeSize;
	}

	return B_OK;
}


status_t
BStringList::Unflatten(type_code code, const void* buffer, ssize_t size)
{
	if (code != B_STRING_LIST_TYPE)
		return B_ERROR;
	const char* bufferStart = (const char*)buffer;

	MakeEmpty();

	off_t offset = 0;
	while (offset < size) {
		const char* string = bufferStart + offset;
		size_t restSize = size - offset;
		size_t read = strnlen(string, restSize);
		if (read == restSize)
			return B_BAD_VALUE;

		if (!Add(string))
			return B_NO_MEMORY;
		offset += read + 1;
	}

	return B_OK;
}


void
BStringList::_IncrementRefCounts() const
{
	int32 count = fStrings.CountItems();
	for (int32 i = 0; i < count; i++) {
		BString::Private::IncrementDataRefCount((char*)fStrings.ItemAt(i));
	}
}


void
BStringList::_DecrementRefCounts() const
{
	int32 count = fStrings.CountItems();
	for (int32 i = 0; i < count; i++)
		BString::Private::DecrementDataRefCount((char*)fStrings.ItemAt(i));
}


BString
BStringList::_Join(const char* separator, int32 length) const
{
	// handle simple cases (0 or 1 element)
	int32 count = CountStrings();
	if (count == 0)
		return BString();
	if (count == 1)
		return StringAt(0);

	// determine the total length
	int32 totalLength = length * (count - 1);
	for (int32 i = 0; i < count; i++)
		totalLength += StringAt(i).Length();

	// compose the result string
	BString result;
	char* buffer = result.LockBuffer(totalLength);
	if (buffer == NULL)
		return result;

	for (int32 i = 0; i < count; i++) {
		if (i > 0 && length > 0) {
			memcpy(buffer, separator, length);
			buffer += length;
		}

		BString string = StringAt(i);
		memcpy(buffer, string.String(), string.Length());
		buffer += string.Length();
	}

	return result.UnlockBuffer(totalLength);
}
