/**
 * @file bitstream.cpp
 * @brief Implementation of BitStream.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @author  Arnaud DELMOTTE
 * @date 2007-07-03
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"

#include "o3d/core/templatemanager.h"
#include "o3d/core/bitstream.h"

#include "o3d/core/debug.h"
#include "o3d/core/objects.h"

using namespace o3d;

// constructor
BitStream::BitStream() :
	m_Data(0,O3D_BITSTREAM_DEFAULT_SIZE),
	m_DataDebug(0,O3D_BITSTREAM_DEFAULT_SIZE)
{
	m_CurByteWrite = 0;
	m_CurBitWrite = 0;
	m_CurByteRead = 0;
	m_CurBitRead = 1;
	m_CurByteWriteDebug = 0;
	m_CurBitWriteDebug = 0;
	m_CurByteReadDebug = 0;
	m_CurBitReadDebug = 0;
	m_Data.push(0);
	debug = False;
	#ifdef O3D_BITSTREAM_DEBUG
	m_DataDebug.push(0);
	debug = True;
	#endif
	addData((UInt8*)&debug,1);
}

// destructor
BitStream::~BitStream()
{
	destroy();
}

// destroy the bit stream
void BitStream::destroy()
{
#ifdef _DEBUG
//	if ((m_CurByteRead != 0) || (m_CurBitRead > 1))
//		O3D_ASSERT((m_CurByteWrite==m_CurByteRead) && (m_CurBitWrite==m_CurBitRead));
#endif
}

// push nb_bits data from buf to the BitStream
void BitStream::addData(const UInt8 *buf,Int32 nb_bits)
{
	O3D_ASSERT(buf != NULL);

	Int32 k=0,l=0;

	for (Int32 i=0; i < nb_bits; i++, k++)
	{
		if (k == 8)
		{
			k = 0;
			++l;
		}

		m_Data[m_CurByteWrite] |= ((buf[l] >> k) & 1) << m_CurBitWrite;
		m_CurBitWrite++;

		if(m_CurBitWrite == 8)
		{
			m_CurBitWrite = 0;
			++m_CurByteWrite;
			m_Data.push(0);
		}
	}
}

// push nb_bits data from buf to the debug buffer
void BitStream::addDataDebug(const UInt8 *buf,Int32 nb_bits)
{
	O3D_ASSERT(buf != NULL);

	Int32 k=0,l=0;

	for (Int32 i=0; i < nb_bits; i++, k++)
	{
		if (k == 8)
		{
			k = 0;
			++l;
		}

		m_DataDebug[m_CurByteWriteDebug] |= ((buf[l] >> k) & 1) << m_CurBitWriteDebug;
		m_CurBitWriteDebug++;

		if(m_CurBitWriteDebug == 8)
		{
			m_CurBitWriteDebug = 0;
			++m_CurByteWriteDebug;
			m_DataDebug.push(0);
		}
	}
}

// push nb_bits data from the BitStream to buf
void BitStream::getData(UInt8 *buf,Int32 nb_bits)
{
	O3D_ASSERT(buf != NULL);

	Int32 l = (nb_bits-1) / 8;
	Int32 k = nb_bits - 1 - l*8;
	m_CurByteRead += (nb_bits+m_CurBitRead)/8;
    m_CurBitRead = (nb_bits+m_CurBitRead)%8;

	for (Int32 i = nb_bits-1; i >= 0; i--, k--)
	{
		if (m_CurBitRead == 0)
		{
			m_CurBitRead = 7;
			--m_CurByteRead;
		}
		else --m_CurBitRead;

		buf[l] |= ((m_Data[m_CurByteRead] >> m_CurBitRead) & 1) << k;
		if( k == 0)
		{
			--l;
			k = 8;
		}
	}

	m_CurByteRead += (nb_bits+m_CurBitRead)/8;
    m_CurBitRead = (nb_bits+m_CurBitRead)%8;
}

// push nb_bits data from the debug buffer to buf
void BitStream::getDataDebug(UInt8 *buf,Int32 nb_bits)
{
	O3D_ASSERT(buf != NULL);

	Int32 l = (nb_bits-1) / 8;
	Int32 k = nb_bits - 1 - l*8;
	m_CurByteReadDebug += (nb_bits+m_CurBitReadDebug)/8;
    m_CurBitReadDebug = (nb_bits+m_CurBitReadDebug)%8;

	for (Int32 i = nb_bits-1; i >= 0; i--, k--)
	{
		if (m_CurBitReadDebug == 0)
		{
			m_CurBitReadDebug = 7;
			--m_CurByteReadDebug;
		}
		else --m_CurBitReadDebug;

		buf[l] |= ((m_DataDebug[m_CurByteReadDebug] >> m_CurBitReadDebug) & 1) << k;
		if( k == 0)
		{
			--l;
			k = 8;
		}
	}

	m_CurByteReadDebug += (nb_bits+m_CurBitReadDebug)/8;
    m_CurBitReadDebug = (nb_bits+m_CurBitReadDebug)%8;
}

// return String value from the bitstream
String BitStream::getString()
{
	#ifdef O3D_BITSTREAM_DEBUG
	if(debug)
	{
		UInt8 type=0;
		getDataDebug(&type,5);
		O3D_ASSERT(type == Type_String);
	}
	#endif

	String buffer;
	UInt16 size = getUInt16();
	for(UInt32 i=0;i<size;i++)
		buffer << getChar();
	return buffer;
}

void BitStream::getString(String &val)
{
	#ifdef O3D_BITSTREAM_DEBUG
	if(debug)
	{
		UInt8 type=0;
		getDataDebug(&type,5);
		O3D_ASSERT(type == Type_String);
	}
	#endif

	ArrayChar utf8String;
	UInt16 size = getUInt16();

	for(UInt32 i=0;i<size;i++)
	{
		utf8String.push(getChar());
	}

	val.fromUtf8(utf8String.getData());
}

// push an O3DDate value to the bitstream
void BitStream::getDate(Date &val)
{
	#ifdef O3D_BITSTREAM_DEBUG
	if(debug)
	{
		UInt8 type=0;
		getDataDebug(&type,5);
		O3D_ASSERT(type == Type_Date);
	}
	#endif

	val.year = getUInt16(12);
	val.month = (Month)getUInt8(4);
	val.day = (Day)getUInt8(3);
	val.mday = getUInt8(5);
	val.hour = getUInt8(5);
	val.minute = getUInt8(6);
	val.second = getUInt8(6);
	val.millisecond = getUInt8(7);
}

// push String value to the bitstream
void BitStream::pushString(const String &val)
{
	#ifdef O3D_BITSTREAM_DEBUG
	if(debug)
	{
		UInt8 type = Type_String;
		addDataDebug(&type,5);
	}
	#endif

	CString utf8String = val.toUtf8();
	pushUInt16(UInt16(utf8String.length()));

	for(UInt32 i=0;i<utf8String.length();i++)
	{
		pushChar(utf8String.getData()[i]);
	}
}

// push an O3DDate value to the bitstream
void BitStream::pushDate(const Date &val)
{
	#ifdef O3D_BITSTREAM_DEBUG
	if(debug)
	{
		UInt8 type = Type_Date;
		addDataDebug(&type,5);
	}
	#endif

	pushUInt16(val.year,12);
	pushUInt8((UInt8)val.month,4);
	pushUInt8((UInt8)val.day,3);
	pushUInt8((UInt8)val.mday,5);
	pushUInt8((UInt8)val.hour,5);
	pushUInt8((UInt8)val.minute,6);
	pushUInt8((UInt8)val.second,6);
	pushUInt8((UInt8)val.millisecond,7);
}

