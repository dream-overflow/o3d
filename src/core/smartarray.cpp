/**
 * @file smartarray.cpp
 * @brief Implementation of SmartArray.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-07-31
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/smartarray.h"

#include "o3d/core/instream.h"
#include "o3d/core/outstream.h"

using namespace o3d;

template<> Bool SmartArray<Char>::writeToFile(OutStream &os) const
{
    os << m_size;

    if (m_size && m_data)
        os.write(m_data, m_size);

    return True;
}

template<> Bool SmartArray<Char>::readFromFile(InStream &is)
{
    releaseCheckAndDelete();

    UInt32 size;
    is >> size;

	if (size > 0)
    {
		allocate(size);
        is.read(m_data, size);
    }

    return True;
}

template<> Bool SmartArray<UInt8>::writeToFile(OutStream &os) const
{
    os << m_size;

    if (m_size && m_data)
        os.write(m_data, m_size);

    return True;
}

template<> Bool SmartArray<UInt8>::readFromFile(InStream &is)
{
    releaseCheckAndDelete();

    UInt32 size;
    is >> size;

	if (size > 0)
    {
		allocate(size);
        is.read(m_data, size);
    }

    return True;
}

template<> Bool SmartArray<Int16>::writeToFile(OutStream &os) const
{
    os << m_size;

    if (m_size && m_data)
        os.write(m_data, m_size);

    return True;
}

template<> Bool SmartArray<Int16>::readFromFile(InStream &is)
{
    releaseCheckAndDelete();

    UInt32 size;
    is >> size;

	if (size > 0)
    {
		allocate(size);
        is.read(m_data, size);
    }

    return True;
}

template<> Bool SmartArray<UInt16>::writeToFile(OutStream &os) const
{
    os << m_size;

    if (m_size && m_data)
        os.write(m_data, m_size);

    return True;
}

template<> Bool SmartArray<UInt16>::readFromFile(InStream &is)
{
    releaseCheckAndDelete();

    UInt32 size;
    is >> size;

	if (size > 0)
    {
		allocate(size);
        is.read(m_data, size);
    }

    return True;
}

template<> Bool SmartArray<Int32>::writeToFile(OutStream &os) const
{
    os << m_size;

    if (m_size && m_data)
        os.write(m_data, m_size);

    return True;
}

template<> Bool SmartArray<Int32>::readFromFile(InStream &is)
{
    releaseCheckAndDelete();

    UInt32 size;
    is >> size;

	if (size > 0)
    {
		allocate(size);
        is.read(m_data, size);
    }

    return True;
}

template<> Bool SmartArray<UInt32>::writeToFile(OutStream &os) const
{
    os << m_size;

    if (m_size && m_data)
        os.write(m_data, m_size);

    return True;
}

template<> Bool SmartArray<UInt32>::readFromFile(InStream &is)
{
    releaseCheckAndDelete();

    UInt32 size;
    is >> size;

	if (size > 0)
    {
		allocate(size);
        is.read(m_data, size);
    }

    return True;
}

template<> Bool SmartArray<Float>::writeToFile(OutStream &os) const
{
    os << m_size;

    if (m_size && m_data)
        os.write(m_data, m_size);

    return True;
}

template<> Bool SmartArray<Float>::readFromFile(InStream &is)
{
    releaseCheckAndDelete();

    UInt32 size;
    is >> size;

	if (size > 0)
    {
		allocate(size);
        is.read(m_data, size);
    }

    return True;
}

template<> Bool SmartArray<Double>::writeToFile(OutStream &os) const
{
    os << m_size;

    if (m_size && m_data)
        os.write(m_data, m_size);

    return True;
}

template<> Bool SmartArray<Double>::readFromFile(InStream &is)
{
    releaseCheckAndDelete();

    UInt32 size;
    is >> size;

	if (size > 0)
    {
		allocate(size);
        is.read(m_data, size);
    }

    return True;
}

