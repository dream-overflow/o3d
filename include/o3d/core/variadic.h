/**
 * @file variadic.h
 * @brief Variadic container.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2019-09-19
 * @copyright Copyright (c) 2001-2019 Dream Overflow. All rights reserved.
 * @details Variadic object container.
 */

#ifndef _O3D_VARIADIC_H
#define _O3D_VARIADIC_H

#include "string.h"
#include "stringmap.h"

#include <vector>

namespace o3d {

class Variadic;

typedef std::vector<Variadic> T_VariadicVector;
typedef T_VariadicVector::iterator IT_VariadicVector;
typedef T_VariadicVector::const_iterator CIT_VariadicVector;

/**
 * @brief The Variadic base class.
 */
class O3D_API Variadic
{
public:

    enum Type {
        UNDEFINED = -1,
        BOOL = 0,
        INT8,
        UINT8,
        CHAR,
        WCHAR,
        INT16,
        UINT16,
        INT32,
        UINT32,
        INT64,
        UINT64,
        FLOAT32,
        FLOAT64,
        STRING,
        CSTRING,
        OBJECT,   //!< Hash map of variadics (std::hashmap<String, Variadic>)
        ARRAY,    //!< Array of objects (std::list<VariadicObject>
    };

    Variadic(Type type=OBJECT);
    Variadic(const Variadic &dup);

    ~Variadic();

    Type type() const { return m_type; }

    Bool asBool() const;
    void setBool(Bool v);

    Int8 asInt8() const;
    void setInt8(Int8 v);

    UInt8 asUInt8() const;
    void setUInt8(UInt8 v);

    Int8 asChar() const;
    void setChar(Char v);

    WChar asWChar() const;
    void setWChar(WChar v);

    Int16 asInt16() const;
    void setInt16(Int16 v);

    UInt16 asUInt16() const;
    void setUInt16(UInt16 v);

    Int32 asInt32() const;
    void setInt32(Int32 v);

    UInt32 asUInt32() const;
    void setUInt32(UInt32 v);

    Int64 asInt64() const;
    void setInt64(Int64 v);

    UInt64 asUInt64() const;
    void setUInt64(UInt64 v);

    Float asFloat() const;
    void setFloat(Float v);

    Double asDouble() const;
    void setDouble(Double v);

    String asString() const;
    void setString(const String &v);

    CString asCString() const;
    void setCString(const CString &v);

    //
    // array
    //

    inline Bool isArray() const { return m_type == ARRAY; }

    Variadic* at(o3d::Int32 index);
    const Variadic* at(o3d::Int32 index) const;

    o3d::Int32 size() const;

    void push(const Variadic &v);

    //
    // object
    //

    inline Bool isObject() const { return m_type == OBJECT; }

    Bool has(const String &key) const;

    void insert(const String &key, Variadic &v);

    Variadic* find(const String &key);
    const Variadic* find(const String &key) const;

    //
    // serialization
    //

    Bool writeToFile(class OutStream &os) const;
    Bool readFromFile(class InStream &is);

private:

    Type m_type;
    UInt8 *m_data;
};

} // namespace o3d

#endif // _O3D_VARIADIC_H
