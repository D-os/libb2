/*
 * Copyright (C) 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ANDROID_PERSISTABLE_BUNDLE_H
#define ANDROID_PERSISTABLE_BUNDLE_H

#include <map>
#include <vector>

#include <binder/Parcelable.h>
#include <utils/String.h>
#include <utils/StrongPointer.h>

namespace android {

namespace os {

/*
 * C++ implementation of PersistableBundle, a mapping from String values to
 * various types that can be saved to persistent and later restored.
 */
class PersistableBundle : public Parcelable {
public:
    PersistableBundle() = default;
    virtual ~PersistableBundle() = default;
    PersistableBundle(const PersistableBundle& bundle) = default;

    status_t writeToParcel(Parcel* parcel) const override;
    status_t readFromParcel(const Parcel* parcel) override;

    bool empty() const;
    size_t size() const;
    size_t erase(const String& key);

    /*
     * Setters for PersistableBundle. Adds a a key-value pair instantiated with
     * |key| and |value| into the member map appropriate for the type of |value|.
     * If there is already an existing value for |key|, |value| will replace it.
     */
    void putBoolean(const String& key, bool value);
    void putInt(const String& key, int32_t value);
    void putLong(const String& key, int64_t value);
    void putDouble(const String& key, double value);
    void putString(const String& key, const String& value);
    void putBooleanVector(const String& key, const std::vector<bool>& value);
    void putIntVector(const String& key, const std::vector<int32_t>& value);
    void putLongVector(const String& key, const std::vector<int64_t>& value);
    void putDoubleVector(const String& key, const std::vector<double>& value);
    void putStringVector(const String& key, const std::vector<String>& value);
    void putPersistableBundle(const String& key, const PersistableBundle& value);

    /*
     * Getters for PersistableBundle. If |key| exists, these methods write the
     * value associated with |key| into |out|, and return true. Otherwise, these
     * methods return false.
     */
    bool getBoolean(const String& key, bool* out) const;
    bool getInt(const String& key, int32_t* out) const;
    bool getLong(const String& key, int64_t* out) const;
    bool getDouble(const String& key, double* out) const;
    bool getString(const String& key, String* out) const;
    bool getBooleanVector(const String& key, std::vector<bool>* out) const;
    bool getIntVector(const String& key, std::vector<int32_t>* out) const;
    bool getLongVector(const String& key, std::vector<int64_t>* out) const;
    bool getDoubleVector(const String& key, std::vector<double>* out) const;
    bool getStringVector(const String& key, std::vector<String>* out) const;
    bool getPersistableBundle(const String& key, PersistableBundle* out) const;

    friend bool operator==(const PersistableBundle& lhs, const PersistableBundle& rhs) {
        return (lhs.mBoolMap == rhs.mBoolMap && lhs.mIntMap == rhs.mIntMap &&
                lhs.mLongMap == rhs.mLongMap && lhs.mDoubleMap == rhs.mDoubleMap &&
                lhs.mStringMap == rhs.mStringMap && lhs.mBoolVectorMap == rhs.mBoolVectorMap &&
                lhs.mIntVectorMap == rhs.mIntVectorMap &&
                lhs.mLongVectorMap == rhs.mLongVectorMap &&
                lhs.mDoubleVectorMap == rhs.mDoubleVectorMap &&
                lhs.mStringVectorMap == rhs.mStringVectorMap &&
                lhs.mPersistableBundleMap == rhs.mPersistableBundleMap);
    }

    friend bool operator!=(const PersistableBundle& lhs, const PersistableBundle& rhs) {
        return !(lhs == rhs);
    }

private:
    status_t writeToParcelInner(Parcel* parcel) const;
    status_t readFromParcelInner(const Parcel* parcel, size_t length);

    std::map<String, bool> mBoolMap;
    std::map<String, int32_t> mIntMap;
    std::map<String, int64_t> mLongMap;
    std::map<String, double> mDoubleMap;
    std::map<String, String> mStringMap;
    std::map<String, std::vector<bool>> mBoolVectorMap;
    std::map<String, std::vector<int32_t>> mIntVectorMap;
    std::map<String, std::vector<int64_t>> mLongVectorMap;
    std::map<String, std::vector<double>> mDoubleVectorMap;
    std::map<String, std::vector<String>> mStringVectorMap;
    std::map<String, PersistableBundle> mPersistableBundleMap;
};

}  // namespace os

}  // namespace android

#endif  // ANDROID_PERSISTABLE_BUNDLE_H
