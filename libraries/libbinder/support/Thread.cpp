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

#include <CmnErrors.h>
#include <ErrorMgr.h>
#include <support/Thread.h>

#include <support/Autolock.h>
#include <support/Looper.h>
#include <support/StdIO.h>
#include <support/atomic.h>

#if TARGET_HOST == TARGET_HOST_PALMOS
#include <SysThread.h>
#endif

namespace os {
namespace support {

SysHandle SpawnThread(thread_func function_name, const char* thread_name, int32_t priority, void* arg)
{
  SysHandle thid;

//	bout << "SLooper: " << This() << " (" << find_thread(NULL) << ") SpwanThread: entering" << endl;
#if BINDER_DEBUG_LIB
  spawn_thread_info* info = (spawn_thread_info*)malloc(sizeof(spawn_thread_info));
  if (info) {
    info->parent = This();
    info->func   = function_name;
    info->arg    = arg;
    SysThreadCreate(NULL, thread_name, priority, sysThreadStackUI,
                    (SysThreadEnterFunc*)_ThreadEntry, info, &thid);
  }
  else {
    thid = B_NO_MEMORY;
  }
#else
  // Because of the UNSIGNED nature of SysHandle, we can only return a failure sentinal or the thread ID.
  if (SysThreadCreate(NULL, thread_name, priority, sysThreadStackUI, (SysThreadEnterFunc*)function_name, arg, &thid))
    return 0;
#endif

  //	bout << "SLooper: " << This() << " (" << find_thread(NULL) << ") SpwanThread: exiting" << endl;
  return thid;
}

SThread::SThread()
    : m_status(B_OK), m_thread(0), m_lock("SThread Access"), m_ended(sysConditionVariableInitializer), m_ending(false)
{
}

SThread::~SThread()
{
  // DO NOT CALL WAITFOREXIT() HERE!
  // In the path where the object gets destroyed, its thread
  // can't safely open the critical section because it is
  // unable to acquire a strong reference.
}

status_t
SThread::AboutToRun(SysHandle thread)
{
  (void)thread;
  return errNone;
}

status_t
SThread::Run(const char* name, int32_t priority, size_t stackSize)
{
  m_lock.Lock();

  if (m_status < B_OK || m_thread != 0) {
    m_lock.Unlock();
    ErrFatalError("SThread::Run() already called.");
    return m_status;
  }

  // Acquire a strong reference on behalf of the thread
  // we are spawning.
  incStrong(this);

  (void)stackSize;  // XXX currently ignored.
  m_thread = SpawnThread((thread_func)top_entry, name, priority, this);
  if (m_thread != B_ERROR) {
    m_status = AboutToRun(m_thread);
    if (m_status == B_OK) {
      m_status = (SysThreadStart(m_thread) != errNone) ? B_ERROR : B_OK;
      if (m_status < B_OK) {
        m_thread = m_status;
      }
    }
  }

  // If there was a problem, don't leak!
  if (m_status < B_OK) decStrong(this);

  m_lock.Unlock();

  return m_status;
}

void SThread::RequestExit()
{
  m_lock.Lock();
  m_ending = true;
  m_lock.Unlock();
}

void SThread::WaitForExit()
{
  RequestExit();

  if (m_thread == SysCurrentThread()) {
    // Naughty, naughty.
    ErrFatalError("Can't call WaitForExit() from target thread!");
  }
  else if (m_status >= B_OK) {
    // Wait for the thread to exit.
#if 1  // FIXME: For Linux
    SysConditionVariableWait(&m_ended, NULL);
#else
    KALConditionVariableWait(&m_ended, NULL);
#endif
  }
}

bool SThread::ExitRequested() const
{
  return m_ending;
}

#if TARGET_HOST == TARGET_HOST_PALMOS
void SThread::top_entry(void* object)
#else
int32_t SThread::top_entry(void* object)
#endif
{
  /*	We want to allow this thread to run at least
		once.  This is to support code such as:

		void do_stuff()
		{
			sptr<SThread> thread = new myThreadSubclass();
			thread->Run();
		}

		In this case the external reference on the object
		will go away immediately, and by the time we get
		to this function it may be gone.

		Upon entry to this function, we still hold a strong
		reference to the object, which is convenient.
	*/

  bool result;

  // Always keep a weak pointer to the object, so we can
  // later attempt to acquire a strong pointer.
  weakref_type* weak = ((SThread*)object)->createWeak(object);

  do {
    result = ((SThread*)object)->ThreadEntry();

    // We absolutely want to exit if either the thread or
    // an external party requested we do so.
    if (result == false || ((SThread*)object)->m_ending) {
      // In this case the object is still alive, so we
      // need to be nice to anyone else who may have called
      // End() on it.
      ((SThread*)object)->m_ending = true;
#if 1  // FIXME: Linux
      SysConditionVariableOpen(&((SThread*)object)->m_ended);
#else
      KALConditionVariableOpen(&((SThread*)object)->m_ended);
#endif
      ((SThread*)object)->decStrong(object);
      break;
    }

    // Release reference on the object, allowing it to die
    // a natural death.  Repeat the loop if we are able to
    // acquire another strong reference.
    ((SThread*)object)->decStrong(object);
  } while (weak->attemptIncStrong(object));

  // Goodbye.
  weak->decWeak(object);

#if TARGET_HOST != TARGET_HOST_PALMOS
  return 0;
#endif
}

}  // namespace support
}  // namespace os
