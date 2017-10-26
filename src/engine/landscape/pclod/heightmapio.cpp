/**
 * @file heightmapio.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2006-08-24
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/landscape/pclod/heightmapio.h"

#include "o3d/core/instream.h"
#include "o3d/core/outstream.h"

#include <sstream>

using namespace o3d;

/*===================================================
				Class O3DHeightmapIO
===================================================*/
HeightmapIO * HeightmapIO::m_instance;

/* Return the instance of the singleton */
HeightmapIO * HeightmapIO::instance()
{
	if (m_instance == NULL)
		m_instance = new HeightmapIO();

	return m_instance;
}

/* Release the singleton */
void HeightmapIO::destroy()
{
	if (m_instance)
	{
		delete m_instance;
		m_instance = NULL;
	}
}

HeightmapIO::HeightmapIO():
	m_formatMap()
{
	m_formatMap[O3D_HEIGHTMAP_IO_BASIC] = new HeightmapIOBasic();
	m_formatMap[O3D_HEIGHTMAP_IO_DELTA] = new HeightmapIODelta();
}

HeightmapIO::~HeightmapIO()
{
	for (IT_O3DFormatMap it = m_formatMap.begin() ; it != m_formatMap.end() ; it++)
		deletePtr(it->second);

	m_formatMap.clear();
}

Bool HeightmapIO::writeToFile(OutStream & _os, HeightmapIOFormat _format, Float * _buffer, UInt32 _width, UInt32 _height, const T_HeightmapParams & _params)
{
	IT_O3DFormatMap it = m_formatMap.find(_format);
	O3D_ASSERT(it != m_formatMap.end());

	Char header[5] = { "HMP " };
    _os.write(header, 4);
    _os << UInt32(_format);

    return it->second->writeToFile(_os, _format, _buffer, _width, _height);
}

Bool HeightmapIO::readFromFile(InStream & _is, Float * & _buffer, UInt32 & _width, UInt32 & _height)
{
	Char header[5] = { 0 };
    _is.read(header, 4);
	O3D_ASSERT(strncmp(header, "HMP ", 4) == 0);

	UInt32 format;
    _is >> format;

	HeightmapIOFormat heightmapFormat = HeightmapIOFormat(format);

	IT_O3DFormatMap it = m_formatMap.find(heightmapFormat);
	O3D_ASSERT(it != m_formatMap.end());

    return it->second->readFromFile(_is, _buffer, _width, _height);
}

void HeightmapIO::registerFormat(HeightmapIOFormat _format, HeightmapIOBase * _ptr)
{
	if (_ptr == NULL)
	{
		/* In this case, we erase the existing format */
		IT_O3DFormatMap it = m_formatMap.find(_format);

		if (it != m_formatMap.end())
			m_formatMap.erase(it);
	}
	else
	{
		/* We replace or add the format */
		IT_O3DFormatMap it = m_formatMap.find(_format);

		if (it != m_formatMap.end())
			deletePtr(it->second);

		m_formatMap[_format] = _ptr;
	}
}

/*===================================================
			Class O3DHeightmapIOBase
===================================================*/
HeightmapIOBase::HeightmapIOBase(HeightmapIOFormat _format):
	m_format(_format)
{
}

HeightmapIOBase::~HeightmapIOBase()
{
}

/* Functions */
Bool HeightmapIOBase::writeToFile(OutStream & _os, HeightmapIOFormat _format, Float * _buffer, UInt32 _width, UInt32 _height, const T_HeightmapParams & _params)
{
	return False;
}

Bool HeightmapIOBase::readFromFile(InStream & _is, Float * & _buffer, UInt32 & _width, UInt32 & _height)
{
	return False;
}

/*===================================================
			Class O3DHeightmapIOBasic
===================================================*/
HeightmapIOBasic::HeightmapIOBasic():
	HeightmapIOBase(O3D_HEIGHTMAP_IO_BASIC)
{
}

/* Functions */
Bool HeightmapIOBasic::writeToFile(OutStream & _os, HeightmapIOFormat _format, Float * _buffer, UInt32 _width, UInt32 _height, const T_HeightmapParams & _params)
{
	O3D_ASSERT(_buffer != NULL);
	O3D_ASSERT((_width > 0) && (_height > 0));

    _os << UInt32(getFormat());
    _os << _width;
    _os << _height;

    _os.write(_buffer, _width * _height);
    _os << UInt32(0xFFFFFFFF);	//!< Used to check file consistence

	return True;
}

Bool HeightmapIOBasic::readFromFile(InStream & _is, Float * & _buffer, UInt32 & _width, UInt32 & _height)
{
    O3D_ASSERT(_buffer == nullptr);

	UInt32 format;
    _is >> format;

	if (format != UInt32(O3D_HEIGHTMAP_IO_BASIC))
		return False;

    _is >> _width;
    _is >> _height;

	O3D_ASSERT((_width > 0) && (_height > 0));

	UInt32 size = _width * _height;
	UInt32 check;

	_buffer = new Float[size];
    UInt32 byteRead = _is.read(_buffer, size);

    _is >> check;

	if ((byteRead != size * sizeof(Float)) || (check != 0xFFFFFFFF))
	{
		deleteArray(_buffer);
		_width = 0;
		_height = 0;

		return False;
	}

	return True;
}

/*===================================================
			Class HeightmapIODelta
===================================================*/
Bool HeightmapIODelta::getParamFloat(
	const T_HeightmapParams & _params,
	const String & _paramName,
	Float & _value)
{
	for (CIT_HeightmapParams it = _params.begin() ; it != _params.end() ; it++)
	{
		String paramName = it->first;

        paramName.lower();

		if (paramName == _paramName)
		{
			std::wistringstream iss(it->second.getData());

			Bool ret = ((iss >> _value) && (iss.eof()));

			if (!ret)
				_value = 0.01f;

			return ret;
		}
	}

	_value = 0.01f;	// Default value

	return False;
}

HeightmapIODelta::HeightmapIODelta():
	HeightmapIOBase(O3D_HEIGHTMAP_IO_DELTA)
{
}

/* Functions */
Bool HeightmapIODelta::writeToFile(OutStream & _os, HeightmapIOFormat _format, Float * _buffer, UInt32 _width, UInt32 _height, const T_HeightmapParams & _params)
{
	O3D_ASSERT(0);
	return False;
/*	O3D_ASSERT(_file.isOpenWriting());
	O3D_ASSERT(_buffer != NULL);
	O3D_ASSERT((_width > 0) && (_height > 0));

	Float _precision;

	GetParamFloat(_params, "precision", _precision);

	_file << UInt32(GetFormat());
	_file << _width;
	_file << _height;

	_file << _precision;

    UInt32 pos = _file.getPosition();
	_file << pos;
	_file << _buffer[0];

	Float inv_precision = 1/_precision;
	UInt32 i,j,k;
	for(i = 1, j = 1,k = 0; i < _width * _height; ++i, ++j)
	{
	   if(o3d::abs(_buffer[i]-_buffer[i-1])/_precision > 127.0)
	   {
	      _file << (Int8)128;
		  _file << _buffer[i];
		  k += sizeof(Int8) + sizeof(Float);
		  if(j == _width)
		      j = 1;
	   }
	   else
	   {
		   if(j == _width)
		   {
			  _file << (Int8)((_buffer[i]-_buffer[i-_width])*inv_precision);
			  j = 1;
		   }
	       else
			  _file << (Int8)((_buffer[i]-_buffer[i-1])*inv_precision);
		   ++k;
	   }
	}
	_file.posFromStart(pos);
	_file << k;
	_file.posFromCur(k);
	_file << UInt32(0xFFFFFFFF);	//!< Used to check file consistence

	return True;*/
}

Bool HeightmapIODelta::readFromFile(InStream & _is, Float * & _buffer, UInt32 & _width, UInt32 & _height)
{
	O3D_ASSERT(0);
	return False;
/*	O3D_ASSERT(_file.isOpenReading());
	O3D_ASSERT(_buffer == NULL);

	UInt32 format;
	_file >> format;

	if (format != UInt32(O3D_HEIGHTMAP_IO_DELTA))
		return False;

	_file >> _width;
	_file >> _height;

	Float precision;
	_file >> precision;
	UInt32 size_data;
	_file >> size_data;
	O3D_ASSERT((_width > 0) && (_height > 0));

	UInt32 size = _width * _height;
	UInt32 check;

	_buffer = new Float[size];
	UInt32 size_buf = 256;
	Int8 *temp_buf = new Int8[size_buf];
	UInt32 i,j,k=0,l;
	Int8 temp;
	size_buf = _file.Read<Int8>(temp_buf, sizeof(Int8), size_buf);
	_buffer[0] = *(Float*)temp_buf;
	k += sizeof(Float);
	for( i = 1, j = 1; i < size; ++i, ++j)
	{
	   if(k>size_buf-5)
	   {
		   for(l=0;l<size_buf-k;l++)
		      temp_buf[l]=temp_buf[k+l];
		   if(size_data>size_buf-l)
		   {
		      _file.Read<Int8>(temp_buf+l, sizeof(Int8), size_buf-l);
		      size_data -= size_buf-l;
		   }
		   else
		   {
		      _file.Read<Int8>(temp_buf+l, sizeof(Int8), size_data);
		      size_data = 0;
		   }
		   k = 0;
	   }
	   temp = temp_buf[k];
	   k++;
	   if(temp==128)
	   {
	      _buffer[i] = *(Float*)(temp_buf+k);
		  if(j == _width)
			  j=1;
	      k += sizeof(Float);
	   }
	   else
	   {
	      if(j == _width)
		  {
		     _buffer[i] = _buffer[i-_width] + (Float)temp * precision;
			 j = 1;
		  }
		  else
	         _buffer[i] = _buffer[i-1] + (Float)temp * precision;
	   }
	}

	_file >> check;

	if ((i != size) || (check != 0xFFFFFFFF))
	{
		deleteArray(_buffer);
		_width = 0;
		_height = 0;

		return False;
	}

	return True;*/
}

