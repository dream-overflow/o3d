/**
 * @file tga.h
 * @brief Load of TGA picture (8,24,32 bpp) support of palette and RLE compression.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @author  Romain LOCCI
 * @date 2001-12-23
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_TGA_H
#define _O3D_TGA_H

#include "imgformat.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class Tga
//-------------------------------------------------------------------------------------
//! Load of TGA picture (8,24,32 bpp) support of palette and RLE compression.
//---------------------------------------------------------------------------------------
class O3D_API Tga : public ImgFormat
{
public:

	Tga() : ImgFormat(), m_seekPos(0), m_bpp(0) {}

	virtual ImgFormat* clone() const { return (ImgFormat*)new Tga(*this); }

    virtual Bool checkFormat(InStream &is);

    virtual Bool load(InStream &is);
    virtual Bool load(InStream &is, PixelFormat pixelFormat);
    virtual Bool save(OutStream &os, PixelFormat pixelFormat);

	virtual PixelFormat getPixelFormat() const;

protected:

	Int32 m_seekPos;
	UInt32 m_bpp;

	Bool hFlip();

    Bool loadDefault(InStream &is);
    Bool loadRgb24(InStream &is);
    Bool loadRgba32(InStream &is);
};

} // namespace o3d

#endif // _O3D_TGA_H

