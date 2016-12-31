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

#include <support/Value.h>
//#include <support/KeyedVector.h>
//#include <support/Atom.h>
//#include <support/Debug.h>
//#include <support/ITextStream.h>
#include <support/String.h>
//#include <support/TypeConstants.h>
//#include <support/Binder.h>
#include <binder/Parcel.h>
//#include <support/Looper.h>
//#include <support/SharedBuffer.h>
//#include <support/StdIO.h>
//#include <support/StringIO.h>

//#include <support_p/SupportMisc.h>
#include "ValueMap.h"
//#include <support_p/WindowsCompatibility.h>
//#include <support_p/binder_module.h>

//#include <math.h>
//#include <ctype.h>
//#include <float.h>
//#include <new>
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>

#include "ValueInternal.h"

#ifndef VALIDATES_VALUE
#define VALIDATES_VALUE 0
#endif

#ifndef DISABLE_COW
#define DISABLE_COW 0
#endif

#if VALIDATES_VALUE
#define DB_INTEGRITY_CHECKS 1
#define DB_CORRECTNESS_CHECKS 1
#else
#if BUILD_TYPE == BUILD_TYPE_DEBUG
#define DB_INTEGRITY_CHECKS 1
#else
#define DB_INTEGRITY_CHECKS 0
#endif
#define DB_CORRECTNESS_CHECKS 0
#endif

namespace os {
namespace support {

//using namespace os::osp;

#if DB_INTEGRITY_CHECKS
#define CHECK_INTEGRITY(v) (v).check_integrity()
#else
#define CHECK_INTEGRITY(v)
#endif

#if DB_CORRECTNESS_CHECKS
static void check_correctness(const SValue& mine, const SValue& correct,
    const SValue& opLeft, const char* opName, const SValue& opRight)
{
    if (mine != correct) {
        sp<BStringIO> sio(new BStringIO);
        sp<ITextOutput> io(sio.ptr());
        io << "Correctness check failed!\nWhile computing:\n"
            << indent << opLeft << dedent << endl
            << opName << endl
            << indent << opRight << dedent << endl
            << "Result:\n"
            << indent << (mine) << dedent << endl
            << "Should have been:\n"
            << indent << correct << dedent << endl;
        LOG_ALWAYS_FATAL(sio->String());
    }
}
#define INIT_CORRECTNESS(ops) ops
#define CHECK_CORRECTNESS(mine, correct, left, op, right) check_correctness((mine), (correct), (left), (op), (right))
#else
#define INIT_CORRECTNESS(ops)
#define CHECK_CORRECTNESS(mine, correct, left, op, right)
#endif

#if SUPPORTS_TEXT_STREAM
struct printer_registry {
    Mutex lock;
    SKeyedVector<type_code, SValue::print_func> funcs;

#if !(__CC_ARM)
    printer_registry()	: lock("SValue printer registry"), funcs((SValue::print_func)NULL) { }
#else
    printer_registry()	: lock("SValue printer registry"), funcs() { }
#endif
};

static atomic_int g_havePrinters(0);
static printer_registry* g_printers = NULL;

static printer_registry* Printers()
{
    if (g_havePrinters&2) return g_printers;
    if (atomic_fetch_or(&g_havePrinters, 1) == 0) {
        g_printers = new B_NO_THROW printer_registry;
        atomic_fetch_or(&g_havePrinters, 2);
    } else {
        if ((g_havePrinters&2) == 0)
           SysThreadDelay(B_MILLISECONDS(10), B_RELATIVE_TIMEOUT);
    }
    return g_printers;
}
#endif

#if SUPPORTS_TEXT_STREAM
static const char* UIntToHex(uint64_t val, char* out)
{
    sprintf(out, "0x%" B_FORMAT_INT64 "x", val);
    return out;
}
#endif

status_t SValue::registerPrintFunc(type_code type, print_func func)
{
#if SUPPORTS_TEXT_STREAM
    printer_registry* reg = Printers();
    if (reg && reg->lock.lock() == OK) {
        reg->funcs.addItem(type, func);
        reg->lock.unlock();
    }
    return OK;
#else
    (void)type;
    (void)func;
    return INVALID_OPERATION;
#endif
}

status_t SValue::unregisterPrintFunc(type_code type, print_func func)
{
#if SUPPORTS_TEXT_STREAM
    printer_registry* reg = Printers();
    if (reg && reg->lock.lock() == OK) {
        bool found;
        print_func f = reg->funcs.valueFor(type, &found);
        if (found && (f == func || func == NULL))
            reg->funcs.removeItemFor(type);
        reg->lock.unlock();
    }
    return OK;
#else
    (void)type;
    (void)func;
    return INVALID_OPERATION;
#endif
}

// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------

void SValue::freeData()
{
    const ssize_t s = B_UNPACK_TYPE_LENGTH(m_type);
    if ((s < sizeof(void*)) || ((s == sizeof(void*)) && !is_object()))
        return;

    switch (s) {
        case B_TYPE_LENGTH_LARGE:
            m_data.buffer->release();
#if BUILD_TYPE == BUILD_TYPE_DEBUG
            m_type = 0xdeadf00d;
            m_data.uinteger = 0xdeadf00d;
#endif
            return;
        case B_TYPE_LENGTH_MAP:
            m_data.map->release();
#if BUILD_TYPE == BUILD_TYPE_DEBUG
            m_type = 0xdeadf00d;
            m_data.uinteger = 0xdeadf00d;
#endif
            return;
        case sizeof(void*):
            STUB;
            //release_object(*(small_flat_data*)this, this);
#if BUILD_TYPE == BUILD_TYPE_DEBUG
            m_type = 0xdeadf00d;
            m_data.uinteger = 0xdeadf00d;
#endif
            return;
    }
}

void* SValue::alloc_data(type_code type, size_t len)
{
    LOG_FATAL_IF(type == B_UNDEFINED_TYPE, "B_UNDEFINED_TYPE not valid here.");

    if (len <= B_TYPE_LENGTH_MAX) {
        m_type = B_PACK_SMALL_TYPE(type, len);
        return m_data.local;
    }

    if ((m_data.buffer=SharedBuffer::alloc(len)) != NULL) {
        m_type = B_PACK_LARGE_TYPE(type);
        return const_cast<void*>(m_data.buffer->data());
    }

    m_type = kErrorTypeCode;
    m_data.integer = NO_MEMORY;
    return NULL;
}

void SValue::shrink_map(BValueMap* map)
{
    switch (map->countMaps()) {
        case 0: {
            undefine();
        } break;
        case 1: {
            const BValueMap::pair& p = map->mapAt(0);
            if (p.key.is_wild()) {
                map->acquire();
                *this = p.value;
                map->release();
            }
        } break;
    }
}

// -----------------------------------------------------------------------

SValue::SValue(type_code type, const SharedBuffer* buffer)
{
    //printf("*** Creating SValue %p from type,SharedBuffer\n", this);
    VALIDATE_TYPE(type);
    if (buffer) {
        init_as_shared_buffer(type, buffer);
    } else {
        LOG_FATAL_IF(type == B_UNDEFINED_TYPE, "B_UNDEFINED_TYPE not valid here.");
        m_type = B_PACK_SMALL_TYPE(type, 0);
    }
}

SValue::SValue(const char* str)
{
    //printf("*** Creating SValue %p from string\n", this);
    if (str)
        initAsRaw(B_STRING_TYPE, str, strlen(str)+1);
    else {
        m_type = B_PACK_SMALL_TYPE(B_STRING_TYPE, 1);
        m_data.local[0] = 0;
    }
}

SValue::SValue(const SString& str)
{
    //printf("*** Creating SValue %p from SString\n", this);
    const SharedBuffer* sb = str.sharedBuffer();
    if (sb) {
        init_as_shared_buffer(B_STRING_TYPE, sb);
    } else {
        m_type = B_PACK_SMALL_TYPE(B_STRING_TYPE, 1);
        m_data.integer = 0;
    }
}

SValue::SValue(const sp<IBinder>& binder)
{
    //printf("*** Creating SValue %p from sp<IBinder>\n", this);
    STUB;
    //flatten_binder(binder, (small_flat_data*)this);
    //acquire_object(*(small_flat_data*)this, this);
}

SValue::SValue(const wp<IBinder>& binder)
{
    //printf("*** Creating SValue %p from wp<IBinder>\n", this);
    STUB;
    //flatten_binder(binder, (small_flat_data*)this);
    //acquire_object(*(small_flat_data*)this, this);
}

// ---------------------------------------------------------------------

status_t SValue::statusCheck() const
{
    status_t val = errorCheck();

    if (val == OK) {
        switch (m_type) {
            case B_PACK_SMALL_TYPE(B_STATUS_TYPE, sizeof(status_t)):
                return m_data.integer;
            case kUndefinedTypeCode:
                return NO_INIT;
        }
    }

    return val;
}

status_t SValue::errorCheck() const
{
    return is_error() ? (status_t)m_data.integer : OK;
}

void SValue::setError(status_t error)
{
    set_error(error);
}

SValue& SValue::assign(const SValue& o)
{
    if (this != &o) {
        freeData();
        initAsCopy(o);
    }

    return *this;
}

SValue& SValue::assign(type_code type, const void* data, size_t len)
{
    VALIDATE_TYPE(type);
    freeData();
    initAsRaw(type, data, len);

    return *this;
}

void SValue::initAsCopy(const SValue& o)
{
    m_data = o.m_data;
    m_type = o.m_type;
    switch (B_UNPACK_TYPE_LENGTH(m_type)) {
        case B_TYPE_LENGTH_LARGE:
            m_data.buffer->acquire();
#if DISABLE_COW
            m_data.buffer = o.m_data.buffer->edit();
#endif
            CHECK_INTEGRITY(*this);
            return;
        case B_TYPE_LENGTH_MAP:
            m_data.map->acquire();
            CHECK_INTEGRITY(*this);
            return;
        case sizeof(void*):
            STUB;//if (is_object()) acquire_object(*(small_flat_data*)this, this);
            CHECK_INTEGRITY(*this);
            return;
    }

    CHECK_INTEGRITY(*this);
}

void SValue::initAsRaw(type_code type, const void* data, size_t len)
{
    LOG_FATAL_IF(type == B_UNDEFINED_TYPE, "B_UNDEFINED_TYPE not valid here.");

    VALIDATE_TYPE(type);

    if (len <= B_TYPE_LENGTH_MAX) {
        memcpy(m_data.local, data, len);
        m_type = B_PACK_SMALL_TYPE(type, len);
        if (CHECK_IS_SMALL_OBJECT(m_type)) {
            STUB;//acquire_object(*(small_flat_data*)this, this);
        }
        CHECK_INTEGRITY(*this);
        return;
    }

    if ((m_data.buffer=SharedBuffer::alloc(len)) != NULL) {
        m_type = B_PACK_LARGE_TYPE(type);
        memcpy(const_cast<void*>(m_data.buffer->data()), data, len);
        CHECK_INTEGRITY(*this);
        return;
    }

    m_type = kErrorTypeCode;
    m_data.integer = NO_MEMORY;
    CHECK_INTEGRITY(*this);
    return;
}

void SValue::initAsMap(const SValue& key, const SValue& value)
{
    initAsMap(key, value, 0, 1);
}

void SValue::initAsMap(const SValue& key, const SValue& value, uint32_t flags)
{
    initAsMap(key, value, flags, 1);
}

void SValue::initAsMap(const SValue& key, const SValue& value, uint32_t flags, size_t numMappings)
{
    if (!key.is_error() && !value.is_error()) {
        if (key.is_defined() && value.is_defined()) {
            if (!key.is_wild()) {
                if (!key.is_map() || (flags&B_NO_VALUE_FLATTEN)) {
                    m_type = kMapTypeCode;
                    BValueMap* map = BValueMap::Create(numMappings);
                    m_data.map = map;
                    if (map) {
                        map->setFirstMap(key, value);
                    } else {
                        set_error(NO_MEMORY);
                    }
                } else {
                    m_type = kUndefinedTypeCode;
                    size_t i=0;
                    SValue k, v;
                    while (key.getNextItem(reinterpret_cast<void**>(&i), &k, &v) >= OK) {
                        joinItem(k, SValue(v, value));
                    }
                }
            } else {
                initAsCopy(value);
            }
        } else {
            m_type = kUndefinedTypeCode;
        }
    } else if (key.is_error()) {
        initAsCopy(key);
    } else {
        initAsCopy(value);
    }

    CHECK_INTEGRITY(*this);
}

void SValue::init_as_shared_buffer(type_code type, const SharedBuffer* buffer)
{
    LOG_FATAL_IF(type == B_UNDEFINED_TYPE, "B_UNDEFINED_TYPE not valid here.");

    if (buffer->size() <= B_TYPE_LENGTH_MAX) {
        m_type = B_PACK_SMALL_TYPE(type, buffer->size());
        m_data.integer = *(int32_t*)buffer->data();
    } else {
        m_type = B_PACK_LARGE_TYPE(type);
        buffer->acquire();
        m_data.buffer = buffer;
#if DISABLE_COW
        m_data.buffer = buffer->edit();
#endif
    }
}

// ---------------------------------------------------------------------

void SValue::MoveBefore(SValue* to, SValue* from, size_t count)
{
    memmove(to, from, sizeof(SValue)*count);
#if SUPPORTS_ATOM_DEBUG
    for (size_t i=0; i<count; i++) {
        if (to[i].is_object()) {
            STUB;//rename_object(*(small_flat_data*)(to+i), to+i, from+i);
        }
    }
#endif
}

void SValue::MoveAfter(SValue* to, SValue* from, size_t count)
{
    memmove(to, from, sizeof(SValue)*count);
#if SUPPORTS_ATOM_DEBUG
    while (count > 0) {
        count--;
        if (to[count].is_object()) {
            STUB;//rename_object(*(small_flat_data*)(to+count), to+count, from+count);
        }
    }
#endif
}

typedef uint8_t dummy_value[sizeof(SValue)];

void SValue::swap(SValue& with)
{
    dummy_value buffer;

    memcpy(&buffer, &with, sizeof(SValue));
    memcpy(&with, this, sizeof(SValue));
    memcpy(this, &buffer, sizeof(SValue));
}

void SValue::undefine()
{
    freeData();
    m_type = kUndefinedTypeCode;
    CHECK_INTEGRITY(*this);
}

bool SValue::isObject() const
{
    return CHECK_IS_SMALL_OBJECT(m_type);
}

SValue& SValue::join(const SValue& from, uint32_t flags)
{
    // If 'from' is wild or an error, propagate it through.
    if (from.is_final()) {
        *this = from;

    // If we currently have a value, and that value is not simple or is not
    // the same as 'from', then we need to build a mapping.
    } else if (is_specified() && (is_map() || this->compare(from) != 0)) {
        size_t i=0;
        SValue key, value;
        SValue *cur;
        BValueMap **map = NULL;

        while (from.getNextItem(reinterpret_cast<void**>(&i), &key, &value) >= OK) {

            if (!key.is_wild() && (cur=beginEditItem(key)) != NULL) {
                // Combine with an existing key.
                if (!(flags&B_NO_VALUE_RECURSION))
                    cur->join(value, flags);
                else
                    cur->assign(value);
                endEditItem(cur);
                map = NULL;
            } else if (map != NULL || (map=edit_map()) != NULL) {
                // Add a new key.
                const ssize_t result = BValueMap::addNewMap(map, key, value);
                if (result < OK && result != BAD_VALUE && result != ALREADY_EXISTS) {
                    set_error(result);
                }
            }

        }

    // If we currently aren't defined, then we're now just 'from'.
    } else if (!is_defined()) {
        *this = from;

    }

    CHECK_INTEGRITY(*this);
    return *this;
}

SValue & SValue::joinItem(const SValue& key, const SValue& value, uint32_t flags)
{
    SValue *cur;
    BValueMap **map;

    if (!is_final() && key.is_defined() && value.is_defined()) {
        // Most of these checks are to ensure that the value stays
        // normalized, by not creating a BValueMap unless really needed.
        const bool wildkey = key.is_wild();
        if (wildkey && (!is_defined() || value.is_final())) {
            // Handle (wild -> A) special cases.
            assign(value);
        } else if (wildkey && value.is_map()) {
            // Handle (wild -> (B -> ...)) special case.
            join(value, flags);
        } else if (value.is_null()) {
            // Handle (* -> null) special case.
            join(key, flags);
        } else if (key.is_map() || key.is_error() || value.is_error()) {
            // Need to normalize keys or propagate error.
            join(SValue(key, value, flags), flags);
        } else if (!wildkey && (cur=beginEditItem(key)) != NULL) {
            // Combine with an existing key.
            if (!(flags&B_NO_VALUE_RECURSION))
                cur->join(value, flags);
            else
                cur->assign(value);
            endEditItem(cur);
        } else if ((map=edit_map()) != NULL) {
            // Add a new key.
            const ssize_t result = BValueMap::addNewMap(map, key, value);
            if (result < OK && result != BAD_VALUE && result != ALREADY_EXISTS) {
                set_error(result);
            }
        }
    }

    CHECK_INTEGRITY(*this);
    return *this;
}

const SValue SValue::joinCopy(const SValue& from, uint32_t flags) const
{
    // XXX this could be optimized.
    SValue out(*this);
    out.join(from, flags);
    return out;
}

SValue& SValue::overlay(const SValue& from, uint32_t flags)
{
    // If 'from' is wild or an error, propagate it through.
    if (from.is_final()) {
        *this = from;

    // If we currently have a value, and that value is not simple or the
    // from value is not simple, then we need to build a mapping.
    } else if (is_specified() && (is_map() || from.is_map())) {
        size_t i=0;
        SValue key, value;
        SValue *cur;
        BValueMap **map = NULL;
        bool setCleared = false;

        while (from.getNextItem(reinterpret_cast<void**>(&i), &key, &value) >= OK) {

            if (!key.is_wild() && (cur=beginEditItem(key)) != NULL) {
                // Combine with an existing key.
                if (!(flags&B_NO_VALUE_RECURSION))
                    cur->overlay(value, flags);
                else
                    cur->assign(value);
                endEditItem(cur);
                map = NULL;
            } else {
                // Add a new key.
                if (!setCleared && key.is_wild()) {
                    // A single item replaces all other set items.
                    setCleared = true;
                    map = make_map_without_sets();
                }
                if (map != NULL || (map=edit_map()) != NULL) {
                    const ssize_t result = BValueMap::addNewMap(map, key, value);
                    if (result < OK && result != BAD_VALUE && result != ALREADY_EXISTS) {
                        set_error(result);
                    }
                }
            }

        }

        if (setCleared) shrink_map(*map);

    // If we aren't an error, overwrite with new value.
    } else if (!is_error() && from.is_defined()) {
        *this = from;

    }

    CHECK_INTEGRITY(*this);
    return *this;
}

const SValue SValue::overlayCopy(const SValue& from, uint32_t flags) const
{
    // XXX this could be optimized.
    SValue out(*this);
    out.overlay(from, flags);
    return out;
}

SValue& SValue::inherit(const SValue& from, uint32_t flags)
{
    INIT_CORRECTNESS(SValue correct(from.OverlayCopy(*this)); SValue orig(*this));

    // If 'from' is an error, propagate it through.
    if (from.is_error()) {
        *this = from;

    // If we currently have a value, then we need to see if there are any
    // new values to add.
    } else if (is_specified()) {
        size_t i1=0, i2=0;
        SValue k1, v1, k2, v2;
        BValueMap** map = NULL;
        bool first = true, finish = false;
        bool terminals = false;
        bool termcheck = false;
        int32_t cmp;
        while (from.getNextItem(reinterpret_cast<void**>(&i2), &k2, &v2) >= OK) {
            cmp = -1;
            while (!finish && (first || (cmp=compare_map(&k1,&v1,&k2,&v2)) < 0)) {
                if (getNextItem(reinterpret_cast<void**>(&i1), &k1, &v1) < OK)
                    finish = termcheck = true;
                else if (!terminals && !termcheck && k1.isWild())
                    terminals = true;
                first = false;
            }

            if (cmp != 0) {
                if (k2.is_wild()) {
                    // Don't add in new terminal(s) if the current value
                    // already has a terminal.
                    if (!terminals && !termcheck) {
                        size_t it = i1;
                        SValue kt, vt;
                        while (getNextItem(reinterpret_cast<void**>(&it), &kt, &vt) >= OK
                                && !terminals) {
                            if (kt.is_wild()) {
                                terminals = true;
                            }
                        }
                        termcheck = true;
                    }
                    if (!terminals) joinItem(k2, v2, flags|B_NO_VALUE_FLATTEN);
                } else {
                    joinItem(k2, v2, flags|B_NO_VALUE_FLATTEN);
                }
                map = NULL;
            } else if (cmp == 0 && !(flags&B_NO_VALUE_RECURSION)) {
                if (!k2.is_wild()) {
                    if (map != NULL || (map=edit_map()) != NULL) {
                        SValue* cur = BValueMap::beginEditMapAt(map, i1-1);
                        if (cur) {
                            cur->inherit(v2, flags);
                            BValueMap::endEditMapAt(map);
                        } else {
                            set_error(NO_MEMORY);
                        }
                    }
                }
            }
        }
    } else if (!is_defined()) {
        *this = from;
    }

    CHECK_INTEGRITY(*this);
    CHECK_CORRECTNESS(*this, correct, orig, "Inherit", from);

    return *this;
}

const SValue SValue::inheritCopy(const SValue& from, uint32_t flags) const
{
    // XXX this could be optimized.
    SValue out(*this);
    out.inherit(from, flags);
    return out;
}

SValue& SValue::mapValues(const SValue& from, uint32_t flags)
{
    // XXX this could be optimized.
    assign(mapValuesCopy(from, flags));
    return *this;
}

const SValue SValue::mapValuesCopy(const SValue& from, uint32_t flags) const
{
    SValue out;

    #if DB_CORRECTNESS_CHECKS
        // This is the formal definition of MapValues().
        SValue correct;
        void* i=NULL;
        SValue key, value;
        while (getNextItem(&i, &key, &value) >= OK) {
            correct.joinItem(key, from.valueFor(value, flags));
        }
    #endif

    if (!is_error()) {
        if (!from.is_final()) {
            void* i=NULL;
            SValue key1, value1, value2;
            while (getNextItem(&i, &key1, &value1) >= OK) {
                value2 = from.valueFor(value1, flags);
                if (value2.is_defined()) out.joinItem(key1, value2);
            }
        } else if (from.is_wild() || is_final()) {
            out = *this;
        } else {
            out = from;
        }
    } else {
        out = *this;
    }

    CHECK_INTEGRITY(*this);
    CHECK_CORRECTNESS(out, correct, *this, "MapValues", from);

    return out;
}

//#if DB_CORRECTNESS_CHECKS
// This is the formal definition of Remove().
static SValue correct_remove(const SValue& lhs, const SValue& rhs, uint32_t flags)
{
    if (rhs.errorCheck() < OK) {
        return lhs.errorCheck() < OK ? lhs : rhs;
    } else if (lhs.isSimple()) {
        // Terminal cases.
        if (rhs.isSimple()) {
            return (!rhs.isWild() && lhs != rhs) ? lhs : SValue::Undefined();
        } else {
            return !rhs.hasItem(SValue::Wild(), lhs) ? lhs : SValue::Undefined();
        }
    } else {
        void* i=NULL;
        SValue key, value;
        SValue result;
        if (!(flags&B_NO_VALUE_RECURSION)) {
            while (lhs.getNextItem(&i, &key, &value) >= OK) {
                result.joinItem(
                    key,
                    correct_remove(value, rhs.valueFor(key), flags),
                    flags|B_NO_VALUE_FLATTEN);
            }
        } else {
            while (lhs.getNextItem(&i, &key, &value) >= OK) {
                if (!rhs.valueFor(key).isDefined()) {
                    result.joinItem(key, value, flags|B_NO_VALUE_FLATTEN);
                }
            }
        }
        return result;
    }
}
//#endif

SValue& SValue::remove(const SValue& from, uint32_t flags)
{
    // XXX this could be optimized.
    assign(removeCopy(from, flags));
    return *this;
}

const SValue SValue::removeCopy(const SValue& from, uint32_t flags) const
{
    #if DB_CORRECTNESS_CHECKS
        SValue correct(correct_remove(*this, from, flags));
    #endif

    // Some day this might be optimized.
    const SValue result(correct_remove(*this, from, flags));

    CHECK_INTEGRITY(result);
    CHECK_CORRECTNESS(result, correct, *this, "Remove", from);

    return result;
}

status_t SValue::removeItem(const SValue& key, const SValue& value)
{
    status_t result;

    if (!is_error() && key.is_defined()) {
        if (key.is_wild() &&
                (*this == value || value.is_wild())) {
            undefine();
            result = OK;
        } else if (is_map()) {
            // Removing a simple value, or all values under a mapping?
            if (key.is_wild() || value.is_wild()) {
                BValueMap** map = edit_map();
                if (map) {
                    result = BValueMap::removeMap(map, key, value);
                    if (result >= OK) shrink_map(*map);
                } else {
                    result = NAME_NOT_FOUND;
                }
            } else {
                SValue* cur = beginEditItem(key);
                result = errorCheck();
                if (cur) {
                    cur->remove(value);
                    endEditItem(cur);
                    result = errorCheck();
                } else if (result >= OK) {
                    result = NAME_NOT_FOUND;
                }
            }
        } else {
            result = NAME_NOT_FOUND;
        }
    } else {
        result = errorCheck();
    }

    CHECK_INTEGRITY(*this);
    return result;
}

//#if DB_CORRECTNESS_CHECKS
// This is the formal definition of retain().
static SValue correct_retain(const SValue& lhs, const SValue& rhs, uint32_t flags)
{
    if (rhs.errorCheck() < OK) {
        return lhs.errorCheck() < OK ? lhs : rhs;
    } else if (lhs.isSimple()) {
        // Terminal cases.
        if (rhs.isSimple()) {
            return (rhs.isWild() || lhs == rhs)
                    ? lhs
                    : (lhs.isWild() ? rhs : SValue::Undefined());
        } else {
            return rhs.hasItem(SValue::Wild(), lhs) ? lhs : SValue::Undefined();
        }
    } else {
        void* i=NULL;
        SValue key, value;
        SValue result;
        if (!(flags&B_NO_VALUE_RECURSION)) {
            while (lhs.getNextItem(&i, &key, &value) >= OK) {
                result.joinItem(
                    key,
                    correct_retain(value, rhs.valueFor(key), flags),
                    flags|B_NO_VALUE_FLATTEN);
            }
        } else {
            while (lhs.getNextItem(&i, &key, &value) >= OK) {
                if (rhs.valueFor(key).isDefined()) {
                    result.joinItem(key, value, flags|B_NO_VALUE_FLATTEN);
                }
            }
        }
        return result;
    }
}
//#endif

SValue& SValue::retain(const SValue& from, uint32_t flags)
{
    // XXX this could be optimized.
    assign(retainCopy(from, flags));
    return *this;
}

const SValue SValue::retainCopy(const SValue& from, uint32_t flags) const
{
    #if DB_CORRECTNESS_CHECKS
        SValue correct(correct_retain(*this, from, flags));
    #endif

    // Some day this might be optimized.
    const SValue result(correct_retain(*this, from, flags));

    CHECK_INTEGRITY(result);
    CHECK_CORRECTNESS(result, correct, *this, "Retain", from);

    return result;
}

status_t SValue::renameItem(const SValue& old_key,
                            const SValue& new_key)
{
    status_t result;

    if (is_map()) {
        BValueMap** map = edit_map();
        if (map) {
            result = BValueMap::renameMap(map, old_key, new_key);
            if (result != OK && result != ALREADY_EXISTS
                    && result != NAME_NOT_FOUND && result != BAD_VALUE) {
                set_error(result);
            }
        } else if (is_error()) {
            result = errorCheck();
        } else {
            result = NAME_NOT_FOUND;
        }
    } else if (!is_error()) {
        result = NAME_NOT_FOUND;
    } else {
        result = errorCheck();
    }

    CHECK_INTEGRITY(*this);
    return result;
}

bool SValue::hasItem(const SValue& key, const SValue& value) const
{
    if (!is_error()) {
        if (is_wild()) {
            return true;
        } else if (key.is_wild()) {
            if (value.is_wild()) {
                return is_defined();
            } else if (is_map()) {
                return m_data.map->indexFor(key, value) >= 0;
            } else {
                return *this == value;
            }
        } else {
            if (is_map()) {
                ssize_t index = m_data.map->indexFor(key, value);
                if (index >= 0) return true;

                // Look in a set
                return m_data.map->indexFor(B_WILD_VALUE,key) >= 0;
            } else if (key == *this)
                return true;
        }
    }
    return false;
}

const SValue SValue::valueFor(const SValue& key, uint32_t flags) const
{
    if (!key.is_map() || (flags&B_NO_VALUE_RECURSION)) return valueFor(key);

    if (is_error()) {
        return *this;
    } else if (is_wild()) {
        return key;
    } else {
        // Index by entire hierarchy, for each map in the key.
        SValue val;
        void* i=NULL;
        SValue key1, value1, value2;
        while (key.getNextItem(&i, &key1, &value1) >= OK) {
            value2 = valueFor(key1, flags | B_NO_VALUE_RECURSION);
            value2 = value2.valueFor(value1, flags);
            if (value2.is_defined()) val.join(value2, flags);
        }
        return val;
    }
    return SValue::Undefined();
}

const SValue& SValue::valueFor(const SValue& key) const
{
    if (key.is_wild()) {
        return *this;
    } else if (is_map()) {
        ssize_t index = m_data.map->indexFor(key);
//		bout << "SValue::ValueFor: " << index << endl;
        if (index >= OK) return m_data.map->mapAt(index).value;

        // Look in a set
        index = m_data.map->indexFor(B_WILD_VALUE,key);
        if (index >= OK) return B_NULL_VALUE;
    } else if (is_wild()) {
        return key;
    } else if (key == *this) {
        return B_NULL_VALUE;
    } else if (is_error()) {
        return *this;
    }
    return SValue::Undefined();
}

const SValue& SValue::valueFor(const char* key) const
{
    if (is_map()) {
        const size_t len = strlen(key)+1;
        const uint32_t type = len <= B_TYPE_LENGTH_MAX
            ? B_PACK_SMALL_TYPE(B_STRING_TYPE, len) : B_PACK_LARGE_TYPE(B_STRING_TYPE);
        ssize_t index = m_data.map->indexFor(type, key, len);
        if (index >= OK) return m_data.map->mapAt(index).value;

        // Look in a set
        index = m_data.map->indexFor(B_WILD_VALUE,SValue(key));
        if (index >= OK) return B_NULL_VALUE;
    } else if (is_wild()) {
        LOG_FATAL("valueFor(const char*) from wild not supported!");
        return SValue::Undefined();
        //val = SValue(key);
    } else if (*this == key)
        return B_NULL_VALUE;

    return SValue::Undefined();
}

const SValue& SValue::valueFor(const SString& key) const
{
    if (is_map()) {
        const size_t len = key.length()+1;
        const uint32_t type = len <= B_TYPE_LENGTH_MAX
            ? B_PACK_SMALL_TYPE(B_STRING_TYPE, len) : B_PACK_LARGE_TYPE(B_STRING_TYPE);
        ssize_t index = m_data.map->indexFor(type, key.string(), len);
        if (index >= OK) return m_data.map->mapAt(index).value;

        // Look in a set
        index = m_data.map->indexFor(B_WILD_VALUE,SValue(key));
        if (index >= OK) return B_NULL_VALUE;
    } else if (is_wild()) {
        LOG_FATAL("valueFor(SString) from wild not supported!");
        return SValue::Undefined();
        //val = SValue(key);
    } else if (*this == key)
        return B_NULL_VALUE;
    return SValue::Undefined();
}

const SValue& SValue::valueFor(int32_t index) const
{
    // Optimize?
    return valueFor(SSimpleValue<int32_t>(index));
}

int32_t SValue::countItems() const
{
    return is_map() ? m_data.map->countMaps() : ( is_defined() ? 1 : 0 );
}

status_t SValue::getNextItem(void** cookie, SValue* out_key, SValue* out_value) const
{
    status_t result = find_item_index(*reinterpret_cast<size_t*>(cookie), out_key, out_value);
    if (result == OK) {
        *cookie = reinterpret_cast<void*>((*reinterpret_cast<size_t*>(cookie)) + 1);
    } else if (result == BAD_INDEX) {
        result = NAME_NOT_FOUND;
    }
    return result;
}

SValue SValue::keys() const
{
    SValue val;

    if (!is_error()) {
        if (is_map()) {
            for (size_t index=0; index< m_data.map->countMaps(); index++) {
                const BValueMap::pair& p = m_data.map->mapAt(index);
                if (!p.key.is_wild()) { // This entry is a set
                    val.join(p.key);
                }
            }
        } else if (is_defined()) {
            val = SValue::Undefined();
        } else {
            val = SValue::Undefined();
        }
    }

    CHECK_INTEGRITY(*this);
    return val;
}

SValue *SValue::beginEditItem(const SValue& key)
{
    SValue *val;
    if (key.is_wild()) {
        val = NULL;
    } else if (is_map()) {
        ssize_t index = m_data.map->indexFor(key);
        if (index >= OK) {
            BValueMap** map = edit_map();
            val = map ? BValueMap::beginEditMapAt(map, index) : NULL;
        } else {
            val = NULL;
        }
    } else {
        val = NULL;
    }

    return val;
}

void SValue::endEditItem(SValue* cur)
{
    if (is_map()) {
        BValueMap** map = edit_map();
        if (map) {
            status_t err = cur->errorCheck();
            BValueMap::endEditMapAt(map);
            if (err < OK) set_error(err);
            else shrink_map(*map);
        }
    } else {
        LOG_FATAL("BeginEditItem has not been called");
    }
}

status_t SValue::remove_item_index(size_t index)
{
    status_t result;

    if (!is_error()) {
        if (is_map()) {
            const size_t N = m_data.map->countMaps();
            if (index == 0 && N <= 1) {
                undefine();
                result = OK;

            } else if (index < m_data.map->countMaps()) {
                BValueMap** map = edit_map();
                if (map) {
                    BValueMap::removeMapAt(map, index);
                    shrink_map(*map);
                    result = OK;
                } else {
                    result = NO_MEMORY;
                }
            } else {
                result = BAD_INDEX;
            }
        } else if (index == 0) {
            undefine();
            result = OK;
        } else {
            result = BAD_INDEX;
        }
    } else {
        result = errorCheck();
    }

    CHECK_INTEGRITY(*this);
    return result;
}

status_t SValue::find_item_index(size_t index, SValue* out_key, SValue* out_value) const
{
    status_t result;

    if (!is_error()) {
        if (is_map()) {
            if (index < m_data.map->countMaps()) {
                const BValueMap::pair& p = m_data.map->mapAt(index);
                if (out_key) *out_key = p.key;
                if (out_value) *out_value = p.value;
                result = OK;
            } else {
                result = BAD_INDEX;
            }
        } else if (index == 0 && is_defined()) {
            if (out_key) *out_key = SValue::Wild();
            if (out_value) *out_value = *this;
            result = OK;
        } else {
            result = BAD_INDEX;
        }
    } else {
        result = errorCheck();
    }

    CHECK_INTEGRITY(*this);
    return result;
}

// ------------------------------------------------------------------------

#if VALIDATES_VALUE
#define FINISH_ARCHIVE(func)																\
    ssize_t total = func;																	\
    if (total != archivedSize()) {															\
        bout << "Written size: " << total << ", expected: " << archivedSize() << endl;		\
        bout << "Myself: " << *this << endl;												\
        bout << "The data so far: " << into << endl;										\
        LOG_ALWAYS_FATAL("Archived size wrong!");													\
    }																						\
    return total;
#else
#define FINISH_ARCHIVE(func) return func
#endif

ssize_t SValue::archivedSize() const
{
    const uint32_t len = B_UNPACK_TYPE_LENGTH(m_type);

    // Small data value.
    if (len <= B_TYPE_LENGTH_MAX) {
        if (!is_error())
            return is_object() ? sizeof (flat_binder_object) : sizeof(small_flat_data);
        return errorCheck();
    }

    // Large data value.
    if (len == B_TYPE_LENGTH_LARGE)
        return sizeof(large_flat_header) + value_data_align(m_data.buffer->size());

    // Else -- it's a map.
    LOG_FATAL_IF(len != B_TYPE_LENGTH_MAP, "Archiving with unexpected length!");
    return sizeof(value_map_header) + m_data.map->archivedSize();
}

ssize_t SValue::archive(Parcel& into) const
{
    const uint32_t len = B_UNPACK_TYPE_LENGTH(m_type);

    // Small data value.
    if (len <= B_TYPE_LENGTH_MAX) {
        if (!is_error()) {
            if (!is_object()) {
                // This is a small piece of data, and SValue happens to
                // store that in its flattened form.  How convenient!
                FINISH_ARCHIVE(into.WriteSmalldata(*reinterpret_cast<const small_flat_data*>(this)));
            }

            // Objects require special handling because reference counts
            // must be maintained.  However, we store them internally
            // in the standard flattened structure, so we just need to
            // tell Parcel to keep track of the data we write.
#if BUILD_TYPE == BUILD_TYPE_DEBUG
            if (m_type == B_PACK_SMALL_TYPE(B_ATOM_TYPE, sizeof(void*))
                    || m_type == B_PACK_SMALL_TYPE(B_ATOM_WEAK_TYPE, sizeof(void*))) {
                LOG_ALWAYS_FATAL("Can't flatten SValue containing RefBase pointers (maybe you should have added binder objects instead).");
                return BAD_VALUE;
            }
#endif
            // make a small_flat_data from the SValue: type, this, atom
            FINISH_ARCHIVE(into.WriteBinder(*reinterpret_cast<const small_flat_data*>(this)));
        }
        return errorCheck();
    }

    // Large data value.
    if (len == B_TYPE_LENGTH_LARGE) {
        FINISH_ARCHIVE(into.WriteLargedata(m_type, m_data.buffer->size(), m_data.buffer->data()));
    }

    // Else -- it's a map.

    LOG_FATAL_IF(len != B_TYPE_LENGTH_MAP, "Archiving with unexpected length!");

    value_map_header header;
    header.header.type = kMapTypeCode;
    header.header.length = sizeof(value_map_info) + value_data_align(m_data.map->archivedSize());
    header.info.count = m_data.map->countMaps();
    header.info.order = 1;

    ssize_t err;
    if ((err=into.write(&header, sizeof(header))) < 0) return err;

    err = m_data.map->archive(into);
    if (err >= OK) {
        FINISH_ARCHIVE((sizeof(header)+err));
    }

    return err;
}

#undef FINISH_ARCHIVE

ssize_t SValue::unarchive(Parcel& from)
{
    if (is_defined()) undefine();
    return unarchive_internal(from, 0x7fffffff);
}

ssize_t SValue::unarchive_internal(Parcel& from, size_t avail)
{
    ssize_t err = OK;

    if (avail < sizeof(small_flat_data)) goto error;

    STUB;
#if 0
    small_flat_data head;
    if ((err=from.ReadSmallDataOrObject(&head, this)) >= sizeof(head)) {

        // Optimization -- avoid aliasing of 'head'
        const uint32_t htype = head.type;
        const uint32_t hdata = head.data.uinteger;

        const uint32_t len = B_UNPACK_TYPE_LENGTH(htype);
        if (len <= B_TYPE_LENGTH_MAX) {
            // Do the easy case first -- a small block of data.
            m_type = htype;
            m_data.uinteger = hdata;
            CHECK_INTEGRITY(*this);
            return err;
        }

        if (len == B_TYPE_LENGTH_LARGE) {
            //  Next case -- a large block of data.
            const size_t readSize = value_data_align(hdata);
            void* buf = alloc_data(htype, hdata);
            if (buf) {
                if ((err=from.Read(buf, readSize)) == readSize) {
                    CHECK_INTEGRITY(*this);
                    return sizeof(head) + readSize;
                }
                goto error;
            }
            // retrieve error from alloc_data().
            return errorCheck();
        }

        // Else -- it's a map.

        LOG_FATAL_IF(len != B_TYPE_LENGTH_MAP, "Unarchiving with unexpected length!");

        // Need to read the map info struct.
        value_map_info info;
        LOG_FATAL_IF(sizeof(value_map_info) != sizeof(small_flat_data), "Ooops!");
        if ((err=from.ReadSmalldata((small_flat_data*)&info)) == sizeof(info)) {
            ssize_t retErr;	// don't use err so ADS doesn't alias it.
            BValueMap* obj = BValueMap::Create(from, avail-sizeof(value_map_header), info.count, &retErr);
            err = retErr;
            if (err >= 0) {
                // Plug in this value.
                m_type = kMapTypeCode;
                m_data.map = obj;
                CHECK_INTEGRITY(*this);
                return sizeof(value_map_header) + err;
            }
        }
    }
#endif

error:
    m_type = kUndefinedTypeCode;
    return set_error(err < OK ? err : BAD_VALUE);
}

// ------------------------------------------------------------------------

// This is a comparison between values with two nice properties:
// * It is significantly faster than the standard SValue comparison,
//   as it doesn't try to put things in a "nice" order.  In practice,
//   the speedup is about 2x.
// * It has a clearly defined order, where-as the normal ordering of
//   BValues will change whenever we add special cases for other types.
//   This allows us to proclaim the following algorithm as the offical
//   order for items in flattened data.
int32_t SValue::compare(const SValue& o) const
{
    if (m_type != o.m_type)
        return m_type < o.m_type ? -1 : 1;
    // Special case compare to put integers in a nice order.
    size_t len = B_UNPACK_TYPE_LENGTH(m_type);
    switch (len) {
        case 0:
            return 0;
        case 1:
        case 2:
        case 3:
            return memcmp(m_data.local, o.m_data.local, len);
        case 4:
            return (m_data.integer != o.m_data.integer)
                ? ( (m_data.integer < o.m_data.integer) ? -1 : 1 )
                : 0;
        case B_TYPE_LENGTH_LARGE:
            len = m_data.buffer->size();
            if (len != o.m_data.buffer->size())
                return len < o.m_data.buffer->size() ? -1 : 1;
            return memcmp(m_data.buffer->data(), o.m_data.buffer->data(), len);
        case B_TYPE_LENGTH_MAP:
            return m_data.map->compare(*o.m_data.map);
    }
    return 0;
}

int32_t SValue::compare(const char* str) const
{
    return compare(B_STRING_TYPE, str, strlen(str)+1);
}

int32_t SValue::compare(const SString& str) const
{
    return compare(B_STRING_TYPE, str.String(), str.length()+1);
}

int32_t SValue::compare(type_code type, const void* data, size_t length) const
{
    return compare(length <= B_TYPE_LENGTH_MAX
        ? B_PACK_SMALL_TYPE(type, length) : B_PACK_LARGE_TYPE(type),
        data, length);
}

//// This version assumes 'type' has already been packed.
//int32_t SValue::compare(uint32_t type, const void* data, size_t length) const
//{
//    if (m_type != type)
//        return m_type < type ? -1 : 1;
//    // Special case compare to put integers in a nice order.
//    size_t len = B_UNPACK_TYPE_LENGTH(m_type);
//    switch (len) {
//        case 0:
//            return 0;
//        case 1:
//        case 2:
//        case 3:
//            return memcmp(m_data.local, data, len);
//        case 4:
//            return (m_data.integer != *(int32_t*)data)
//                ? ( (m_data.integer < *(int32_t*)data) ? -1 : 1 )
//                : 0;
//        case B_TYPE_LENGTH_LARGE:
//            len = m_data.buffer->size();
//            if (len != length)
//                return len < length ? -1 : 1;
//            return memcmp(m_data.buffer->data(), data, len);
//    }
//    return 0;
//}

#define TYPED_DATA_COMPARE(d1, d2, type)												\
    ( (*reinterpret_cast<const type*>(d1)) < (*reinterpret_cast<const type*>(d2))		\
        ? -1																			\
        : (	(*reinterpret_cast<const type*>(d1)) > (*reinterpret_cast<const type*>(d2))	\
            ? 1 : 0 )																	\
    )

int32_t SValue::lexicalCompare(const SValue& o) const
{
    if (is_simple() && o.is_simple()) {
        if (B_UNPACK_TYPE_CODE(m_type) != B_UNPACK_TYPE_CODE(o.m_type))
            return m_type < o.m_type ? -1 : 1;
        if (B_UNPACK_TYPE_LENGTH(m_type) == B_UNPACK_TYPE_LENGTH(o.m_type)) {
            switch (m_type) {
                case kUndefinedTypeCode:
                case kWildTypeCode:
                    return 0;
                case B_PACK_SMALL_TYPE(B_BOOL_TYPE, sizeof(int8_t)):
                case B_PACK_SMALL_TYPE(B_INT8_TYPE, sizeof(int8_t)):
                    return TYPED_DATA_COMPARE(m_data.local, o.m_data.local, int8_t);
                case B_PACK_SMALL_TYPE(B_UINT8_TYPE, sizeof(uint8_t)):
                    return TYPED_DATA_COMPARE(m_data.local, o.m_data.local, uint8_t);
                case B_PACK_SMALL_TYPE(B_INT16_TYPE, sizeof(int16_t)):
                    return TYPED_DATA_COMPARE(m_data.local, o.m_data.local, int16_t);
                case B_PACK_SMALL_TYPE(B_UINT16_TYPE, sizeof(uint16_t)):
                    return TYPED_DATA_COMPARE(m_data.local, o.m_data.local, uint16_t);
                case B_PACK_SMALL_TYPE(B_INT32_TYPE, sizeof(int32_t)):
                    return TYPED_DATA_COMPARE(m_data.local, o.m_data.local, int32_t);
                case B_PACK_SMALL_TYPE(B_UINT32_TYPE, sizeof(uint32_t)):
                    return TYPED_DATA_COMPARE(m_data.local, o.m_data.local, uint32_t);
                case B_PACK_SMALL_TYPE(B_FLOAT_TYPE, sizeof(float)):
                    return TYPED_DATA_COMPARE(m_data.local, o.m_data.local, float);
                case B_INT64_TYPE:
                    if (m_data.buffer->size() == sizeof(int64_t)
                        && o.m_data.buffer->size() == sizeof(int64_t)) {
                        return TYPED_DATA_COMPARE(m_data.buffer->data(), o.m_data.buffer->data(), int64_t);
                    }
                    break;
                case B_UINT64_TYPE:
                    if (m_data.buffer->size() == sizeof(uint64_t)
                        && o.m_data.buffer->size() == sizeof(uint64_t)) {
                        return TYPED_DATA_COMPARE(m_data.buffer->data(), o.m_data.buffer->data(), uint64_t);
                    }
                    break;
                case B_DOUBLE_TYPE:
                    if (m_data.buffer->size() == sizeof(double)
                        && o.m_data.buffer->size() == sizeof(double)) {
                        return TYPED_DATA_COMPARE(m_data.buffer->data(), o.m_data.buffer->data(), double);
                    }
                    break;
            }
        }

        const size_t len = length();
        const size_t olen = o.length();

        int32_t result = memcmp(data(), o.data(), len < olen ? len : olen);
        if (result == 0 && len != olen) {
            return len < olen ? -1 : 1;
        }
        return 0;
    }
    if (m_type != o.m_type)
        return m_type < o.m_type ? -1 : 1;
    return m_data.map->lexicalCompare(*o.m_data.map);
}

#undef TYPED_DATA_COMPARE

int32_t SValue::compare_map(const SValue* key1, const SValue* value1,
                            const SValue* key2, const SValue* value2)
{
    const SValue* cmp1;
    const SValue* cmp2;

    // Fix ordering of wild keys -- we want to order the value along
    // with similar non-wild keys.
    const bool key1Wild = key1->is_wild();
    const bool key2Wild = key2->is_wild();
    cmp1 = key1Wild ? value1 : key1;
    cmp2 = key2Wild ? value2 : key2;

    // First, quick check if types are different.  However,
    // DON'T do this case if key1 is wild, meaning both keys
    // are wild and we need to use the "simple" case below.
    if (cmp1->m_type != cmp2->m_type && !key1Wild) {
        return cmp1->m_type < cmp2->m_type ? -1 : 1;

    // Types aren't different, so they are both either a map
    // or simple.  First the simple case.
    } else if (cmp1->is_simple()) {
        const int32_t c = cmp1->compare(*cmp2);
        // If they look the same, take into account whether one or
        // the other has a wild key.
        if (c == 0 && key1Wild != key2Wild) {
            return key1Wild ? -1 : 1;
        }
        return c;
    }

    // Types aren't different and both are maps.  Don't need to
    // worry about the wild key case here, because the construct
    // (wild -> (a -> b)) is not a normalized form.
    return key1->compare(*key2);
}

// ------------------------------------------------------------------------

status_t SValue::getBinder(sp<IBinder> *obj) const
{
    if (is_object()) {
        STUB;//return unflatten_binder(*(small_flat_data*)this, obj);
    }
    return BAD_TYPE;
}

status_t SValue::getWeakBinder(wp<IBinder> *obj) const
{
    if (is_object()) {
        STUB;//return unflatten_binder(*(small_flat_data*)this, obj);
    }
    return BAD_TYPE;
}

status_t SValue::getString(const char** a_string) const
{
    if (B_UNPACK_TYPE_CODE(m_type) != B_STRING_TYPE) return BAD_TYPE;
    *a_string = static_cast<const char*>(data());
    return OK;
}

status_t SValue::getString(SString* a_string) const
{
    if (B_UNPACK_TYPE_CODE(m_type) != B_STRING_TYPE) return BAD_TYPE;

    switch (B_UNPACK_TYPE_LENGTH(m_type)) {
        case B_TYPE_LENGTH_LARGE:
            *a_string = SString(m_data.buffer);
            return OK;
        case 0:
            *a_string = "";
            return OK;
    }

    const SharedBuffer *buf = SharedBuffer();
    if (!buf)
        return NO_MEMORY;

    SString out(buf);
    *a_string = out;
    buf->release();

    return OK;
}

status_t SValue::getBool(bool* val) const
{
    int8_t v;
    status_t err = copy_small_data(B_PACK_SMALL_TYPE(B_BOOL_TYPE, sizeof(int8_t)), &v, sizeof(int8_t));
    *val = v ? true : false;
    return err;
}

status_t SValue::getInt8(int8_t* val) const
{
    return copy_small_data(B_PACK_SMALL_TYPE(B_INT8_TYPE, sizeof(*val)), val, sizeof(*val));
}

status_t SValue::getInt16(int16_t* val) const
{
    return copy_small_data(B_PACK_SMALL_TYPE(B_INT16_TYPE, sizeof(*val)), val, sizeof(*val));
}

status_t SValue::getInt32(int32_t* val) const
{
    return copy_small_data(B_PACK_SMALL_TYPE(B_INT32_TYPE, sizeof(*val)), val, sizeof(*val));
}

status_t SValue::getStatus(status_t* val) const
{
    return copy_small_data(B_PACK_SMALL_TYPE(B_STATUS_TYPE, sizeof(*val)), val, sizeof(*val));
}

status_t SValue::getInt64(int64_t* val) const
{
    return copy_big_data(B_PACK_LARGE_TYPE(B_INT64_TYPE), val, sizeof(*val));
}

status_t SValue::getTime(nsecs_t* val) const
{
    return copy_big_data(B_PACK_LARGE_TYPE(B_NSECS_TYPE), val, sizeof(*val));
}

status_t SValue::getFloat(float* val) const
{
    return copy_small_data(B_PACK_SMALL_TYPE(B_FLOAT_TYPE, sizeof(*val)), val, sizeof(*val));
}

status_t SValue::getDouble(double* val) const
{
    return copy_big_data(B_PACK_LARGE_TYPE(B_DOUBLE_TYPE), val, sizeof(*val));
}

status_t SValue::getRefBase(sp<RefBase>* atom) const
{
    RefBase* a;
    status_t err = copy_small_data(B_PACK_SMALL_TYPE(B_ATOM_TYPE, sizeof(a)), &a, sizeof(a));
    if (err >= OK) *atom = a;
    return err;
}

status_t SValue::getWeakRefBase(wp<RefBase>* atom) const
{
    RefBase::weak_atom_ptr* wp;
    status_t err = copy_small_data(B_PACK_SMALL_TYPE(B_ATOM_WEAK_TYPE, sizeof(wp)), &wp, sizeof(wp));
    if (err < OK) err = copy_small_data(B_PACK_SMALL_TYPE(B_ATOM_WEAK_TYPE, sizeof(wp)), &wp, sizeof(wp));
    if (err >= OK) *atom = wp->atom;
    return err;
}

status_t
SValue::type_conversion_error() const
{
    status_t err = statusCheck();
    return err < OK ? err : BAD_TYPE;
}

status_t SValue::asStatus(status_t* result) const
{
    status_t val = asSSize(result);
    if (val <= OK) return val;

    if (result) *result = BAD_TYPE;
    return OK;
}

ssize_t SValue::asSSize(status_t* result) const
{
    status_t val = errorCheck();
    status_t r = OK;

    if (val == OK) {
        if (m_type == B_PACK_SMALL_TYPE(B_STATUS_TYPE, sizeof(status_t))) {
            val = m_data.integer;
        } else {
            val = asInt(&r);
        }
    }

    if (result) *result = r;
    return val;
}

sp<IBinder>
SValue::asBinder(status_t *result) const
{
    if (is_object()) {
        STUB;
#if 0
        sp<IBinder> val;
        status_t err = unflatten_binder(*(small_flat_data*)this, &val);
        if (err != OK) {
            wp<IBinder> wval;
            err = unflatten_binder(*(small_flat_data*)this, &wval);
            if (err == OK) {
                val = wval.promote();
            } else if (m_type == B_PACK_SMALL_TYPE(B_NULL_TYPE, 0)) {
                err = OK;
            }
        }
        if (result) *result = err;
        return val;
#endif
    }

    if (result) *result = type_conversion_error();
    return sp<IBinder>();
}

wp<IBinder>
SValue::asWeakBinder(status_t *result) const
{
    if (is_object()) {
        STUB;
#if 0
        wp<IBinder> val;
        status_t err = unflatten_binder(*(small_flat_data*)this, &val);
        if (err != OK && m_type == B_PACK_SMALL_TYPE(B_NULL_TYPE, 0)) {
            err = OK;
        }
        if (result) *result = err;
        return val;
#endif
    }

    if (result) *result = type_conversion_error();
    return wp<IBinder>();
}

#if 0
// Some code generation tests...
static int32_t testa_int32(volatile int32_t& v, int32_t* out)
{
    if ((v&0x80000000) == 0) {
        *out = v&0x7fffffff;
        return 1;
    }

    if ((v&0x80000000) == 0x80000000) {
        *out = v&0x80000000;
        return 2;
    }

    if ((v&0x00000001) == 0x00000000) {
        *out = v>>1;
        return 3;
    }

    if ((v&0x00000001) == 0x00000001) {
        *out = v&0x00000001;
        return 4;
    }

    if ((v&0x40000000) == 0) {
        *out = v&~0x40000000;
        return 21;
    }

    if ((v&0x40000000) == 0x40000000) {
        *out = v&0x40000000;
        return 22;
    }

    if ((v&0x00000002) == 0x00000000) {
        *out = v&~0x00000002;
        return 23;
    }

    if ((v&0x00000002) == 0x00000002) {
        *out = v&0x00000002;
        return 24;
    }

    if ((v&0x80000001) == 0x00000000) {
        *out = v&0x7ffffffe;
        return 5;
    }

    if ((v&0x80000001) == 0x80000001) {
        *out = v&0x80000001;
        return 6;
    }

    if ((v&0x80000001) == 0x00000001) {
        *out = v&0xfffffffe;
        return 7;
    }

    if ((v&0x80000001) == 0x80000001) {
        *out = v&0xfffffffe;
        return 8;
    }

    if ((v&0xc0000000) == 0x00000000) {
        *out = v&~0xc0000000;
        return 9;
    }

    if ((v&0xc0000000) == 0x40000000) {
        *out = v&0xc0000000;
        return 10;
    }

    if ((v&0xc0000000) == 0x80000000) {
        *out = v&~0xc0000000;
        return 11;
    }

    if ((v&0x00000003) == 0x00000000) {
        *out = v&~0x00000003;
        return 12;
    }

    if ((v&0x00000003) == 0x00000001) {
        *out = v&0x00000003;
        return 13;
    }

    if ((v&0x00000003) == 0x00000002) {
        *out = v&0x00000003;
        return 14;
    }

    if ((v&0x000f0000) == 0x00000000) {
        *out = v&0x000f0000;
        return 15;
    }

    if ((v&0x000f0000) == 0x000f0000)  {
        *out = v&~0x000f0000;
        return 16;
    }

    if ((v&0x0000f000) == 0x00000000) {
        *out = v&0x0000f000;
        return 17;
    }

    if ((v&0x0000f000) == 0x0000f000) {
        *out = v&~0x0000f000;
        return 18;
    }

    return 0;
}

static int32_t testb_int32(volatile int32_t& v, int32_t* out)
{
    int32_t result = 0;

    if ((v&0x80000000) == 0) {
        *out = v&0x7fffffff;
        result = 1;
    }

    else if ((v&0x80000000) == 0x80000000) {
        *out = v&0x80000000;
        result = 2;
    }

    else if ((v&0x00000001) == 0x00000000) {
        *out = v>>1;
        result = 3;
    }

    else if ((v&0x00000001) == 0x00000001) {
        *out = v&0x00000001;
        result = 4;
    }

    else if ((v&0x40000000) == 0) {
        *out = v&~0x40000000;
        result = 21;
    }

    else if ((v&0x40000000) == 0x40000000) {
        *out = v&0x40000000;
        result = 22;
    }

    else if ((v&0x00000002) == 0x00000000) {
        *out = v&~0x00000002;
        result = 23;
    }

    else if ((v&0x00000002) == 0x00000002) {
        *out = v&0x00000002;
        result = 24;
    }

    else if ((v&0x80000001) == 0x00000000) {
        *out = v&0x7ffffffe;
        result = 5;
    }

    else if ((v&0x80000001) == 0x80000001) {
        *out = v&0x80000001;
        result = 6;
    }

    else if ((v&0x80000001) == 0x00000001) {
        *out = v&0xfffffffe;
        result = 7;
    }

    else if ((v&0x80000001) == 0x80000001) {
        *out = v&0xfffffffe;
        result = 8;
    }

    else if ((v&0xc0000000) == 0x00000000) {
        *out = v&~0xc0000000;
        result = 9;
    }

    else if ((v&0xc0000000) == 0x40000000) {
        *out = v&0xc0000000;
        result = 10;
    }

    else if ((v&0xc0000000) == 0x80000000) {
        *out = v&~0xc0000000;
        result = 11;
    }

    else if ((v&0x00000003) == 0x00000000) {
        *out = v&~0x00000003;
        result = 12;
    }

    else if ((v&0x00000003) == 0x00000001) {
        *out = v&0x00000003;
        result = 13;
    }

    else if ((v&0x00000003) == 0x00000002) {
        *out = v&0x00000003;
        result = 14;
    }

    else if ((v&0x000f0000) == 0x00000000) {
        *out = v&0x000f0000;
        result = 15;
    }

    else if ((v&0x000f0000) == 0x000f0000)  {
        *out = v&~0x000f0000;
        result = 16;
    }

    else if ((v&0x0000f000) == 0x00000000) {
        *out = v&0x0000f000;
        result = 17;
    }

    else if ((v&0x0000f000) == 0x0000f000) {
        *out = v&~0x0000f000;
        result = 18;
    }

    return result;
}

static int32_t testa_uint32(volatile uint32_t& v, int32_t* out)
{
    if ((v&0x80000000) == 0) {
        *out = v&0x7fffffff;
        return 1;
    }

    if ((v&0x80000000) == 0x80000000) {
        *out = v&0x80000000;
        return 2;
    }

    if ((v&0x00000001) == 0x00000000) {
        *out = v>>1;
        return 3;
    }

    if ((v&0x00000001) == 0x00000001) {
        *out = v&0x00000001;
        return 4;
    }

    if ((v&0x40000000) == 0) {
        *out = v&~0x40000000;
        return 21;
    }

    if ((v&0x40000000) == 0x40000000) {
        *out = v&0x40000000;
        return 22;
    }

    if ((v&0x00000002) == 0x00000000) {
        *out = v&~0x00000002;
        return 23;
    }

    if ((v&0x00000002) == 0x00000002) {
        *out = v&0x00000002;
        return 24;
    }

    if ((v&0x80000001) == 0x00000000) {
        *out = v&0x7ffffffe;
        return 5;
    }

    if ((v&0x80000001) == 0x80000001) {
        *out = v&0x80000001;
        return 6;
    }

    if ((v&0x80000001) == 0x00000001) {
        *out = v&0xfffffffe;
        return 7;
    }

    if ((v&0x80000001) == 0x80000001) {
        *out = v&0xfffffffe;
        return 8;
    }

    if ((v&0xc0000000) == 0x00000000) {
        *out = v&~0xc0000000;
        return 9;
    }

    if ((v&0xc0000000) == 0x40000000) {
        *out = v&0xc0000000;
        return 10;
    }

    if ((v&0xc0000000) == 0x80000000) {
        *out = v&~0xc0000000;
        return 11;
    }

    if ((v&0x00000003) == 0x00000000) {
        *out = v&~0x00000003;
        return 12;
    }

    if ((v&0x00000003) == 0x00000001) {
        *out = v&0x00000003;
        return 13;
    }

    if ((v&0x00000003) == 0x00000002) {
        *out = v&0x00000003;
        return 14;
    }

    if ((v&0x000f0000) == 0x00000000) {
        *out = v&0x000f0000;
        return 15;
    }

    if ((v&0x000f0000) == 0x000f0000)  {
        *out = v&~0x000f0000;
        return 16;
    }

    if ((v&0x0000f000) == 0x00000000) {
        *out = v&0x0000f000;
        return 17;
    }

    if ((v&0x0000f000) == 0x0000f000) {
        *out = v&~0x0000f000;
        return 18;
    }

    return 0;
}

static int32_t testb_uint32(volatile uint32_t& v, int32_t* out)
{
    int32_t result = 0;

    if ((v&0x80000000) == 0) {
        *out = v&0x7fffffff;
        result = 1;
    }

    else if ((v&0x80000000) == 0x80000000) {
        *out = v&0x80000000;
        result = 2;
    }

    else if ((v&0x00000001) == 0x00000000) {
        *out = v>>1;
        result = 3;
    }

    else if ((v&0x00000001) == 0x00000001) {
        *out = v&0x00000001;
        result = 4;
    }

    else if ((v&0x40000000) == 0) {
        *out = v&~0x40000000;
        result = 21;
    }

    else if ((v&0x40000000) == 0x40000000) {
        *out = v&0x40000000;
        result = 22;
    }

    else if ((v&0x00000002) == 0x00000000) {
        *out = v&~0x00000002;
        result = 23;
    }

    else if ((v&0x00000002) == 0x00000002) {
        *out = v&0x00000002;
        result = 24;
    }

    else if ((v&0x80000001) == 0x00000000) {
        *out = v&0x7ffffffe;
        result = 5;
    }

    else if ((v&0x80000001) == 0x80000001) {
        *out = v&0x80000001;
        result = 6;
    }

    else if ((v&0x80000001) == 0x00000001) {
        *out = v&0xfffffffe;
        result = 7;
    }

    else if ((v&0x80000001) == 0x80000001) {
        *out = v&0xfffffffe;
        result = 8;
    }

    else if ((v&0xc0000000) == 0x00000000) {
        *out = v&~0xc0000000;
        result = 9;
    }

    else if ((v&0xc0000000) == 0x40000000) {
        *out = v&0xc0000000;
        result = 10;
    }

    else if ((v&0xc0000000) == 0x80000000) {
        *out = v&~0xc0000000;
        result = 11;
    }

    else if ((v&0x00000003) == 0x00000000) {
        *out = v&~0x00000003;
        result = 12;
    }

    else if ((v&0x00000003) == 0x00000001) {
        *out = v&0x00000003;
        result = 13;
    }

    else if ((v&0x00000003) == 0x00000002) {
        *out = v&0x00000003;
        result = 14;
    }

    else if ((v&0x000f0000) == 0x00000000) {
        *out = v&0x000f0000;
        result = 15;
    }

    else if ((v&0x000f0000) == 0x000f0000)  {
        *out = v&~0x000f0000;
        result = 16;
    }

    else if ((v&0x0000f000) == 0x00000000) {
        *out = v&0x0000f000;
        result = 17;
    }

    else if ((v&0x0000f000) == 0x0000f000) {
        *out = v&~0x0000f000;
        result = 18;
    }

    return result;
}
#endif

SString SValue::asString(status_t* result) const
{
    // First check the fast and easy cases.
    switch (m_type) {
        case B_PACK_LARGE_TYPE(B_STRING_TYPE):
            if (result) *result = OK;
            return SString(m_data.buffer);
        case B_PACK_SMALL_TYPE(B_STRING_TYPE, 0):
        case B_PACK_SMALL_TYPE(B_NULL_TYPE, 0):
            if (result) *result = OK;
            return SString();
    }

    SString val;
    status_t r;
    switch (B_UNPACK_TYPE_CODE(m_type)) {
        case B_STRING_TYPE: {
            r = getString(&val);
        } break;
        case B_DOUBLE_TYPE: {
            double ex;
            if ((r = getDouble(&ex)) == OK) {
                char buffer[64];
                sprintf(buffer, "%g", ex);
                if( !strchr(buffer, '.') && !strchr(buffer, 'e') &&
                    !strchr(buffer, 'E') ) {
                    strncat(buffer, ".0", sizeof(buffer)-1);
                }
                val = buffer;
            }
        } break;
        case B_FLOAT_TYPE: {
            float ex;
            if ((r = getFloat(&ex)) == OK) {
                char buffer[64];
                sprintf(buffer, "%g", (double)ex);
                if( !strchr(buffer, '.') && !strchr(buffer, 'e') &&
                    !strchr(buffer, 'E') ) {
                    strncat(buffer, ".0", sizeof(buffer)-1);
                }
                val = buffer;
            }
        } break;
        case B_INT64_TYPE: {
            int64_t ex;
            if ((r = getInt64(&ex)) == OK) {
                char buffer[64];
                sprintf(buffer, "%" B_FORMAT_INT64 "d", ex);
                val = buffer;
            }
        } break;
        case B_NSECS_TYPE: {
            nsecs_t ex;
            if ((r = getTime(&ex)) == OK) {
                char buffer[64];
                sprintf(buffer, "%" B_FORMAT_INT64 "d", ex);
                val = buffer;
            }
        } break;
        case B_INT32_TYPE: {
            int32_t ex;
            if ((r = getInt32(&ex)) == OK) {
                char buffer[64];
                sprintf(buffer, "%ld", ex);
                val = buffer;
            }
        } break;
        case B_INT16_TYPE: {
            int16_t ex;
            if ((r = getInt16(&ex)) == OK) {
                char buffer[64];
                sprintf(buffer, "%d", ex);
                val = buffer;
            }
        } break;
        case B_INT8_TYPE: {
            int8_t ex;
            if ((r = getInt8(&ex)) == OK) {
                char buffer[64];
                sprintf(buffer, "%d", ex);
                val = buffer;
            }
        } break;
        case B_BOOL_TYPE: {
            bool ex;
            if ((r = getBool(&ex)) == OK) {
                val = ex ? "true" : "false";
            }
        } break;
        case B_UUID_TYPE: {
            if (length() == sizeof(uuid_t)) {
                uuid_t* u = (uuid_t*)data();
                char buffer[64];
                sprintf(buffer, "%8.8x-%4.4x-%4.4x-%2.2x%2.2x-%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x",
                        u->time_low,
                        u->time_mid,
                        u->time_hi_and_version,
                        u->clock_seq_hi_and_reserved,
                        u->clock_seq_low,
                        u->node[0], u->node[1], u->node[2], u->node[3], u->node[4], u->node[5]);
                val = buffer;
                r = OK;
            } else {
                r = type_conversion_error();
            }
        } break;
        default:
            r = type_conversion_error();
    }
#if 0
    volatile int32_t ini = r;
    volatile uint32_t inu = r;
    int32_t out = 0;
    testa_int32(ini, &out);
    testb_int32(ini, &out);
    testa_uint32(inu, &out);
    testb_uint32(inu, &out);
#endif
    if (result) *result = r;
    return val;
}

bool SValue::asBool(status_t* result) const
{
    // First check the fast and easy cases.
    switch (m_type) {
        case B_PACK_SMALL_TYPE(B_BOOL_TYPE, sizeof(int8_t)):
            if (result) *result = OK;
            return (*reinterpret_cast<const int8_t*>(m_data.local)) ? true : false;
        case B_PACK_SMALL_TYPE(B_INT32_TYPE, sizeof(int32_t)):
            if (result) *result = OK;
            return m_data.integer ? true : false;
        case B_PACK_SMALL_TYPE(B_NULL_TYPE, 0):
            if (result) *result = OK;
            return false;
    }

    bool val = false;
    status_t r;
    switch (B_UNPACK_TYPE_CODE(m_type)) {
        case B_STRING_TYPE: {
            const char* ex;
            if ((r = getString(&ex)) == OK) {
                if (strcasecmp(ex, "true") == 0) val = true;
                else val = strtol(ex, const_cast<char**>(&ex), 10) != 0 ? true : false;
            }
        } break;
        case B_DOUBLE_TYPE: {
            double ex;
            if ((r = getDouble(&ex)) == OK) {
                val = (ex > DBL_EPSILON || ex < (-DBL_EPSILON)) ? true : false;
            }
        } break;
        case B_FLOAT_TYPE: {
            float ex;
            if ((r = getFloat(&ex)) == OK) {
                val = (ex > FLT_EPSILON || ex < (-FLT_EPSILON)) ? true : false;
            }
        } break;
        case B_INT64_TYPE: {
            int64_t ex;
            if ((r = getInt64(&ex)) == OK) {
                val = ex != 0 ? true : false;
            }
        } break;
        case B_NSECS_TYPE: {
            nsecs_t ex;
            if ((r = getTime(&ex)) == OK) {
                val = ex != 0 ? true : false;
            }
        } break;
        case B_INT16_TYPE: {
            int16_t ex;
            if ((r = getInt16(&ex)) == OK) {
                val = ex != 0 ? true : false;
            }
        } break;
        case B_INT8_TYPE: {
            int8_t ex;
            if ((r = getInt8(&ex)) == OK) {
                val = ex != 0 ? true : false;
            }
        } break;
        case B_ATOM_TYPE: {
            sp<RefBase> ex;
            if ((r = getRefBase(&ex)) == OK) {
                val = ex != NULL ? true : false;
            }
        } break;
        case B_ATOM_WEAK_TYPE: {
            wp<RefBase> ex;
            if ((r = getWeakRefBase(&ex)) == OK) {
                val = ex != NULL ? true : false;
            }
        } break;
        default: {
            wp<IBinder> binder = asWeakBinder(&r);
            if (r == OK) {
                val = binder != NULL ? true : false;
            } else {
                r = type_conversion_error();
            }
        }
    }
    if (result) *result = r;
    return val;
}

static int64_t string_to_number(const char* str, status_t* outResult)
{
    int64_t val = 0;
    bool neg = false;

    while (isspace(*str)) str++;
    if (str[0] == '+') str++;
    if (str[0] == '-') {
        neg = true;
        str++;
    }
    if (str[0] >= '0' && str[0] <= '9') {
        // It's a real, honest-to-ghod number...
        int32_t base = 10;
        if (str[0] == '0' && str[1] == 'x') {
            str += 2;
            base = 16;
        }
        val = strtoll(str, const_cast<char**>(&str), base);
        *outResult = OK;
    } else if (str[0] == '[' || str[0] == '\'') {
        const char end = (str[0] == '[') ? ']' : '\'';
        val = 0;
        str++;
        while (str[0] != 0 && str[0] != end) {
            val = (val<<8) | str[0];
            str++;
        }
        if (str[0] == end) {
            *outResult = OK;
        } else {
            *outResult = BAD_TYPE;
        }
    } else {
        *outResult = BAD_TYPE;
    }

    if (neg) val = -val;
    return val;
}

int SValue::asInt(status_t* result) const
{
    // First check the fast and easy cases.
    switch (m_type) {
        case B_PACK_SMALL_TYPE(B_INT32_TYPE, sizeof(int32_t)):
            if (result) *result = OK;
            return m_data.integer;
        case B_PACK_SMALL_TYPE(B_NULL_TYPE, 0):
            if (result) *result = OK;
            return 0;
    }

    int32_t val = 0;
    status_t r;
    switch (B_UNPACK_TYPE_CODE(m_type)) {
        case B_STRING_TYPE: {
            const char* ex;
            if ((r = getString(&ex)) == OK) {
                val = static_cast<int32_t>(string_to_number(ex, &r));
            }
        } break;
        case B_DOUBLE_TYPE: {
            double ex;
            if ((r = getDouble(&ex)) == OK) {
                val = static_cast<int32_t>((ex > 0.0 ? (ex+.5) : (ex-.5)));
            }
        } break;
        case B_FLOAT_TYPE: {
            float ex;
            if ((r = getFloat(&ex)) == OK) {
                val = static_cast<int32_t>((ex > 0.0f ? (ex+.5f) : (ex-.5f)));
            }
        } break;
        case B_INT64_TYPE: {
            int64_t ex;
            if ((r = getInt64(&ex)) == OK) {
                val = static_cast<int32_t>(ex);
            }
        } break;
        case B_NSECS_TYPE: {
            nsecs_t ex;
            if ((r = getTime(&ex)) == OK) {
                val = static_cast<int32_t>(ex);
            }
        } break;
        case B_INT16_TYPE: {
            int16_t ex;
            if ((r = getInt16(&ex)) == OK) {
                val = ex;
            }
        } break;
        case B_INT8_TYPE: {
            int8_t ex;
            if ((r = getInt8(&ex)) == OK) {
                val = ex;
            }
        } break;
        case B_BOOL_TYPE: {
            bool ex;
            if ((r = getBool(&ex)) == OK) {
                val = ex ? 1 : 0;
            }
        } break;
        default:
            r = type_conversion_error();
    }
    if (result) *result = r;
    return val;
}

static bool decode_time(const char** str, nsecs_t* inoutTime)
{
    nsecs_t factor = 0;
    // First check for a suffix.
    const char* num = *str;
    while (*num && isspace(*num)) num++;
    const char* unit = num;
    while (*unit && *unit >= '0' && *unit <= '9') unit++;
    if ((num-unit) == 0) return false;
    while (*unit && isspace(*unit)) unit++;
    const char* end = unit;
    while (*end && !isspace(*end) && (*end < '0' || *end > '9')) end++;
    if ((end-unit) == 0) {
        factor = 1;
    } else if ((end-unit) == 1) {
        if (*unit == 'd') factor = B_ONE_SECOND*60*60*24;
        else if (*unit == 'h') factor = B_ONE_SECOND*60*60;
        else if (*unit == 'm') factor = B_ONE_SECOND*60;
        else if (*unit == 's') factor = B_ONE_SECOND;
    } else if ((end-unit) == 2) {
        if (strncmp(unit, "ms", 2) == 0) factor = B_ONE_MILLISECOND;
        else if (strncmp(unit, "us", 2) == 0) factor = B_ONE_MICROSECOND;
        else if (strncmp(unit, "ns", 2) == 0) factor = 1;
    } else if ((end-unit) == 4) {
        if (strncmp(unit, "day", 3) == 0) factor = B_ONE_SECOND*60*60*24;
        else if (strncmp(unit, "min", 3) == 0) factor = B_ONE_SECOND*60;
        else if (strncmp(unit, "sec", 3) == 0) factor = B_ONE_SECOND;
    } else if ((end-unit) == 4) {
        if (strncmp(unit, "hour", 4) == 0) factor = B_ONE_MILLISECOND*60*60;
        else if (strncmp(unit, "msec", 4) == 0) factor = B_ONE_MILLISECOND;
        else if (strncmp(unit, "usec", 4) == 0) factor = B_ONE_MICROSECOND;
        else if (strncmp(unit, "nsec", 4) == 0) factor = 1;
    }
    if (factor == 0) return false;
    *inoutTime += strtoll(num, const_cast<char**>(&num), 10) * factor;
    *str = end;
    return true;
}

nsecs_t SValue::asTime(status_t* result) const
{
    nsecs_t val = 0;
    status_t r;
    switch (B_UNPACK_TYPE_CODE(m_type)) {
        case B_STRING_TYPE: {
            const char* ex;
            if ((r = getString(&ex)) == OK) {
                bool gotSomething = false;
                while (decode_time(&ex, &val)) gotSomething = true;
                if (!gotSomething) r = BAD_TYPE;
            }
        } break;
        case B_DOUBLE_TYPE: {
            double ex;
            if ((r = getDouble(&ex)) == OK) {
                val = static_cast<nsecs_t>((ex > 0.0 ? (ex+.5) : (ex-.5)));
            }
        } break;
        case B_FLOAT_TYPE: {
            float ex;
            if ((r = getFloat(&ex)) == OK) {
                val = static_cast<nsecs_t>((ex > 0.0f ? (ex+.5f) : (ex-.5f)));
            }
        } break;
        case B_INT64_TYPE: {
            int64_t ex;
            if ((r = getInt64(&ex)) == OK) {
                val = static_cast<nsecs_t>(ex);
            }
        } break;
        case B_NSECS_TYPE: {
            r = getTime(&val);
        } break;
        case B_INT32_TYPE: {
            int32_t ex;
            if ((r = getInt32(&ex)) == OK) {
                val = ex;
            }
        } break;
        case B_INT16_TYPE: {
            int16_t ex;
            if ((r = getInt16(&ex)) == OK) {
                val = ex;
            }
        } break;
        case B_INT8_TYPE: {
            int8_t ex;
            if ((r = getInt8(&ex)) == OK) {
                val = ex;
            }
        } break;
        case B_BOOL_TYPE: {
            bool ex;
            if ((r = getBool(&ex)) == OK) {
                val = ex ? 1 : 0;
            }
        } break;
        case B_NULL_TYPE: {
            r = OK;
            val = 0;
        } break;
        default:
            r = type_conversion_error();
    }
    if (result) *result = r;
    return val;
}

float SValue::asFloat(status_t* result) const
{
    // First check the fast and easy cases.
    switch (m_type) {
        case B_PACK_SMALL_TYPE(B_FLOAT_TYPE, sizeof(float)):
            if (result) *result = OK;
            return *reinterpret_cast<const float*>(m_data.local);
        case B_PACK_SMALL_TYPE(B_INT32_TYPE, sizeof(int32_t)):
            if (result) *result = OK;
            return static_cast<float>(m_data.integer);
        case B_PACK_SMALL_TYPE(B_NULL_TYPE, 0):
            if (result) *result = OK;
            return 0.0f;
    }

    float val = 0;
    status_t r;
    switch (B_UNPACK_TYPE_CODE(m_type)) {
        case B_STRING_TYPE: {
            const char* ex;
            if ((r = getString(&ex)) == OK) {
                val = static_cast<float>(strtod(ex, const_cast<char**>(&ex)));
            }
        } break;
        case B_DOUBLE_TYPE: {
            double ex;
            if ((r = getDouble(&ex)) == OK) {
                val = static_cast<float>(ex);
            }
        } break;
        case B_INT64_TYPE: {
            int64_t ex;
            if ((r = getInt64(&ex)) == OK) {
                val = static_cast<float>(ex);
            }
        } break;
        case B_NSECS_TYPE: {
            nsecs_t ex;
            if ((r = getTime(&ex)) == OK) {
                val = static_cast<float>(ex);
            }
        } break;
        case B_INT16_TYPE: {
            int16_t ex;
            if ((r = getInt16(&ex)) == OK) {
                val = ex;
            }
        } break;
        case B_INT8_TYPE: {
            int8_t ex;
            if ((r = getInt8(&ex)) == OK) {
                val = ex;
            }
        } break;
        case B_BOOL_TYPE: {
            bool ex;
            if ((r = getBool(&ex)) == OK) {
                val = ex ? 1.0f : 0.0f;
            }
        } break;
        default:
            r = type_conversion_error();
    }
    if (result) *result = r;
    return val;
}

double SValue::asDouble(status_t* result) const
{
    double val = 0;
    status_t r;
    switch (B_UNPACK_TYPE_CODE(m_type)) {
        case B_STRING_TYPE: {
            const char* ex;
            if ((r = getString(&ex)) == OK) {
                val = strtod(ex, const_cast<char**>(&ex));
            }
        } break;
        case B_DOUBLE_TYPE: {
            r = getDouble(&val);
        } break;
        case B_FLOAT_TYPE: {
            float ex;
            if ((r = getFloat(&ex)) == OK) {
                val = ex;
            }
        } break;
        case B_INT64_TYPE: {
            int64_t ex;
            if ((r = getInt64(&ex)) == OK) {
                val = static_cast<double>(ex);
            }
        } break;
        case B_NSECS_TYPE: {
            nsecs_t ex;
            if ((r = getTime(&ex)) == OK) {
                val = static_cast<double>(ex);
            }
        } break;
        case B_INT32_TYPE: {
            int32_t ex;
            if ((r = getInt32(&ex)) == OK) {
                val = ex;
            }
        } break;
        case B_INT16_TYPE: {
            int16_t ex;
            if ((r = getInt16(&ex)) == OK) {
                val = ex;
            }
        } break;
        case B_INT8_TYPE: {
            int8_t ex;
            if ((r = getInt8(&ex)) == OK) {
                val = ex;
            }
        } break;
        case B_BOOL_TYPE: {
            bool ex;
            if ((r = getBool(&ex)) == OK) {
                val = ex ? 1 : 0;
            }
        } break;
        case B_NULL_TYPE: {
            r = OK;
            val = 0.0;
        } break;
        default:
            r = type_conversion_error();
    }
    if (result) *result = r;
    return val;
}

sp<RefBase> SValue::asRefBase(status_t* result) const
{
    sp<RefBase> val;
    status_t r = getRefBase(&val);
    if (r < OK) {
        wp<RefBase> ex;
        if ((r = getWeakRefBase(&ex)) == OK) {
            val = ex.promote();
        } else if (m_type == B_PACK_SMALL_TYPE(B_NULL_TYPE, 0)) {
            r = OK;
        } else {
            r = type_conversion_error();
        }
    }
    if (result) *result = r;
    return val;
}

wp<RefBase> SValue::asWeakRefBase(status_t* result) const
{
    wp<RefBase> val;
    status_t r = getWeakRefBase(&val);
    if (r != OK && m_type == B_PACK_SMALL_TYPE(B_NULL_TYPE, 0)) {
        r = OK;
    }
    if (result) *result = r;
    return val;
}

type_code SValue::type() const
{
    return B_UNPACK_TYPE_CODE(m_type);
}

size_t SValue::length() const
{
    const uint32_t len = B_UNPACK_TYPE_LENGTH(m_type);
    if (len <= B_TYPE_LENGTH_MAX) return len;
    if (len == B_TYPE_LENGTH_LARGE) return m_data.buffer->size();
    return 0;
}

const void* SValue::data() const
{
    if (B_UNPACK_TYPE_LENGTH(m_type) <= B_TYPE_LENGTH_MAX) return m_data.local;
    else if (B_UNPACK_TYPE_LENGTH(m_type) == B_TYPE_LENGTH_LARGE) return m_data.buffer->data();
    return NULL;
}

const void* SValue::data(type_code type, size_t length) const
{
    if (length <= B_TYPE_LENGTH_MAX) {
        if (m_type == B_PACK_SMALL_TYPE(type, length)) return m_data.local;
        return NULL;
    }
    if (B_PACK_LARGE_TYPE(type) == m_type
            && m_data.buffer->size() == length) {
        return m_data.buffer->data();
    }
    return NULL;
}

const void* SValue::data(type_code type, size_t* inoutMinLength) const
{
    if (*inoutMinLength > B_TYPE_LENGTH_MAX) {
        if (m_type == B_PACK_LARGE_TYPE(type) && m_data.buffer->size() >= *inoutMinLength) {
            *inoutMinLength = m_data.buffer->size();
            return m_data.buffer->data();
        }
        return NULL;
    }
    if (B_UNPACK_TYPE_CODE(m_type) == type) {
        size_t len = B_UNPACK_TYPE_LENGTH(m_type);
        if (len <= B_TYPE_LENGTH_MAX && len >= *inoutMinLength) {
            *inoutMinLength = len;
            return m_data.local;
        }
        if (len == B_TYPE_LENGTH_LARGE) {
            *inoutMinLength = m_data.buffer->size();
            return m_data.buffer->data();
        }
    }
    return NULL;
}

const SharedBuffer* SValue::sharedBuffer() const
{
    if (B_UNPACK_TYPE_LENGTH(m_type) == B_TYPE_LENGTH_LARGE) {
        m_data.buffer->acquire();
        return m_data.buffer;
    } else if (B_UNPACK_TYPE_LENGTH(m_type) <= B_TYPE_LENGTH_MAX) {
        SharedBuffer* buf = SharedBuffer::alloc(B_UNPACK_TYPE_LENGTH(m_type));
        if (buf) *(int32_t*)buf->data() = m_data.integer;
        return buf;
    }
    return NULL;
}

void SValue::pool()
{
    if (B_UNPACK_TYPE_LENGTH(m_type) == B_TYPE_LENGTH_LARGE) {
        STUB;//m_data.buffer = m_data.buffer->pool();
    } else if (is_map()) {
        //const BValueMap* oldMap = m_data.map;
        BValueMap** map = edit_map();
        if (map) {
            (*map)->pool();
            //if (oldMap != *map) bout << "BValue pool had to copy: " << *this << endl;
        }
    }
}

status_t SValue::setType(type_code newType)
{
    if (is_simple() && !CHECK_IS_SMALL_OBJECT(m_type) && !CHECK_IS_SMALL_OBJECT(newType)) {
        size_t len = length();
        m_type = len <= B_TYPE_LENGTH_MAX
            ? B_PACK_SMALL_TYPE(newType, len) : B_PACK_LARGE_TYPE(newType);
        return OK;
    }

    return BAD_TYPE;
}

void* SValue::beginEditBytes(type_code type, size_t length, uint32_t flags)
{
    void* data;
    if (is_simple() && !CHECK_IS_SMALL_OBJECT(m_type) && !CHECK_IS_SMALL_OBJECT(type)) {
        if (!(flags&B_EDIT_VALUE_DATA)) {
            if (!is_error()) {
                freeData();
                data = alloc_data(type, length);
            } else data = NULL;
        } else {
            data = edit_data(length);
            if (data) {
                m_type = length <= B_TYPE_LENGTH_MAX
                    ? B_PACK_SMALL_TYPE(type, length) : B_PACK_LARGE_TYPE(type);
            }
        }
    } else {
        data = NULL;
    }
    return data;
}

status_t SValue::endEditBytes(ssize_t final_length)
{
    if (!is_error()) {
        if (final_length < 0) return OK;
        const size_t len = length();
        if (final_length == len) return OK;
        if (final_length < (ssize_t)len) {
            // Need to convert to inline storage.
            if (edit_data(final_length)) return OK;
            return errorCheck();
        }
        return BAD_VALUE;
    }
    return errorCheck();
}

#if SUPPORTS_TEXT_STREAM
static bool LooksLikeFourCharCode(uint32_t t)
{
    for (int i = 3; i >= 0; i--)
    {
        uint8_t c = (uint8_t)((t >> (8 * i)) & 0x00FF);

        if ((c < ' ') || (c >= 0x7F))
            return false;
    }

    return true;
}
#endif

status_t SValue::PrintToStream(const sp<ITextOutput>& io, uint32_t flags) const
{
#if SUPPORTS_TEXT_STREAM
    if (flags&B_PRINT_STREAM_HEADER) io << "SValue(";

    if (isSimple()) {

        sp<IBinder> binder;
        bool handled = true;
        char buf[128];

        switch (m_type) {
            case kUndefinedTypeCode:
                io << "undefined";
                break;
            case kWildTypeCode:
                io << "wild";
                break;
            case B_PACK_SMALL_TYPE(B_NULL_TYPE, 0):
                io << "null";
                break;
            case B_PACK_SMALL_TYPE(B_CHAR_TYPE, sizeof(char)):
                io << "char(" << (int32_t)(char)m_data.local[0]
                    << " or " << UIntToHex(m_data.local[0], buf);
                if (isprint(m_data.local[0])) io << " or '" << (char)m_data.local[0] << "'";
                io << ")";
                break;
            case B_PACK_SMALL_TYPE(B_INT8_TYPE, sizeof(int8_t)):
                io << "int8_t(" << (int32_t)(int8_t)m_data.local[0]
                    << " or " << UIntToHex(m_data.local[0], buf);
                if (isprint(m_data.local[0])) io << " or '" << (char)m_data.local[0] << "'";
                io << ")";
                break;
            case B_PACK_SMALL_TYPE(B_INT16_TYPE, sizeof(int16_t)):
                io << "int16_t(" << *reinterpret_cast<const int16_t*>(m_data.local)
                    << " or " << UIntToHex(*(uint16_t*)m_data.local, buf) << ")";
                break;
            case B_PACK_SMALL_TYPE(B_INT32_TYPE, sizeof(int32_t)):
                io << "int32_t(" << *reinterpret_cast<const int32_t*>(m_data.local)
                    << " or " << UIntToHex(*(uint32_t*)m_data.local, buf);
                if (LooksLikeFourCharCode(*reinterpret_cast<const uint32_t*>(m_data.local)))
                    io << " or " << STypeCode(*(uint32_t*)m_data.local);
                io << ")";
                break;
            case B_PACK_LARGE_TYPE(B_INT64_TYPE):
                if (m_data.buffer->size() == sizeof(int64_t)) {
                    io << "int64_t(" << *reinterpret_cast<const int64_t*>(m_data.buffer->data())
                        << " or " << UIntToHex(*(uint64_t*)m_data.buffer->data(), buf) << ")";
                } else {
                    handled = false;
                }
                break;
            case B_PACK_SMALL_TYPE(B_UINT8_TYPE, sizeof(uint8_t)):
                io << "uint8_t(" << (uint32_t)(uint8_t)m_data.local[0]
                    << " or " << UIntToHex(m_data.local[0], buf);
                if (isprint(m_data.local[0])) io << " or '" << (char)m_data.local[0] << "'";
                io << ")";
                break;
            case B_PACK_SMALL_TYPE(B_UINT16_TYPE, sizeof(uint16_t)):
                io << "uint16_t(" << (uint32_t)*reinterpret_cast<const uint16_t*>(m_data.local)
                    << " or " << UIntToHex(*(uint16_t*)m_data.local, buf) << ")";
                break;
            case B_PACK_SMALL_TYPE(B_UINT32_TYPE, sizeof(uint32_t)):
                io << "uint32_t(" << *reinterpret_cast<const uint32_t*>(m_data.local)
                    << " or " << UIntToHex(*(uint32_t*)m_data.local, buf) << ")";
                break;
            case B_PACK_LARGE_TYPE(B_UINT64_TYPE):
                if (m_data.buffer->size() == sizeof(uint64_t)) {
                    io << "uint64_t(" << *reinterpret_cast<const uint64_t*>(m_data.buffer->data())
                        << " or " << UIntToHex(*(uint64_t*)m_data.buffer->data(), buf) << ")";
                } else {
                    handled = false;
                }
                break;
            case B_PACK_SMALL_TYPE(B_ERROR_TYPE, sizeof(status_t)): {
                // Internal error
                io << "ERR: " << SStatus(*reinterpret_cast<const status_t*>(m_data.local));
                #if 0
                STextDecoder dec;
                dec.DeviceToUTF8(str, strlen(str));
                io << dec.Buffer();
                #endif
            } break;
            case B_PACK_SMALL_TYPE(B_STATUS_TYPE, sizeof(status_t)): {
                io << SStatus(*reinterpret_cast<const status_t*>(m_data.local));
                #if 0
                STextDecoder dec;
                dec.DeviceToUTF8(str, strlen(str));
                io << dec.Buffer();
                #endif
            } break;
            case B_PACK_SMALL_TYPE(B_FLOAT_TYPE, sizeof(float)):
                io << "float(" << *reinterpret_cast<const float*>(m_data.local) << ")";
                break;
            case B_PACK_LARGE_TYPE(B_DOUBLE_TYPE):
                if (m_data.buffer->size() == sizeof(double)) {
                    io << "double(" << *reinterpret_cast<const double*>(m_data.buffer->data()) << ")";
                } else {
                    handled = false;
                }
                break;
            case B_PACK_SMALL_TYPE(B_BOOL_TYPE, sizeof(int8_t)):
                if (flags&B_PRINT_VALUE_TYPES) io << "bool(";
                io << ((*reinterpret_cast<const int8_t*>(m_data.local)) ? "true" : "false");
                if (flags&B_PRINT_VALUE_TYPES) io << ")";
                break;
            case B_PACK_LARGE_TYPE(B_NSECS_TYPE):
                if (m_data.buffer->size() == sizeof(nsecs_t)) {
                    const nsecs_t val = *reinterpret_cast<const nsecs_t*>(m_data.buffer->data());
                    io << "nsecs_t(" << SDuration(val) << " or " << UIntToHex(val, buf) << ")";
                } else {
                    handled = false;
                }
                break;
            case B_PACK_LARGE_TYPE(B_OFF_T_TYPE):
                if (m_data.buffer->size() == sizeof(off_t)) {
                    io << "off_t(" << *reinterpret_cast<const off_t*>(m_data.buffer->data())
                        << " or " << UIntToHex(*(off_t*)m_data.buffer->data(), buf) << ")";
                } else {
                    handled = false;
                }
                break;
            case B_PACK_SMALL_TYPE(B_SIZE_T_TYPE, sizeof(size_t)):
                io << "size_t(" << *reinterpret_cast<const size_t*>(m_data.local)
                    << " or " << UIntToHex(*(size_t*)m_data.local, buf) << ")";
                break;
            case B_PACK_SMALL_TYPE(B_SSIZE_T_TYPE, sizeof(ssize_t)):
                io << "ssize_t(" << *reinterpret_cast<const ssize_t*>(m_data.local)
                    << " or " << UIntToHex(*(size_t*)m_data.local, buf) << ")";
                break;
            case kPackedSmallAtomType: {
                const RefBase* a = *reinterpret_cast<RefBase* const *>(m_data.local);
                io << "sp<RefBase>(" << a;
#if _SUPPORTS_RTTI
                if (a) io << " " << typeid(*a).name();
#endif
                io << ")";
            } break;
            case kPackedSmallAtomWeakType: {
                const RefBase::weak_atom_ptr* wp = *reinterpret_cast<RefBase::weak_atom_ptr* const *>(m_data.local);
                io << "wp<RefBase>(" << (wp ? wp->atom : NULL);
#if _SUPPORTS_RTTI
                if (wp && wp->atom) io << " " << typeid(*wp->atom).name();
#endif
                io << ")";
            } break;
            case kPackedSmallBinderType: {
                const IBinder* b = *reinterpret_cast<IBinder* const *>(m_data.local);
                io << "sp<IBinder>(" << b;
#if _SUPPORTS_RTTI
                if (b) io << " " << typeid(*b).name();
#endif
                io << ")";
                if (flags&(B_PRINT_BINDER_INTERFACES|B_PRINT_BINDER_CONTENTS)) {
                    binder = const_cast<IBinder*>(b);
                }
            } break;
            case kPackedSmallBinderWeakType: {
                const RefBase::weak_atom_ptr* wp = *reinterpret_cast<RefBase::weak_atom_ptr* const *>(m_data.local);
                const IBinder* real = (wp && wp->atom->AttemptIncStrong(this) ? ((const IBinder*)wp->cookie) : NULL);
                io << "wp<IBinder>(" << (wp ? wp->cookie : NULL);
#if _SUPPORTS_RTTI
                if (real) io << " " << typeid(*real).name();
#endif
                io << ")";
                if (flags&(B_PRINT_BINDER_INTERFACES|B_PRINT_BINDER_CONTENTS)) {
                    binder = const_cast<IBinder*>(real);
                }
                if (real) real->DecStrong(this);
            } break;
            case kPackedSmallBinderHandleType:
                io << "IBinder::shnd(" << UIntToHex(*reinterpret_cast<const int32_t*>(m_data.local), buf) << ")";
                if (flags&(B_PRINT_BINDER_INTERFACES|B_PRINT_BINDER_CONTENTS)) {
                    binder = SValue(*this).AsBinder();
                }
                break;
            case kPackedSmallBinderWeakHandleType:
                io << "IBinder::whnd(" << UIntToHex(*reinterpret_cast<const int32_t*>(m_data.local), buf) << ")";
                if (flags&(B_PRINT_BINDER_INTERFACES|B_PRINT_BINDER_CONTENTS)) {
                    binder = SValue(*this).AsBinder();
                }
                break;
            default:
                handled = false;
                break;
        }

        if (!handled) {
            const type_code type = type();
            const size_t length = length();
            const void* data = data();
            if (type == B_STRING_TYPE || type == B_SYSTEM_TYPE) {
                bool valid = true;
                if (length > 0) {
                    if (static_cast<const char*>(data)[length-1] != 0) valid = false;
                    for (size_t i=0; valid && i<length-1; i++) {
                        if (static_cast<const char*>(data)[i] == 0) valid = false;
                    }
                } else {
                    valid = false;
                }
                if (valid) {
                    if (type == B_STRING_TYPE) {
                        if (flags&B_PRINT_VALUE_TYPES) io << "string(\"";
                        else io << "\"";
                        io << static_cast<const char*>(data);
                        if (flags&B_PRINT_VALUE_TYPES) io << "\", " << length << " bytes)";
                        else io << "\"";
                    } else {
                        io << "system(\"" << static_cast<const char*>(data) << "\")";
                    }
                    handled = true;
                } else if (type == B_STRING_TYPE) {
                    io	<< indent << "string " << length << " bytes: "
                        << (SHexDump(data, length).SetSingleLineCutoff(16)) << dedent;
                    handled = true;
                }
            }
            else if (type == B_UUID_TYPE && length() == sizeof(uuid_t))
            {
                uuid_t* u = (uuid_t*)data;
                io << "UUID(\"";
                io << SPrintf("%8.8x-%4.4x-%4.4x-%2.2x%2.2x-%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x",
                            u->time_low,
                            u->time_mid,
                            u->time_hi_and_version,
                            u->clock_seq_hi_and_reserved,
                            u->clock_seq_low,
                            u->node[0], u->node[1], u->node[2], u->node[3], u->node[4], u->node[5]);
                io << "\")";
                handled = true;
            }

            if (!handled) {
                printer_registry* reg = Printers();
                if (reg && reg->lock.Lock() == OK) {
                    print_func f = reg->funcs.valueFor(type);
                    reg->lock.Unlock();
                    if (f) handled = ( (*f)(io, *this, flags|B_PRINT_STREAM_HEADER) == OK );
                }
            }

            if (!handled) {
                io	<< indent << STypeCode(type) << " " << length << " bytes:"
                    << (SHexDump(data, length).SetSingleLineCutoff(16)) << dedent;
            }
        }

        if (binder != NULL) {
            bool didInterfaces = false;
            if ((flags&(B_PRINT_BINDER_INTERFACES|0x10000000)) == B_PRINT_BINDER_INTERFACES) {
                SValue val(binder->Inspect(binder, SValue::Wild()));
                if (val.isDefined()) {
                    io << " ";
                    val.PrintToStream(io, flags&(~B_PRINT_STREAM_HEADER)|0x10000000);
                    didInterfaces = true;
                }
            }
            if (!didInterfaces) {
                if ((flags&B_PRINT_CATALOG_CONTENTS) != 0) {
                    // We need a real catalog interface!!!!!!!
                    if (!binder->Inspect(binder, SValue::Wild()).isDefined()) {
                        SValue val(binder->Get(SValue::Wild()));
                        io << " ";
                        val.PrintToStream(io, flags&(~(B_PRINT_STREAM_HEADER|0x10000000)));
                    }
                } else if ((flags&B_PRINT_BINDER_CONTENTS) != 0) {
                    SValue val(binder->Get(SValue::Wild()));
                    io << " ";
                    val.PrintToStream(io, flags&(~(B_PRINT_STREAM_HEADER|0x10000000)));
                }
            }
        }

    } else {

        SValue key, value;
        const size_t N = countItems();
        if (N > 1) {
            io << "{" << endl << indent;
        }
        for (size_t i=0; i<N; i++) {
            find_item_index(i, &key, &value);
            if (!key.isWild()) {
                const bool need_bracket = !key.isSimple() && key.countItems() <= 1;
                if (need_bracket) io << "{ ";
                key.PrintToStream(io, flags&(~B_PRINT_STREAM_HEADER));
                if (need_bracket) io << " }";
                io << " -> ";
            }
            value.PrintToStream(io, flags&(~B_PRINT_STREAM_HEADER));
            if (N > 1) {
                if (i < (N-1)) io << "," << endl;
                else io << endl;
            }
        }
        if (N > 1) {
            io << dedent << "}";
        }
    }

    if (flags&B_PRINT_STREAM_HEADER) io << ")";
    return OK;
#else
    (void)io;
    (void)flags;
    return INVALID_OPERATION;
#endif
}

const TextOutput& operator<<(const TextOutput& io, const SValue& value)
{
#if SUPPORTS_TEXT_STREAM
    value.PrintToStream(io, B_PRINT_STREAM_HEADER);
#else
    (void)value;
#endif
    return io;
}

void* SValue::edit_data(size_t len)
{
    if (!is_error()) {

        // Are we currently storing data inline?
        if (B_UNPACK_TYPE_LENGTH(m_type) <= B_TYPE_LENGTH_MAX) {
            if (len <= B_TYPE_LENGTH_MAX) {
                m_type = (m_type&~B_TYPE_LENGTH_MASK) | len;
                return m_data.local;
            }

            // Need to create a new buffer for the data.
            SharedBuffer* buf = SharedBuffer::alloc(len);
            if (buf) {
                *(int32_t*)buf->data() = m_data.integer;
                m_type = (m_type&~B_TYPE_LENGTH_MASK) | B_TYPE_LENGTH_LARGE;
                m_data.buffer = buf;
                return buf->data();
            }

            set_error(NO_MEMORY);
            return NULL;

        // Are we currently storing data in a shared buffer?
        } else if (B_UNPACK_TYPE_LENGTH(m_type) == B_TYPE_LENGTH_LARGE) {

            // Shrinking to inline storage?
            if (len <= B_TYPE_LENGTH_MAX) {
                const SharedBuffer* buf = m_data.buffer;
                m_data.integer = *(const int32_t*)buf->data();
                m_type = (m_type&~B_TYPE_LENGTH_MASK) | len;
                buf->release();
                return m_data.local;
            }

            // Need to resize an existing buffer.
            SharedBuffer* buf = m_data.buffer->editResize(len);
            if (buf) {
                m_data.buffer = buf;
                return buf->data();
            }

            set_error(NO_MEMORY);
            return NULL;

        }

    }

    return NULL;
}

status_t SValue::copy_small_data(type_code type, void* buf, size_t len) const
{
    LOG_FATAL_IF(type == B_UNDEFINED_TYPE, "B_UNDEFINED_TYPE not valid here.");

    if (m_type == type) {
        memcpy(buf, m_data.local, len);
        return OK;
    }

    if (is_error()) return errorCheck();
    if (B_UNPACK_TYPE_CODE(m_type) != B_UNPACK_TYPE_CODE(type)) return BAD_TYPE;
    return BAD_VALUE;
}

status_t SValue::copy_big_data(type_code type, void* buf, size_t len) const
{
    LOG_FATAL_IF(type == B_UNDEFINED_TYPE, "B_UNDEFINED_TYPE not valid here.");

    if (m_type == type && m_data.buffer->size() == len) {
        memcpy(buf, m_data.buffer->data(), len);
        return OK;
    }

    if (is_error()) return errorCheck();
    if (B_UNPACK_TYPE_CODE(m_type) != B_UNPACK_TYPE_CODE(type)) return BAD_TYPE;
    return BAD_VALUE;
}

BValueMap** SValue::edit_map()
{
    BValueMap** map = (BValueMap**)(&m_data.map);
    BValueMap* other;
    if (is_map()) {
        if ((*map)->isShared()) {
            other = (*map)->clone();
            if (other) {
                (*map)->release();
                *map = other;
            } else {
                map = NULL;
                set_error(NO_MEMORY);
            }
        }
    } else if (!is_error()) {
        other = BValueMap::Create(1);
        if (other) {
            if (is_specified()) other->setFirstMap(SValue::Wild(), *this);
            freeData();
            m_type = kMapTypeCode;
            *map = other;
        } else {
            set_error(NO_MEMORY);
            map = NULL;
        }
    } else {
        map = NULL;
    }
    return map;
}

BValueMap** SValue::make_map_without_sets()
{
    BValueMap **map = NULL;

    if (!is_map()) undefine();
    else {
        size_t N = m_data.map->countMaps();
        size_t i = 0;
        while (i < N) {
            if (m_data.map->mapAt(i).key.isWild()) {
                if (!map) map = edit_map();
                if (!map) break;
                BValueMap::removeMapAt(map, i);
                N--;
            } else {
                i++;
            }
        }
    }

    return map;
}

status_t SValue::set_error(ssize_t code)
{
    freeData();
    m_type = kErrorTypeCode;
    m_data.integer = code < OK ? code : UNKNOWN_ERROR;
    CHECK_INTEGRITY(*this);
    return m_data.integer;
}

#if DB_INTEGRITY_CHECKS
static int32_t gMallocDebugLevel = -1;

static void ReadMallocDebugLevel();
static inline int32_t MallocDebugLevel()
{
    if (gMallocDebugLevel < 0) ReadMallocDebugLevel();
    return gMallocDebugLevel;
}

static void ReadMallocDebugLevel()
{
    char envBuffer[128];
    const char* env = getenv("MALLOC_DEBUG");
    if (env) {
        gMallocDebugLevel = atoi(env);
        if (gMallocDebugLevel < 0)
            gMallocDebugLevel = 0;
    } else {
        gMallocDebugLevel = 0;
    }
}
#endif

bool SValue::check_integrity() const
{
    bool passed = true;

#if DB_INTEGRITY_CHECKS
#if !VALIDATES_VALUE
    if (MallocDebugLevel() <= 0) return passed;
#endif

    if ((m_type&(B_TYPE_BYTEORDER_MASK)) != B_TYPE_BYTEORDER_NORMAL && m_type != kUndefinedTypeCode) {
        LOG_ALWAYS_FATAL("SValue: byte order bits are incorrect");
        passed = false;
    }

    if ((m_type&~(B_TYPE_BYTEORDER_MASK|B_TYPE_LENGTH_MASK|B_TYPE_CODE_MASK)) != 0) {
        LOG_ALWAYS_FATAL("SValue: unnused type bits are non-zero");
        passed = false;
    }

    switch (B_UNPACK_TYPE_CODE(m_type)) {
        case B_UNDEFINED_TYPE:
        case B_WILD_TYPE:
        case B_NULL_TYPE:
            if (B_UNPACK_TYPE_LENGTH(m_type) != 0) {
                LOG_ALWAYS_FATAL("SValue: undefined, wild, or null value with non-zero length");
                passed = false;
            }
            break;

        case B_VALUE_TYPE:
            if (B_UNPACK_TYPE_LENGTH(m_type) != B_TYPE_LENGTH_MAP) {
                LOG_ALWAYS_FATAL("SValue: value map type not using B_TYPE_LENGTH_MAP");
                passed = false;
            }
            break;

        case B_ERROR_TYPE:
            if (B_UNPACK_TYPE_LENGTH(m_type) != sizeof(status_t)) {
                LOG_ALWAYS_FATAL("SValue: error value with wrong length");
                passed = false;
            }
            break;
    }

    switch (B_UNPACK_TYPE_LENGTH(m_type)) {
        case B_TYPE_LENGTH_LARGE:
        {
            if (m_data.buffer == NULL) {
                LOG_ALWAYS_FATAL("SValue: length is B_TYPE_LENGTH_LARGE, but m_data.buffer is NULL");
                passed = false;
            }
            else if (m_data.buffer->size() <= B_TYPE_LENGTH_MAX) {
                LOG_ALWAYS_FATAL("SValue: length is B_TYPE_LENGTH_LARGE, but buffer is <= B_TYPE_LENGTH_MAX");
                passed = false;
            } else if (m_data.buffer->Users() < 0) {
                LOG_ALWAYS_FATAL("SValue: shared buffer has negative users; maybe it was deallocated?");
                passed = false;
            }
        } break;

        case B_TYPE_LENGTH_MAP:
        {
            if (B_UNPACK_TYPE_CODE(m_type) != B_VALUE_TYPE) {
                LOG_ALWAYS_FATAL("SValue: B_TYPE_LENGTH_MAP used with wrong type code");
                passed = false;
            } else if (m_data.map == NULL) {
                LOG_ALWAYS_FATAL("SValue: contains a map, but m_data.map is NULL");
                passed = false;
            } else {
                if (m_data.map->countMaps() <= 0) {
                    LOG_ALWAYS_FATAL("SValue: contains a BValueMap of no items");
                    passed = false;
                } else if (m_data.map->countMaps() == 1) {
                    const BValueMap::pair& p = m_data.map->mapAt(0);
                    if (p.key == SValue::Wild()) {
                        LOG_ALWAYS_FATAL("SValue: contains a redundant BValueMap");
                        passed = false;
                    }
                } else if (m_data.map->countMaps() > 0x7fffffff) {
                    LOG_ALWAYS_FATAL("SValue: contains a huge BValueMap; maybe it was deallocated?");
                    passed = false;
                }
            }
        } break;
    }
#endif

    return passed;
}

const SValue& SValue::replaceValues(const SValue* newValues, const size_t* indices, size_t count)
{
    if (!is_error() && is_map())
    {
        BValueMap **map=edit_map();
        if (map != NULL) {
            size_t i, mapCount = (*map)->countMaps();
            for (i=0; i<count; i++) {
                const size_t idx(indices[i]);
                if (idx < mapCount) {
                    // this can't use BeginEditMap / EndEditMap since that normalizes the undefined
                    // values, and doing that will screw up our indices.
                    BValueMap::pair& p = (BValueMap::pair&)(*map)->mapAt(idx);
                    p.value = newValues[i];
                    LOG_FATAL_IF(p.value.isDefined() == false, "ReplaceValues cannot be used with undefined values");
                }
            }
        }
    }

    CHECK_INTEGRITY(*this);
    return *this;
}

void SValue::getKeyIndices(const SValue* keys, size_t* outIndices, size_t count)
{
    if (!is_error() && is_map())
    {
        for (size_t i = 0 ; i < count ; i++)
        {
            outIndices[i] = m_data.map->indexFor(keys[i]);
        }
    }
    else
    {
        for (size_t i = 0 ; i < count ; i++)
        {
            outIndices[i] = ~0;
        }
    }
}

// -----------------------------------------------------------------
// Specializations for marshalling SVector<SValue>
// -----------------------------------------------------------------

SValue BArrayAsValue(const SValue* from, size_t count)
{
    SValue result;
    for (size_t i = 0; i < count; i++) {
        result.joinItem(SSimpleValue<int32_t>(i), *from);
        from++;
    }
    return result;
}

status_t BArrayConstruct(SValue* to, const SValue& value, size_t count)
{
    for (size_t i = 0; i < count; i++) {
    *to = value[SSimpleValue<int32_t>(i)];
        to++;
    }
    return OK;
}

#if _SUPPORTS_NAMESPACE
} }	// namespace palmos::support
#endif
