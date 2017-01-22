#ifndef _SUPPORT_VALUE_H
#define _SUPPORT_VALUE_H

#include "msgpack11.hpp"
#include <binder/Parcelable.h>
#include <binder/Parcel.h>
#include <support/String.h>

namespace os { namespace support {

template <typename>
struct ValueTypeTraits;

template<> struct ValueTypeTraits<flat_binder_object> {
    static const size_t extension_id;
    static const flat_binder_object empty_value;
};

class Value final : public ::msgpack11::MsgPack, public Parcelable {

    using MsgPack::MsgPack;

    public:
    Value() noexcept    { m_ptr = nullptr; }    // invalid

    Value(const MsgPack &mp) : MsgPack(mp) {}
    Value(const sp<IBinder> &binder);

    // Implicit constructor: extension objects - anything with a extension_id type trait.
    template <class E, typename std::enable_if<bool(ValueTypeTraits<E>::extension_id), int>::type = 0>
    Value(const E & e) : MsgPack(extension(ValueTypeTraits<E>::extension_id, binary((const char *)&e, (const char *)&e + sizeof(e)))) {}

    bool valid()        const { return m_ptr != nullptr; }

    template <typename T>
    bool is()           const;

    template <typename T>
    T as() const;

    const Value & operator[](const String &key) const {
        return static_cast<const Value&>(MsgPack::operator[](key.string()));
    }

    // Archiving.
    status_t writeToParcel(Parcel* parcel) const override;
    status_t readFromParcel(const Parcel* parcel) override;
};

} } // namespace os::support
#endif
