#include "Message.h"

#include <ctype.h>
#include <doctest/doctest.h>
#include <stdlib.h>
#include <string.h>

#include <forward_list>
#include <utility>
#include <vector>

#include "Errors.h"

typedef std::vector<std::pair<ssize_t, const void *const>> NodeData;

struct Node
{
	const char *const name;
	type_code		  type;
	NodeData		  data;
};

class BMessage::impl
{
	std::forward_list<Node> *m_nodes;

   public:
	impl() : m_nodes{nullptr} {}

	~impl()
{
		clearNodes();
}

	bool hasNodes() const
	{
		return m_nodes;
	}

	std::forward_list<Node> &nodes()
	{
		if (m_nodes == nullptr) m_nodes = new std::forward_list<Node>();
		return *m_nodes;
	}

	status_t pushNode(int32 count, const char *const name, type_code type,
					  ssize_t size, const void *const data);

	void clearNodes()
	{
		if (hasNodes()) {
			for (auto &node : nodes()) {
				for (auto &d : node.data) {
					free(const_cast<void *>(d.second));
				}
				free(const_cast<char *>(node.name));
			}

			delete m_nodes;
			m_nodes = nullptr;
		}
	}
};

status_t BMessage::impl::pushNode(int32 count, const char *const name, type_code type,
								  ssize_t size, const void *const data)
{
	auto &nodes = this->nodes();

	Node *node = nullptr;
	for (auto &el : nodes) {
		if (strncmp(el.name, name, B_FIELD_NAME_LENGTH) == 0) {
			if (el.type != type) return B_BAD_TYPE;

			node = &el;
			break;
		}
	}
	if (!node) {
		const char *new_name = strndup(name, B_FIELD_NAME_LENGTH);
		Node		new_node{new_name, type};
		new_node.data.reserve(count);
		nodes.push_front(std::move(new_node));
		node = &nodes.front();
	}

	node->data.push_back({size, data});
	return B_OK;
}

BMessage::BMessage()
{
}

BMessage::BMessage(uint32 what) : what(what)
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
	if (m->hasNodes())
		for (auto &node : m->nodes()) {
			if (type == B_ANY_TYPE || type == node.type) count++;
		}
	return count;
}

bool BMessage::IsEmpty() const
{
	return !m->hasNodes() || m->nodes().empty();
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

status_t BMessage::Unflatten(BDataIO *stream)
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

status_t BMessage::AddData(const char *name, type_code type, const void *data,
						   ssize_t num_bytes, bool is_fixed_size, int32 count)
{
	auto name_copy = strndup(name, B_FIELD_NAME_LENGTH);
	if (!name_copy) return B_NO_MEMORY;

	auto data_copy = malloc(num_bytes);
	if (!data_copy) return B_NO_MEMORY;
	memcpy(data_copy, data, num_bytes);

	return m->pushNode(count, name_copy, type, num_bytes, data_copy);
}

status_t BMessage::RemoveData(const char *name, int32 index)
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

status_t BMessage::RemoveName(const char *name)
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

status_t BMessage::MakeEmpty()
{
	m->clearNodes();
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

		test.MakeEmpty();
		CHECK(test.IsEmpty());
		CHECK(test.CountNames(B_ANY_TYPE) == 0);
	}
}
