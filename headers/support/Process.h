/*
 * Copyright (c) 2005 Palmsource, Inc.
 *
 * This software is licensed as described in the file LICENSE, which
 * you should have received as part of this distribution. The terms
 * are also available at http://www.openbinder.org/license.html.
 *
 * This software consists of voluntary contributions made by many
 * individuals. For the exact contribution history, see the revision
 * history and logs, available at http://www.openbinder.org
 */

#ifndef SUPPORT_PROCESS_H
#define SUPPORT_PROCESS_H

/*!	@file support/Process.h
	@ingroup CoreSupportBinder
	@brief Implementation of the IProcess interface for the local process.
*/

#include <support/Atom.h>
#include <support/ConditionVariable.h>
#include <support/IBinder.h>
#include <support/InstantiateComponent.h>
#include <support/KeyedVector.h>
#include <support/Locker.h>
#include <support/Looper.h>
#include <support/Package.h>
#include <support/String.h>
#include <support/SupportDefs.h>
#include <support/Value.h>
#include <support/Vector.h>

#if !LIBBE_BOOTSTRAP
#include <support/IProcess.h>
#endif

namespace os {
namespace support {

/*!	@addtogroup CoreSupportBinder
	@{
*/

class SHandler;
class IVirtualMachine;

extern int32_t this_team();

/*----------------------------------------------------------------*/

enum {
  B_FORGET_CURRENT_ENVIRONMENT = 0x00000001
};

//!	Representation of your currently running process.
class BProcess
#if !LIBBE_BOOTSTRAP
    : public BnProcess
#endif
{
 public:
  BProcess(team_id tid);

  team_id ID() const;

#if !LIBBE_BOOTSTRAP
  //!	Create a new, empty team, which you can start instantiating
  //!	components in.
  static sptr<IProcess> Spawn(const SString& name  = SString(),
                              const SValue&  env   = SValue(),
                              uint32_t       flags = 0);

  //!	Load the requested image as a new team, and return its root object.
  static sptr<IBinder> SpawnFile(const SString& file_path,
                                 const SValue&  env   = SValue(),
                                 uint32_t       flags = 0);

  //! Implement IProcess interface.
  virtual sptr<IBinder> InstantiateComponent(const sptr<INode>& node,
                                             const SValue&      componentInfo,
                                             const SString&     component,
                                             const SValue&      args,
                                             status_t*          outError = NULL);
  virtual int32_t       AtomMarkLeakReport();
  virtual void          AtomLeakReport(int32_t mark, int32_t last, uint32_t flags);

  virtual void PrintBinderReferences(void);
#endif

  virtual void RestartMallocProfiling(void);
  virtual void SetMallocProfiling(bool enabled, int32_t dumpPeriod, int32_t maxItems, int32_t stackDepth);
  virtual void RestartVectorProfiling(void);
  virtual void SetVectorProfiling(bool enabled, int32_t dumpPeriod, int32_t maxItems, int32_t stackDepth);
  virtual void RestartMessageIPCProfiling(void);
  virtual void SetMessageIPCProfiling(bool enabled, int32_t dumpPeriod, int32_t maxItems, int32_t stackDepth);
  virtual void RestartBinderIPCProfiling(void);
  virtual void SetBinderIPCProfiling(bool enabled, int32_t dumpPeriod, int32_t maxItems, int32_t stackDepth);
  virtual void PrintBinderIPCProfiling(void);

  typedef void (*catchReleaseFunc)(IBinder* obj);
  void CatchHandleRelease(const sptr<IBinder>& remoteObject, catchReleaseFunc callbackFunc);

  nsecs_t GetNextEventTime() const;
  void    SetHandlerConcurrency(int32_t maxConcurrency);
  void    BatchPutReferences();

  // ----------- The remaining methods are not for public use -----------

  void DispatchMessage(SLooper* looper);

  sptr<IBinder> GetStrongProxyForHandle(int32_t handle);
  wptr<IBinder> GetWeakProxyForHandle(int32_t handle);
  void          ExpungeHandle(int32_t handle, IBinder* binder);
  void          StrongHandleGone(IBinder* binder);

  void    Shutdown();
  bool    IsShuttingDown() const;
  nsecs_t GetEventDelayTime(SLooper* caller) const;

  class ComponentImage : public BSharedObject
  {
   public:
    inline ComponentImage(const SValue& file, const SPackage& package, bool fake, attach_func attach)
        : BSharedObject(file, package, attach), m_instantiate(NULL), m_fake(fake), m_numPendingExpunge(1), m_expunged(0)
    {
    }

    // Ask this .so to instantiate a component
    inline sptr<IBinder> InstantiateComponent(const SString& component, const SContext& context, const SValue& args) const;

    inline status_t InitCheck() const { return (m_instantiate || m_fake) ? B_OK : B_NO_INIT; }

    inline int32_t DecPending() { return atomic_fetch_dec(&m_numPendingExpunge); }
    inline void    MakeExpunged() { atomic_fetch_or(&m_expunged, 1U); }

   protected:
    virtual ~ComponentImage() {}

    virtual void     InitAtom();
    virtual status_t FinishAtom(const void* id);
    virtual status_t IncStrongAttempted(uint32_t flags, const void* id);

   private:
    instantiate_component_func m_instantiate;
    bool                       m_fake;
    volatile atomic_int        m_numPendingExpunge;
    volatile atomic_uint       m_expunged;

   private:
    // copy constructor not supported
    ComponentImage(const ComponentImage& rhs);
  };

  bool ExpungePackage(const wptr<ComponentImage>& image);

 protected:
  virtual ~BProcess();
  virtual void InitAtom();

 private:
  friend class SHandler;
  friend class ProcessFreeKey;

  static bool ResumingScheduling();
  static void ClearSchedulingResumed();
  static bool InsertHandler(SHandler** handlerP, SHandler* handler);
  void        UnscheduleHandler(SHandler* h, bool lock = true);
  bool        ScheduleNextEvent();
  void        ScheduleHandler(SHandler* h);
  // This function is called with the lock held, and returns with it released.
  void                 ScheduleNextHandler();
  IBinder*&            BinderForHandle(int32_t handle);
  sptr<ComponentImage> get_shared_object(const SValue& file, const SValue& info, bool fake = false, attach_func attach = NULL);
  sptr<IBinder>        DoInstantiate(const SContext&   context,
                                     const SValue&     componentInfo,
                                     const SString&    component,
                                     SVector<SString>& vmIDs,
                                     const SValue&     args,
                                     status_t*         outError);
  void                 GetRemoteHostInfo(int32_t handle);
  void                 ReleaseRemoteReferences();

  enum {
    MAX_LOOPERS_PER_TEAM = 63
  };

  const team_id   m_id;
  mutable SLocker m_lock;
  SHandler*       m_pendingHandlers;
  nsecs_t         m_nextEventTime;
  int32_t         m_maxEventConcurrency;
  int32_t         m_currentEventConcurrency;
  bool            m_shutdown;

  // Information about loaded component images.
  SNestedLocker m_imageLock;
  struct ImageData;
  ImageData* m_imageData;

  // Information about remote binders.
  mutable SNestedLocker m_handleRefLock;
  bool                  m_remoteRefsReleased;
  SVector<IBinder*>     m_handleRefs;
  SKeyedVector<IBinder*, catchReleaseFunc>
      m_catchers;

  // -----------------------------------------------
  // These are used for scheduling handlers when
  // there is no driver available to do so.

  //! The amount of idle time before the thread exits.
  nsecs_t GetIdleTimeout() const;
  //! Pops 'looper' if on top and if when is <= the next event time.
  bool PopLooper(SLooper* looper, nsecs_t when);
  //! Pushes a looper onto the stack.
  void PushLooper(SLooper* looper);
  //! Removes the looper from the stack.
  bool RemoveLooper(SLooper* looper);
  //! Removes the top looper from the stack.
  SLooper* UnsafePopLooper();
  //! The current looper count.
  int32_t LooperCount() const;
  //! The maximum number of loopers allowed in this team.
  int32_t MaximumLoopers() const;
  //! The minimum number of loopers allowed in this team.
  int32_t MinimumLoopers() const;
  //! Increments the looper count by one.
  void IncrementLoopers();
  //! Decrements the looper count by one.
  void DecrementLoopers();
  //! Used for debugging.
  void CheckIntegrity() const;

  int32_t  m_idleCount;
  SLooper* m_idleLooper;
  nsecs_t  m_idleTimeout;
  int32_t  m_loopers;
  int32_t  m_maxLoopers;
  int32_t  m_minLoopers;
};

/*!	@} */

inline sptr<IBinder>
BProcess::ComponentImage::InstantiateComponent(const SString& component, const SContext& context, const SValue& args) const
{
  if (m_instantiate)
    return m_instantiate(component, context, args);
  else
    return NULL;
}

}  // namespace support
}  // namespace os

#endif /* SUPPORT_PROCESS_H */
