#ifndef	_SUPPORT_ISTORAGE_INTERFACE_H
#define	_SUPPORT_ISTORAGE_INTERFACE_H

/*!	@file support/IStorage.h
    @ingroup CoreSupportDataModel
    @brief Random-access interface to a block of raw data.
*/

#include <binder/IBinder.h>
#include <binder/IInterface.h>
#include <binder/Status.h>
#include <cstdint>
#include <sys/uio.h>
#include <support/Value.h>
#include <utils/Errors.h>
#include <utils/String.h>
#include <utils/StrongPointer.h>

namespace os {
namespace support {

/*!	@addtogroup CoreSupportDataModel
    @{
*/

/*-------------------------------------------------------------*/
/*------- IStorage Class --------------------------------------*/

//! Raw storage read and write transaction codes.
enum {
    B_WRITE_AT_TRANSACTION		= 'WRAT',
    B_READ_AT_TRANSACTION		= 'RDAT'
};

//! This is the contents of a B_READ_AT_TRANSACTION parcel.
/*!	The reply parcel contains a ssize_t followed by the raw data. */
struct read_at_transaction
{
    off_t	position;
    size_t	size;
};

//!	This is the front of a B_WRITE_AT_TRANSACTION parcel; it
//!	is immediately followed by the data to be written.
/*!	The reply parcel contains a single ssize_t indicating the
    number of bytes actually written, or an error code.
*/
struct write_at_transaction
{
    off_t	position;
};

//!	Random-access interface to a block of raw data.
class IStorage : public ::android::IInterface {
    public:

        DECLARE_META_INTERFACE(Storage);

                //!	Return the total number of bytes in the store.
        virtual	off_t		size() const = 0;
                //!	Set the total number of bytes in the store.
        virtual	status_t	setSize(off_t size) = 0;

                //!	Read the bytes described by \a iovec from location \a position in the storage.
                /*!	Returns the number of bytes actually read, or a
                    negative error code. */
        virtual	ssize_t		readAtV(off_t position, const struct iovec *vector, ssize_t count) = 0;

                //!	Convenience for reading a vector of one buffer.
                ssize_t		readAt(off_t position, void* buffer, size_t size);

                //!	Write the bytes described by \a iovec at location \a position in the storage.
                /*!	Returns the number of bytes actually written,
                    or a negative error code. */
        virtual	ssize_t		writeAtV(off_t position, const struct iovec *vector, ssize_t count) = 0;

                //!	Convenience for reading a vector of one buffer.
                ssize_t		writeAt(off_t position, const void* buffer, size_t size);

                //!	Make sure all data in the storage is written to its physical device.
                /*!	Returns B_OK if the data is safely stored away, else
                    an error code. */
        virtual	status_t	sync() = 0;

       enum Call {
           SIZE = ::android::IBinder::FIRST_CALL_TRANSACTION + 0,
           SETSIZE = ::android::IBinder::FIRST_CALL_TRANSACTION + 1,
           READATV = ::android::IBinder::FIRST_CALL_TRANSACTION + 2,
           WRITEATV = ::android::IBinder::FIRST_CALL_TRANSACTION + 3,
           SYNC = ::android::IBinder::FIRST_CALL_TRANSACTION + 4,
       };
};

/*-----------------------------------------------------------------*/

/*!	@} */

class BnStorage : public ::android::BnInterface<IStorage>
{
public:
        virtual	status_t		onTransact(uint32_t code, const ::android::Parcel& data, ::android::Parcel* reply, uint32_t flags = 0) override;

protected:
        inline					BnStorage() : BnInterface<IStorage>() { }
        inline virtual			~BnStorage() { }

//		virtual	status_t		HandleEffect(const SValue &in, const SValue &inBindings, const SValue &outBindings, SValue *out);

private:
                                BnStorage(const BnStorage& o);	// no implementation
        BnStorage&				operator=(const BnStorage& o);	// no implementation
};

/*-----------------------------------------------------------------*/

/*!	@} */

class BpStorage : public ::android::BpInterface<IStorage> {
public:
explicit BpStorage(const ::android::sp<::android::IBinder>& _aidl_impl);
virtual ~BpStorage() = default;
off_t		size() const override;
status_t	setSize(off_t size) override;
ssize_t		readAtV(off_t position, const struct iovec *vector, ssize_t count) override;
ssize_t		writeAtV(off_t position, const struct iovec *vector, ssize_t count) override;
status_t	sync() override;
};

/*-------------------------------------------------------------*/
/*---- No user serviceable parts after this -------------------*/

inline ssize_t IStorage::readAt(off_t position, void *buffer, size_t size)
{
    iovec v;
    v.iov_base = buffer;
    v.iov_len = size;
    return readAtV(position, &v,1);
}

inline ssize_t IStorage::writeAt(off_t position, const void *buffer, size_t size)
{
    iovec v;
    v.iov_base = const_cast<void*>(buffer);
    v.iov_len = size;
    return writeAtV(position, &v,1);
}

/*--------------------------------------------------------------*/

} } // namespace os::support

#endif	// _SUPPORT_ISTORAGE_INTERFACE_H
