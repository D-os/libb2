/*
 * Copyright 2005-2015 Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Michael Lotz, mmlr@mlotz.ch
 */
#ifndef _MESSAGE_H
#define _MESSAGE_H


#include <new>

#include <BeBuild.h>
#include <DataIO.h>
#include <Flattenable.h>
#include <OS.h>
#include <Rect.h>
#include <Size.h>

#include <AppDefs.h>		/* For convenience */
#include <TypeConstants.h>	/* For convenience */


class BAlignment;
class BBlockCache;
class BMessenger;
class BHandler;
class BString;
class BStringList;
struct entry_ref;


// Name lengths and Scripting specifiers
#define B_FIELD_NAME_LENGTH			255
#define B_PROPERTY_NAME_LENGTH		255

enum {
    B_NO_SPECIFIER = 0,
    B_DIRECT_SPECIFIER = 1,
    B_INDEX_SPECIFIER,
    B_REVERSE_INDEX_SPECIFIER,
    B_RANGE_SPECIFIER,
    B_REVERSE_RANGE_SPECIFIER,
    B_NAME_SPECIFIER,
    B_ID_SPECIFIER,

    B_SPECIFIERS_END = 128
    // app-defined specifiers start at B_SPECIFIERS_END + 1
};


class BMessage {
public:
            uint32				what;

                                BMessage();
                                BMessage(uint32 what);
                                BMessage(const BMessage& other);
    virtual						~BMessage();

            BMessage&			operator=(const BMessage& other);

    // Statistics and misc info
            status_t			GetInfo(type_code typeRequested, int32 index,
                                    char** nameFound, type_code* typeFound,
                                    int32* countFound = NULL) const;
            status_t			GetInfo(const char* name, type_code* typeFound,
                                    int32* countFound = NULL) const;
            status_t			GetInfo(const char* name, type_code* typeFound,
                                    bool* fixedSize) const;
            status_t			GetInfo(const char* name, type_code* typeFound,
                                    int32* countFound, bool* fixedSize) const;

            int32				CountNames(type_code type) const;
            bool				IsEmpty() const;
            bool				IsSystem() const;
            bool				IsReply() const;
            void				PrintToStream() const;

            status_t			Rename(const char* oldEntry,
                                    const char* newEntry);

    // Delivery info
            bool				WasDelivered() const;
            bool				IsSourceWaiting() const;
            bool				IsSourceRemote() const;
            BMessenger			ReturnAddress() const;
            const BMessage*		Previous() const;
            bool				WasDropped() const;
            BPoint				DropPoint(BPoint* offset = NULL) const;

    // Replying
            status_t			SendReply(uint32 command,
                                    BHandler* replyTo = NULL);
            status_t			SendReply(BMessage* reply,
                                    BHandler* replyTo = NULL,
                                    bigtime_t timeout = B_INFINITE_TIMEOUT);
            status_t			SendReply(BMessage* reply, BMessenger replyTo,
                                    bigtime_t timeout = B_INFINITE_TIMEOUT);

            status_t			SendReply(uint32 command,
                                    BMessage* replyToReply);
            status_t			SendReply(BMessage* reply,
                                    BMessage* replyToReply,
                                    bigtime_t sendTimeout = B_INFINITE_TIMEOUT,
                                    bigtime_t replyTimeout
                                        = B_INFINITE_TIMEOUT);

    // Flattening data
            ssize_t				FlattenedSize() const;
            status_t			Flatten(char* buffer, ssize_t size) const;
            status_t			Flatten(BDataIO* stream,
                                    ssize_t* size = NULL) const;
            status_t			Unflatten(const char* flatBuffer);
            status_t			Unflatten(BDataIO* stream);

    // Specifiers (scripting)
            status_t			AddSpecifier(const char* property);
            status_t			AddSpecifier(const char* property, int32 index);
            status_t			AddSpecifier(const char* property, int32 index,
                                    int32 range);
            status_t			AddSpecifier(const char* property,
                                    const char* name);
            status_t			AddSpecifier(const BMessage* specifier);

            status_t			SetCurrentSpecifier(int32 index);
            status_t			GetCurrentSpecifier(int32* index,
                                    BMessage* specifier = NULL,
                                    int32* what = NULL,
                                    const char** property = NULL) const;
            bool				HasSpecifiers() const;
            status_t			PopSpecifier();

    // Adding data
            status_t			AddAlignment(const char* name,
                                    const BAlignment& alignment);
            status_t			AddRect(const char* name, BRect rect);
            status_t			AddPoint(const char* name, BPoint point);
            status_t			AddSize(const char* name, BSize size);
            status_t			AddString(const char* name, const char* string);
            status_t			AddString(const char* name,
                                    const BString& string);
            status_t			AddStrings(const char* name,
                                    const BStringList& list);
            status_t			AddInt8(const char* name, int8 value);
            status_t			AddUInt8(const char* name, uint8 value);
            status_t			AddInt16(const char* name, int16 value);
            status_t			AddUInt16(const char* name, uint16 value);
            status_t			AddInt32(const char* name, int32 value);
            status_t			AddUInt32(const char* name, uint32 value);
            status_t			AddInt64(const char* name, int64 value);
            status_t			AddUInt64(const char* name, uint64 value);
            status_t			AddBool(const char* name, bool value);
            status_t			AddFloat(const char* name, float value);
            status_t			AddDouble(const char* name, double value);
            status_t			AddPointer(const char* name,
                                    const void* pointer);
            status_t			AddMessenger(const char* name,
                                    BMessenger messenger);
            status_t			AddRef(const char* name, const entry_ref* ref);
            status_t			AddMessage(const char* name,
                                    const BMessage* message);
            status_t			AddFlat(const char* name, BFlattenable* object,
                                    int32 count = 1);
            status_t			AddFlat(const char* name,
                                    const BFlattenable* object, int32 count = 1);
            status_t			AddData(const char* name, type_code type,
                                    const void* data, ssize_t numBytes,
                                    bool isFixedSize = true, int32 count = 1);

            status_t			Append(const BMessage& message);

    // Removing data
            status_t			RemoveData(const char* name, int32 index = 0);
            status_t			RemoveName(const char* name);
            status_t			MakeEmpty();

    // Finding data
            status_t			FindAlignment(const char* name,
                                    BAlignment* alignment) const;
            status_t			FindAlignment(const char* name, int32 index,
                                    BAlignment* alignment) const;

            status_t			FindRect(const char* name, BRect* rect) const;
            status_t			FindRect(const char* name, int32 index,
                                    BRect* rect) const;
            status_t			FindPoint(const char* name,
                                    BPoint* point) const;
            status_t			FindPoint(const char* name, int32 index,
                                    BPoint* point) const;

            status_t			FindSize(const char* name, BSize* size) const;
            status_t			FindSize(const char* name, int32 index,
                                    BSize* size) const;

            status_t			FindString(const char* name,
                                    const char** string) const;
            status_t			FindString(const char* name, int32 index,
                                    const char** string) const;
            status_t			FindString(const char* name,
                                    BString* string) const;
            status_t			FindString(const char* name, int32 index,
                                    BString* string) const;
            status_t			FindStrings(const char* name,
                                    BStringList* list) const;
            status_t			FindInt8(const char* name, int8* value) const;
            status_t			FindInt8(const char* name, int32 index,
                                    int8* value) const;
            status_t			FindUInt8(const char* name, uint8* value) const;
            status_t			FindUInt8(const char* name, int32 index,
                                    uint8* value) const;
            status_t			FindInt16(const char* name, int16* value) const;
            status_t			FindInt16(const char* name, int32 index,
                                    int16* value) const;
            status_t			FindUInt16(const char* name,
                                    uint16* value) const;
            status_t			FindUInt16(const char* name, int32 index,
                                    uint16* value) const;
            status_t			FindInt32(const char* name, int32* value) const;
            status_t			FindInt32(const char* name, int32 index,
                                    int32* value) const;
            status_t			FindUInt32(const char* name,
                                    uint32* value) const;
            status_t			FindUInt32(const char* name, int32 index,
                                    uint32* value) const;
            status_t			FindInt64(const char* name, int64* value) const;
            status_t			FindInt64(const char* name, int32 index,
                                    int64* value) const;
            status_t			FindUInt64(const char* name,
                                    uint64* value) const;
            status_t			FindUInt64(const char* name, int32 index,
                                    uint64* value) const;
            status_t			FindBool(const char* name, bool* value) const;
            status_t			FindBool(const char* name, int32 index,
                                    bool* value) const;
            status_t			FindFloat(const char* name, float* value) const;
            status_t			FindFloat(const char* name, int32 index,
                                    float* value) const;
            status_t			FindDouble(const char* name,
                                    double* value) const;
            status_t			FindDouble(const char* name, int32 index,
                                    double* value) const;
            status_t			FindPointer(const char* name,
                                    void** pointer) const;
            status_t			FindPointer(const char* name, int32 index,
                                    void** pointer) const;
            status_t			FindMessenger(const char* name,
                                    BMessenger* messenger) const;
            status_t			FindMessenger(const char* name, int32 index,
                                    BMessenger* messenger) const;
            status_t			FindRef(const char* name, entry_ref* ref) const;
            status_t			FindRef(const char* name, int32 index,
                                    entry_ref* ref) const;
            status_t			FindMessage(const char* name,
                                    BMessage* message) const;
            status_t			FindMessage(const char* name, int32 index,
                                    BMessage* message) const;
            status_t			FindFlat(const char* name,
                                    BFlattenable* object) const;
            status_t			FindFlat(const char* name, int32 index,
                                    BFlattenable* object) const;
            status_t			FindData(const char* name, type_code type,
                                    const void** data, ssize_t* numBytes) const;
            status_t			FindData(const char* name, type_code type,
                                    int32 index, const void** data,
                                    ssize_t* numBytes) const;

    // Replacing data
            status_t			ReplaceAlignment(const char* name,
                                    const BAlignment& alignment);
            status_t			ReplaceAlignment(const char* name, int32 index,
                                    const BAlignment& alignment);

            status_t			ReplaceRect(const char* name, BRect rect);
            status_t			ReplaceRect(const char* name, int32 index,
                                    BRect rect);

            status_t			ReplacePoint(const char* name, BPoint aPoint);
            status_t			ReplacePoint(const char* name, int32 index,
                                    BPoint aPoint);
            status_t			ReplaceSize(const char* name, BSize aSize);
            status_t			ReplaceSize(const char* name, int32 index,
                                    BSize aSize);

            status_t			ReplaceString(const char* name,
                                    const char* string);
            status_t			ReplaceString(const char* name, int32 index,
                                    const char* string);
            status_t			ReplaceString(const char* name,
                                    const BString& string);
            status_t			ReplaceString(const char* name, int32 index,
                                    const BString& string);
            status_t			ReplaceInt8(const char* name, int8 value);
            status_t			ReplaceInt8(const char* name, int32 index,
                                    int8 value);
            status_t			ReplaceUInt8(const char* name, uint8 value);
            status_t			ReplaceUInt8(const char* name, int32 index,
                                    uint8 value);
            status_t			ReplaceInt16(const char* name, int16 value);
            status_t			ReplaceInt16(const char* name, int32 index,
                                    int16 value);
            status_t			ReplaceUInt16(const char* name, uint16 value);
            status_t			ReplaceUInt16(const char* name, int32 index,
                                    uint16 value);
            status_t			ReplaceInt32(const char* name, int32 value);
            status_t			ReplaceInt32(const char* name, int32 index,
                                    int32 value);
            status_t			ReplaceUInt32(const char* name, uint32 value);
            status_t			ReplaceUInt32(const char* name, int32 index,
                                    uint32 value);
            status_t			ReplaceInt64(const char* name, int64 value);
            status_t			ReplaceInt64(const char* name, int32 index,
                                    int64 value);
            status_t			ReplaceUInt64(const char* name, uint64 value);
            status_t			ReplaceUInt64(const char* name, int32 index,
                                    uint64 value);
            status_t			ReplaceBool(const char* name, bool aBoolean);
            status_t			ReplaceBool(const char* name, int32 index,
                                    bool value);
            status_t			ReplaceFloat(const char* name, float value);
            status_t			ReplaceFloat(const char* name, int32 index,
                                    float value);
            status_t			ReplaceDouble(const char* name, double value);
            status_t			ReplaceDouble(const char* name, int32 index,
                                    double value);
            status_t			ReplacePointer(const char* name,
                                    const void* pointer);
            status_t			ReplacePointer(const char* name, int32 index,
                                    const void* pointer);
            status_t			ReplaceMessenger(const char* name,
                                    BMessenger messenger);
            status_t			ReplaceMessenger(const char* name, int32 index,
                                    BMessenger messenger);
            status_t			ReplaceRef(const char* name,
                                    const entry_ref* ref);
            status_t			ReplaceRef(const char* name, int32 index,
                                    const entry_ref* ref);
            status_t			ReplaceMessage(const char* name,
                                    const BMessage* message);
            status_t			ReplaceMessage(const char* name, int32 index,
                                    const BMessage* message);
            status_t			ReplaceFlat(const char* name,
                                    BFlattenable* object);
            status_t			ReplaceFlat(const char* name, int32 index,
                                    BFlattenable* object);
            status_t			ReplaceData(const char* name, type_code type,
                                    const void* data, ssize_t numBytes);
            status_t			ReplaceData(const char* name, type_code type,
                                    int32 index, const void* data,
                                    ssize_t numBytes);

    // Comparing data - Haiku experimental API
            bool				HasSameData(const BMessage& other,
                                    bool ignoreFieldOrder = true,
                                    bool deep = false) const;

            void*				operator new(size_t size);
            void*				operator new(size_t, void* pointer);
            void*				operator new(size_t,
                                    const std::nothrow_t& noThrow);
            void				operator delete(void* pointer, size_t size);

    // Private, reserved, or obsolete
            bool				HasAlignment(const char* name,
                                    int32 n = 0) const;
            bool				HasRect(const char* name, int32 n = 0) const;
            bool				HasPoint(const char* name, int32 n = 0) const;
            bool				HasSize(const char* name, int32 n = 0) const;
            bool				HasString(const char* name, int32 n = 0) const;
            bool				HasInt8(const char* name, int32 n = 0) const;
            bool				HasUInt8(const char* name, int32 n = 0) const;
            bool				HasInt16(const char* name, int32 n = 0) const;
            bool				HasUInt16(const char* name, int32 n = 0) const;
            bool				HasInt32(const char* name, int32 n = 0) const;
            bool				HasUInt32(const char* name, int32 n = 0) const;
            bool				HasInt64(const char* name, int32 n = 0) const;
            bool				HasUInt64(const char* name, int32 n = 0) const;
            bool				HasBool(const char* name, int32 n = 0) const;
            bool				HasFloat(const char* name, int32 n = 0) const;
            bool				HasDouble(const char* name, int32 n = 0) const;
            bool				HasPointer(const char* name, int32 n = 0) const;
            bool				HasMessenger(const char* name,
                                    int32 n = 0) const;
            bool				HasRef(const char* name, int32 n = 0) const;
            bool				HasMessage(const char* name, int32 n = 0) const;
            bool				HasFlat(const char* name,
                                    const BFlattenable* object) const;
            bool				HasFlat(const char* name, int32 n,
                                    const BFlattenable* object) const;
            bool				HasData(const char* name, type_code ,
                                    int32 n = 0) const;
            BRect				FindRect(const char* name, int32 n = 0) const;
            BPoint				FindPoint(const char* name, int32 n = 0) const;
            const char*			FindString(const char* name, int32 n = 0) const;
            int8				FindInt8(const char* name, int32 n = 0) const;
            int16				FindInt16(const char* name, int32 n = 0) const;
            int32				FindInt32(const char* name, int32 n = 0) const;
            int64				FindInt64(const char* name, int32 n = 0) const;
            bool				FindBool(const char* name, int32 n = 0) const;
            float				FindFloat(const char* name, int32 n = 0) const;
            double				FindDouble(const char* name, int32 n = 0) const;

    // Convenience methods
            bool				GetBool(const char* name,
                                    bool defaultValue = false) const;
            bool				GetBool(const char* name, int32 index,
                                    bool defaultValue) const;
            int8				GetInt8(const char* name,
                                    int8 defaultValue) const;
            int8				GetInt8(const char* name, int32 index,
                                    int8 defaultValue) const;
            uint8				GetUInt8(const char* name,
                                    uint8 defaultValue) const;
            uint8				GetUInt8(const char* name, int32 index,
                                    uint8 defaultValue) const;
            int16				GetInt16(const char* name,
                                    int16 defaultValue) const;
            int16				GetInt16(const char* name, int32 index,
                                    int16 defaultValue) const;
            uint16				GetUInt16(const char* name,
                                    uint16 defaultValue) const;
            uint16				GetUInt16(const char* name, int32 index,
                                    uint16 defaultValue) const;
            int32				GetInt32(const char* name,
                                    int32 defaultValue) const;
            int32				GetInt32(const char* name, int32 index,
                                    int32 defaultValue) const;
            uint32				GetUInt32(const char* name,
                                    uint32 defaultValue) const;
            uint32				GetUInt32(const char* name, int32 index,
                                    uint32 defaultValue) const;
            int64				GetInt64(const char* name,
                                    int64 defaultValue) const;
            int64				GetInt64(const char* name, int32 index,
                                    int64 defaultValue) const;
            uint64				GetUInt64(const char* name,
                                    uint64 defaultValue) const;
            uint64				GetUInt64(const char* name, int32 index,
                                    uint64 defaultValue) const;
            float				GetFloat(const char* name,
                                    float defaultValue) const;
            float				GetFloat(const char* name, int32 index,
                                    float defaultValue) const;
            double				GetDouble(const char* name,
                                    double defaultValue) const;
            double				GetDouble(const char* name, int32 index,
                                    double defaultValue) const;
            const char*			GetString(const char* name,
                                    const char* defaultValue = NULL) const;
            const char*			GetString(const char* name, int32 index,
                                    const char* defaultValue) const;
            BAlignment			GetAlignment(const char* name, int32 index,
                                    const BAlignment& defaultValue) const;
            BAlignment			GetAlignment(const char* name,
                                    const BAlignment& defaultValue) const;
            BRect				GetRect(const char* name, int32 index,
                                    const BRect& defaultValue) const;
            BRect				GetRect(const char* name,
                                    const BRect& defaultValue) const;
            BPoint				GetPoint(const char* name, int32 index,
                                    const BPoint& defaultValue) const;
            BPoint				GetPoint(const char* name,
                                    const BPoint& defaultValue) const;
            BSize				GetSize(const char* name, int32 index,
                                    const BSize& defaultValue) const;
            BSize				GetSize(const char* name,
                                    const BSize& defaultValue) const;

    // fixed size fields only
            status_t			SetBool(const char* name, bool value);
            status_t			SetInt8(const char* name, int8 value);
            status_t			SetUInt8(const char* name, uint8 value);
            status_t			SetInt16(const char* name, int16 value);
            status_t			SetUInt16(const char* name, uint16 value);
            status_t			SetInt32(const char* name, int32 value);
            status_t			SetUInt32(const char* name, uint32 value);
            status_t			SetInt64(const char* name, int64 value);
            status_t			SetUInt64(const char* name, uint64 value);
            status_t			SetPointer(const char* name, const void* value);
            status_t			SetString(const char* name, const char* string);
            status_t			SetString(const char* name,
                                    const BString& string);
            status_t			SetFloat(const char* name, float value);
            status_t			SetDouble(const char* name, double value);
            status_t			SetAlignment(const char* name,
                                    const BAlignment& value);
            status_t			SetPoint(const char* name, const BPoint& value);
            status_t			SetRect(const char* name, const BRect& value);
            status_t			SetSize(const char* name, const BSize& value);
            status_t			SetData(const char* name, type_code type,
                                    const void* data, ssize_t numBytes,
                                    bool fixedSize = true, int count = 1);

    class Private;
    struct message_header;
    struct field_header;

private:
    friend class Private;
    friend class BMessageQueue;

            status_t			_InitCommon(bool initHeader);
            status_t			_InitHeader();
            status_t			_Clear();

            status_t			_FlattenToArea(message_header** _header) const;
            status_t			_CopyForWrite();
            status_t			_Reference();
            status_t			_Dereference();

            status_t			_ValidateMessage();

            void				_UpdateOffsets(uint32 offset, int32 change);
            status_t			_ResizeData(uint32 offset, int32 change);

            uint32				_HashName(const char* name) const;
            status_t			_FindField(const char* name, type_code type,
                                    field_header** _result) const;
            status_t			_AddField(const char* name, type_code type,
                                    bool isFixedSize, field_header** _result);
            status_t			_RemoveField(field_header* field);

            void				_PrintToStream(const char* indent) const;

private:
                                BMessage(BMessage* message);
                                    // deprecated

    virtual	void				_ReservedMessage1();
    virtual	void				_ReservedMessage2();
    virtual	void				_ReservedMessage3();

            status_t			_SendMessage(port_id port, team_id portOwner,
                                    int32 token, bigtime_t timeout,
                                    bool replyRequired,
                                    BMessenger& replyTo) const;
            status_t			_SendMessage(port_id port, team_id portOwner,
                                    int32 token, BMessage* reply,
                                    bigtime_t sendTimeout,
                                    bigtime_t replyTimeout) const;
    static	status_t			_SendFlattenedMessage(void* data, int32 size,
                                    port_id port, int32 token,
                                    bigtime_t timeout);

    static	void				_StaticInit();
    static	void				_StaticReInitForkedChild();
    static	void				_StaticCleanup();
    static	void				_StaticCacheCleanup();
    static	int32				_StaticGetCachedReplyPort();

private:
            message_header*		fHeader;
            field_header*		fFields;
            uint8*				fData;

            uint32				fFieldsAvailable;
            size_t				fDataAvailable;

            mutable	BMessage*	fOriginal;

            BMessage*			fQueueLink;
                // fQueueLink is used by BMessageQueue to build a linked list

            void*				fArchivingPointer;

            uint32				fReserved[8];

            enum				{ sNumReplyPorts = 3 };
    static	port_id				sReplyPorts[sNumReplyPorts];
    static	int32				sReplyPortInUse[sNumReplyPorts];
    static	int32				sGetCachedReplyPort();

    static	BBlockCache*		sMsgCache;
};


#endif	// _MESSAGE_H
