/******************************************************************************
/
/	File:			MessageQueue.h
/
/	Description:	BMessageQueue class creates objects that are used by
/					BLoopers to store in-coming messages.
/
/	Copyright 1995-98, Be Incorporated, All Rights Reserved.
/
*******************************************************************************/

#ifndef _MESSAGE_QUEUE_H
#define _MESSAGE_QUEUE_H

#include <BeBuild.h>
#include <Locker.h>
#include <Message.h>	/* For convenience */

/*----------------------------------------------------------------------*/
/*----- BMessageQueue class --------------------------------------------*/

class	BMessageQueue {
public:
					BMessageQueue();	
virtual				~BMessageQueue();

/* Queue manipulation and query */
		void		AddMessage(BMessage *an_event);
		bool		RemoveMessage(BMessage *an_event);
		BMessage	*NextMessage();
		BMessage	*FindMessage(int32 index) const;
		BMessage	*FindMessage(uint32 what, int32 index = 0) const;
		int32		CountMessages() const;
		bool		IsEmpty() const;

/* Queue locking */
		bool		Lock();
		void		Unlock();

/*----- Private or reserved -----------------------------------------*/

private:

virtual	void		_ReservedMessageQueue1();
virtual	void		_ReservedMessageQueue2();
virtual	void		_ReservedMessageQueue3();

					BMessageQueue(const BMessageQueue &);
		BMessageQueue &operator=(const BMessageQueue &);

		char		message_filter(BMessage *an_event);
		BMessage	*FindMessage(bool anyWhat, uint32 what, int32 index) const;

		BMessage	*the_queue;
		BMessage	*tail;
		int32		message_count;
		BLocker		locker;
		uint32		_reserved[3];
};

/*-------------------------------------------------------------*/
/*-------------------------------------------------------------*/

#endif /* _MESSAGE_QUEUE_H */


