/**
 * @file png.h
 * @brief Loading/Writing of PNG pictures (portable network graphic).
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-01-23
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_PNG_H
#define _O3D_PNG_H

#include "imgformat.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class Png
//-------------------------------------------------------------------------------------
//! Loading/Writing of PNG pictures (portable network graphic).
//! @note Non grayscale support.
//---------------------------------------------------------------------------------------
class O3D_API Png : public ImgFormat
{
public:

	Png() :
		ImgFormat(),
		m_palbpp(0),
		m_bpp(0),
		m_palette(NULL)
	{}

	virtual ImgFormat* clone() const { return (ImgFormat*)new Png(*this); }

    virtual Bool checkFormat(InStream &is);

    virtual Bool load(InStream &is);
    virtual Bool load(InStream &is, PixelFormat pixelFormat);
    virtual Bool save(OutStream &os, PixelFormat pixelFormat);

	virtual PixelFormat getPixelFormat() const;

protected:

	UInt32 m_palbpp;
	UInt32 m_bpp;
	UInt8 *m_palette;

	Bool convert8to24();
	Bool convert8to32();
    Bool convert16to24();
    Bool convert16to32();
	Bool convert24to32();
	Bool convert32to24();

    Bool loadDefault(InStream &is);
    Bool loadRgb24(InStream &is);
    Bool loadRgba32(InStream &is);
};

} // namespace o3d

#endif // _O3D_PNG_H

