#include "Message.h"

#include <Messenger.h>
#include <Point.h>
#include <Rect.h>
#include <String.h>
#include <doctest/doctest.h>
#include <pimpl.h>

#include <cstring>
#include <iomanip>
#include <iostream>
#include <vector>

struct DataItem
{
	ssize_t		size;
	const void *data;

	DataItem(ssize_t size, const void *data) : size(size), data(data) {}

	DataItem(const DataItem &)			  = delete;
	DataItem &operator=(const DataItem &) = delete;

	DataItem(DataItem &&source) : size(source.size), data(source.data)
	{
		source.size = 0;
		source.data = nullptr;
	}

	~DataItem()
	{
		if (data) free(const_cast<void *>(data));
	}
};

struct Node
{
	std::string			  name;
	type_code			  type;
	std::vector<DataItem> data;
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

	impl &operator=(const impl &other)
	{
		clearNodes();
		if (other.m_nodes) {
			for (auto &node : *other.m_nodes) {
				const auto data_size = node.data.size();
				for (auto &d : node.data) {
					addNode(data_size, node.name.c_str(), node.type, d.size, d.data);
				}
			}
		}
		return *this;
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

	/// Copy given data to NodeData
	status_t addNode(int32 count, const char *const name, type_code type,
					 ssize_t size, const void *const data);

	/// Takes ownership of given *data into NodeData
	/// Always make copy of *name string
	status_t pushNode(int32 count, const char *const name, type_code type,
					  ssize_t size, const void *const data);

	void clearNodes()
	{
		if (m_nodes) {
			delete m_nodes;
			m_nodes = nullptr;
		}
	}
};

status_t BMessage::impl::addNode(int32 count, const char *const name, type_code type,
								 ssize_t size, const void *const data)
{
	auto data_copy = malloc(size);
	if (!data_copy) return B_NO_MEMORY;
	memcpy(data_copy, data, size);

	return pushNode(count, name, type, size, data_copy);
}

status_t BMessage::impl::pushNode(int32 count, const char *const name, type_code type,
								  ssize_t size, const void *const data)
{
	auto &nodes = this->nodes();

	Node *node = nullptr;
	for (auto &el : nodes) {
		if (strncmp(el.name.c_str(), name, B_FIELD_NAME_LENGTH) == 0) {
			if (el.type != type) {
				free(const_cast<void *>(data));
				return B_BAD_TYPE;
			}

			node = &el;
			break;
		}
	}
	if (!node) {
		Node new_node{std::string(name, B_FIELD_NAME_LENGTH), type};
		new_node.data.reserve(count);
		nodes.push_front(std::move(new_node));
		node = &nodes.front();
	}

	DataItem data_item(size, data);
	node->data.push_back(std::move(data_item));
	return B_OK;
}

BMessage::BMessage()
{
}

BMessage::BMessage(uint32 what) : what(what)
{
}

BMessage::BMessage(const BMessage &msg) : what(msg.what)
{
	*m = *msg.m;
}

BMessage::~BMessage()
{
	// FIXME: free() all data in node_list.data.1
}

BMessage &BMessage::operator=(const BMessage &msg)
{
	what = msg.what;
	*m	 = *msg.m;
	return *this;
}

int32 BMessage::CountNames(type_code type) const
{
	int32 count = 0;
	if (m->hasNodes())
		for (auto &node : m->nodes()) {
			if (type == B_ANY_TYPE || type == node.type) count += node.data.size();
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

void BMessage::PrintToStream() const
{
	std::cout << *this;
}

ssize_t BMessage::FlattenedSize() const
{
	debugger(__PRETTY_FUNCTION__);
	return -1;
}

status_t BMessage::Flatten(char *buffer, ssize_t size) const
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

status_t BMessage::Flatten(BDataIO *stream, ssize_t *size) const
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

status_t BMessage::Unflatten(const char *flat_buffer)
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

status_t BMessage::Unflatten(BDataIO *stream)
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

status_t BMessage::AddData(const char *name, type_code type, const void *data,
						   ssize_t num_bytes, bool is_fixed_size, int32 count)
{
	return m->addNode(count, name, type, num_bytes, data);
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

status_t BMessage::FindData(const char *name, type_code type, const void **data, ssize_t *numBytes) const
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

status_t BMessage::FindData(const char *name, type_code type, int32 index, const void **data, ssize_t *numBytes) const
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

status_t BMessage::ReplaceData(const char *name, type_code type, const void *data, ssize_t data_size)
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

status_t BMessage::ReplaceData(const char *name, type_code type, int32 index, const void *data, ssize_t data_size)
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

// #pragma mark - Macro definitions for data access methods

/* Relay functions from here on (Add... -> AddData, Find... -> FindData) */

#define DEFINE_FUNCTIONS(type, typeName, typeCode)                             \
	status_t                                                                   \
		BMessage::Add##typeName(const char *name, type val)                    \
	{                                                                          \
		return AddData(name, typeCode, &val, sizeof(type), true);              \
	}                                                                          \
                                                                               \
	status_t                                                                   \
		BMessage::Find##typeName(const char *name, type *p) const              \
	{                                                                          \
		return Find##typeName(name, 0, p);                                     \
	}                                                                          \
                                                                               \
	status_t                                                                   \
		BMessage::Find##typeName(const char *name, int32 index, type *p) const \
	{                                                                          \
		type	 *ptr   = nullptr;                                              \
		ssize_t	 bytes = 0;                                                    \
		status_t error = B_OK;                                                 \
                                                                               \
		*p	  = type();                                                        \
		error = FindData(name, typeCode, index, (const void **)&ptr, &bytes);  \
                                                                               \
		if (error == B_OK)                                                     \
			*p = *ptr;                                                         \
                                                                               \
		return error;                                                          \
	}                                                                          \
                                                                               \
	status_t                                                                   \
		BMessage::Replace##typeName(const char *name, type value)              \
	{                                                                          \
		return ReplaceData(name, typeCode, 0, &value, sizeof(type));           \
	}                                                                          \
                                                                               \
	status_t                                                                   \
		BMessage::Replace##typeName(const char *name, int32 index, type value) \
	{                                                                          \
		return ReplaceData(name, typeCode, index, &value, sizeof(type));       \
	}

DEFINE_FUNCTIONS(BPoint, Point, B_POINT_TYPE);
DEFINE_FUNCTIONS(BRect, Rect, B_RECT_TYPE);
DEFINE_FUNCTIONS(int8, Int8, B_INT8_TYPE);
DEFINE_FUNCTIONS(uint8, UInt8, B_UINT8_TYPE);
DEFINE_FUNCTIONS(int16, Int16, B_INT16_TYPE);
DEFINE_FUNCTIONS(uint16, UInt16, B_UINT16_TYPE);
DEFINE_FUNCTIONS(int32, Int32, B_INT32_TYPE);
DEFINE_FUNCTIONS(uint32, UInt32, B_UINT32_TYPE);
DEFINE_FUNCTIONS(int64, Int64, B_INT64_TYPE);
DEFINE_FUNCTIONS(uint64, UInt64, B_UINT64_TYPE);
DEFINE_FUNCTIONS(bool, Bool, B_BOOL_TYPE);
DEFINE_FUNCTIONS(float, Float, B_FLOAT_TYPE);
DEFINE_FUNCTIONS(double, Double, B_DOUBLE_TYPE);

#undef DEFINE_FUNCTIONS

status_t BMessage::AddString(const char *name, const char *string)
{
	return AddData(name, B_STRING_TYPE, string, string ? strlen(string) + 1 : 0,
				   false);
}

status_t BMessage::AddString(const char *name, const BString &string)
{
	return AddData(name, B_STRING_TYPE, string.String(), string.Length() + 1,
				   false);
}

status_t BMessage::AddPointer(const char *name, const void *pointer)
{
	return AddData(name, B_POINTER_TYPE, &pointer, sizeof(pointer), true);
}

status_t BMessage::AddMessenger(const char *name, BMessenger messenger)
{
	return AddData(name, B_MESSENGER_TYPE, &messenger, sizeof(messenger), true);
}

// status_t BMessage::AddRef(const char *name, const entry_ref *ref)
// {
// 	size_t size = sizeof(entry_ref) + B_PATH_NAME_LENGTH;
// 	char   buffer[size];

// 	status_t error = BPrivate::entry_ref_flatten(buffer, &size, ref);

// 	if (error >= B_OK)
// 		error = AddData(name, B_REF_TYPE, buffer, size, false);

// 	return error;
// }

status_t BMessage::AddMessage(const char *name, const BMessage *message)
{
	if (message == NULL)
		return B_BAD_VALUE;

	// TODO: This and the following functions waste time by allocating and
	// copying an extra buffer. Functions can be added that return a direct
	// pointer into the message.

	char	stackBuffer[16384];
	ssize_t size = message->FlattenedSize();

	char *buffer;
	if (size > (ssize_t)sizeof(stackBuffer)) {
		buffer = (char *)malloc(size);
		if (buffer == NULL)
			return B_NO_MEMORY;
	}
	else
		buffer = stackBuffer;

	status_t error = message->Flatten(buffer, size);

	if (error >= B_OK)
		error = AddData(name, B_MESSAGE_TYPE, buffer, size, false);

	if (buffer != stackBuffer)
		free(buffer);

	return error;
}

// status_t BMessage::AddFlat(const char *name, BFlattenable *object, int32 count)
// {
// 	return AddFlat(name, (const BFlattenable *)object, count);
// }

// status_t BMessage::AddFlat(const char *name, const BFlattenable *object, int32 count)
// {
// 	if (object == NULL)
// 		return B_BAD_VALUE;

// 	char	stackBuffer[16384];
// 	ssize_t size = object->FlattenedSize();

// 	char *buffer;
// 	if (size > (ssize_t)sizeof(stackBuffer)) {
// 		buffer = (char *)malloc(size);
// 		if (buffer == NULL)
// 			return B_NO_MEMORY;
// 	}
// 	else
// 		buffer = stackBuffer;

// 	status_t error = object->Flatten(buffer, size);

// 	if (error >= B_OK)
// 		error = AddData(name, object->TypeCode(), buffer, size, false);

// 	if (buffer != stackBuffer)
// 		free(buffer);

// 	return error;
// }

status_t BMessage::FindString(const char *name, const char **string) const
{
	return FindString(name, 0, string);
}

status_t BMessage::FindString(const char *name, int32 index, const char **string) const
{
	ssize_t bytes;
	return FindData(name, B_STRING_TYPE, index, (const void **)string, &bytes);
}

status_t BMessage::FindString(const char *name, BString *string) const
{
	return FindString(name, 0, string);
}

status_t BMessage::FindString(const char *name, int32 index, BString *string) const
{
	if (string == NULL)
		return B_BAD_VALUE;

	const char *value;
	status_t	error = FindString(name, index, &value);

	// Find*() clobbers the object even on failure
	string->SetTo(value);
	return error;
}

status_t BMessage::FindPointer(const char *name, void **pointer) const
{
	return FindPointer(name, 0, pointer);
}

status_t BMessage::FindPointer(const char *name, int32 index, void **pointer) const
{
	if (pointer == NULL)
		return B_BAD_VALUE;

	void	 **data  = NULL;
	ssize_t	 size  = 0;
	status_t error = FindData(name, B_POINTER_TYPE, index,
							  (const void **)&data, &size);

	if (error == B_OK)
		*pointer = *data;
	else
		*pointer = NULL;

	return error;
}

status_t BMessage::FindMessenger(const char *name, BMessenger *messenger) const
{
	return FindMessenger(name, 0, messenger);
}

status_t BMessage::FindMessenger(const char *name, int32 index,
								 BMessenger *messenger) const
{
	if (messenger == NULL)
		return B_BAD_VALUE;

	BMessenger *data  = NULL;
	ssize_t		size  = 0;
	status_t	error = FindData(name, B_MESSENGER_TYPE, index,
								 (const void **)&data, &size);

	if (error == B_OK)
		*messenger = *data;
	else
		*messenger = BMessenger();

	return error;
}

// status_t BMessage::FindRef(const char *name, entry_ref *ref) const
// {
// 	return FindRef(name, 0, ref);
// }

// status_t BMessage::FindRef(const char *name, int32 index, entry_ref *ref) const
// {
// 	if (ref == NULL)
// 		return B_BAD_VALUE;

// 	void	 *data  = NULL;
// 	ssize_t	 size  = 0;
// 	status_t error = FindData(name, B_REF_TYPE, index,
// 							  (const void **)&data, &size);

// 	if (error == B_OK)
// 		error = BPrivate::entry_ref_unflatten(ref, (char *)data, size);
// 	else
// 		*ref = entry_ref();

// 	return error;
// }

status_t BMessage::FindMessage(const char *name, BMessage *message) const
{
	return FindMessage(name, 0, message);
}

status_t BMessage::FindMessage(const char *name, int32 index, BMessage *message) const
{
	if (message == NULL)
		return B_BAD_VALUE;

	void	 *data  = NULL;
	ssize_t	 size  = 0;
	status_t error = FindData(name, B_MESSAGE_TYPE, index,
							  (const void **)&data, &size);

	if (error == B_OK)
		error = message->Unflatten((const char *)data);
	else
		*message = BMessage();

	return error;
}

// status_t BMessage::FindFlat(const char *name, BFlattenable *object) const
// {
// 	return FindFlat(name, 0, object);
// }

// status_t BMessage::FindFlat(const char *name, int32 index, BFlattenable *object) const
// {
// 	if (object == NULL)
// 		return B_BAD_VALUE;

// 	void	 *data	  = NULL;
// 	ssize_t	 numBytes = 0;
// 	status_t error	  = FindData(name, object->TypeCode(), index,
// 								 (const void **)&data, &numBytes);

// 	if (error == B_OK)
// 		error = object->Unflatten(object->TypeCode(), data, numBytes);

// 	return error;
// }

status_t BMessage::ReplaceString(const char *name, const char *string)
{
	if (string == NULL)
		return B_BAD_VALUE;

	return ReplaceData(name, B_STRING_TYPE, 0, string, strlen(string) + 1);
}

status_t BMessage::ReplaceString(const char *name, int32 index, const char *string)
{
	if (string == NULL)
		return B_BAD_VALUE;

	return ReplaceData(name, B_STRING_TYPE, index, string, strlen(string) + 1);
}

status_t BMessage::ReplaceString(const char *name, const BString &string)
{
	return ReplaceData(name, B_STRING_TYPE, 0, string.String(),
					   string.Length() + 1);
}

status_t BMessage::ReplaceString(const char *name, int32 index, const BString &string)
{
	return ReplaceData(name, B_STRING_TYPE, index, string.String(),
					   string.Length() + 1);
}

status_t BMessage::ReplacePointer(const char *name, const void *pointer)
{
	return ReplaceData(name, B_POINTER_TYPE, 0, &pointer, sizeof(pointer));
}

status_t BMessage::ReplacePointer(const char *name, int32 index, const void *pointer)
{
	return ReplaceData(name, B_POINTER_TYPE, index, &pointer, sizeof(pointer));
}

status_t BMessage::ReplaceMessenger(const char *name, BMessenger messenger)
{
	return ReplaceData(name, B_MESSENGER_TYPE, 0, &messenger,
					   sizeof(BMessenger));
}

status_t BMessage::ReplaceMessenger(const char *name, int32 index, BMessenger messenger)
{
	return ReplaceData(name, B_MESSENGER_TYPE, index, &messenger,
					   sizeof(BMessenger));
}

// status_t BMessage::ReplaceRef(const char *name, const entry_ref *ref)
// {
// 	return ReplaceRef(name, 0, ref);
// }

// status_t BMessage::ReplaceRef(const char *name, int32 index, const entry_ref *ref)
// {
// 	size_t size = sizeof(entry_ref) + B_PATH_NAME_LENGTH;
// 	char   buffer[size];

// 	status_t error = BPrivate::entry_ref_flatten(buffer, &size, ref);

// 	if (error >= B_OK)
// 		error = ReplaceData(name, B_REF_TYPE, index, buffer, size);

// 	return error;
// }

status_t BMessage::ReplaceMessage(const char *name, const BMessage *message)
{
	return ReplaceMessage(name, 0, message);
}

status_t BMessage::ReplaceMessage(const char *name, int32 index, const BMessage *message)
{
	if (message == NULL)
		return B_BAD_VALUE;

	ssize_t size = message->FlattenedSize();
	if (size < 0)
		return B_BAD_VALUE;

	char buffer[size];

	status_t error = message->Flatten(buffer, size);

	if (error >= B_OK)
		error = ReplaceData(name, B_MESSAGE_TYPE, index, &buffer, size);

	return error;
}

// status_t BMessage::ReplaceFlat(const char *name, BFlattenable *object)
// {
// 	return ReplaceFlat(name, 0, object);
// }

// status_t BMessage::ReplaceFlat(const char *name, int32 index, BFlattenable *object)
// {
// 	if (object == NULL)
// 		return B_BAD_VALUE;

// 	ssize_t size = object->FlattenedSize();
// 	if (size < 0)
// 		return B_BAD_VALUE;

// 	char buffer[size];

// 	status_t error = object->Flatten(buffer, size);

// 	if (error >= B_OK)
// 		error = ReplaceData(name, object->TypeCode(), index, &buffer, size);

// 	return error;
// }

std::ostream &operator<<(std::ostream &os, const DataItem &value)
{
	os << std::hex << std::setfill('0');
	size_t index = 0;

	do {
		os << std::setw(8) << index;

		for (int i = 0; i < 16; ++i) {
			if (i % 8 == 0) os << ' ';

			auto offset = index + i;
			if (offset < value.size)
				os << ' ' << std::setw(2) << (unsigned int)(*(((unsigned char *)value.data) + offset));
			else
				os << "   ";
		}

		os << "  ";
		for (int i = 0; i < 16; ++i) {
			auto offset = index + i;
			if (offset < value.size) {
				const char chr = *(((char *)value.data) + offset);
				if (isprint(chr))
					os << chr;
				else
					os << '.';
			}
		}

		os << "\n";
		index += 16;
	} while (index < value.size);

	return os;
}

namespace {
inline void type2buf(char buf[11], const uint32 *value)
{
	if (isprint(*(char *)value)) {
		buf[0] = '\'';
		buf[1] = *((char *)value + 3);
		buf[2] = *((char *)value + 2);
		buf[3] = *((char *)value + 1);
		buf[4] = *((char *)value + 0);
		buf[5] = '\'';
		buf[6] = 0;
	}
	else
		snprintf(buf, 11, "0x%" PRIx32, *value);
}
}  // namespace

std::ostream &operator<<(std::ostream &os, const BMessage &value)
{
	os << "BMessage(";

	char buf[11];
	type2buf(buf, &value.what);

	os << buf << ")" << std::endl;

	if (value.m->hasNodes()) {
		size_t index = 0;
		for (auto &node : value.m->nodes()) {
			type2buf(buf, &node.type);
			os << index << " " << node.name << ", type = " << buf << ", count = " << node.data.size() << std::endl;

			for (auto &d : node.data) {
				os << d;
			}

			index += 1;
		}
	}

	return os;
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

		const auto value = true;
		const auto ret	 = test.AddData("test", B_BOOL_TYPE, &value, sizeof(value));
		CHECK(ret == B_BAD_TYPE);

		test.AddData("bool", B_BOOL_TYPE, &value, sizeof(value));
		CHECK(test.CountNames(B_ANY_TYPE) == 2);
		CHECK(test.CountNames(B_STRING_TYPE) == 1);
		CHECK(test.CountNames(B_BOOL_TYPE) == 1);

		test.MakeEmpty();
		CHECK(test.IsEmpty());
		CHECK(test.CountNames(B_ANY_TYPE) == 0);
	}
	TEST_CASE("Copy Message")
	{
		BMessage test;
		test.AddData("test", B_STRING_TYPE, "something", 4);
		test.AddData("test", B_STRING_TYPE, "something", 8);
		const auto value = true;
		test.AddData("bool", B_BOOL_TYPE, &value, sizeof(value));
		CHECK(test.CountNames(B_ANY_TYPE) == 3);

		BMessage test2;
		CHECK(test2.CountNames(B_ANY_TYPE) == 0);
		test2 = test;
		CHECK(test2.CountNames(B_ANY_TYPE) == 3);
		CHECK(test2.CountNames(B_STRING_TYPE) == 2);
		CHECK(test2.CountNames(B_BOOL_TYPE) == 1);

		BMessage test3(test);
		CHECK(test3.CountNames(B_ANY_TYPE) == 3);
		CHECK(test3.CountNames(B_STRING_TYPE) == 2);
		CHECK(test3.CountNames(B_BOOL_TYPE) == 1);
	}
}
