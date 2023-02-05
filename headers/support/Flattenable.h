#ifndef _FLATTENABLE_H
#define _FLATTENABLE_H

#include <SupportDefs.h>

class BFlattenable
{
   public:
	virtual bool	  IsFixedSize() const						= 0;
	virtual type_code TypeCode() const							= 0;
	virtual ssize_t	  FlattenedSize() const						= 0;
	virtual status_t  Flatten(void *buffer, ssize_t size) const = 0;
	virtual bool	  AllowsTypeCode(type_code code) const;
	virtual status_t  Unflatten(type_code c, const void *buf, ssize_t size) = 0;

	virtual ~BFlattenable();
};

#endif /* _FLATTENABLE_H */
