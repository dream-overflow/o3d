/**
 * @file gif.h
 * @brief Loading of Gif 87a/89a
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-19
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_GIF_H
#define _O3D_GIF_H

#include "imgformat.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class Gif
//-------------------------------------------------------------------------------------
//! Loading of Gif 87a/89a
//---------------------------------------------------------------------------------------
class O3D_API Gif : public ImgFormat
{
public:

	static const Int32 GIF_MAX_CODES = 4096; //!< maximum value of a code

	Gif() : ImgFormat(), m_bpp(0) {}

	virtual ImgFormat* clone() const { return (ImgFormat*)new Gif(*this); }

    virtual Bool checkFormat(InStream &is);

    virtual Bool load(InStream &is);
    virtual Bool load(InStream &is, PixelFormat pixelFormat);
    virtual Bool save(OutStream &os, PixelFormat pixelFormat);

	virtual PixelFormat getPixelFormat() const;

protected:

	UInt32 m_bpp;
	
    Bool loadRgb24(InStream &is);
    Bool loadRgba32(InStream &is);

	struct Reader
	{
        Reader(InStream &_is) :
            is(_is),
			buf(0),
			nbits_left(0),
			lbyte(0),
			navail_bytes(0),
			pbytes(NULL)
		{
		}

        InStream &is;

		UInt8  buf;					//!< buffer.
		Int16  nbits_left;			//!< remaining number of bytes.
		UInt8  lbyte;				//!< last read byte.
		Int16  navail_bytes;		//!< availables bytes in pre-buffer.
		UInt8  byte_buff[257];		//!< buffer.
		UInt8* pbytes;				//!< buffer pointer.

		// Read a code if nbits.
		UInt16 readCode(Int16 nbits);
	};
};

} //namespace o3d

#endif // _O3D_GIF_H

