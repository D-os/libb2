#include "Cursor.h"

BCursor::BCursor(const void *cursorData)
{
}

BCursor::BCursor(BMessage *data)
	: BArchivable(data)
{
}

BCursor::~BCursor()
{
}

status_t BCursor::Archive(BMessage *into, bool deep) const
{
	return BArchivable::Archive(into, deep);
}
