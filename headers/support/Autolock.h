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

#ifndef	_SUPPORT_AUTOLOCK_H
#define	_SUPPORT_AUTOLOCK_H

/*!	@file support/Autolock.h
    @ingroup CoreSupportUtilities
    @brief Stack-based automatic locking.
*/

#include <support/SupportDefs.h>
//#include <utils/Mutex.h>

namespace os { namespace support {

/*-----------------------------------------------------------------*/
/*----- Autolock class -------------------------------------------*/

/*!	@addtogroup CoreSupportUtilities
    @{
*/

//!	Smart locking class.
/*!	This is a convenience class that automatically releases a lock
    when the class is destroyed.  Some synchronization classes supply
    their own autolockers (for example SLocker::Autolock) that are
    slightly more efficient.  This class works with any synchronization
    object as lock as it returns lock_status_t from its locking
    operation.

    Example usage:
@code
void MyClass::MySynchronizedMethod()
{
    Autolock _l(m_lock.Lock());

    ...
}
@endcode
*/
class Autolock
{
public:
    inline				Autolock(const lock_status_t& status);
    inline				Autolock();
    inline				~Autolock();
    inline	bool		IsLocked();

    inline	void		SetTo(const lock_status_t& status);
    inline	void		Unlock();

private:
                        Autolock(const Autolock&);
            Autolock&	operator = (const Autolock&);

    lock_status_t	m_status;
};

/*!	@} */

/*-------------------------------------------------------------*/
/*---- No user serviceable parts after this -------------------*/

inline Autolock::Autolock(const lock_status_t& status)
    :	m_status(status)
{
}

inline Autolock::Autolock()
    :	m_status(-1)
{
}

inline Autolock::~Autolock()
{
    m_status.unlock();
}

inline bool Autolock::IsLocked()
{
    return m_status.is_locked();
}

inline void Autolock::SetTo(const lock_status_t& status)
{
    m_status.unlock();
    m_status = status;
}

inline void Autolock::Unlock()
{
    m_status.unlock();
    m_status = lock_status_t(-1);
}

} }	// namespace os::support

#endif /* _SUPPORT_AUTOLOCK_H */
