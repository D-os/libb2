#include "Message.h"

#include <ctype.h>
#include <doctest/doctest.h>
#include <stdlib.h>
#include <string.h>

#include <forward_list>
#include <utility>
#include <vector>

namespace {
typedef std::vector<std::pair<ssize_t, const void *>> NodeData;

struct Node
{
	const char *name;
	type_code	type;
	NodeData	data;
};
}  // namespace

#define DATA_PTR(data) reinterpret_cast<std::forward_list<Node> *>(data)
#define DATA DATA_PTR(this->data)

BMessage::BMessage() : data(nullptr)
{
}

BMessage::BMessage(uint32 what) : what(what), data(nullptr)
{
}

BMessage::BMessage(const BMessage &a_message) : what(a_message.what)
{
	// FIXME: copy a_message.data
	debugger(__PRETTY_FUNCTION__);
}

BMessage::~BMessage()
{
	// FIXME: free() all data in node_list.data.1
}

int32 BMessage::CountNames(type_code type) const
{
	int32 count = 0;
	if (DATA)
		for (auto &node : *DATA) {
			if (type == B_ANY_TYPE || type == node.type) count++;
		}
	return count;
}

bool BMessage::IsEmpty() const
{
	return !DATA || DATA->empty();
}

bool BMessage::IsSystem() const
{
	char a = char(what >> 24);
	char b = char(what >> 16);
	char c = char(what >> 8);
	char d = char(what);

	// The BeBook says:
	//              ... we've adopted a strict convention for assigning values to all
	//              Be-defined constants.  The value assigned will always be formed by
	//              combining four characters into a multicharacter constant, with the
	//              characters limited to uppercase letters and the underbar
	// Between that and what's in AppDefs.h, this algo seems like a safe bet:
	if (a == '_' && isupper(b) && isupper(c) && isupper(d))
		return true;

	return false;
}

status_t BMessage::AddData(const char *name, type_code type, const void *data,
						   ssize_t numBytes, bool is_fixed_size, int32 count)
{
	if (!this->data) this->data = new std::forward_list<Node>();

	Node *node = nullptr;
	for (auto &el : *DATA) {
		if (strncmp(el.name, name, B_FIELD_NAME_LENGTH) == 0) {
			if (el.type != type) return B_BAD_TYPE;

			node = &el;
			break;
		}
	}
	if (!node) {
		const char *new_name = strndup(name, B_FIELD_NAME_LENGTH);
		Node		new_node{new_name, type, NodeData(count)};
		DATA->push_front(std::move(new_node));
		node = &DATA->front();
	}

	void *data_copy = malloc(numBytes);
	if (!data_copy) return B_NO_MEMORY;
	memcpy(data_copy, data, numBytes);
	node->data.push_back({numBytes, data_copy});

	return B_OK;
}

TEST_SUITE("BMessage")
{
	TEST_CASE("Constructing")
	{
		BMessage empty;
		empty.what = 0;
		CHECK(empty.what == 0);

		BMessage one{123};
		CHECK(one.what == 123);

		BMessage chr('_TST');
		CHECK(chr.what == '_TST');
	}

	TEST_CASE("AddData")
	{
		BMessage test;
		CHECK(test.IsEmpty());
		CHECK(test.CountNames(B_ANY_TYPE) == 0);

		test.AddData("test", B_STRING_TYPE, "something", 4);
		CHECK_FALSE(test.IsEmpty());
		CHECK(test.CountNames(B_ANY_TYPE) == 1);
		CHECK(test.CountNames(B_STRING_TYPE) == 1);
	}
}
