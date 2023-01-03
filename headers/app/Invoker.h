#ifndef _INVOKER_H
#define _INVOKER_H

#include <AppDefs.h>
#include <Messenger.h>

// class BHandler;
// class BLooper;
// class BMessage;

class BInvoker
{
   public:
	BInvoker();
	BInvoker(BMessage		  *message,
			 const BHandler *handler,
			 const BLooper  *looper = nullptr);
	BInvoker(BMessage *message, BMessenger target);
	virtual ~BInvoker();

	virtual status_t SetMessage(BMessage *message);
	BMessage		 *Message() const;
	uint32			 Command() const;

	virtual status_t SetTarget(const BHandler *h, const BLooper *loop = nullptr);
	virtual status_t SetTarget(BMessenger messenger);
	bool			 IsTargetLocal() const;
	BHandler		 *Target(BLooper **looper = nullptr) const;
	BMessenger		 Messenger() const;

	virtual status_t SetHandlerForReply(BHandler *handler);
	BHandler		 *HandlerForReply() const;

	virtual status_t Invoke(BMessage *msg = nullptr);

	// Invoke with BHandler notification.  Use this to perform an
	// Invoke() with some other kind of notification change code.
	// (A raw invoke should always notify as B_CONTROL_INVOKED.)
	// Unlike a raw Invoke(), there is no standard message that is
	// sent.  If 'msg' is NULL, then nothing will be sent to this
	// invoker's target...  however, a notification message will
	// still be sent to any watchers of the invoker's handler.
	// Note that the BInvoker class does not actually implement
	// any of this behavior -- it is up to subclasses to override
	// Invoke() and call Notify() with the appropriate change code.
	status_t InvokeNotify(BMessage *msg, uint32 kind = B_CONTROL_INVOKED);

	status_t  SetTimeout(bigtime_t timeout);
	bigtime_t Timeout() const;

   protected:
	// Return the change code for a notification.  This is either
	// B_CONTROL_INVOKED for raw Invoke() calls, or the kind
	// supplied to InvokeNotify().  In addition, 'notify' will be
	// set to true if this was an InvokeNotify() call, else false.
	uint32 InvokeKind(bool *notify = nullptr);

	// Start and end an InvokeNotify context around an Invoke() call.
	// These are only needed for writing custom methods that
	// emulate the standard InvokeNotify() call.
	void BeginInvokeNotify(uint32 kind = B_CONTROL_INVOKED);
	void EndInvokeNotify();

   private:
	BInvoker(const BInvoker &);
	BInvoker &operator=(const BInvoker &);

	BMessage	 *fMessage;
	BMessenger fMessenger;
	BHandler	 *fReplyTo;
	// uint32		fTimeout;
	// uint32		fNotifyKind;
};

#endif /* _INVOKER_H */
