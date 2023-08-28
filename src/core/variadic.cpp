/**
 * @file variadic.cpp
 * @brief Variadic container.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2019-09-19
 * @copyright Copyright (c) 2001-2019 Dream Overflow. All rights reserved.
 * @details Variadic object container.
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/variadic.h"

#include "o3d/core/error.h"

#include "o3d/core/instream.h"
#include "o3d/core/outstream.h"

using namespace o3d;

Variadic::Variadic(Type type) :
    m_type(type),
    m_data(nullptr)
{
    switch (type) {
        case BOOL:
            m_data = reinterpret_cast<UInt8*>(new Bool(false));
            break;

        case CHAR:
            m_data = reinterpret_cast<UInt8*>(new Char(0));
            break;
        case WCHAR:
            m_data = reinterpret_cast<UInt8*>(new WChar(0));
            break;

        case INT8:
            m_data = reinterpret_cast<UInt8*>(new Int8(0));
            break;
        case UINT8:
            m_data = reinterpret_cast<UInt8*>(new UInt8(0));
            break;

        case INT16:
            m_data = reinterpret_cast<UInt8*>(new Int16(0));
            break;
        case UINT16:
            m_data = reinterpret_cast<UInt8*>(new UInt16(0));
            break;

        case INT32:
            m_data = reinterpret_cast<UInt8*>(new Int32(0));
            break;
        case UINT32:
            m_data = reinterpret_cast<UInt8*>(new UInt32(0));
            break;

        case INT64:
            m_data = reinterpret_cast<UInt8*>(new Int64(0));
            break;
        case UINT64:
            m_data = reinterpret_cast<UInt8*>(new UInt64(0));
            break;

        case FLOAT32:
            m_data = reinterpret_cast<UInt8*>(new Float(0));
            break;
        case FLOAT64:
            m_data = reinterpret_cast<UInt8*>(new Double(0));
            break;

        case STRING:
            m_data = reinterpret_cast<UInt8*>(new String());
            break;
        case CSTRING:
            m_data = reinterpret_cast<UInt8*>(new CString());
            break;

        case ARRAY:
            m_data = reinterpret_cast<UInt8*>(new T_VariadicVector());
            break;

        case OBJECT:
            m_data = reinterpret_cast<UInt8*>(new StringMap<Variadic>());
            break;

        default:
            m_type = UNDEFINED;
            break;
    }
}

Variadic::Variadic(const Variadic &dup) :
    m_type(dup.m_type),
    m_data(nullptr)
{
    switch (dup.m_type) {
        case BOOL:
            m_data = reinterpret_cast<UInt8*>(new Bool(*reinterpret_cast<Bool*>(dup.m_data)));
            break;

        case CHAR:
            m_data = reinterpret_cast<UInt8*>(new Char(*reinterpret_cast<Char*>(dup.m_data)));
            break;
        case WCHAR:
            m_data = reinterpret_cast<UInt8*>(new WChar(*reinterpret_cast<WChar*>(dup.m_data)));
            break;

        case INT8:
            m_data = reinterpret_cast<UInt8*>(new Int8(*reinterpret_cast<Int8*>(dup.m_data)));
            break;
        case UINT8:
            m_data = reinterpret_cast<UInt8*>(new UInt8(*reinterpret_cast<UInt8*>(dup.m_data)));
            break;

        case INT16:
            m_data = reinterpret_cast<UInt8*>(new Int16(*reinterpret_cast<Int16*>(dup.m_data)));
            break;
        case UINT16:
            m_data = reinterpret_cast<UInt8*>(new UInt16(*reinterpret_cast<UInt16*>(dup.m_data)));
            break;

        case INT32:
            m_data = reinterpret_cast<UInt8*>(new Int32(*reinterpret_cast<Int32*>(dup.m_data)));
            break;
        case UINT32:
            m_data = reinterpret_cast<UInt8*>(new UInt32(*reinterpret_cast<UInt32*>(dup.m_data)));
            break;

        case INT64:
            m_data = reinterpret_cast<UInt8*>(new Int64(*reinterpret_cast<Int64*>(dup.m_data)));
            break;
        case UINT64:
            m_data = reinterpret_cast<UInt8*>(new UInt64(*reinterpret_cast<UInt64*>(dup.m_data)));
            break;

        case FLOAT32:
            m_data = reinterpret_cast<UInt8*>(new Float(*reinterpret_cast<Float*>(dup.m_data)));
            break;
        case FLOAT64:
            m_data = reinterpret_cast<UInt8*>(new Double(*reinterpret_cast<Double*>(dup.m_data)));
            break;

        case STRING:
            m_data = reinterpret_cast<UInt8*>(new String(*reinterpret_cast<String*>(dup.m_data)));
            break;
        case CSTRING:
            m_data = reinterpret_cast<UInt8*>(new CString(*reinterpret_cast<CString*>(dup.m_data)));
            break;

        case ARRAY:
            m_data = reinterpret_cast<UInt8*>(new T_VariadicVector());
            {
                // duplicate elements
                const T_VariadicVector &src = *reinterpret_cast<T_VariadicVector*>(dup.m_data);
                T_VariadicVector &dst = *reinterpret_cast<T_VariadicVector*>(m_data);

                for (CIT_VariadicVector cit = src.cbegin(); cit != src.cend(); ++cit) {
                    dst.push_back(*cit);
                }
            }
            break;

        case OBJECT:
            m_data = reinterpret_cast<UInt8*>(new StringMap<Variadic>());
            {
                // duplicate elements
                const StringMap<Variadic> &src = *reinterpret_cast<StringMap<Variadic>*>(dup.m_data);
                StringMap<Variadic> &dst = *reinterpret_cast<StringMap<Variadic>*>(m_data);

                for (StringMap<Variadic>::CIT cit = src.cbegin(); cit != src.cend(); ++cit) {
                    dst.insert(std::make_pair(cit->first, cit->second));
                }
            }
            break;

        default:
            m_type = UNDEFINED;
            break;
    }
}

Variadic::~Variadic()
{
     switch (m_type) {
        case BOOL:
            deletePtr(reinterpret_cast<Bool*>(m_data));
            break;

        case CHAR:
            deletePtr(reinterpret_cast<Char*>(m_data));
            break;
        case WCHAR:
            deletePtr(reinterpret_cast<WChar*>(m_data));
            break;

        case INT8:
             deletePtr(reinterpret_cast<Int8*>(m_data));
             break;
        case UINT8:
             deletePtr(reinterpret_cast<UInt8*>(m_data));
             break;

        case INT16:
            deletePtr(reinterpret_cast<Int16*>(m_data));
            break;
        case UINT16:
            deletePtr(reinterpret_cast<UInt16*>(m_data));
            break;

        case INT32:
            deletePtr(reinterpret_cast<Int32*>(m_data));
            break;
        case UINT32:
            deletePtr(reinterpret_cast<UInt32*>(m_data));
            break;

        case INT64:
            deletePtr(reinterpret_cast<Int64*>(m_data));
            break;
        case UINT64:
            deletePtr(reinterpret_cast<UInt64*>(m_data));
            break;

        case FLOAT32:
            deletePtr(reinterpret_cast<Float*>(m_data));
            break;
        case FLOAT64:
            deletePtr(reinterpret_cast<Double*>(m_data));
            break;

        case STRING:
            deletePtr(reinterpret_cast<String*>(m_data));
            break;
        case CSTRING:
            deletePtr(reinterpret_cast<CString*>(m_data));
            break;

        case ARRAY:
             deletePtr(reinterpret_cast<std::list<Variadic>*>(m_data));
             break;

        case OBJECT:
             deletePtr(reinterpret_cast<StringMap<Variadic>*>(m_data));
             break;

        default:
            break;
    }
}

//
// types
//

Bool Variadic::asBool() const
{
    if (m_type == Variadic::BOOL) {
        return *reinterpret_cast<const Bool*>(m_data);
    } else {
        return False;
    }
}

void Variadic::setBool(Bool v)
{
    if (m_type == Variadic::BOOL) {
        *reinterpret_cast<Bool*>(m_data) = v;
    }
}

Int8 Variadic::asInt8() const
{
    if (m_type == Variadic::INT8) {
        return *reinterpret_cast<const Int8*>(m_data);
    } else {
        return 0;
    }
}

void Variadic::setInt8(Int8 v)
{
    if (m_type == Variadic::INT8) {
        *reinterpret_cast<Int8*>(m_data) = v;
    }
}

UInt8 Variadic::asUInt8() const
{
    if (m_type == Variadic::UINT8) {
        return *reinterpret_cast<const UInt8*>(m_data);
    } else {
        return 0;
    }
}

void Variadic::setUInt8(UInt8 v)
{
    if (m_type == Variadic::UINT8) {
        *reinterpret_cast<UInt8*>(m_data) = v;
    }
}

Char Variadic::asChar() const
{
    if (m_type == Variadic::CHAR) {
        return *reinterpret_cast<const Char*>(m_data);
    } else {
        return '\0';
    }
}

void Variadic::setChar(Char v)
{
    if (m_type == Variadic::CHAR) {
        *reinterpret_cast<Char*>(m_data) = v;
    }
}

WChar Variadic::asWChar() const
{
    if (m_type == Variadic::WCHAR) {
        return *reinterpret_cast<const WChar*>(m_data);
    } else {
        return L'\0';
    }
}

void Variadic::setWChar(WChar v)
{
    if (m_type == Variadic::WCHAR) {
        *reinterpret_cast<WChar*>(m_data) = v;
    }
}

Int16 Variadic::asInt16() const
{
    if (m_type == Variadic::INT16) {
        return *reinterpret_cast<const Int16*>(m_data);
    } else {
        return 0;
    }
}

void Variadic::setInt16(Int16 v)
{
    if (m_type == Variadic::INT16) {
        *reinterpret_cast<Int16*>(m_data) = v;
    }
}

UInt16 Variadic::asUInt16() const
{
    if (m_type == Variadic::UINT16) {
        return *reinterpret_cast<const UInt16*>(m_data);
    } else {
        return 0;
    }
}

void Variadic::setUInt16(UInt16 v)
{
    if (m_type == Variadic::UINT16) {
        *reinterpret_cast<UInt16*>(m_data) = v;
    }
}

Int32 Variadic::asInt32() const
{
    if (m_type == Variadic::INT32) {
        return *reinterpret_cast<const Int32*>(m_data);
    } else {
        return 0;
    }
}

void Variadic::setInt32(Int32 v)
{
    if (m_type == Variadic::INT32) {
        *reinterpret_cast<Int32*>(m_data) = v;
    }
}

UInt32 Variadic::asUInt32() const
{
    if (m_type == Variadic::UINT32) {
        return *reinterpret_cast<const UInt32*>(m_data);
    } else {
        return 0;
    }
}

void Variadic::setUInt32(UInt32 v)
{
    if (m_type == Variadic::UINT32) {
        *reinterpret_cast<UInt32*>(m_data) = v;
    }
}

Int64 Variadic::asInt64() const
{
    if (m_type == Variadic::INT64) {
        return *reinterpret_cast<const Int64*>(m_data);
    } else {
        return 0;
    }
}

void Variadic::setInt64(Int64 v)
{
    if (m_type == Variadic::INT64) {
        *reinterpret_cast<Int64*>(m_data) = v;
    }
}

UInt64 Variadic::asUInt64() const
{
    if (m_type == Variadic::UINT64) {
        return *reinterpret_cast<const UInt64*>(m_data);
    } else {
        return 0;
    }
}

void Variadic::setUInt64(UInt64 v)
{
    if (m_type == Variadic::UINT64) {
        *reinterpret_cast<UInt64*>(m_data) = v;
    }
}

Float Variadic::asFloat() const
{
    if (m_type == Variadic::FLOAT32) {
        return *reinterpret_cast<const Float*>(m_data);
    } else {
        return 0;
    }
}

void Variadic::setFloat(Float v)
{
    if (m_type == Variadic::FLOAT32) {
        *reinterpret_cast<Float*>(m_data) = v;
    }
}

Double Variadic::asDouble() const
{
    if (m_type == Variadic::FLOAT64) {
        return *reinterpret_cast<const Double*>(m_data);
    } else {
        return 0;
    }
}

void Variadic::setDouble(Double v)
{
    if (m_type == Variadic::FLOAT64) {
        *reinterpret_cast<Double*>(m_data) = v;
    }
}

String Variadic::asString() const
{
    if (m_type == Variadic::STRING) {
        return *reinterpret_cast<const String*>(m_data);
    } else {
        return "";
    }
}

void Variadic::setString(const String &v)
{
    if (m_type == Variadic::STRING) {
        *reinterpret_cast<String*>(m_data) = v;
    }
}

CString Variadic::asCString() const
{
    if (m_type == Variadic::CSTRING) {
        return *reinterpret_cast<const CString*>(m_data);
    } else {
        return "";
    }
}

void Variadic::setCString(const CString &v)
{
    if (m_type == Variadic::CSTRING) {
        *reinterpret_cast<CString*>(m_data) = v;
    }
}
//

Variadic* Variadic::at(Int32 index)
{
    if (m_type == ARRAY) {
        T_VariadicVector *arr = reinterpret_cast<T_VariadicVector*>(m_data);
        if (index >= 0 && index < static_cast<Int32>(arr->size())) {
            return &(*arr)[index];
        }
    }

    return nullptr;
}

const Variadic *Variadic::at(Int32 index) const
{
    if (m_type == ARRAY) {
        const T_VariadicVector *arr = reinterpret_cast<const T_VariadicVector*>(m_data);
        if (index >= 0 && index < static_cast<Int32>(arr->size())) {
            return &(*arr)[index];
        }
    }

    return nullptr;
}

Int32 Variadic::size() const
{
    if (m_type == OBJECT) {
        return reinterpret_cast<const StringMap<Variadic>*>(m_data)->size();
    } else if (m_type == ARRAY) {
        return reinterpret_cast<const T_VariadicVector*>(m_data)->size();
    }

    return 0;
}

void Variadic::push(const Variadic &v)
{
    if (m_type == ARRAY) {
        return reinterpret_cast<T_VariadicVector*>(m_data)->push_back(v);
    }
}

Bool Variadic::has(const String &key) const
{
    if (m_type == OBJECT) {
        const StringMap<Variadic>* object = reinterpret_cast<const StringMap<Variadic>*>(m_data);
        return object->find(key) != object->cend();
    }

    return False;
}

void Variadic::insert(const String &key, Variadic &v)
{
    if (m_type == OBJECT) {
        StringMap<Variadic>* object = reinterpret_cast<StringMap<Variadic>*>(m_data);
        (*object)[key] = v;
    }
}

Variadic* Variadic::find(const String &key)
{
    if (m_type == OBJECT) {
        StringMap<Variadic>* object = reinterpret_cast<StringMap<Variadic>*>(m_data);
        auto it = object->find(key);
        if (it != object->end()) {
            return &it->second;
        }
    }

    return nullptr;
}

const Variadic* Variadic::find(const String &key) const
{
    if (m_type == OBJECT) {
        const StringMap<Variadic>* object = reinterpret_cast<const StringMap<Variadic>*>(m_data);
        auto cit = object->find(key);
        if (cit != object->cend()) {
            return &cit->second;
        }
    }

    return nullptr;
}

Bool Variadic::writeToFile(OutStream &os) const
{
    // @todo

    return True;
}

Bool Variadic::readFromFile(InStream &is)
{
    // @todo

    return True;
}
