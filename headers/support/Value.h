#include "msgpack11.hpp"
#include <binder/Parcelable.h>

namespace os { namespace support {

class Value final : public ::msgpack11::MsgPack, public Parcelable {
    // Archiving.
    status_t writeToParcel(Parcel* parcel) const override;
    status_t readFromParcel(const Parcel* parcel) override;
}

} } // namespace os::support
