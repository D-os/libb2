#ifndef _CURSOR_H
#define _CURSOR_H

#include <Archivable.h>

class BCursor : BArchivable
{
   public:
	BCursor(const void *cursorData);
	BCursor(BMessage *data);
	virtual ~BCursor();

	virtual status_t	Archive(BMessage *into, bool deep = true) const;
	static BArchivable *Instantiate(BMessage *data);

   private:
	// friend class BApplication;
	// friend class BView;
};

#endif
