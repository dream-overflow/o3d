/**
 * @file bitstream.h
 * @brief Bit Stream.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @author  Arnaud DELMOTTE
 * @date 2007-07-03
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_BITSTREAM_H
#define _O3D_BITSTREAM_H

#include "memorydbg.h"
#include "file.h"
#include "smartcounter.h"
#include "templatearray.h"

#define O3D_BITSTREAM_DEFAULT_SIZE 4096  //!< default threshold size of the bitstream array
#define O3D_BITSTREAM_DEBUG

namespace o3d {

class Date;
class DateTime;

//---------------------------------------------------------------------------------------
//! @class BitStream
//-------------------------------------------------------------------------------------
//! Bit Stream array (bits aligned).
//---------------------------------------------------------------------------------------
class O3D_API BitStream : public SmartCounter<BitStream>
{
public:

	//! @brief Default constructor.
	//! @param Allocate The default bitstream initialization maxsize.
	BitStream();

	//! @brief Destructor
	virtual ~BitStream();

	//! @brief Destroy the bit stream
	virtual void destroy();

	//! @brief Push nb_bits data from buf to the BitStream
	void addData(const UInt8 *buf,Int32 nb_bits);
	//! @brief Get nb_bits data from the BitStream to buf
	void getData(UInt8 *buf,Int32 nb_bits);
	//! @brief Push nb_bits data from buf to the debug buffer
	void addDataDebug(const UInt8 *buf,Int32 nb_bits);
	//! @brief Get nb_bits data from the debug buffer to buf
	void getDataDebug(UInt8 *buf,Int32 nb_bits);

	//! @brief Insert a subarray aligned to bytes
	//! @param data The subarray to insert
	//! @param size The size of the data to insert
	//! @param pos the position (in byte) where to insert the data
	inline void insertData(const UInt8 *buf, UInt32 size, UInt32 pos)
	{
		m_Data.insertArray(buf,size,pos);
		m_CurByteWrite = o3d::max(m_CurByteWrite,pos+size-1);
	}

	//! @brief Insert a debug data subarray aligned to bytes
	//! @param data The subarray to insert
	//! @param size The size of the data to insert
	//! @param pos the position (in byte) where to insert the data
	inline void insertDebugData(const UInt8 *buf, UInt32 size, UInt32 pos)
	{
		m_DataDebug.insertArray(buf,size,pos);
		m_CurByteWriteDebug = o3d::max(m_CurByteWriteDebug,pos+size-1);
	}

	//! @brief Check if there are some debugs information
	//! @return True if there are some debugs information
	inline Bool isDebug() const { return (m_Data[0] & 0x01); }

	//! data types for debug information
	enum Data_Types
	{
		Type_Bool,
		Type_Char,
		Type_Int8,
		Type_UInt8,
		Type_Int16,
		Type_UInt16,
		Type_Int32,
		Type_UInt32,
		Type_Float,
		Type_Double,
		Type_String,
        Type_Date,
        Type_DateTime
	};

	//! @brief Return the main data array
	inline const ArrayUInt8& getDataArray() const { return m_Data; }

	//! @brief Return the debug data array
	inline const ArrayUInt8& getDebugDataArray() const { return m_DataDebug; }

	//! return the total size of the bitstream (data+debug part)
	inline Int32 getSize() const
	{
		return m_Data.getSize() + (isDebug() ? m_DataDebug.getSize() : 0);
	}

	//-----------------------------------------------------------------------------------
	// READ
	//-----------------------------------------------------------------------------------

	//! @brief return a 1 bit boolean
	inline Bool getBool()
	{
		#ifdef O3D_BITSTREAM_DEBUG
		if(debug)
		{
			UInt8 type=0;
			getDataDebug(&type,5);
			O3D_ASSERT(type == Type_Bool);
		}
		#endif

		Bool a = 0;
		getData((UInt8*)&a,1);

		return a;
	}

	//! @brief return signed 32 bits integer value from the bit stream
	inline Int32 getInt32(Int32 nb_bits=32)
	{
		#ifdef O3D_BITSTREAM_DEBUG
		if(debug)
		{
			UInt8 type=0,debug_nb_bits=0;
			getDataDebug(&type,5);
			getDataDebug(&debug_nb_bits,6);
			O3D_ASSERT(type == Type_Int32);
			O3D_ASSERT(debug_nb_bits == nb_bits);
		}
		#endif

		Int32 a = 0;
		Bool signe = 0;
		signe = getBool();//GetData((UInt8*)&signe,1);
		getData((UInt8*)&a,nb_bits-1);

		#ifdef O3D_BIG_ENDIAN
        System::swapBytes4(&a);
		#endif

		return signe==0?-a:a;
	}

	//! @brief return unsigned 32 bits integer value from the bit stream
	inline UInt32 getUInt32(Int32 nb_bits=32)
	{
		#ifdef O3D_BITSTREAM_DEBUG
		if(debug)
		{
			UInt8 type=0,debug_nb_bits=0;
			getDataDebug(&type,5);
			getDataDebug(&debug_nb_bits,6);
			O3D_ASSERT(type == Type_UInt32);
			O3D_ASSERT(debug_nb_bits == nb_bits);
		}
		#endif

		UInt32 a = 0;
		getData((UInt8*)&a,nb_bits);

		#ifdef O3D_BIG_ENDIAN
        System::swapBytes4(&a);
		#endif

		return a;
	}

	//! @brief return signed 16 bits integer value from the bit stream
	inline Int16 getInt16(Int32 nb_bits=16)
	{
		#ifdef O3D_BITSTREAM_DEBUG
		if(debug)
		{
			UInt8 type=0,debug_nb_bits=0;
			getDataDebug(&type,5);
			getDataDebug(&debug_nb_bits,5);
			O3D_ASSERT(type == Type_Int16);
			O3D_ASSERT(debug_nb_bits == nb_bits);
		}
		#endif

		Int16 a = 0;
		Bool signe = 0;
		signe = getBool();//GetData((UInt8*)&signe,1);
		getData((UInt8*)&a,nb_bits-1);

		#ifdef O3D_BIG_ENDIAN
        System::swapBytes2(&a);
		#endif

		return signe==0?-a:a;
	}

	//! @brief return unsigned 16 bits integer value from the bit stream
	inline UInt16 getUInt16(Int32 nb_bits=16)
	{
		#ifdef O3D_BITSTREAM_DEBUG
		if(debug)
		{
			UInt8 type=0,debug_nb_bits=0;
			getDataDebug(&type,5);
			getDataDebug(&debug_nb_bits,5);
			O3D_ASSERT(type == Type_UInt16);
			O3D_ASSERT(debug_nb_bits == nb_bits);
		}
		#endif

		UInt16 a = 0;
		getData((UInt8*)&a,nb_bits);

		#ifdef O3D_BIG_ENDIAN
        System::swapBytes2(&a);
		#endif

		return a;
	}

	//! @brief return 32bits float value from the bit stream
	inline Float getFloat()
	{
		#ifdef O3D_BITSTREAM_DEBUG
		if(debug)
		{
			UInt8 type=0;
			getDataDebug(&type,5);
			O3D_ASSERT(type == Type_Float);
		}
		#endif

		Float a = 0;
		getData((UInt8*)&a,32);

		#ifdef O3D_BIG_ENDIAN
        System::swapBytes4(&a);
		#endif

		return a;
	}

	//! @brief return 64bits double value from the bit stream
	inline Double getDouble()
	{
		#ifdef O3D_BITSTREAM_DEBUG
		if(debug)
		{
			UInt8 type=0;
			getDataDebug(&type,5);
			O3D_ASSERT(type == Type_Double);
		}
		#endif

		Double a = 0;
		getData((UInt8*)&a,64);

		#ifdef O3D_BIG_ENDIAN
        System::swapBytes8(&a);
		#endif

		return a;
	}

	//! @brief return signed char value from the bit stream
	inline Char getChar(Int32 nb_bits=8)
	{
		#ifdef O3D_BITSTREAM_DEBUG
		if(debug)
		{
			UInt8 type=0,debug_nb_bits=0;
			getDataDebug(&type,5);
			getDataDebug(&debug_nb_bits,4);
			O3D_ASSERT(type == Type_Char);
			O3D_ASSERT(debug_nb_bits==nb_bits);
		}
		#endif
		Char a = 0;
		Bool signe = 0;
		signe = getBool();//GetData((UInt8*)&signe,1);
		getData((UInt8*)&a,nb_bits-1);

		return signe==0?-a:a;
	}

	//! @brief return signed 8 bits integer value from the bit stream
	inline Int8 getInt8(Int32 nb_bits=8)
	{
		#ifdef O3D_BITSTREAM_DEBUG
		if(debug)
		{
			UInt8 type=0,debug_nb_bits=0;
			getDataDebug(&type,5);
			getDataDebug(&debug_nb_bits,4);
			O3D_ASSERT(type == Type_Int8);
			O3D_ASSERT(debug_nb_bits==nb_bits);
		}
		#endif

		UInt8 a = 0;
		getData((UInt8*)&a,nb_bits);

		return a;
	}

	//! @brief return unsigned 8 bits integer value from the bit stream
	inline UInt8 getUInt8(Int32 nb_bits=8)
	{
		#ifdef O3D_BITSTREAM_DEBUG
		if(debug)
		{
			UInt8 type=0,debug_nb_bits=0;
			getDataDebug(&type,5);
			getDataDebug(&debug_nb_bits,4);
			O3D_ASSERT(type == Type_UInt8);
			O3D_ASSERT(debug_nb_bits==nb_bits);
		}
		#endif

		UInt8 a = 0;
		getData((UInt8*)&a,nb_bits);

		return a;
	}

	//! @brief return String value from the bitstream
	String getString();
	void getString(String &val);

    //! @brief return Date value from the bitstream
    void getDate(Date &val);

    //! @brief return DateTime value from the bitstream
    void getDateTime(DateTime &val);


	//-----------------------------------------------------------------------------------
	// WRITE
	//-----------------------------------------------------------------------------------

	//! @brief push signed 32 bits integer value to the bitstream
    inline void pushInt32(Int32 val, Int32 nb_bits=32)
	{
		#ifdef O3D_BITSTREAM_DEBUG
        if (debug) {
			UInt8 type = Type_Int32;
			addDataDebug(&type,5);
			addDataDebug((UInt8*)&nb_bits,6);
		}
		#endif

		#ifdef O3D_BIG_ENDIAN
        System::swapBytes4(&val);
		#endif

        if (val<0) {
		   pushBool(0);
		   val = -val;
        } else {
            pushBool(1);
        }

		addData((UInt8*)&val,nb_bits-1);
	}

	//! @brief push unsigned 32 bits integer value to the bitstream
	inline void pushUInt32(UInt32 val,Int32 nb_bits=32)
	{
		#ifdef O3D_BITSTREAM_DEBUG
        if(debug) {
			UInt8 type = Type_UInt32;
			addDataDebug(&type,5);
			addDataDebug((UInt8*)&nb_bits,6);
		}
		#endif

		addData((UInt8*)&val,nb_bits);
	}

	//! @brief push signed 16 bits integer value to the bitstream
	inline void pushInt16(Int16 val,Int32 nb_bits=16)
	{
		#ifdef O3D_BITSTREAM_DEBUG
		if(debug)
		{
			UInt8 type = Type_Int16;
			addDataDebug(&type,5);
			addDataDebug((UInt8*)&nb_bits,5);
		}
		#endif

		#ifdef O3D_BIG_ENDIAN
        System::swapBytes2(&val);
		#endif

		if(val<0)
		{
		   pushBool(0);
		   val = -val;
		}
		else pushBool(1);
		addData((UInt8*)&val,nb_bits-1);
	}

 	//! @brief push unsigned 16 bits integer value to the bitstream
	inline void pushUInt16(UInt16 val,Int32 nb_bits=16)
	{
		#ifdef O3D_BITSTREAM_DEBUG
		if(debug)
		{
			UInt8 type = Type_UInt16;
			addDataDebug(&type,5);
			addDataDebug((UInt8*)&nb_bits,5);
		}
		#endif

		addData((UInt8*)&val,nb_bits);
	}

	//! @brief push signed char value to the bitstream
    inline void pushChar(Char val, Int32 nb_bits=8)
	{
		#ifdef O3D_BITSTREAM_DEBUG
        if (debug) {
			UInt8 type = Type_Char;
            addDataDebug(&type, 5);
            addDataDebug((UInt8*)&nb_bits, 4);
		}
		#endif

        if (val < 0) {
		   pushBool(0);
		   val = -val;
        } else {
            pushBool(1);
        }

		addData((UInt8*)&val,nb_bits-1);
	}

	//! @brief push signed 8 bits integer value to the bitstream
	inline void pushInt8(Int8 val,Int32 nb_bits=8)
	{
		#ifdef O3D_BITSTREAM_DEBUG
		if(debug)
		{
			UInt8 type = Type_Int8;
			addDataDebug(&type,5);
			addDataDebug((UInt8*)&nb_bits,4);
		}
		#endif

		if(val<0)
		{
		   pushBool(0);
		   val = -val;
		}
		else pushBool(1);
		addData((UInt8*)&val,nb_bits-1);
	}

	//! @brief push unsigned char value to the bitstream
	inline void pushUInt8(UInt8 val,Int32 nb_bits=8)
	{
		#ifdef O3D_BITSTREAM_DEBUG
		if(debug)
		{
			UInt8 type = Type_UInt8;
			addDataDebug(&type,5);
			addDataDebug((UInt8*)&nb_bits,4);
		}
		#endif

		addData((UInt8*)&val,nb_bits);
	}

	//! push bool value to the bitstream
	inline void pushBool(Bool val)
	{
		#ifdef O3D_BITSTREAM_DEBUG
		if(debug)
		{
			UInt8 type = Type_Bool;
			addDataDebug(&type,5);
		}
		#endif

		addData((UInt8*)&val,1);
	}

	//! @brief push float value to the bitstream
	inline void pushFloat(Float val)
	{
		#ifdef O3D_BITSTREAM_DEBUG
		if(debug)
		{
			UInt8 type = Type_Float;
			addDataDebug(&type,5);
		}
		#endif

		#ifdef O3D_BIG_ENDIAN
        System::swapBytes4(&val);
		#endif

		addData((UInt8*)&val,32);
	}

	//! @brief push float value to the bitstream
	inline void pushDouble(Double val)
	{
		#ifdef O3D_BITSTREAM_DEBUG
		if(debug)
		{
			UInt8 type = Type_Double;
			addDataDebug(&type,5);
		}
		#endif

		#ifdef O3D_BIG_ENDIAN
        System::swapBytes8(&val);
		#endif

		addData((UInt8*)&val,64);
	}

	//! @brief push String value to the bitstream
	void pushString(const String &val);

    //! @brief push a Date value to the bitstream
	void pushDate(const Date &val);

    //! @brief push a DateTime value to the bitstream
    void pushDateTime(const DateTime &val);

	inline void setAllReaded()
	{
       m_CurByteRead = m_CurByteWrite;
       m_CurBitRead = m_CurBitWrite;
	}

private:

    ArrayUInt8 m_Data;          //!< bitstream buffer
    ArrayUInt8 m_DataDebug;	    //!< debug bitstream

	UInt32 m_CurByteWrite;      //!< current octet position to write
	UInt8 m_CurBitWrite;        //!< current bit position of the last written octet

	UInt32 m_CurByteRead;       //!< current octet position to read
	UInt8 m_CurBitRead;         //!< current bit position of the last read octet

	UInt32 m_CurByteWriteDebug; //!< current octet position to write for the debug buffer
	UInt8 m_CurBitWriteDebug;   //!< current bit position of the last written octet for the debug buffer

	UInt32 m_CurByteReadDebug;  //!< current octet position to read for the debug buffer
	UInt8 m_CurBitReadDebug;    //!< current bit position of the last read octet for the debug buffer

	Bool debug;                 //!< True if debug information are avalaibles
};

} // namespace o3d

#endif // _O3D_BITSTREAM_H
