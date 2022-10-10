#ifndef _ARCHIVABLE_H
#define _ARCHIVABLE_H

#include <image.h>

class BMessage;

class BArchivable
{
   public:
	BArchivable();
	virtual ~BArchivable();

	BArchivable(BMessage *from);
	virtual status_t	Archive(BMessage *into, bool deep = true) const;
	static BArchivable *Instantiate(BMessage *from);
};

typedef BArchivable *(*instantiation_func)(BMessage *);

BArchivable *instantiate_object(BMessage *from, image_id *id);
BArchivable *instantiate_object(BMessage *from);
bool		 validate_instantiation(BMessage	 *from,
									const char *class_name);

instantiation_func find_instantiation_func(const char *class_name,
										   const char *sig);
instantiation_func find_instantiation_func(const char *class_name);
instantiation_func find_instantiation_func(BMessage *archive_data);

#endif /* _ARCHIVABLE_H */
