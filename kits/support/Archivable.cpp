#include "Archivable.h"

#include <Message.h>

#include <cstring>

BArchivable::BArchivable()
{
}

BArchivable::~BArchivable() = default;

BArchivable::BArchivable(BMessage *from)
{
	debugger(__PRETTY_FUNCTION__);
}

status_t BArchivable::Archive(BMessage *into, bool deep) const
{
	if (!into) {
		return B_BAD_VALUE;
	}

	into->AddString("class", "BArchivable");
	return B_OK;
}

BArchivable *BArchivable::Instantiate(BMessage *from)
{
	debugger(__PRETTY_FUNCTION__);
	return nullptr;
}

bool validate_instantiation(BMessage *from, const char *class_name)
{
	if (from == nullptr || class_name == nullptr || *class_name == 0) return false;

	for (int32 i = 0;; i++) {
		const char *_class_name = nullptr;
		from->FindString("class", i, &_class_name);
		if (_class_name == nullptr) break;

		if (strlen(_class_name) != strlen(class_name)) continue;
		if (strcmp(_class_name, class_name) == 0) return true;
	}

	return false;
}

instantiation_func find_instantiation_func(const char *class_name)
{
	// TODO
	return nullptr;
}

instantiation_func find_instantiation_func(BMessage *archive_data)
{
	if (archive_data == nullptr) return nullptr;

	const char *class_name = nullptr;
	for (int32 i = 0;; i++) {
		const char *_class_name = nullptr;
		archive_data->FindString("class", i, &_class_name);
		if (_class_name == nullptr) break;
		class_name = _class_name;
	}
	return find_instantiation_func(class_name);
}
