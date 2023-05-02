/**
 * @file dbvariable.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/dbvariable.h"
#include "o3d/core/debug.h"
#include "o3d/core/date.h"
#include "o3d/core/datetime.h"

using namespace o3d;

DbVariable::DbVariable() :
    m_object(nullptr),
    m_objectPtr(nullptr),
    m_objectSize(0)
{
}

DbVariable::DbVariable(
        DbVariable::IntType intType,
        DbVariable::VarType varType,
        UInt8 *data) :
    m_type(varType),
    m_intType(intType)
{
    // null
    if (!data)
    {
        m_object = m_objectPtr = nullptr;
        return;
    }

    switch (intType)
    {
    case IT_BOOL:
        m_object = m_objectPtr = (UInt8*)new Bool(*(Bool*)data);
        m_objectSize = 1;
        break;
    case IT_INT8:
        m_object = m_objectPtr = (UInt8*)new Int8(*(Int8*)data);
        m_objectSize = 1;
        break;
    case IT_INT16:
        m_object = m_objectPtr = (UInt8*)new Int16(*(Int16*)data);
        m_objectSize = 2;
        break;
    case IT_INT32:
        m_object = m_objectPtr = (UInt8*)new Int32(*(Int32*)data);
        m_objectSize = 4;
        break;
    case IT_UINT8:
        m_object = m_objectPtr = (UInt8*)new UInt8(*(UInt8*)data);
        m_objectSize = 1;
        break;
    case IT_UINT16:
        m_object = m_objectPtr = (UInt8*)new UInt16(*(UInt16*)data);
        m_objectSize = 2;
        break;
    case IT_UINT32:
        m_object = m_objectPtr = (UInt8*)new UInt32(*(UInt32*)data);
        m_objectSize = 4;
        break;
    case IT_FLOAT:
        m_object = m_objectPtr = (UInt8*)new Float(*(Float*)data);
        m_objectSize = 4;
        break;
    case IT_DOUBLE:
        m_object =  m_objectPtr =(UInt8*)new Double(*(Double*)data);
        m_objectSize = 8;
        break;
    case IT_ARRAY_CHAR:
        m_object = (UInt8*)new ArrayChar(*(ArrayChar*)data);
        m_objectPtr = (UInt8*)&((ArrayChar*)m_object)->operator [](0);
        m_objectSize = ((ArrayChar*)data)->getSize();
        break;
    case IT_ARRAY_UINT8:
        m_object = (UInt8*)new ArrayUInt8(*(ArrayUInt8*)data);
        m_objectPtr = (UInt8*)&((ArrayUInt8*)m_object)->operator [](0);
        m_objectSize = ((ArrayUInt8*)data)->getSize();
        break;
    case IT_SMART_ARRAY_UINT8:
        m_object = (UInt8*)new SmartArrayUInt8(*(SmartArrayUInt8*)data);
        m_objectPtr = (UInt8*)&((SmartArrayUInt8*)m_object)->operator [](0);
        m_objectSize = ((SmartArrayUInt8*)data)->getSize();
        break;
    case IT_CSTRING:
        m_object = (UInt8*)new CString(*(CString*)data);
        m_objectPtr = (UInt8*)&((CString*)m_object)->operator [](0);
        m_objectSize = ((CString*)data)->length()+1;
        break;
    case IT_DATE:
        m_object = (UInt8*)new Date(*(Date*)data);
        m_objectPtr = new UInt8[64];
        m_objectSize = 64;
        break;
    case IT_DATETIME:
        m_object = (UInt8*)new DateTime(*(DateTime*)data);
        m_objectPtr = new UInt8[64];
        m_objectSize = 64;
        break;
    case IT_ISTREAM:
        m_object = (UInt8*)data;
        m_objectPtr = nullptr;
        m_objectSize = 0;
        break;
    case IT_OSTREAM:
        m_object = (UInt8*)data;
        m_objectPtr = nullptr;
        m_objectSize = 0;
        break;
    default:
        break;
    }
}

DbVariable::DbVariable(IntType intType,
        VarType varType,
        UInt32 maxSize) :
    m_type(varType),
    m_intType(intType),
    m_objectSize(0)
{
    switch (intType)
    {
    case IT_BOOL:
        m_object = m_objectPtr = (UInt8*)new Bool;
        m_objectSize = 1;
        break;
    case IT_INT8:
        m_object = m_objectPtr = (UInt8*)new Int8;
        m_objectSize = 1;
        break;
    case IT_INT16:
        m_object = m_objectPtr = (UInt8*)new Int16;
        m_objectSize = 2;
        break;
    case IT_INT32:
        m_object = m_objectPtr = (UInt8*)new Int32;
        m_objectSize = 4;
        break;
    case IT_UINT8:
        m_object = m_objectPtr = (UInt8*)new UInt8;
        m_objectSize = 1;
        break;
    case IT_UINT16:
        m_object = m_objectPtr = (UInt8*)new UInt16;
        m_objectSize = 2;
        break;
    case IT_UINT32:
        m_object = m_objectPtr = (UInt8*)new UInt32;
        m_objectSize = 4;
        break;
    case IT_FLOAT:
        m_object = m_objectPtr = (UInt8*)new Float;
        m_objectSize = 4;
        break;
    case IT_DOUBLE:
        m_object =  m_objectPtr =(UInt8*)new Double;
        m_objectSize = 8;
        break;
    case IT_ARRAY_CHAR:
        m_object = (UInt8*)new ArrayChar(maxSize, maxSize);
        m_objectPtr = (UInt8*)&((ArrayChar*)m_object)->operator [](0);
        m_objectSize = maxSize;
        break;
    case IT_ARRAY_UINT8:
        m_object = (UInt8*)new ArrayUInt8(maxSize, maxSize);
        m_objectPtr = (UInt8*)&((ArrayUInt8*)m_object)->operator [](0);
        m_objectSize = maxSize;
        break;
    case IT_SMART_ARRAY_UINT8:
        m_object = (UInt8*)new SmartArrayUInt8(maxSize);
        m_objectPtr = (UInt8*)&((SmartArrayUInt8*)m_object)->operator [](0);
        m_objectSize = maxSize;
        break;
    case IT_CSTRING:
        m_object = (UInt8*)new CString(maxSize);
        m_objectPtr = (UInt8*)&((CString*)m_object)->operator [](0);
        m_objectSize = maxSize;
        break;
    case IT_DATE:
        m_object = (UInt8*)new Date;
        m_objectPtr = new UInt8[64];
        m_objectSize = 64;
        break;
    case IT_DATETIME:
        m_object = (UInt8*)new DateTime;
        m_objectPtr = new UInt8[64];
        m_objectSize = 64;
        break;
    case IT_ISTREAM:
        m_object = nullptr; // TODO with ArrayUInt8
        m_objectPtr = nullptr;
        m_objectSize = 0;
        break;
    case IT_OSTREAM:
        m_object = nullptr; // TODO with ArrayUInt8
        m_objectPtr = nullptr;
        m_objectSize = 0;
        break;
    default:
        break;
    }
}

DbVariable::~DbVariable()
{
    switch (m_intType)
    {
    case IT_BOOL:
        deletePtr((Bool*)m_object);
        break;
    case IT_INT8:
        deletePtr((Int8*)m_object);
        break;
    case IT_INT16:
        deletePtr((Int16*)m_object);
        break;
    case IT_INT32:
        deletePtr((Int32*)m_object);
        break;
    case IT_UINT8:
        deletePtr((UInt8*)m_object);
        break;
    case IT_UINT16:
        deletePtr((UInt16*)m_object);
        break;
    case IT_UINT32:
        deletePtr((UInt32*)m_object);
        break;
    case IT_FLOAT:
        deletePtr((Float*)m_object);
        break;
    case IT_DOUBLE:
        deletePtr((Double*)m_object);
        break;
    case IT_ARRAY_CHAR:
        deletePtr((ArrayChar*)m_object);
        break;
    case IT_ARRAY_UINT8:
        deletePtr((ArrayUInt8*)m_object);
        break;
    case IT_SMART_ARRAY_UINT8:
        deletePtr((SmartArrayUInt8*)m_object);
        break;
    case IT_CSTRING:
        deletePtr((CString*)m_object);
        break;
    case IT_DATE:
        deletePtr((Date*)m_object);
        deleteArray(m_objectPtr);
        break;
    case IT_DATETIME:
        deletePtr((DateTime*)m_object);
        deleteArray(m_objectPtr);
        break;
    case IT_ISTREAM:
        break;
    case IT_OSTREAM:
        break;
    default:
        break;
    }
}

void DbVariable::setDate(const Date &v)
{
    O3D_ASSERT(m_intType == IT_DATE);
    (*(Date*)m_object) = v;
}

const Date &DbVariable::asDate() const
{
    O3D_ASSERT(m_intType == IT_DATE);
    return (*(Date*)m_object);
}

void DbVariable::setDateTime(const DateTime &v)
{
    O3D_ASSERT(m_intType == IT_DATETIME);
    (*(DateTime*)m_object) = v;
}

const DateTime &DbVariable::asDateTime() const
{
    O3D_ASSERT(m_intType == IT_DATETIME);
    return (*(DateTime*)m_object);
}

CString DbVariable::toCString() const
{
    if (m_intType == IT_ARRAY_CHAR) {
        ArrayChar &array = *((ArrayChar*)m_object);
        return CString(array.getData(), array.getSize()-1);
    } else if (m_intType == IT_CSTRING) {
        return CString(*((CString*)m_object));
    } else {
        O3D_ERROR(E_InvalidOperation(""));
    }
}

Int32 DbVariable::toInt32() const
{
    if (m_intType == IT_INT32) {
        return asInt32();
    } else if (m_intType == IT_INT8) {
        return static_cast<Int32>(asInt8());
    } else if (m_intType == IT_INT16) {
        return static_cast<Int32>(asInt16());
    } else if (m_intType == IT_UINT8) {
        return static_cast<Int32>(asUInt8());
    } else if (m_intType == IT_UINT16) {
        return static_cast<Int32>(asUInt16());
    } else if (m_intType == IT_UINT32) {
        return static_cast<Int32>(asUInt32());
    } else if (m_intType == IT_DOUBLE) {
        return static_cast<Int32>(asDouble());
    } else if (m_intType == IT_FLOAT) {
        return static_cast<Int32>(asFloat());
    } else if (m_intType == IT_ARRAY_CHAR) {
        Char *end;
        return static_cast<Int32>(strtol(((ArrayChar*)m_object)->getData(), &end, 10));
    } else if (m_intType == IT_CSTRING) {
        Char *end;
        return static_cast<Int32>(strtol(((CString*)m_object)->getData(), &end, 10));
    } else {
        O3D_ERROR(E_InvalidOperation(""));
    }
}

Double DbVariable::toDouble() const
{
    if (m_intType == IT_DOUBLE) {
        return asDouble();
    } else if (m_intType == IT_FLOAT) {
        return static_cast<Double>(asFloat());
    } else if (m_intType == IT_INT16) {
        return static_cast<Double>(asInt16());
    } else if (m_intType == IT_UINT8) {
        return static_cast<Double>(asUInt8());
    } else if (m_intType == IT_UINT16) {
        return static_cast<Double>(asUInt16());
    } else if (m_intType == IT_UINT32) {
        return static_cast<Double>(asUInt32());
    } else if (m_intType == IT_DOUBLE) {
        return static_cast<Double>(asDouble());
    } else if (m_intType == IT_FLOAT) {
        return static_cast<Double>(asFloat());
    } else if (m_intType == IT_ARRAY_CHAR) {
        Char *end;
        return static_cast<Int32>(strtod(((ArrayChar*)m_object)->getData(), &end));
    } else if (m_intType == IT_CSTRING) {
        Char *end;
        return static_cast<Int32>(strtod(((CString*)m_object)->getData(), &end));
    } else {
        O3D_ERROR(E_InvalidOperation(""));
    }
}

SmartArrayUInt8 DbVariable::toSmartArrayUInt8() const
{
    if (m_intType == IT_ARRAY_CHAR)
    {
        ArrayChar &array = *((ArrayChar*)m_object);
        return SmartArrayUInt8((UInt8*)array.getData(), array.getSize());
    }
    else if (m_intType == IT_ARRAY_UINT8)
    {
        ArrayUInt8 &array = *((ArrayUInt8*)m_object);
        return SmartArrayUInt8(array.getData(), array.getSize());
    }
    else
        O3D_ERROR(E_InvalidOperation(""));
}

void DbVariable::setSmartArrayUInt8(const SmartArrayUInt8 &v)
{
    O3D_ASSERT(m_intType == IT_SMART_ARRAY_UINT8);
    (*(SmartArrayUInt8*)m_object) = v;
}

const SmartArrayUInt8& DbVariable::getSmartArrayUInt8() const
{
    O3D_ASSERT(m_intType == IT_SMART_ARRAY_UINT8);
    return (*(SmartArrayUInt8*)m_object);
}
