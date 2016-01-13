/*
 * Copyright 2005-2015, Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Michael Lotz <mmlr@mlotz.ch>
 */
#ifndef _MESSAGE_PRIVATE_H_
#define _MESSAGE_PRIVATE_H_


#include <Message.h>
//#include <Messenger.h>
//#include <MessengerPrivate.h>
#include <TokenSpace.h>


#define MESSAGE_BODY_HASH_TABLE_SIZE	5
#define MAX_DATA_PREALLOCATION			B_PAGE_SIZE * 10
#define MAX_FIELD_PREALLOCATION			50


//static const int32 kPortMessageCode = 'pjpp';


enum {
    MESSAGE_FLAG_VALID = 0x0001,
    MESSAGE_FLAG_REPLY_REQUIRED = 0x0002,
    MESSAGE_FLAG_REPLY_DONE = 0x0004,
    MESSAGE_FLAG_IS_REPLY = 0x0008,
    MESSAGE_FLAG_WAS_DELIVERED = 0x0010,
    MESSAGE_FLAG_HAS_SPECIFIERS = 0x0020,
    MESSAGE_FLAG_WAS_DROPPED = 0x0040,
    MESSAGE_FLAG_PASS_BY_AREA = 0x0080,
    MESSAGE_FLAG_REPLY_AS_KMESSAGE = 0x0100
};


enum {
    FIELD_FLAG_VALID = 0x0001,
    FIELD_FLAG_FIXED_SIZE = 0x0002,
};

namespace BPrivate {
    class MessageAdapter;
    namespace Archiving {
        class BManagerBase;
    }
}

class BMessage::Private {
    friend class BPrivate::MessageAdapter;
    friend class BPrivate::Archiving::BManagerBase;

public:
    struct field_header {
        uint16		flags;
        uint16		name_length;
        type_code	type;
        uint32		count;
        uint32		data_size;
        uint32		offset;
        int32		next_field;
    } _PACKED;


    struct message_header {
        uint32		format;
        uint32		what;
        uint32		flags;

        int32		target;
        int32		current_specifier;
//        area_id		message_area;

        // reply info
        port_id		reply_port;
        int32		reply_target;
        team_id		reply_team;

        // body info
        uint32		data_size;
        uint32		field_count;
        uint32		hash_table_size;
        int32		hash_table[MESSAGE_BODY_HASH_TABLE_SIZE];

        /*	The hash table does contain indexes into the field list and
            not direct offsets to the fields. This has the advantage
            of not needing to update offsets in two locations.
            The hash table must be reevaluated when we remove a field
            though.
        */
    } _PACKED;

    //    friend class BMessageQueue;

    Private&			operator=(const Private& other);

    status_t			InitCommon(bool initHeader);
    status_t			InitHeader();
    status_t			Clear();

    status_t			FlattenToArea(message_header** _header) const;
    status_t			CopyForWrite();
    status_t			Reference();
    status_t			Dereference();

    status_t			ValidateMessage();

    void				UpdateOffsets(uint32 offset, int32 change);
    status_t			ResizeData(uint32 offset, int32 change);

    uint32				HashName(const char* name) const;
    status_t			FindField(const char* name, type_code type,
                                  field_header** _result) const;
    status_t			AddField(const char* name, type_code type,
                                 bool isFixedSize, field_header** _result);
    status_t			RemoveField(field_header* field);

    void				PrintToStream(const char* indent) const;

    status_t			SendMessage(port_id port, team_id portOwner,
                                    int32 token, bigtime_t timeout,
                                    bool replyRequired,
                                    BMessenger& replyTo) const;
    status_t			SendMessage(port_id port, team_id portOwner,
                                    int32 token, BMessage* reply,
                                    bigtime_t sendTimeout,
                                    bigtime_t replyTimeout) const;
    static	status_t			SendFlattenedMessage(void* data, int32 size,
                                                     port_id port, int32 token,
                                                     bigtime_t timeout);

    static	void				StaticInit();
    static	void				StaticReInitForkedChild();
    static	void				StaticCleanup();
    static	void				StaticCacheCleanup();
    static	int32				StaticGetCachedReplyPort();

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

//    enum				{ sNumReplyPorts = 3 };
//    static	port_id				sReplyPorts[sNumReplyPorts];
//    static	int32				sReplyPortInUse[sNumReplyPorts];
//    static	int32				sGetCachedReplyPort();

    //        static	BBlockCache*		sMsgCache;

    Private(BMessage *msg)
        :
          q_ptr(msg)
    {
    }

    Private(BMessage &msg)
        :
          q_ptr(&msg)
    {
    }

    void
    SetTarget(int32 token)
    {
        fHeader->target = token;
    }

//    void
//    SetReply(BMessenger messenger)
//    {
//        BMessenger::Private * const messenger_d = messenger.d_func();
//        fHeader->reply_port = messenger_d->Port();
//        fHeader->reply_target = messenger_d->Token();
//        fHeader->reply_team = messenger_d->Team();
//    }

    void
    SetReply(team_id team, port_id port, int32 target)
    {
        fHeader->reply_port = port;
        fHeader->reply_target = target;
        fHeader->reply_team = team;
    }

    int32
    GetTarget() const
    {
        return fHeader->target;
    }

    bool
    UsePreferredTarget() const
    {
        return fHeader->target == B_PREFERRED_TOKEN;
    }

    void
    SetWasDropped(bool wasDropped)
    {
        if (wasDropped)
            fHeader->flags |= MESSAGE_FLAG_WAS_DROPPED;
        else
            fHeader->flags &= ~MESSAGE_FLAG_WAS_DROPPED;
    }

    message_header*
    GetMessageHeader() const
    {
        return fHeader;
    }

    field_header*
    GetMessageFields() const
    {
        return fFields;
    }

    uint8*
    GetMessageData() const
    {
        return fData;
    }

    void*
    ArchivingPointer()
    {
        return fArchivingPointer;
    }

    void
    SetArchivingPointer(void* pointer)
    {
        fArchivingPointer = pointer;
    }

private:
    B_DECLARE_PUBLIC(BMessage)
    BMessage* q_ptr;
};


#endif	// _MESSAGE_PRIVATE_H_
