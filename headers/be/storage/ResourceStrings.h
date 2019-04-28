/******************************************************************************
/
/	File:			ResourceStrings.h
/
/	Description:	Get strings from a resource file.
/
/	Copyright 1993-98, Be Incorporated
/
******************************************************************************/


#ifndef _RESOURCE_STRINGS_H
#define _RESOURCE_STRINGS_H

#include <Entry.h>
#include <Locker.h>

class BResources;
class BString;


class BResourceStrings	/* strings from resource files */
{
public:

		BResourceStrings();					/* use application file */
		BResourceStrings(const entry_ref & ref);
virtual	~BResourceStrings();

		status_t InitCheck();				/* if not B_OK, gettings strings will fail */
virtual	BString * NewString(int32 id);			/* you delete returned object */
virtual	const char * FindString(int32 id);		/* returned pointer is valid until ~BResourceStrings() or SetStringFile() called */

virtual	status_t SetStringFile(const entry_ref * file);	/* else it uses app file */
		status_t GetStringFile(entry_ref * out_ref);

		enum {
			RESOURCE_TYPE = 'CSTR'
		};

protected:

		struct _string_id_hash {
			_string_id_hash();
			~_string_id_hash();
			void assign_string(const char * str, bool make_copy);	/* assignment */
			_string_id_hash * next;
			int	id;
			char *	data;
			bool data_alloced;
			bool _reserved_1[3];
			uint32 _reserved_2;
		};
		BLocker _string_lock;
		status_t _init_error;

private:

		entry_ref _cur_string_ref;
		BResources * _string_file;
		_string_id_hash ** _string_hash;
		int _string_hash_size;
		int _num_hashed_strings;
		uint32 _reserved_string_table_[16];

		void _clear_strings();
		status_t _set_string_file(const entry_ref * ref);
		_string_id_hash ** _rehash_strings(_string_id_hash ** old, int old_size, int new_size);
		_string_id_hash * _add_string(char * str, int id, bool was_malloced);

virtual	_string_id_hash * _find_string_id(int id);	/* NewString() and FindString() bottleneck through here */

		/* Mmmmh, padding! */
virtual	status_t _Reserved_ResourceStrings_0(void *);
virtual	status_t _Reserved_ResourceStrings_1(void *);
virtual	status_t _Reserved_ResourceStrings_2(void *);
virtual	status_t _Reserved_ResourceStrings_3(void *);
virtual	status_t _Reserved_ResourceStrings_4(void *);
virtual	status_t _Reserved_ResourceStrings_5(void *);
};

#endif	/* _RESOURCE_STRINGS_H */

