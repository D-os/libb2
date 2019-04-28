/******************************************************************************
/
/	File:			Messenger.h
/
/	Description:	BMessenger class provides the mechanism for delivering
/					BMessages to BLooper/BHandler targets.
/					Eminently stack-allocable.
/
/	Copyright 1995-98, Be Incorporated, All Rights Reserved.
/
*******************************************************************************/

#ifndef _MESSENGER_H
#define _MESSENGER_H

#include <BeBuild.h>
#include <OS.h>
#include <ByteOrder.h>
#include <Message.h>		/* For convenience */

class BHandler;
class BLooper;

/*---------------------------------------------------------------*/
/* --------- BMessenger class----------------------------------- */

_IMPEXP_BE bool operator<(const BMessenger & a, const BMessenger & b);
_IMPEXP_BE bool operator!=(const BMessenger & a, const BMessenger & b);

class BMessenger {
public:	
					BMessenger();

					BMessenger(const char *mime_sig, 
								team_id team = -1,
								status_t *perr = NULL);

					BMessenger(const BHandler *handler, 
								const BLooper *looper = NULL,
								status_t *perr = NULL);
					BMessenger(const BMessenger &from);
					~BMessenger();

/* Target */
		bool		IsTargetLocal() const;
		BHandler	*Target(BLooper **looper) const;
		bool		LockTarget() const;
		status_t	LockTargetWithTimeout(bigtime_t timeout) const;

/* Message sending */
		status_t	SendMessage(uint32 command, BHandler *reply_to = NULL) const;
		status_t	SendMessage(BMessage *a_message,
								BHandler *reply_to = NULL,
								bigtime_t timeout = B_INFINITE_TIMEOUT) const;
		status_t	SendMessage(BMessage *a_message,
								BMessenger reply_to,
								bigtime_t timeout = B_INFINITE_TIMEOUT) const;
	
		status_t	SendMessage(uint32 command, BMessage *reply) const;
		status_t	SendMessage(BMessage *a_message,
								BMessage *reply,
								bigtime_t send_timeout = B_INFINITE_TIMEOUT,
								bigtime_t reply_timeout = B_INFINITE_TIMEOUT) const;
	
/* Operators and misc */
		BMessenger	&operator=(const BMessenger &from);
		bool		operator==(const BMessenger &other) const;

		bool		IsValid() const;
		team_id		Team() const;

/*----- Private or reserved ------------------------------*/
private:
friend class BRoster;
friend class _TRoster_;
friend class BMessage;
friend inline void	_set_message_reply_(BMessage *, BMessenger);
friend status_t		swap_data(type_code, void *, size_t, swap_action);
friend bool		operator<(const BMessenger & a, const BMessenger & b);
friend bool		operator!=(const BMessenger & a, const BMessenger & b);
				
					BMessenger(team_id team,
								port_id port,
								int32 token,
								bool preferred);

		void		InitData(const char *mime_sig,
							team_id team,
							status_t *perr);

		port_id		fPort;
		int32		fHandlerToken;
		team_id		fTeam;
		int32		extra0;
		int32		extra1;
		bool		fPreferredTarget;
		bool		extra2;
		bool		extra3;
		bool		extra4;
};

/*-------------------------------------------------------------*/
/*-------------------------------------------------------------*/

#endif /* _MESSENGER_H */
