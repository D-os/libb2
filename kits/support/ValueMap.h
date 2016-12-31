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

#ifndef _SUPPORT_VALUEMAP_H
#define _SUPPORT_VALUEMAP_H

//#include <support/IByteStream.h>
#include <support/Value.h>
#include <utils/Mutex.h>
#include <utils/Vector.h>

#include "ValueMapFormat.h"
//#include <support_p/SupportMisc.h>

namespace os {
namespace support {

class SParcel;
class BValueMap;

class BValueMapPool
{
public:
    BValueMapPool();
    ~BValueMapPool();

    BValueMap* create(size_t initSize);
    void destroy(BValueMap* map);

    int32_t hits() const { return m_hits; }
    int32_t misses() const { return m_misses; }

private:
    Mutex m_lock;
    struct pool {
        int32_t count;
        BValueMap* objects;

        pool() : count(0), objects(NULL) { }
    };
    enum { NUM_POOLS = 3 };
    pool m_pools[NUM_POOLS];
    int32_t m_hits, m_misses;
};

extern BValueMapPool g_valueMapPool;

class BValueMap
{
public:
            struct pair {
                SValue key;
                SValue value;

                inline pair() { }
                inline pair(const SValue& k, const SValue& v)	:	key(k), value(v) { }
                inline pair(const pair& o)						:	key(o.key), value(o.value) { }
                inline ~pair() { }

                inline pair& operator=(const pair& o) { key = o.key; value = o.value; return *this; }
            };

    static	BValueMap*		Create(size_t initSize = 1);
    static	BValueMap*		Create(SParcel& from, size_t avail, size_t count, ssize_t* out_size);
            BValueMap*		clone() const;

            void			setFirstMap(const SValue& key, const SValue& value);

            void			acquire() const;
            void			release() const;
            bool			isShared() const;

            ssize_t			archivedSize() const;
            ssize_t			archive(SParcel& into) const;
            ssize_t			indexFor(	const SValue& key,
                                        const SValue& value = B_UNDEFINED_VALUE) const;
            ssize_t			indexFor(	uint32_t type, const void* data, size_t length) const;
            const pair&		mapAt(size_t index) const;

            size_t			countMaps() const;
            int32_t			compare(const BValueMap& o) const;
            int32_t			lexicalCompare(const BValueMap& o) const;

    static	ssize_t			addNewMap(BValueMap** This, const SValue& key, const SValue& value);
    static	status_t		removeMap(	BValueMap** This,
                                        const SValue& key,
                                        const SValue& value = B_UNDEFINED_VALUE);
    static	void			removeMapAt(BValueMap** This, size_t index);
    static	status_t		renameMap(	BValueMap** This,
                                        const SValue& old_key,
                                        const SValue& new_key);

    static	SValue*			beginEditMapAt(BValueMap** This, size_t index);
    static	void			endEditMapAt(BValueMap** This);
            bool			isEditing() const;

            void			pool();

            void 			assertEditing() const;

private:
    friend	class			BValueMapPool;

                            // These are not implemented.
                            BValueMap();
                            BValueMap(const BValueMap& o);
                            ~BValueMap();

            void			construct(int32_t avail);
            void			destroy();

    static	ssize_t			addMapAt(BValueMap** This, size_t index, const SValue& key, const SValue& value);

            bool			getIndexOf(	uint32_t type, const void* data, size_t length,
                                        size_t* index) const;
            bool			getIndexOf(	const SValue& k, const SValue& v,
                                        size_t* index) const;
            ssize_t			computeArchivedSize() const;

    mutable	atomic_int		m_users;
    mutable	ssize_t			m_dataSize;
            ssize_t			m_size;
            ssize_t			m_avail;
            int32_t			m_pad0;
            ssize_t			m_editIndex;

            // Mappings start here.
            pair			m_maps[1];
};

inline void BValueMap::acquire() const
{
    assertEditing();

    m_users.fetch_add(1, std::memory_order_relaxed);
}

inline void BValueMap::release() const
{
    assertEditing();

    if (m_users.fetch_sub(1, std::memory_order_release) == 1)
        const_cast<BValueMap*>(this)->destroy();
}

inline bool BValueMap::isShared() const
{
    return m_users > 1;
}

inline ssize_t BValueMap::archivedSize() const
{
    if (m_dataSize >= 0) return m_dataSize;
    return computeArchivedSize();
}

inline ssize_t BValueMap::indexFor(const SValue& key, const SValue& value) const
{
    size_t index;
    return getIndexOf(key, value, &index) ? index : NAME_NOT_FOUND;
}

inline ssize_t BValueMap::indexFor(uint32_t type, const void* data, size_t length) const
{
    size_t index;
    return getIndexOf(type, data, length, &index) ? index : NAME_NOT_FOUND;
}

inline const BValueMap::pair& BValueMap::mapAt(size_t index) const
{
    return m_maps[index];
}

inline size_t BValueMap::countMaps() const
{
    return m_size;
}

inline bool	BValueMap::isEditing() const
{
    return m_editIndex >= 0;
}

inline void BValueMap::assertEditing() const
{
    LOG_ALWAYS_FATAL_IF(isEditing(), "This operation can not be performed while editing a value");
}

} }	// namespace os::support
#endif
