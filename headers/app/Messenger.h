#ifndef _MESSENGER_H
#define _MESSENGER_H

#include <OS.h>

class BMessage;
class BMessenger;
class BHandler;
class BLooper;

bool operator<(const BMessenger &a, const BMessenger &b);
bool operator!=(const BMessenger &a, const BMessenger &b);

class BMessenger
{
   public:
	BMessenger();

	BMessenger(const char *mime_sig,
			   team_id	   team = -1,
			   status_t	*perr = nullptr);

	BMessenger(const BHandler *handler,
			   const BLooper	 *looper = nullptr,
			   status_t		*perr	  = nullptr);
	BMessenger(const BMessenger &from);
	~BMessenger();

	/// Target
	bool	  IsTargetLocal() const;
	BHandler *Target(BLooper **looper) const;
	bool	  LockTarget() const;
	status_t  LockTargetWithTimeout(bigtime_t timeout) const;

	/// Message sending
	status_t SendMessage(uint32 command, BHandler *reply_to = nullptr) const;
	status_t SendMessage(BMessage *a_message,
						 BHandler *reply_to = nullptr,
						 bigtime_t timeout	= B_INFINITE_TIMEOUT) const;
	status_t SendMessage(BMessage  *a_message,
						 BMessenger reply_to,
						 bigtime_t	timeout = B_INFINITE_TIMEOUT) const;

	status_t SendMessage(uint32 command, BMessage *reply) const;
	status_t SendMessage(BMessage *a_message,
						 BMessage *reply,
						 bigtime_t send_timeout	 = B_INFINITE_TIMEOUT,
						 bigtime_t reply_timeout = B_INFINITE_TIMEOUT) const;

	/// Operators and misc
	BMessenger &operator=(const BMessenger &from);
	bool		operator==(const BMessenger &other) const;

	bool	IsValid() const;
	team_id Team() const;

   private:
	friend bool operator<(const BMessenger &a, const BMessenger &b);
	friend bool operator!=(const BMessenger &a, const BMessenger &b);

	BMessenger(team_id team,
			   port_id port,
			   int32   token,
			   bool	   preferred);
};

#endif /* _MESSENGER_H */
