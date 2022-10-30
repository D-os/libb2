#ifndef _LOOPER_H
#define _LOOPER_H

#include <Handler.h>
#include <List.h>

#include <mutex>

class BMessageQueue;

/// Port (Message Queue) Capacity
#define B_LOOPER_PORT_DEFAULT_CAPACITY 100

class BLooper : public BHandler
{
   public:
	BLooper(const char *name		  = nullptr,
			int32		priority	  = B_NORMAL_PRIORITY,
			int32		port_capacity = B_LOOPER_PORT_DEFAULT_CAPACITY);
	virtual ~BLooper();

	/// Archiving
	BLooper(BMessage *data);
	static BArchivable *Instantiate(BMessage *data);
	virtual status_t	Archive(BMessage *data, bool deep = true) const override;

	/// Message transmission
	status_t PostMessage(uint32 command);
	status_t PostMessage(BMessage *message);
	status_t PostMessage(uint32	   command,
						 BHandler *handler,
						 BHandler *reply_to = nullptr);
	status_t PostMessage(BMessage *message,
						 BHandler *handler,
						 BHandler *reply_to = nullptr);

	virtual void   DispatchMessage(BMessage *message, BHandler *handler);
	virtual void   MessageReceived(BMessage *msg) override;
	BMessage		 *CurrentMessage() const;
	BMessage		 *DetachCurrentMessage();
	BMessageQueue *MessageQueue() const;
	bool		   IsMessageWaiting() const;

	/// Message handlers
	void	  AddHandler(BHandler *handler);
	bool	  RemoveHandler(BHandler *handler);
	int32	  CountHandlers() const;
	BHandler *HandlerAt(int32 index) const;
	int32	  IndexOf(BHandler *handler) const;

	BHandler *PreferredHandler() const;
	void	  SetPreferredHandler(BHandler *handler);

	/// Loop control
	virtual thread_id Run();
	virtual void	  Quit();
	virtual bool	  QuitRequested();
	bool			  Lock();
	void			  Unlock();
	bool			  IsLocked() const;
	status_t		  LockWithTimeout(bigtime_t timeout);
	thread_id		  Thread() const;
	team_id			  Team() const;
	static BLooper   *LooperForThread(thread_id tid);

	/// Loop debugging
	thread_id LockingThread() const;
	int32	  CountLocks() const;
	int32	  CountLockRequests() const;
	sem_id	  Sem() const;

	/// Scripting
	virtual BHandler *ResolveSpecifier(BMessage	*msg,
									   int32	   index,
									   BMessage	*specifier,
									   int32	   form,
									   const char *property) override;
	virtual status_t  GetSupportedSuites(BMessage *data) override;

	/// Message filters (also see BHandler).
	virtual void AddCommonFilter(BMessageFilter *filter);
	virtual bool RemoveCommonFilter(BMessageFilter *filter);
	virtual void SetCommonFilterList(BList *filters);
	BList		  *CommonFilterList() const;

   protected:
	/// called from overridden task_looper
	BMessage *MessageFromPort(bigtime_t = B_INFINITE_TIMEOUT);

   private:
	friend class BApplication;
	friend class BWindow;

	BLooper(const BLooper &);
	BLooper &operator=(const BLooper &);

	status_t _PostMessage(BMessage *msg,
						  BHandler *handler,
						  BHandler *reply_to);

	static status_t _task0_(void *arg);
	virtual void	task_looper();
	bool			AssertLocked() const;

	BMessageQueue *fQueue;
	BMessage		 *fLastMessage;
	sem_id		   fLockSem;	// locks Looper between threads
	std::mutex	   fLockMutex;	// protects Lock/Unlock critical sections in-thread
	unsigned long  fOwnerCount;
	thread_id	   fOwner;
	thread_id	   fThread;
	int32		   fInitPriority;
	BHandler		 *fPreferred;
	BList		   fHandlers;
	bool		   fTerminating;
	bool		   fRunCalled;
};

#endif /* _LOOPER_H */
