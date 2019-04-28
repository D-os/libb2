/******************************************************************************
/
/	File:			Archivable.h
/
/	Description:	BArchivable mix-in class defines the archiving
/					protocol.  Also some global archiving functions.
/
/	Copyright 1993-98, Be Incorporated
/
******************************************************************************/

#ifndef _ARCHIVABLE_H
#define _ARCHIVABLE_H

#include <BeBuild.h>
#include <SupportDefs.h>
#include <image.h>

class BMessage;

/*--------------------------------------------------------------------*/
/*----- BArchivable class --------------------------------------------*/
class BArchivable {
public:
					BArchivable();
virtual				~BArchivable();	

					BArchivable(BMessage *from);
virtual	status_t	Archive(BMessage *into, bool deep = true) const;
static	BArchivable	*Instantiate(BMessage *from);

/*----- Private or reserved ---------------*/
virtual status_t	Perform(perform_code d, void *arg);

private:

virtual	void		_ReservedArchivable1();
virtual	void		_ReservedArchivable2();
virtual	void		_ReservedArchivable3();

		uint32		_reserved[2];
};


/*----------------------------------------------------------------*/
/*----- Global Functions -----------------------------------------*/

typedef BArchivable *(*instantiation_func) (BMessage *); 

_IMPEXP_BE BArchivable		*instantiate_object(BMessage *from, image_id *id);
_IMPEXP_BE BArchivable		*instantiate_object(BMessage *from);
_IMPEXP_BE bool				validate_instantiation(	BMessage *from, 
											const char *class_name);

_IMPEXP_BE instantiation_func	find_instantiation_func(const char *class_name,
														const char *sig);
_IMPEXP_BE instantiation_func	find_instantiation_func(const char *class_name);
_IMPEXP_BE instantiation_func	find_instantiation_func(BMessage *archive_data);


/*-------------------------------------------------------------*/
/*-------------------------------------------------------------*/

#endif /* _ARCHIVABLE_H */
