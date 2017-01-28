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

#ifndef _STORAGE_STREAMDATUM_H
#define _STORAGE_STREAMDATUM_H

/*!	@file storage/StreamDatum.h
    @ingroup CoreSupportDataModel
    @brief Base implementation of IDatum from stream-like data operations.
*/

#include <storage/GenericDatum.h>

#include <os/support/BnByteInput.h>
#include <os/support/BnByteOutput.h>
#include <os/support/BnByteSeekable.h>
#include <support/IStorage.h>
#include <utils/Mutex.h>

namespace os {
namespace storage {

/*!	@addtogroup CoreSupportDataModel
    @{
*/

// ==========================================================================
// ==========================================================================

//!	Base implementation of IDatum from stream-like data operations.
/*!	This is the standard base implementation of IDatum.  It takes care
    of all the IDatum APIs, introducing new higher-level virtuals
    that derived classes can then implement.  In particular, it introduces
    a locking regimen to the class, and a standard implementation of
    Open() that generates stream objects which call back on to
    StreamDatum for their implementation.

    There are two general approaches derived classes can take to
    create a concrete class: using a stream model or a memory model.

    To use a stream model, you will need to implement at least the
    virtuals StoreSizeLocked(), StoreLocked(), ReadLocked(), and WriteLocked().
    You may also implement other virtuals to provide additional
    functionality (such as StoreValueTypeLocked()) or a more optimal
    implementation (such as StoreValueLocked()).

    To use a memory model, you will need to implement at least the
    virtuals StoreSizeLocked(), SizeLocked(), StartReadingLocked(),
    FinishReadingLocked(), StartWritingLocked(), and FinishWritingLocked().
    You will also almost always be able to provide a better implementation
    of StoreValueLocked() and ValueLocked(), so should do so.

    @nosubgrouping
*/
class StreamDatum : public GenericDatum
{
public:
    // --------------------------------------------------------------
    /*!	@name Bookkeeping
        Creation, destruction, locking, etc. */
    //@{
    StreamDatum(uint32_t mode = IDatum::READ_WRITE);
    StreamDatum(const Context& context, uint32_t mode = IDatum::READ_WRITE);

protected:
    virtual ~StreamDatum();

public:
    //!	Return a new mode value based on a requested mode and desired read/write limitations.
    static uint32_t restrictMode(uint32_t requested, uint32_t limit);

    //!	Return the mode flags the class was instantiated with.
    uint32_t mode() const;

    //!	Lock the datum's state.
    /*!	To keep itself consistent, this class provides a public lock that
                is used when doing read/write operations on the class state as well
                as the actual data.  The default implemention of this method acquires
                an internal lock.  You can override it to cause all internal
                implementation to acquire some other lock.  Be sure to also override
                Unlock() if doing so. */
    virtual lock_status_t lock() const;

    //!	Unlock the datum's state.
    virtual void unlock() const;

    //@}

    //  ------------------------------------------------------------------
    /*!	@name IDatum Implementation
        This class provides a default implementation of the IDatum API.  You will need
        to fill in the new pure virtuals it has to have a concrete implementation. */
    //@{

    //!	Acquires the datum lock and calls ValueTypeLocked().
    virtual uint32_t valueType() const;
    //!	Acquires the datum lock and calls SetValueTypeLocked().
    virtual void setValueType(uint32_t type);
    //!	Acquires the datum lock and calls SizeLocked().
    virtual off_t size() const;
    //!	Acquires the datum lock and calls SetSizeLocked().
    virtual void setSize(off_t size);
    //!	Acquires the datum lock and calls ValueLocked().
    virtual Value value() const;
    //!	Acquires the datum lock and calls SetValueLocked().
    virtual void setValue(const Value& value);
    //!	Creates a new Stream object on the datum.
    virtual Status open(int32_t mode, const sp<IBinder>& editor, int32_t newType, sp<IBinder>* _aidl_return) = 0;

    //!	Uses StoreValueTypeLocked() to change the type and calls ReportChangeLocked() as appropriate.
    status_t setValueTypeLocked(uint32_t type);
    //!	Uses StoreSizeLocked() to change the size and calls ReportChangeLocked() as appropriate.
    status_t setSizeLocked(off_t size);
    //!	Uses StoreValueLocked() to change the value and calls ReportChangeLocked() as appropriate.
    status_t setValueLocked(const Value& value);
    //@}

    class Stream;

    //  ------------------------------------------------------------------
    /*!	@name New IDatum Virtuals
        Subclasses must implement SizeLocked() and StoreSizeLocked() as part
        of the basic protocol.  You can override the other virtuals to
        customize/optimize how values are returned.  You don't need to do your
        own locking, since these are called with the datum lock held.  You
        also don't need to worry about pushing properties or events as
        a result of changes, which will also be done for you by this
        class. */
    //@{

    //!	The default implementation returns B_RAW_TYPE.
    virtual uint32_t valueTypeLocked() const;
    //!	The default implementation does nothing, returning B_UNSUPPORTED.
    virtual status_t storeValueTypeLocked(uint32_t type);
    //!	Derived classes must implement to return the total amount of data available.
    virtual off_t sizeLocked() const = 0;
    //!	Derived classes must implement to set the total amount of data.
    virtual status_t storeSizeLocked(off_t size) = 0;
    //!	The default implementation generates a new Value from the datum.
    /*!	The Value is created by calling ValueTypeLocked(), SizeLocked(), and
                ReadLocked().  This is fairly inefficient, so you will want to provide
                your own implementation if possible.  It will not attempt to return
                the datum as an Value if the datum is larger than a reasonable size
                (a couple kilobytes). */
    virtual Value valueLocked() const;
    //!	The default implementation changes the contents of the datum to the value.
    /*!	The datum is changed by calling StoreValueTypeLocked(), StoreSizeLocked(),
                and StoreLocked().  This is fairly inefficient, so you will want to
                provide your own implementation if possible. */
    virtual status_t storeValueLocked(const Value& value);

    enum {
        TYPE_CHANGED = 0x0001, //!< Use with ReportChangeLocked() when the type property has changed.
        SIZE_CHANGED = 0x0002, //!< Use with ReportChangeLocked() when the size property has changed.
        DATA_CHANGED = 0x0004  //!< Use with ReportChangeLocked() when the data contents have changed.
    };

    //!	Called when any part of the datum (data, type, size) changes.
    /*!	The default implementation pushes the appropriate properties
                and events based on the @a changes flags, which may be any
                of TYPE_CHANGED, SIZE_CHANGED, or DATA_CHANGED.  The value
                property is always pushed.  For DATA_CHANGED, @a start and
                @a length must be supplied with the range of data that
                changed; for all other changes, ValueLocked(), ValueTypeLocked(),
                and SizeLocked() are called as needed to retrieve the
                current values.

                You can override this method in your own subclass to easily
                discover when changes happen.  If you do so, be sure to also
                call through to this implementation. */
    virtual void reportChangeLocked(const sp<IBinder>& editor, uint32_t changes, off_t start = -1, off_t length = -1);

    //@}

    //  ------------------------------------------------------------------
    /*!	@name Raw Stream Virtuals
        These virtuals allow you to implement raw byte stream operations
        on the datum.  These are used to implement stream-style datums,
        such as one representing a file on a filesystem.  The default
        implementation translates these into the memory-style operations
        StartReadingLocked(), FinishReadingLocked(), StartWritingLocked(),
        and FinishWritingLocked().

        One case where you may need to override these instead is to implement
        datums that can block when reading or writing.  In that case you
        can temporarily unlock the datum while inside these functions (need
        to check if the implementation here will actually be happy with
        that).

        @note These functions will be called with stream set to NULL when
        being used by the default implementation of ValueLocked() and
        StoreValueLocked(). */
    //@{

    //!	Retrieve bytes from the datum and place them into the iovec.
    virtual ssize_t readLocked(const sp<Stream>& stream, off_t position,
                               const struct iovec* vector, ssize_t count, uint32_t flags) const;
    //!	Write bytes from the iovec into the datum.
    virtual ssize_t writeLocked(const sp<Stream>& stream, off_t position,
                                const struct iovec* vector, ssize_t count, uint32_t flags);
    //!	Block until all data has been written to storage.
    virtual status_t syncLocked();

    //@}

    //  ------------------------------------------------------------------
    /*!	@name Stream Virtuals
        These virtuals allow you to implement memory operations on the datum.
        Return a buffer of data from the Start functions, which will be
        read or written as needed; you can do whatever cleanup is needed
        after that in the Finish functions.  Note that StartWritingLocked()
        (or FinishWritingLocked()) MUST take care of the B_WRITE_END flag
        for truncating the datum size.

        You are guaranteed that the datum will not be unlocked between the
        start and end calls.

        @note These functions will be called with stream set to NULL when
        being used by the default implementation of ValueLocked() and
        StoreValueLocked(). */
    //@{

    //!	The default implementation returns NULL.
    virtual const void* startReadingLocked(const sp<Stream>& stream, off_t position,
                                           ssize_t* inoutSize, uint32_t flags) const;
    //!	The default implementation does nothing.
    virtual void finishReadingLocked(const sp<Stream>& stream, const void* data) const;
    //!	The default implementation returns NULL.
    virtual void* startWritingLocked(const sp<Stream>& stream, off_t position,
                                     ssize_t* inoutSize, uint32_t flags);
    //!	The default implementation does nothing.
    virtual void finishWritingLocked(const sp<Stream>& stream, void* data);

    //@}

protected:
private:
    StreamDatum(const StreamDatum&);
    StreamDatum& operator=(const StreamDatum&);

    friend class Stream;

    ssize_t write_wrapper_l(const sp<Stream>& stream, off_t position,
                            const struct iovec* vector, ssize_t count, uint32_t flags);

    mutable Mutex  m_lock;
    const uint32_t m_mode;
    SortedVector<wp<Stream>>
        m_streams;
};

// --------------------------------------------------------------------------

//!	An open stream on StreamDatum.
/*!
    @nosubgrouping
*/
class StreamDatum::Stream : public BnByteInput, public BnByteOutput, public BnByteSeekable, public BnStorage
{
public:
    // --------------------------------------------------------------
    /*!	@name Bookkeeping
        Creation, destruction, etc. */
    //@{
    Stream(const Context& context, const sp<StreamDatum>& datum,
           uint32_t mode, uint32_t type, const sp<IBinder>& editor);

    //!	Return the available stream interfaces based on the open mode.
    virtual Value Inspect(const sp<IBinder>& caller, const Value& which, uint32_t flags = 0);

protected:
    virtual ~Stream();
    //!	Attach to the StreamDatum and set type code / erase as requested.
    virtual void InitAtom();
    //!	The destructor must call Lock(), so return FINISH_ATOM_ASYNC to avoid deadlocks.
    virtual status_t FinishAtom(const void* id);

public:
    //@}

    // --------------------------------------------------------------
    /*!	@name IByteInput/IByteOutput/IByteSeekable */
    //@{

    virtual ssize_t readV(const struct iovec* vector, ssize_t count, uint32_t flags = 0);
    virtual ssize_t writeV(const struct iovec* vector, ssize_t count, uint32_t flags = 0);
    virtual status_t sync();
    virtual off_t seek(off_t position, uint32_t seek_mode);
    virtual off_t position() const;

    //@}

    // --------------------------------------------------------------
    /*!	@name IStorage */
    //@{

    virtual off_t    size() const;
    virtual status_t setSize(off_t size);
    virtual ssize_t readAtV(off_t position, const struct iovec* vector, ssize_t count);
    virtual ssize_t writeAtV(off_t position, const struct iovec* vector, ssize_t count);

    //@}

    // --------------------------------------------------------------
    /*!	@name Access Stream Information */
    //@{

    //!	Datum the stream is from.  Doesn't acquire the lock.
    sp<StreamDatum> datum() const;
    //!	Open mode of the stream.  Doesn't acquire the lock.
    uint32_t mode() const;
    //!	Open type code of the stream.  Doesn't acquire the lock.
    uint32_t type() const;
    //!	Editor object associated with the stream.  Doesn't acquire the lock.
    sp<IBinder> editor() const;
    //!	Default stream IBinder (IByteInput or IByteOutput) to return.  Doesn't acquire the lock.
    sp<IBinder> defaultInterface() const;

    //@}

private:
    Stream(const Stream&);

    const sp<StreamDatum> m_datum;
    const uint32_t        m_mode;
    const uint32_t        m_type;
    const sp<IBinder>     m_editor;
    off_t                 m_pos;
};

// ==========================================================================
// ==========================================================================

/*!	@} */
}
} // namespace os::storage

#endif // _STORAGE_STREAMDATUM_H
