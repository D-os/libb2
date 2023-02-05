#ifndef _MESSAGE_RUNNER_H
#define _MESSAGE_RUNNER_H

#include <Messenger.h>

class BMessageRunner
{
   public:
	BMessageRunner(BMessenger target, const BMessage *msg,
				   bigtime_t interval, int32 count = -1);
	BMessageRunner(BMessenger target, const BMessage *msg,
				   bigtime_t interval, int32 count, BMessenger reply_to);
	virtual ~BMessageRunner();

	status_t InitCheck() const;

	status_t SetInterval(bigtime_t interval);
	status_t SetCount(int32 count);
	status_t GetInfo(bigtime_t *interval, int32 *count) const;

   private:
	BMessageRunner(const BMessageRunner &);
	BMessageRunner &operator=(const BMessageRunner &);

	class impl;
	pimpl<impl> m;
};

#endif /* _MESSAGE_QUEUE_H */
