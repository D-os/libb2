#include "String.h"

#include <doctest/doctest.h>

#include <sstream>
#include <string>

#include "./utf8_functions.h"

const char *B_EMPTY_STRING = "";

// helper function, returns minimum of two given values (but clamps to 0):
static inline int32 min_clamp0(int32 num1, int32 num2)
{
	if (num1 < num2)
		return num1 > 0 ? num1 : 0;

	return num2 > 0 ? num2 : 0;
}

//! Returns length of given string (but clamps to given maximum).
static inline int32 strlen_clamp(const char *string, int32 max)
{
	// this should yield 0 for max<0:
	return max <= 0 ? 0 : strnlen(string, max);
}

#define DATA_PTR(data) reinterpret_cast<std::string *>(data)
#define DATA DATA_PTR(this->data)

BString::BString() : data(new std::string())
{
}

BString::BString(const char *str) : data(new std::string(str))
{
}

BString::BString(const BString &other) : data(new std::string(*DATA_PTR(other.data)))
{
}

BString::BString(const char *str, int32 maxLength) : data(new std::string(str, maxLength))
{
}

BString::~BString()
{
	delete DATA;
}

const char *BString::String() const
{
	return DATA->c_str();
}

int32 BString::Length() const
{
	return DATA->length();
}

int32 BString::CountChars() const
{
	size_t		count = 0;
	const char *p	  = DATA->c_str();

	while (*p != 0) {
		// skip every byte that is an UTF-8 continuation
		if ((*p & 0xc0) != 0x80)
			++count;
		++p;
	}

	return count;
}

BString &BString::operator=(const BString &from)
{
	*DATA = *DATA_PTR(from.data);
	return *this;
}

BString &BString::operator=(const char *str)
{
	*DATA = str;
	return *this;
}

BString &BString::operator=(char ch)
{
	*DATA = ch;
	return *this;
}

BString &BString::SetTo(const char *str)
{
	DATA->assign(str);
	return *this;
}

BString &BString::SetTo(const char *str, int32 length)
{
	DATA->assign(str, length);
	return *this;
}

BString &BString::SetTo(const BString &from)
{
	DATA->assign(*DATA_PTR(from.data));
	return *this;
}

BString &BString::SetTo(const BString &from, int32 length)
{
	DATA->assign(*DATA_PTR(from.data), length);
	return *this;
}

BString &BString::SetTo(char ch, int32 count)
{
	DATA->assign(count, ch);
	return *this;
}

BString &BString::CopyInto(BString &into, int32 fromOffset, int32 length) const
{
	if (this != &into)
		into.SetTo(DATA->c_str() + fromOffset, length);
	return into;
}

void BString::CopyInto(char *into, int32 fromOffset, int32 length) const
{
	if (into) {
		length = min_clamp0(length, Length() - fromOffset);
		memcpy(into, DATA->c_str() + fromOffset, length);
	}
}

BString &BString::Adopt(BString &from)
{
	data	  = from.data;
	from.data = new std::string();
	return *this;
}

BString &BString::Adopt(BString &from, int32 length)
{
	Adopt(from);
	Truncate(length, false);
	return *this;
}

#pragma mark - Appending

BString &BString::operator+=(const char *string)
{
	if (string) {
		*DATA += string;
	}
	return *this;
}

BString &BString::operator+=(char c)
{
	*DATA += c;
	return *this;
}

BString &BString::Append(const BString &string, int32 length)
{
	if (&string != this) {
		length = min_clamp0(length, string.Length());
		if (length > 0)
			DATA->append(DATA_PTR(string.data)->c_str(), length);
	}
	return *this;
}

BString &BString::Append(const char *string, int32 length)
{
	if (string) {
		length = strlen_clamp(string, length);
		if (length > 0)
			DATA->append(string, length);
	}
	return *this;
}

BString &BString::Append(char c, int32 count)
{
	if (count > 0)
		DATA->append(count, c);
	return *this;
}

BString &BString::AppendChars(const BString &string, int32 charCount)
{
	return Append(string, UTF8CountBytes(string.String(), charCount));
}

BString &BString::AppendChars(const char *string, int32 charCount)
{
	return Append(string, UTF8CountBytes(string, charCount));
}

#pragma mark - Prepending

BString &BString::Prepend(const char *string)
{
	if (string)
		DATA->insert(0, string);
	return *this;
}

BString &BString::Prepend(const BString &string)
{
	if (&string != this)
		DATA->insert(0, DATA_PTR(string.data)->c_str());
	return *this;
}

BString &BString::Prepend(const char *string, int32 length)
{
	if (string)
		DATA->insert(0, string, strlen_clamp(string, length));
	return *this;
}

BString &BString::Prepend(const BString &string, int32 length)
{
	if (&string != this)
		DATA->insert(0, DATA_PTR(string.data)->c_str(), min_clamp0(length, string.Length()));
	return *this;
}

BString &BString::Prepend(char c, int32 count)
{
	if (count > 0)
		DATA->insert(0, count, c);
	return *this;
}

BString &BString::PrependChars(const char *string, int32 charCount)
{
	return Prepend(string, UTF8CountBytes(string, charCount));
}

BString &BString::PrependChars(const BString &string, int32 charCount)
{
	return Prepend(string, UTF8CountBytes(string.String(), charCount));
}

#pragma mark - Inserting

BString &BString::Insert(const char *string, int32 position)
{
	if (string && position <= Length()) {
		DATA->insert(position, string);
	}
	return *this;
}

BString &BString::Insert(const char *string, int32 length, int32 position)
{
	if (string && position <= Length()) {
		DATA->insert(position, string, length);
	}
	return *this;
}

BString &BString::Insert(const char *string, int32 fromOffset, int32 length, int32 position)
{
	if (string)
		Insert(string + fromOffset, length, position);
	return *this;
}

BString &BString::Insert(const BString &string, int32 position)
{
	if (&string != this && string.Length() > 0)
		Insert(DATA_PTR(string.data)->c_str(), position);
	return *this;
}

BString &BString::Insert(const BString &string, int32 length, int32 position)
{
	if (&string != this && string.Length() > 0)
		Insert(string.String(), length, position);
	return *this;
}

BString &BString::Insert(const BString &string, int32 fromOffset, int32 length, int32 position)
{
	if (&string != this && string.Length() > 0)
		Insert(string.String() + fromOffset, length, position);
	return *this;
}

BString &BString::Insert(char c, int32 count, int32 position)
{
	if (position < 0) {
		count	 = MAX(count + position, 0);
		position = 0;
	}
	else
		position = min_clamp0(position, Length());

	if (count > 0)
		DATA->insert(position, count, c);

	return *this;
}

BString &BString::InsertChars(const char *string, int32 charPosition)
{
	return Insert(string, UTF8CountBytes(DATA->c_str(), charPosition));
}

BString &BString::InsertChars(const char *string, int32 charCount, int32 charPosition)
{
	return Insert(string, UTF8CountBytes(string, charCount),
				  UTF8CountBytes(DATA->c_str(), charPosition));
}

BString &BString::InsertChars(const char *string, int32 fromCharOffset, int32 charCount, int32 charPosition)
{
	int32 fromOffset = UTF8CountBytes(string, fromCharOffset);
	return Insert(string, fromOffset,
				  UTF8CountBytes(string + fromOffset, charCount),
				  UTF8CountBytes(DATA->c_str(), charPosition));
}

BString &BString::InsertChars(const BString &string, int32 charPosition)
{
	return Insert(string, UTF8CountBytes(DATA->c_str(), charPosition));
}

BString &BString::InsertChars(const BString &string, int32 charCount, int32 charPosition)
{
	return Insert(string, UTF8CountBytes(string.String(), charCount),
				  UTF8CountBytes(DATA->c_str(), charPosition));
}

BString &BString::InsertChars(const BString &string, int32 fromCharOffset, int32 charCount, int32 charPosition)
{
	int32 fromOffset = UTF8CountBytes(string.String(), fromCharOffset);
	return Insert(string, fromOffset,
				  UTF8CountBytes(string.String() + fromOffset, charCount),
				  UTF8CountBytes(DATA->c_str(), charPosition));
}

#pragma mark - Removing

BString &BString::Truncate(int32 charCount, bool lazy)
{
	if (charCount < DATA->size()) DATA->resize(charCount);
	if (!lazy) DATA->shrink_to_fit();
	return *this;
}

BString &BString::Remove(int32 from, int32 length)
{
	if (length > 0 && from < Length())
		DATA->erase(from, length);
	return *this;
}

BString &BString::RemoveChars(int32 fromCharOffset, int32 charCount)
{
	auto  str		 = DATA->c_str();
	int32 fromOffset = UTF8CountBytes(str, fromCharOffset);
	return Remove(fromOffset, UTF8CountBytes(str + fromOffset, charCount));
}

bool BString::operator<(const BString &value) const
{
	return *DATA < *DATA_PTR(value.data);
}

bool BString::operator<=(const BString &value) const
{
	return *DATA <= *DATA_PTR(value.data);
}

bool BString::operator==(const BString &value) const
{
	return *DATA == *DATA_PTR(value.data);
}

bool BString::operator>=(const BString &value) const
{
	return *DATA >= *DATA_PTR(value.data);
}

bool BString::operator>(const BString &value) const
{
	return *DATA > *DATA_PTR(value.data);
}

bool BString::operator!=(const BString &value) const
{
	return *DATA != *DATA_PTR(value.data);
}

bool BString::operator<(const char *value) const
{
	return *DATA < value;
}

bool BString::operator<=(const char *value) const
{
	return *DATA <= value;
}

bool BString::operator==(const char *value) const
{
	return *DATA == value;
}

bool BString::operator>=(const char *value) const
{
	return *DATA >= value;
}

bool BString::operator>(const char *value) const
{
	return *DATA > value;
}

bool BString::operator!=(const char *value) const
{
	return *DATA != value;
}

BString &BString::operator<<(const char *value)
{
	DATA->append(value);
	return *this;
}

BString &BString::operator<<(const BString &value)
{
	DATA->append(*DATA_PTR(value.data));
	return *this;
}

BString &BString::operator<<(char value)
{
	std::string ch;
	ch.assign(1, value);
	DATA->append(ch);
	return *this;
}

BString &BString::operator<<(uint32 value)
{
	std::ostringstream str;
	str << value;
	DATA->append(str.str());
	return *this;
}

BString &BString::operator<<(int32 value)
{
	std::ostringstream str;
	str << value;
	DATA->append(str.str());
	return *this;
}

BString &BString::operator<<(uint64 value)
{
	std::ostringstream str;
	str << value;
	DATA->append(str.str());
	return *this;
}

BString &BString::operator<<(int64 value)
{
	std::ostringstream str;
	str << value;
	DATA->append(str.str());
	return *this;
}

BString &BString::operator<<(float value)
{
	std::ostringstream str;
	str << value;
	DATA->append(str.str());
	return *this;
}

std::ostream &operator<<(std::ostream &os, const BString &value)
{
	os << value.String();
	return os;
}

TEST_SUITE("BString")
{
	using namespace std::literals;

	TEST_CASE("Constructing")
	{
		BString empty;
		CHECK(strlen(empty.String()) == 0);

		BString string("test");
		CHECK(string.String() == "test"sv);

		BString copy(string);
		string.Truncate(0);
		CHECK(std::string(copy.String()) == "test");

		BString limit("some test", 4);
		CHECK(std::string_view(limit.String()) == "some");
	}
	TEST_CASE("Comparison")
	{
		const char *str = "tst";
		CHECK(std::string_view(BString("tsa").String()) < str);
		CHECK(std::string_view(BString("tsa").String()) <= str);
		CHECK(std::string_view(BString("tst").String()) <= str);
		CHECK(std::string_view(BString("tst").String()) == str);
		CHECK(std::string_view(BString("tst").String()) >= str);
		CHECK(std::string_view(BString("tsz").String()) >= str);
		CHECK(std::string_view(BString("tsz").String()) > str);
		CHECK(std::string_view(BString("foo").String()) != str);
		CHECK_FALSE(std::string_view(BString("tst").String()) != str);
		CHECK_FALSE(std::string_view(BString("foo").String()) == str);
		const BString bst(str);
		CHECK(std::string_view(BString("tsa").String()) < std::string_view(bst.String()));
		CHECK(std::string_view(BString("tsa").String()) <= std::string_view(bst.String()));
		CHECK(std::string_view(BString("tst").String()) <= std::string_view(bst.String()));
		CHECK(std::string_view(BString("tst").String()) == std::string_view(bst.String()));
		CHECK(std::string_view(BString("tst").String()) >= std::string_view(bst.String()));
		CHECK(std::string_view(BString("tsz").String()) >= std::string_view(bst.String()));
		CHECK(std::string_view(BString("tsz").String()) > std::string_view(bst.String()));
		CHECK(std::string_view(BString("foo").String()) != std::string_view(bst.String()));
		CHECK_FALSE(std::string_view(BString("tst").String()) != std::string_view(bst.String()));
		CHECK_FALSE(std::string_view(BString("foo").String()) == std::string_view(bst.String()));
	}
	TEST_CASE("Assigning")
	{
		BString string("construct");
		CHECK(string == "construct");

		string.SetTo("SetTo");
		CHECK(string == "SetTo");

		string = "assign";
		CHECK(string == "assign");

		string.Truncate(0);
		string << (int32)8 << " Bits";
		CHECK(string == "8 Bits");
	}
}
