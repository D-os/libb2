#include "Archivable.h"

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

	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}
