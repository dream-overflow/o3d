/**
 * @file bmp.h
 * @brief Bitmap file loading (1,2,4,8,24 bpp) and writing (24 bpp).
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-10
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_BMP_H
#define _O3D_BMP_H

#include "imgformat.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class Bmp
//---------------------------------------------------------------------------------------
//! bitmap file loading (1,2,4,8,24 bpp) and writing (24 bpp)
//---------------------------------------------------------------------------------------
class O3D_API Bmp : public ImgFormat
{
public:

	Bmp() : ImgFormat(), m_bpp(0) {}

	virtual ImgFormat* clone() const { return (ImgFormat*)new Bmp(*this); }

    virtual Bool checkFormat(InStream &is);

    virtual Bool load(InStream &is);
    virtual Bool load(InStream &is, PixelFormat pixelFormat);
    virtual Bool save(OutStream &os, PixelFormat pixelFormat);

	virtual PixelFormat getPixelFormat() const;

protected:

	UInt32 m_bpp;

    Bool loadDefault(InStream &is);
    Bool loadRgb24(InStream &is);
    Bool loadRgba32(InStream &is);
};

} // namespace o3d

#endif // _O3D_BMP_H

