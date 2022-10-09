#include "String.h"

#include <doctest/doctest.h>

#include <sstream>
#include <string>

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

BString &BString::Truncate(int32 charCount, bool lazy)
{
	if (charCount < DATA->size()) DATA->resize(charCount);
	if (!lazy) DATA->shrink_to_fit();
	return *this;
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

BString &BString::operator<<(int value)
{
	std::ostringstream str;
	str << value;
	DATA->append(str.str());
	return *this;
}

BString &BString::operator<<(unsigned int value)
{
	std::ostringstream str;
	str << value;
	DATA->append(str.str());
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
