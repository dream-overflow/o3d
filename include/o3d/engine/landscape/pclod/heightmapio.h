/**
 * @file heightmapio.h
 * @brief
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_HEIGHTMAPMANIP_H
#define _O3D_HEIGHTMAPMANIP_H

#include <map>
#include <memory>

#include "o3d/core/smartpointer.h"

namespace o3d {

/* Type definition */
enum HeightmapIOFormat {
	O3D_HEIGHTMAP_IO_UNKNOWN = 0,
	O3D_HEIGHTMAP_IO_BASIC,
	O3D_HEIGHTMAP_IO_DELTA,
	O3D_HEIGHTMAP_IO_LAST};

class HeightmapIOBase;
class InStream;
class OutStream;

typedef std::map<String, String>			T_HeightmapParams;
typedef T_HeightmapParams::iterator			IT_HeightmapParams;
typedef T_HeightmapParams::const_iterator	CIT_HeightmapParams;

const T_HeightmapParams O3D_HEIGHTMAP_IO_DEFAULT_PARAMS = T_HeightmapParams();

/*===================================================
				Class O3DHeightmapIO
===================================================*/
class O3D_API HeightmapIO
{
    friend class AutoPtr<HeightmapIO>;

private:

	/* Internal types */
	typedef std::map<HeightmapIOFormat, HeightmapIOBase*>	T_O3DFormatMap;
	typedef T_O3DFormatMap::iterator						IT_O3DFormatMap;
	typedef T_O3DFormatMap::const_iterator					CIT_O3DFormatMap;

	/* Members */
	T_O3DFormatMap m_formatMap;

	static HeightmapIO * m_instance;

	/* Constructors */
	HeightmapIO();
	~HeightmapIO();

public:

	/* Return the instance of the singleton */
	static HeightmapIO * instance();

	/* Release the singleton */
	static void destroy();

	/* Functions */
    Bool writeToFile(OutStream & _is, HeightmapIOFormat _format, Float * _buffer, UInt32 _width, UInt32 _height, const T_HeightmapParams & _params = O3D_HEIGHTMAP_IO_DEFAULT_PARAMS);
    Bool readFromFile(InStream &_is, Float * & _buffer, UInt32 & _width, UInt32 & _height);

	void registerFormat(HeightmapIOFormat _format, HeightmapIOBase * _ptr);
};


/*===================================================
			Class HeightmapIOBase
===================================================*/
class O3D_API HeightmapIOBase
{
private:

	/* Members */
	HeightmapIOFormat m_format;

public:

	/* Constructors */
	HeightmapIOBase(HeightmapIOFormat _format);
	virtual ~HeightmapIOBase();

	/* Functions */
    virtual Bool writeToFile(OutStream & _is, HeightmapIOFormat _format, Float * _buffer, UInt32 _width, UInt32 _height, const T_HeightmapParams & _params = O3D_HEIGHTMAP_IO_DEFAULT_PARAMS) = 0;
    virtual Bool readFromFile(InStream &_is, Float * & _buffer, UInt32 & _width, UInt32 & _height) = 0;

	HeightmapIOFormat getFormat() const { return m_format; }
};

/*===================================================
			Class HeightmapIOBasic
===================================================*/
class O3D_API HeightmapIOBasic : public HeightmapIOBase
{
public:

	/* Constructors */
	HeightmapIOBasic();

	/* Functions */
    virtual Bool writeToFile(OutStream & _is, HeightmapIOFormat _format, Float * _buffer, UInt32 _width, UInt32 _height, const T_HeightmapParams & _params = O3D_HEIGHTMAP_IO_DEFAULT_PARAMS);
    virtual Bool readFromFile(InStream &_is, Float * & _buffer, UInt32 & _width, UInt32 & _height);
};

class O3D_API HeightmapIODelta : public HeightmapIOBase
{
private:

	/* Restricted */
	Bool getParamFloat(const T_HeightmapParams & _params, const String & _paramName, Float & _value);

public:

	/* Constructors */
	HeightmapIODelta();

	/* Functions */
    virtual Bool writeToFile(OutStream & _is, HeightmapIOFormat _format, Float * _buffer, UInt32 _width, UInt32 _height, const T_HeightmapParams & _params = O3D_HEIGHTMAP_IO_DEFAULT_PARAMS);
    virtual Bool readFromFile(InStream &_is, Float * & _buffer, UInt32 & _width, UInt32 & _height);
};

} // namespace o3d

#endif // _O3D_HEIGHTMAPMANIP_H

