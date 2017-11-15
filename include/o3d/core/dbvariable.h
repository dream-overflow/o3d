/**
 * @file dbvariable.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_DBVARIABLE_H
#define _O3D_DBVARIABLE_H

#include "templatearray.h"
#include "smartarray.h"

namespace o3d {

class Date;

/**
 * @brief DbVariable
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-12-03
 */
class O3D_API DbVariable
{
public:

    //! Database type of a variable
    enum IntType
    {
        IT_BOOL,
        IT_CHAR,
        IT_INT8,
        IT_UINT8,
        IT_INT16,
        IT_UINT16,
        IT_INT32,
        IT_UINT32,
        IT_FLOAT,
        IT_DOUBLE,
        IT_ARRAY_CHAR,
        IT_ARRAY_UINT8,
        IT_SMART_ARRAY_UINT8,
        IT_CSTRING,
        IT_DATE,
        IT_ISTREAM,
        IT_OSTREAM
    };

    //! Database type of a variable
    enum VarType
    {
        BOOLEAN,
        CHAR,
        INT8,
        UINT8,
        INT16,
        UINT16,
        INT24,
        UINT24,
        INT32,
        UINT32,
        FLOAT32,
        FLOAT64,
        VARCHAR,
        STRING,
        TIMESTAMP,
        TINY_ARRAY,
        ARRAY,
        MEDIUM_ARRAY,
        LONG_ARRAY
    };

    DbVariable();

    DbVariable(
            IntType intType,
            VarType varType,
            UInt8 *data);

    DbVariable(
            IntType intType,
            VarType varType,
            UInt32 maxSize);

    virtual ~DbVariable();

    //! Set the variable as a Bool
    void setBool(Bool v) {
        O3D_ASSERT(m_intType == IT_BOOL);
        (*(Bool*)m_object) = v;
    }

    //! Return the variable as a Bool
    inline Bool asBool() const {
        O3D_ASSERT(m_intType == IT_BOOL);
        return (*(Bool*)m_object);
    }

    //! Set the variable as a Char
    void setChar(Char v) {
        O3D_ASSERT(m_intType == IT_CHAR);
        (*(Char*)m_object) = v;
    }

    //! Return the variable as a Char
    inline Char asChar() const {
        O3D_ASSERT(m_intType == IT_CHAR);
        return (*(Char*)m_object);
    }

    //! Set the variable as a Int8
    void setInt8(Int8 v) {
        O3D_ASSERT(m_intType == IT_INT8);
        (*(Int8*)m_object) = v;
    }

    //! Return the variable as a int8
    inline Int8 asInt8() const {
        O3D_ASSERT(m_intType == IT_INT8);
        return (*(Int8*)m_object);
    }

    //! Set the variable as a Int16
    void setInt16(Int16 v) {
        O3D_ASSERT(m_intType == IT_INT16);
        (*(Int16*)m_object) = v;
    }

    //! Return the variable as a int16
    inline Int16 asInt16() const {
        O3D_ASSERT(m_intType == IT_INT16);
        return (*(Int16*)m_object);
    }

    //! Set the variable as a Int32
    void setInt32(Int32 v) {
        O3D_ASSERT(m_intType == IT_INT32);
        (*(Int32*)m_object) = v;
    }

    //! Return the variable as a int32
    inline Int32 asInt32() const {
        O3D_ASSERT(m_intType == IT_INT32);
        return (*(Int32*)m_object);
    }

    //! Set the variable as a Float
    void setFloat(Float v) {
        O3D_ASSERT(m_intType == IT_FLOAT);
        (*(Float*)m_object) = v;
    }

    //! Return the variable as a float
    inline Bool asFloat() const {
        O3D_ASSERT(m_intType == IT_FLOAT);
        return (*(Float*)m_object);
    }

    //! Set the variable as a Double
    void setDouble(Double v) {
        O3D_ASSERT(m_intType == IT_DOUBLE);
        (*(Double*)m_object) = v;
    }

    //! Return the variable as a double
    inline Double asDouble() const {
        O3D_ASSERT(m_intType == IT_DOUBLE);
        return (*(Double*)m_object);
    }

    //! Set the variable as a UInt8
    void setUInt8(UInt8 v) {
        O3D_ASSERT(m_intType == IT_UINT8);
        (*(UInt8*)m_object) = v;
    }

    //! Return the variable as a string
    inline UInt8 asUInt8() const {
        O3D_ASSERT(m_intType == IT_UINT8);
        return (*(UInt8*)m_object);
    }

    //! Set the variable as a UInt16
    void setUInt16(UInt16 v) {
        O3D_ASSERT(m_intType == IT_UINT16);
        (*(UInt16*)m_object) = v;
    }

    //! Return the variable as a UInt16
    inline UInt16 asUInt16() const {
        O3D_ASSERT(m_intType == IT_UINT16);
        return (*(UInt16*)m_object);
    }

    //! Set the variable as a UInt32
    void setUInt32(UInt32 v) {
        O3D_ASSERT(m_intType == IT_UINT32);
        (*(UInt32*)m_object) = v;
    }

    //! Return the variable as a UInt32
    inline UInt32 asUInt32() const {
        O3D_ASSERT(m_intType == IT_UINT32);
        return (*(UInt32*)m_object);
    }

    //! Set the variable as a CString
    void setCString(const CString &v) {
        O3D_ASSERT(m_intType == IT_CSTRING);
        (*(CString*)m_object) = v;
    }

    //! Return the variable as a CString
    inline const CString& asCString() const {
        O3D_ASSERT(m_intType == IT_CSTRING);
        return (*(CString*)m_object);
    }

    //! Set the variable as an ArrayChar
    void setArrayChar(const ArrayChar &v) {
        O3D_ASSERT(m_intType == IT_ARRAY_CHAR);
        (*(ArrayChar*)m_object) = v;
    }

    //! Return the variable as an ArrayChar
    inline const ArrayChar& asArrayChar() const {
        O3D_ASSERT(m_intType == IT_ARRAY_CHAR);
        return (*(ArrayChar*)m_object);
    }

    //! Set the variable as an ArrayUInt8
    void setArrayUInt8(const ArrayUInt8 &v) {
        O3D_ASSERT(m_intType == IT_ARRAY_UINT8);
        (*(ArrayUInt8*)m_object) = v;
    }

    //! Return the variable as an ArrayUInt8
    inline const ArrayUInt8& asArrayUInt8() const {
        O3D_ASSERT(m_intType == IT_ARRAY_UINT8);
        return (*(ArrayUInt8*)m_object);
    }

    //! Set the variable as a SmartArrayUInt8
    void setSmartArrayUInt8(const SmartArray<UInt8> &v);

    //! Return the variable as a SmartArrayUInt8
    const SmartArray<UInt8>& getSmartArrayUInt8() const;

    //! Set the variable as a Date
    void setDate(const Date &v);

    //! Return the variable as a Date
    const Date& asDate() const;

    //! Return the variable as a CString with a convertion as necessary
    CString toCString() const;

    //! Return the variable as a SmartArrayUInt8 with a convertion as necessary
    SmartArrayUInt8 toSmartArrayUInt8() const;

public:

    //! Get the DB type
    inline VarType getType() const { return m_type; }

    //! Get the object internal type
    inline IntType getIntType() const { return m_intType; }

    //! Get the object ptr
    inline UInt8* getObject() { return m_object; }

    //! Get the object ptr
    inline UInt8* getObjectPtr() { return m_objectPtr; }

    //! Set the object size
    inline void setObjectSize(UInt32 size) { m_objectSize = size; }

    //! Get the object size
    inline UInt32 getObjectSize() const { return m_objectSize; }

    //! Set the object null
    virtual void setNull(Bool isNull = True) = 0;

    //! Is the object null
    virtual Bool isNull() const = 0;

    //! Get the isNull pointer.
    virtual UInt8* getIsNullPtr() = 0;

    //! Set the length
    virtual void setLength(UInt32 len) = 0;

    //! Get the length
    virtual UInt32 getLength() const = 0;

    //! Get the length pointer.
    virtual UInt8* getLengthPtr() = 0;

    //! Get the error pointer.
    virtual UInt8* getErrorPtr() = 0;

protected:

    VarType m_type;
    IntType m_intType;
    UInt8 *m_object;
    UInt8 *m_objectPtr;
    UInt32 m_objectSize;
};

} // namespace o3d

#endif // _O3D_DBVARIABLE_H

