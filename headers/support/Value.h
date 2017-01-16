/* Value (Variant) implementation based on msgpack11 by Masahiro Wada
 * https://github.com/ar90n/msgpack11 */
#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <initializer_list>

#include <support/SupportDefs.h>

namespace os { namespace support {

class MsgPackValue;

class Value final {
public:
    // Types
    enum Type {
        UNDEF,
        NUL,
        BOOL,
        FLOAT32,
        FLOAT64,
        INT8,
        INT16,
        INT32,
        INT64,
        UINT8,
        UINT16,
        UINT32,
        UINT64,
        STRING,
        BINARY,
        ARRAY,
        OBJECT,
        EXTENSION
    };

    // Array and object typedefs
    typedef std::vector<Value> array;
    typedef std::map<Value, Value> object;

    // Binary and extension typedefs
    typedef std::vector<uint8_t> binary;
    typedef std::tuple<int8_t, binary> extension;

    // Constructors for the various types of JSON value.
    Value() noexcept;                // UNDEF
    Value(std::nullptr_t) noexcept;  // NUL
    Value(float value);              // FLOAT32
    Value(double value);             // FLOAT64
    Value(int8_t value);             // INT8
    Value(int16_t value);            // INT16
    Value(int32_t value);            // INT32
    Value(int64_t value);            // INT64
    Value(uint8_t value);            // UINT8
    Value(uint16_t value);           // UINT16
    Value(uint32_t value);           // UINT32
    Value(uint64_t value);           // UINT64
    Value(bool value);               // BOOL
    Value(const std::string &value); // STRING
    Value(std::string &&value);      // STRING
    Value(const char * value);       // STRING
    Value(const array &values);      // ARRAY
    Value(array &&values);           // ARRAY
    Value(const object &values);     // OBJECT
    Value(object &&values);          // OBJECT
    Value(const binary &values);     // BINARY
    Value(binary &&values);          // BINARY
    Value(const extension &values);  // EXTENSION
    Value(extension &&values);       // EXTENSION

    // Implicit constructor: anything with a to_value() function.
    template <class T, class = decltype(&T::to_value)>
    Value(const T & t) : Value(t.to_value()) {}

    // Implicit constructor: map-like objects (std::map, std::unordered_map, etc)
    template <class M, typename std::enable_if<
        std::is_constructible<Value, typename M::key_type>::value
        && std::is_constructible<Value, typename M::mapped_type>::value,
            int>::type = 0>
    Value(const M & m) : Value(object(m.begin(), m.end())) {}

    // Implicit constructor: vector-like objects (std::list, std::vector, std::set, etc)
    template <class V, typename std::enable_if<
        std::is_constructible<Value, typename V::value_type>::value &&
        !std::is_same<typename binary::value_type, typename V::value_type>::value,
            int>::type = 0>
    Value(const V & v) : Value(array(v.begin(), v.end())) {}

    template <class V, typename std::enable_if<
        std::is_constructible<Value, typename V::value_type>::value &&
        std::is_same<typename binary::value_type, typename V::value_type>::value,
            int>::type = 0>
    Value(const V & v) : Value(binary(v.begin(), v.end())) {}

    // This prevents Value(some_pointer) from accidentally producing a bool. Use
    // Value(bool(some_pointer)) if that behavior is desired.
    Value(void *) = delete;

    // Accessors
    Type type() const;

    bool is_valid()     const { return type() != UNDEF; }
    bool is_undefined() const { return type() == UNDEF; }
    bool is_null()      const { return type() == NUL; }
    bool is_bool()      const { return type() == BOOL; }
    bool is_number()    const { return type() == FLOAT64; }
    bool is_float32()   const { return type() == FLOAT32; }
    bool is_float64()   const { return type() == FLOAT64; }
    bool is_int()       const { return type() == INT32; }
    bool is_int8()      const { return type() == INT8; }
    bool is_int16()     const { return type() == INT16; }
    bool is_int32()     const { return type() == INT32; }
    bool is_int64()     const { return type() == INT64; }
    bool is_uint8()     const { return type() == UINT8; }
    bool is_uint16()    const { return type() == UINT16; }
    bool is_uint32()    const { return type() == UINT32; }
    bool is_uint64()    const { return type() == UINT64; }
    bool is_string()    const { return type() == STRING; }
    bool is_array()     const { return type() == ARRAY; }
    bool is_binary()    const { return type() == BINARY; }
    bool is_object()    const { return type() == OBJECT; }
    bool is_extension() const { return type() == EXTENSION; }

    // Return the enclosed value if this is a number, 0 otherwise. Note that msgpack11 does not
    // distinguish between integer and non-integer numbers - number_value() and int_value()
    // can both be applied to a NUMBER-typed object.
    double number_value() const;
    float float32_value() const;
    double float64_value() const;
    int32_t int_value() const;
    int8_t int8_value() const;
    int16_t int16_value() const;
    int32_t int32_value() const;
    int64_t int64_value() const;
    uint8_t uint8_value() const;
    uint16_t uint16_value() const;
    uint32_t uint32_value() const;
    uint64_t uint64_value() const;

    // Return the enclosed value if this is a boolean, false otherwise.
    bool bool_value() const;
    // Return the enclosed string if this is a string, "" otherwise.
    const std::string &string_value() const;
    // Return the enclosed std::vector if this is an array, or an empty vector otherwise.
    const array &array_items() const;
    // Return the enclosed std::map if this is an object, or an empty map otherwise.
    const object &object_items() const;
    // Return the enclosed std::vector if this is an binary, or an empty map otherwise.
    const binary &binary_items() const;
    // Return the enclosed std::tuple if this is an extension, or an empty map otherwise.
    const extension &extension_items() const;

    // Return a reference to arr[i] if this is an array, Value() otherwise.
    const Value & operator[](size_t i) const;
    // Return a reference to obj[key] if this is an object, Value() otherwise.
    const Value & operator[](const std::string &key) const;

    // Serialize.
    status_t dump(std::string &out) const;
    std::string dump() const {
        std::string out;
        dump(out);
        return out;
    }

    // Parse. If parse fails, return Value() and assign an error to err.
    static Value parse(const std::string & in, status_t & err);
    static Value parse(const char * in, size_t len, status_t & err) {
        if (in) {
            return parse(std::string(in,in+len), err);
        } else {
            err = UNEXPECTED_NULL;
            return nullptr;
        }
    }
    // Parse multiple objects, concatenated or separated by whitespace
    static std::vector<Value> parse_multi(const std::string & in,
        std::string::size_type & parser_stop_pos,
        status_t &err);

    static inline std::vector<Value> parse_multi(
        const std::string & in,
        status_t & err) {
        std::string::size_type parser_stop_pos;
        return parse_multi(in, parser_stop_pos, err);
    }

    bool operator== (const Value &rhs) const;
    bool operator<  (const Value &rhs) const;
    bool operator!= (const Value &rhs) const { return !(*this == rhs); }
    bool operator<= (const Value &rhs) const { return !(rhs < *this); }
    bool operator>  (const Value &rhs) const { return  (rhs < *this); }
    bool operator>= (const Value &rhs) const { return !(*this < rhs); }

    /* has_shape(types, err)
     *
     * Return true if this is a MsgPack object and, for each item in types, has a field of
     * the given type. If not, return false and set err status.
     */
    typedef std::initializer_list<std::pair<std::string, Type>> shape;
    bool has_shape(const shape & types, status_t & err) const;

private:
    std::shared_ptr<MsgPackValue> m_ptr;
};

// Internal class hierarchy - MsgPackValue objects are not exposed to users of this API.
class MsgPackValue {
protected:
    friend class Value;
    friend class MsgPackFloat;
    friend class MsgPackDouble;
    friend class MsgPackInt8;
    friend class MsgPackInt16;
    friend class MsgPackInt32;
    friend class MsgPackInt64;
    friend class MsgPackUint8;
    friend class MsgPackUint16;
    friend class MsgPackUint32;
    friend class MsgPackUint64;
    virtual Value::Type type() const = 0;
    virtual bool equals(const MsgPackValue * other) const = 0;
    virtual bool less(const MsgPackValue * other) const = 0;
    virtual status_t dump(std::string &out) const = 0;
    virtual double number_value() const;
    virtual float float32_value() const;
    virtual double float64_value() const;
    virtual int32_t int_value() const;
    virtual int8_t int8_value() const;
    virtual int16_t int16_value() const;
    virtual int32_t int32_value() const;
    virtual int64_t int64_value() const;
    virtual uint8_t uint8_value() const;
    virtual uint16_t uint16_value() const;
    virtual uint32_t uint32_value() const;
    virtual uint64_t uint64_value() const;
    virtual bool bool_value() const;
    virtual const std::string &string_value() const;
    virtual const Value::array &array_items() const;
    virtual const Value::binary &binary_items() const;
    virtual const Value &operator[](size_t i) const;
    virtual const Value::object &object_items() const;
    virtual const Value &operator[](const std::string &key) const;
    virtual const Value::extension &extension_items() const;
    virtual ~MsgPackValue() {}
};

} } // namespace os::support
