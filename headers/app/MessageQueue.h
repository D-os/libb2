#ifndef _MESSAGE_QUEUE_H
#define _MESSAGE_QUEUE_H

#include <Locker.h>
#include <SupportDefs.h>

#include <memory>

class BMessage;

class BMessageQueue
{
   public:
	BMessageQueue();
	virtual ~BMessageQueue();

	/// Queue manipulation and query
	void	  AddMessage(BMessage *an_event);
	void	  RemoveMessage(BMessage *an_event);
	BMessage *NextMessage();
	BMessage *FindMessage(int32 index) const;
	BMessage *FindMessage(uint32 what, int32 index = 0) const;
	int32	  CountMessages() const;
	bool	  IsEmpty() const;

	/// Queue locking
	bool Lock();
	void Unlock();

   private:
	BMessageQueue(const BMessageQueue &);
	BMessageQueue &operator=(const BMessageQueue &);

	class impl;
	std::unique_ptr<impl> _impl;

	mutable BLocker locker;
};

/*-------------------------------------------------------------*/
/*-------------------------------------------------------------*/

#endif /* _MESSAGE_QUEUE_H */
