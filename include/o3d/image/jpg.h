/**
 * @file jpg.h
 * @brief Loading/writing Jpeg/JFif files. No support for GrayScale.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2005-01-23
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_JPG_H
#define _O3D_JPG_H

#include "imgformat.h"
#include "../core/memorydbg.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class Jpg
//-------------------------------------------------------------------------------------
//! Loading/writing Jpeg/JFif files. No support for GrayScale
//---------------------------------------------------------------------------------------
class O3D_API Jpg : public ImgFormat
{
public:

	Jpg() : ImgFormat(), m_bpp(0) {}

	virtual ImgFormat* clone() const { return (ImgFormat*)new Jpg(*this); }

    virtual Bool checkFormat(InStream &is);

    virtual Bool load(InStream &is);
    virtual Bool load(InStream &is, PixelFormat pixelFormat);
    virtual Bool save(OutStream &os, PixelFormat pixelFormat);

	virtual PixelFormat getPixelFormat() const;

private:

	Bool convert24to32();

    Bool loadDefault(InStream &is);
    Bool loadRgb24(InStream &is);
    Bool loadRgba32(InStream &is);

	UInt32 m_bpp;
};

} // namespace o3d

#endif // _O3D_JPG_H

