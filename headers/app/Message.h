#ifndef _MESSAGE_H
#define _MESSAGE_H

#include <OS.h>

#include <iostream>

/// For convenience
#include <AppDefs.h>
#include <TypeConstants.h>

class BDataIO;
class BFlattenable;
class BHandler;
class BMessenger;
class BPoint;
class BRect;
class BString;

/// Name lengths and Scripting specifiers
#define B_FIELD_NAME_LENGTH 255
#define B_PROPERTY_NAME_LENGTH 255

enum {
	B_NO_SPECIFIER	   = 0,
	B_DIRECT_SPECIFIER = 1,
	B_INDEX_SPECIFIER,
	B_REVERSE_INDEX_SPECIFIER,
	B_RANGE_SPECIFIER,
	B_REVERSE_RANGE_SPECIFIER,
	B_NAME_SPECIFIER,
	B_ID_SPECIFIER,

	B_SPECIFIERS_END = 128
	/// app-defined specifiers start at B_SPECIFIERS_END+1
};

class BMessage
{
   public:
	uint32 what;

	BMessage();
	BMessage(uint32 what);
	BMessage(const BMessage &a_message);
	virtual ~BMessage();

	BMessage &operator=(const BMessage &msg);

	/// Statistics and misc info
	status_t GetInfo(type_code typeRequested, int32 which, char **name,
					 type_code *typeReturned, int32 *count = NULL) const;

	status_t GetInfo(const char *name, type_code *type, int32 *c = 0) const;
	status_t GetInfo(const char *name, type_code *type, bool *fixed_size) const;

	int32 CountNames(type_code type) const;
	bool  IsEmpty() const;
	bool  IsSystem() const;
	bool  IsReply() const;
	void  PrintToStream() const;

	status_t Rename(const char *old_entry, const char *new_entry);

	/// Delivery info
	bool			WasDelivered() const;
	bool			IsSourceWaiting() const;
	bool			IsSourceRemote() const;
	BMessenger		ReturnAddress() const;
	const BMessage *Previous() const;
	bool			WasDropped() const;
	BPoint			DropPoint(BPoint *offset = NULL) const;

	/// Replying
	status_t SendReply(uint32 command, BHandler *reply_to = NULL);
	status_t SendReply(BMessage *the_reply, BHandler *reply_to = NULL,
					   bigtime_t timeout = B_INFINITE_TIMEOUT);
	status_t SendReply(BMessage *the_reply, BMessenger reply_to,
					   bigtime_t timeout = B_INFINITE_TIMEOUT);

	status_t SendReply(uint32 command, BMessage *reply_to_reply);
	status_t SendReply(BMessage *the_reply, BMessage *reply_to_reply,
					   bigtime_t send_timeout  = B_INFINITE_TIMEOUT,
					   bigtime_t reply_timeout = B_INFINITE_TIMEOUT);

	/// Flattening data
	ssize_t	 FlattenedSize() const;
	status_t Flatten(char *buffer, ssize_t size) const;
	status_t Flatten(BDataIO *stream, ssize_t *size = NULL) const;
	status_t Unflatten(const char *flat_buffer);
	status_t Unflatten(BDataIO *stream);

	/// Specifiers (scripting)
	status_t AddSpecifier(const char *property);
	status_t AddSpecifier(const char *property, int32 index);
	status_t AddSpecifier(const char *property, int32 index, int32 range);
	status_t AddSpecifier(const char *property, const char *name);
	status_t AddSpecifier(const BMessage *specifier);

	status_t SetCurrentSpecifier(int32 index);
	status_t GetCurrentSpecifier(int32 *index, BMessage *specifier = NULL,
								 int32 *form = NULL, const char **property = NULL) const;
	bool	 HasSpecifiers() const;
	status_t PopSpecifier();

	/// Adding data
	status_t AddRect(const char *name, BRect a_rect);
	status_t AddPoint(const char *name, BPoint a_point);
	status_t AddString(const char *name, const char *a_string);
	status_t AddString(const char *name, const BString &a_string);
	status_t AddInt8(const char *name, int8 val);
	status_t AddUInt8(const char *name, uint8 val);
	status_t AddInt16(const char *name, int16 val);
	status_t AddUInt16(const char *name, uint16 val);
	status_t AddInt32(const char *name, int32 val);
	status_t AddUInt32(const char *name, uint32 val);
	status_t AddInt64(const char *name, int64 val);
	status_t AddUInt64(const char *name, uint64 val);
	status_t AddBool(const char *name, bool a_boolean);
	status_t AddFloat(const char *name, float a_float);
	status_t AddDouble(const char *name, double a_double);
	status_t AddPointer(const char *name, const void *ptr);
	status_t AddMessenger(const char *name, BMessenger messenger);
	status_t AddRef(const char *name, const entry_ref *ref);
	status_t AddMessage(const char *name, const BMessage *msg);
	status_t AddFlat(const char *name, BFlattenable *obj, int32 count = 1);
	status_t AddData(const char *name, type_code type, const void *data,
					 ssize_t numBytes, bool is_fixed_size = true, int32 count = 1);

	/// Removing data
	status_t RemoveData(const char *name, int32 index = 0);
	status_t RemoveName(const char *name);
	status_t MakeEmpty();

	/// Finding data
	status_t FindRect(const char *name, BRect *rect) const;
	status_t FindRect(const char *name, int32 index, BRect *rect) const;
	status_t FindPoint(const char *name, BPoint *pt) const;
	status_t FindPoint(const char *name, int32 index, BPoint *pt) const;
	status_t FindString(const char *name, const char **str) const;
	status_t FindString(const char *name, int32 index, const char **str) const;
	status_t FindString(const char *name, BString *str) const;
	status_t FindString(const char *name, int32 index, BString *str) const;
	status_t FindInt8(const char *name, int8 *value) const;
	status_t FindInt8(const char *name, int32 index, int8 *val) const;
	status_t FindUInt8(const char *name, uint8 *value) const;
	status_t FindUInt8(const char *name, int32 index, uint8 *val) const;
	status_t FindInt16(const char *name, int16 *value) const;
	status_t FindInt16(const char *name, int32 index, int16 *val) const;
	status_t FindUInt16(const char *name, uint16 *value) const;
	status_t FindUInt16(const char *name, int32 index, uint16 *val) const;
	status_t FindInt32(const char *name, int32 *value) const;
	status_t FindInt32(const char *name, int32 index, int32 *val) const;
	status_t FindUInt32(const char *name, uint32 *value) const;
	status_t FindUInt32(const char *name, int32 index, uint32 *val) const;
	status_t FindInt64(const char *name, int64 *value) const;
	status_t FindInt64(const char *name, int32 index, int64 *val) const;
	status_t FindUInt64(const char *name, uint64 *value) const;
	status_t FindUInt64(const char *name, int32 index, uint64 *val) const;
	status_t FindBool(const char *name, bool *value) const;
	status_t FindBool(const char *name, int32 index, bool *value) const;
	status_t FindFloat(const char *name, float *f) const;
	status_t FindFloat(const char *name, int32 index, float *f) const;
	status_t FindDouble(const char *name, double *d) const;
	status_t FindDouble(const char *name, int32 index, double *d) const;
	status_t FindPointer(const char *name, void **ptr) const;
	status_t FindPointer(const char *name, int32 index, void **ptr) const;
	status_t FindMessenger(const char *name, BMessenger *m) const;
	status_t FindMessenger(const char *name, int32 index, BMessenger *m) const;
	status_t FindRef(const char *name, entry_ref *ref) const;
	status_t FindRef(const char *name, int32 index, entry_ref *ref) const;
	status_t FindMessage(const char *name, BMessage *msg) const;
	status_t FindMessage(const char *name, int32 index, BMessage *msg) const;
	status_t FindFlat(const char *name, BFlattenable *obj) const;
	status_t FindFlat(const char *name, int32 index, BFlattenable *obj) const;
	status_t FindData(const char *name, type_code type,
					  const void **data, ssize_t *numBytes) const;
	status_t FindData(const char *name, type_code type, int32 index,
					  const void **data, ssize_t *numBytes) const;

	/// Replacing data
	status_t ReplaceRect(const char *name, BRect a_rect);
	status_t ReplaceRect(const char *name, int32 index, BRect a_rect);
	status_t ReplacePoint(const char *name, BPoint a_point);
	status_t ReplacePoint(const char *name, int32 index, BPoint a_point);
	status_t ReplaceString(const char *name, const char *string);
	status_t ReplaceString(const char *name, int32 index, const char *string);
	status_t ReplaceString(const char *name, const BString &string);
	status_t ReplaceString(const char *name, int32 index, const BString &string);
	status_t ReplaceInt8(const char *name, int8 val);
	status_t ReplaceInt8(const char *name, int32 index, int8 val);
	status_t ReplaceUInt8(const char *name, uint8 val);
	status_t ReplaceUInt8(const char *name, int32 index, uint8 val);
	status_t ReplaceInt16(const char *name, int16 val);
	status_t ReplaceInt16(const char *name, int32 index, int16 val);
	status_t ReplaceUInt16(const char *name, uint16 val);
	status_t ReplaceUInt16(const char *name, int32 index, uint16 val);
	status_t ReplaceInt32(const char *name, int32 val);
	status_t ReplaceInt32(const char *name, int32 index, int32 val);
	status_t ReplaceUInt32(const char *name, uint32 val);
	status_t ReplaceUInt32(const char *name, int32 index, uint32 val);
	status_t ReplaceInt64(const char *name, int64 val);
	status_t ReplaceInt64(const char *name, int32 index, int64 val);
	status_t ReplaceUInt64(const char *name, uint64 val);
	status_t ReplaceUInt64(const char *name, int32 index, uint64 val);
	status_t ReplaceBool(const char *name, bool a_bool);
	status_t ReplaceBool(const char *name, int32 index, bool a_bool);
	status_t ReplaceFloat(const char *name, float a_float);
	status_t ReplaceFloat(const char *name, int32 index, float a_float);
	status_t ReplaceDouble(const char *name, double a_double);
	status_t ReplaceDouble(const char *name, int32 index, double a_double);
	status_t ReplacePointer(const char *name, const void *ptr);
	status_t ReplacePointer(const char *name, int32 index, const void *ptr);
	status_t ReplaceMessenger(const char *name, BMessenger messenger);
	status_t ReplaceMessenger(const char *name, int32 index, BMessenger msngr);
	status_t ReplaceRef(const char *name, const entry_ref *ref);
	status_t ReplaceRef(const char *name, int32 index, const entry_ref *ref);
	status_t ReplaceMessage(const char *name, const BMessage *msg);
	status_t ReplaceMessage(const char *name, int32 index, const BMessage *msg);
	status_t ReplaceFlat(const char *name, BFlattenable *obj);
	status_t ReplaceFlat(const char *name, int32 index, BFlattenable *obj);
	status_t ReplaceData(const char *name, type_code type,
						 const void *data, ssize_t data_size);
	status_t ReplaceData(const char *name, type_code type, int32 index,
						 const void *data, ssize_t data_size);

	/// TODO: implement caching
	// void *operator new(size_t size);
	// void  operator delete(void *ptr, size_t size);

   private:
	friend class BMessageQueue;
	friend class BMessenger;
	friend class BApplication;
	friend class BView;
	friend class BWindow;
	friend std::ostream &operator<<(std::ostream &, const BMessage &);

	class impl;
	pimpl<impl> m;
	friend class BLooper;
	void	  _set_handler(BHandler *);
	BHandler *_get_handler() const;
	void	  _set_reply_handler(BHandler *);
	BHandler *_get_reply_handler() const;
};

/// C++ standard way of providing string conversions
std::ostream &operator<<(std::ostream &, const BMessage &);

#endif /* _MESSAGE_H */
