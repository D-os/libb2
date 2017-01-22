#include <support/Value.h>

#include <binder/ProcessState.h>

namespace os { namespace support {

/* * * * * * * * * * * * * * * * * * * *
 * Type Traits
 */

const size_t ValueTypeTraits<flat_binder_object>::extension_id = 1;
const flat_binder_object ValueTypeTraits<flat_binder_object>::empty_value = {};

Value::Value(const sp<IBinder> &binder) {
    m_ptr = nullptr;
    flat_binder_object flat;
    ::android::flatten_binder(::android::ProcessState::self(), binder, &flat);
//    m_ptr = make_shared<MsgPackExtension>(flat);
    Value val(flat);
}

template<>
bool Value::is<void>() const { return is_null(); }

//template<>
//int Value::as<int>() const { return int_value(); };

template<>
sp<IBinder> Value::as<sp<IBinder>>() const {
    const flat_binder_object& flat = as<flat_binder_object>();

    sp<IBinder> out;
    status_t status = ::android::unflatten_binder(::android::ProcessState::self(), flat, &out);
    if (status == OK)
        return out;

    return sp<IBinder>();
}

template <typename T>
T Value::as() const {
    if (is_extension()) {
        auto ext = extension_items();
        if (ValueTypeTraits<T>::extension_id == std::get<0>(ext)) {
            return *reinterpret_cast<T*>(std::get<1>(ext).data());
        }
    }

    return ValueTypeTraits<T>::empty_value;
}


/* * * * * * * * * * * * * * * * * * * *
 * Archiving
 */

status_t Value::writeToParcel(Parcel* parcel) const {
    std::string out;
    status_t status = dump(out);
    return status != OK ?
        status :
        parcel->writeByteArray(out.length(), reinterpret_cast<const uint8_t *>(out.data()));
}

status_t Value::readFromParcel(const Parcel* parcel) {
    std::vector<uint8_t> val;
    status_t status = parcel->readByteVector(&val);
    if (status != OK)
        return status;
    if (val.empty())
        return NOT_ENOUGH_DATA;
    std::string str(reinterpret_cast<const char *>(val.data()), val.size());
    Value parsed = parse(str, status);
    if (status != OK)
        return status;
    *this = parsed;
    return OK;
}

} } // namespace os::support
