#ifndef _HANDLER_H
#define _HANDLER_H

#include <Archivable.h>

class BHandler : public BArchivable
{
   public:
	BHandler(const char *name = NULL);
	virtual ~BHandler();
};

#endif /* _HANDLER_H */
