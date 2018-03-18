/**
 * @file image.cpp
 * @brief Implementation of Picture.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-09
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/image/precompiled.h"
#include "o3d/image/image.h"

#include "o3d/image/imgformat.h"
#include "o3d/core/filemanager.h"
#include "o3d/core/resourcemanager.h"

#include "o3d/image/bmp.h"
#include "o3d/image/dds.h"
#include "o3d/image/gif.h"
#include "o3d/image/jpg.h"
#include "o3d/image/png.h"
#include "o3d/image/tga.h"

using namespace o3d;

Image::Image() :
	SmartCounter<Image>(),
	m_state(False),
	m_width(0),
	m_height(0),
	m_pixelFormat(PF_RGBA_8),
    m_pic(nullptr),
    m_data(nullptr),
	m_size(0),
	m_isDirty(False)
{
}

Image::Image(const String& filename) :
	SmartCounter<Image>(),
	m_state(False),
	m_width(0),
	m_height(0),
	m_pixelFormat(PF_RGBA_8),
    m_pic(nullptr),
    m_data(nullptr),
	m_size(0),
	m_isDirty(False)
{
	load(filename);
}

Image::Image(const String& filename, PixelFormat pixelFormat) :
	SmartCounter<Image>(),
	m_state(False),
	m_width(0),
	m_height(0),
	m_pixelFormat(PF_RGBA_8),
    m_pic(nullptr),
    m_data(nullptr),
	m_size(0),
	m_isDirty(False)
{
	load(filename, pixelFormat);
}

Image::Image(InStream &is) :
	SmartCounter<Image>(),
	m_state(False),
	m_width(0),
	m_height(0),
	m_pixelFormat(PF_RGBA_8),
    m_pic(nullptr),
    m_data(nullptr),
	m_size(0),
	m_isDirty(False)
{
    load(is);
}

Image::Image(InStream &is, PixelFormat pixelFormat) :
	SmartCounter<Image>(),
	m_state(False),
	m_width(0),
	m_height(0),
	m_pixelFormat(PF_RGBA_8),
    m_pic(nullptr),
    m_data(nullptr),
	m_size(0),
	m_isDirty(False)
{
    load(is, pixelFormat);
}

Image::Image(const Image &pic) :
	SmartCounter<Image>(pic),
	m_state(False),
	m_width(0),
	m_height(0),
	m_pixelFormat(PF_RGBA_8),
    m_pic(nullptr),
    m_data(nullptr),
	m_size(0),
	m_isDirty(False)
{
	if (!pic.m_state)
		return;

	m_pixelFormat = pic.m_pixelFormat;
	m_isDirty = pic.m_isDirty;

	// the data is external
	if (pic.m_pic.get())
	{
		UInt8 *data;

		// shallow copy
		m_pic = pic.m_pic;
		m_pic->getInfo(m_width,m_height,data,m_size);
	}
	// the data is internal to Image
	else if (pic.m_data.get())
	{
		m_width = pic.m_width;
		m_height = pic.m_height;

		m_size = pic.m_size;

		// shallow copy
		m_data = pic.m_data;
	}

	m_state = True;
}

Image::~Image()
{
	destroy();
}

// duplicate
Image& Image::operator= (const Image &pic)
{
	if (m_state)
        destroy();

	m_pixelFormat = pic.m_pixelFormat;
	m_isDirty = pic.m_isDirty;

	if (!pic.m_state)
	{
        m_pic = nullptr;
		m_width = m_height = m_size = 0;
        m_data = nullptr;

		m_state = False;

		return *this;
	}

	// the data is external
	if (pic.m_pic.get())
	{
		UInt8 *data;

		// shallow copy
		m_pic = pic.m_pic;
		m_pic->getInfo(m_width,m_height,data,m_size);
	}
	// the data is internal to Image
	else if (pic.m_data.get())
	{
		m_width = pic.m_width;
		m_height = pic.m_height;

		m_size = pic.m_size;

		// shallow copy
		m_data = pic.m_data;
	}

	m_state = True;
	return *this;
}

// load a picture given its filename
Bool Image::load(const String &filename)
{
	if (m_state)
		destroy();

    InStream* is = FileManager::instance()->openInStream(filename);

    Bool ret = load(*is);
    deletePtr(is);

    return ret;
}

// load a picture given its file
Bool Image::load(InStream &is)
{
	if (m_state)
		destroy();

	Char id[6];

    is.read(id, 6);   // file type
    is.seek(-6);      // we have read nothing

	UInt8 *data;

	// BMP
	if ((id[0] == 'B') && (id[1] == 'M'))
	{
		m_pic = new Bmp;

        m_state = m_pic->load(is);
		m_pic->getInfo(m_width,m_height,data,m_size);

		m_pixelFormat = m_pic->getPixelFormat();
		m_data = new ImageData(data);

		// no longer needed
		m_pic->detach();
        m_pic = nullptr;
	}
	// GIF format 87a/89a
	else if ( !strncmp(id,"GIF87a",6) || !strncmp(id,"GIF89a",6))
	{
		m_pic = new Gif;

        m_state = m_pic->load(is);
		m_pic->getInfo(m_width,m_height,data,m_size);

		m_pixelFormat = m_pic->getPixelFormat();
		m_data = new ImageData(data);

		// no longer needed
        m_pic = nullptr;
	}
	// TGA
	else if ((id[2] == 1) || (id[2] == 2) || (id[2] == 3) || (id[2] == 9) || (id[2] == 10) || (id[2] == 11))
	{
		m_pic = new Tga;

        m_state = m_pic->load(is);
		m_pic->getInfo(m_width,m_height,data,m_size);

		m_pixelFormat = m_pic->getPixelFormat();
		m_data = new ImageData(data);

		// no longer needed
		m_pic->detach();
        m_pic = nullptr;
	}
	// JPEG
	else if (((UInt8)id[0] == 0xff) && ((UInt8)id[1] == 0xd8))
	{
		m_pic = new Jpg;

        m_state = m_pic->load(is);
		m_pic->getInfo(m_width,m_height,data,m_size);

		m_pixelFormat = m_pic->getPixelFormat();
		m_data = new ImageData(data);

		// no longer needed
		m_pic->detach();
        m_pic = nullptr;
	}
	// PNG
	else if (((UInt8)id[0] == 0x89) && ((UInt8)id[1] == 'P') && ((UInt8)id[2] == 'N') &&
			((UInt8)id[3] == 'G')  && ((UInt8)id[4] == 0x0d)&& ((UInt8)id[5] == 0x0a))
	{
		m_pic = new Png;

        m_state = m_pic->load(is);
		m_pic->getInfo(m_width,m_height,data,m_size);

		m_pixelFormat = m_pic->getPixelFormat();
		m_data = new ImageData(data);

		// no longer needed
		m_pic->detach();
        m_pic = nullptr;
	}
	// DDS
	else if (((UInt8)id[0] == 'D') && ((UInt8)id[1] == 'D') && ((UInt8)id[2] == 'S') &&
			 ((UInt8)id[3] == ' '))
	{
		m_pic = new DdsImg;

        m_state = m_pic->load(is);
		m_pic->getInfo(m_width,m_height,data,m_size);

		m_pixelFormat = m_pic->getPixelFormat();
        //m_data = ImageData(nullptr);
	}
	else	// unknown
	{
		m_state = False;
        O3D_ERROR(E_InvalidFormat("Unsupported or invalid picture format "));
	}

	m_isDirty = False;
	return m_state;
}

// load a picture given its filename
Bool Image::load(const String &filename, PixelFormat pixelFormat)
{
    InStream *is = FileManager::instance()->openInStream(filename);

    Bool ret = load(*is, pixelFormat);
    deletePtr(is);

    return ret;
}

// load a picture given its file
Bool Image::load(InStream &is, PixelFormat pixelFormat)
{
	if (m_state)
		destroy();

	Char id[6];

    is.read(id, 6);  // file type
    is.seek(-6);     // we have read nothing

	UInt8 *data;

	// BMP
	if ((id[0] == 'B') && (id[1] == 'M'))
	{
		m_pic = new Bmp;

        m_state = m_pic->load(is, pixelFormat);
		m_pic->getInfo(m_width,m_height,data,m_size);

		m_pixelFormat = m_pic->getPixelFormat();
		m_data = new ImageData(data);

		// no longer needed
		m_pic->detach();
        m_pic = nullptr;
	}
	// GIF format 87a/89a
	else if ( !strncmp(id,"GIF87a",6) || !strncmp(id,"GIF89a",6))
	{
		m_pic = new Gif;

        m_state = m_pic->load(is, pixelFormat);
		m_pic->getInfo(m_width,m_height,data,m_size);

		m_pixelFormat = m_pic->getPixelFormat();
		m_data = new ImageData(data);

		// no longer needed
        m_pic = nullptr;
	}
	// TGA
	else if ((id[2] == 1) || (id[2] == 2) || (id[2] == 3) || (id[2] == 9) || (id[2] == 10) || (id[2] == 11))
	{
		m_pic = new Tga;

        m_state = m_pic->load(is, pixelFormat);
		m_pic->getInfo(m_width,m_height,data,m_size);

		m_pixelFormat = m_pic->getPixelFormat();
		m_data = new ImageData(data);

		// no longer needed
		m_pic->detach();
        m_pic = nullptr;
	}
	// JPEG
	else if (((UInt8)id[0] == 0xff) && ((UInt8)id[1] == 0xd8))
	{
		m_pic = new Jpg;

        m_state = m_pic->load(is, pixelFormat);
		m_pic->getInfo(m_width,m_height,data,m_size);

		m_pixelFormat = m_pic->getPixelFormat();
		m_data = new ImageData(data);

		// no longer needed
		m_pic->detach();
        m_pic = nullptr;
	}
	// PNG
	else if (((UInt8)id[0] == 0x89) && ((UInt8)id[1] == 'P') && ((UInt8)id[2] == 'N') &&
			((UInt8)id[3] == 'G')  && ((UInt8)id[4] == 0x0d)&& ((UInt8)id[5] == 0x0a))
	{
		m_pic = new Png;

        m_state = m_pic->load(is, pixelFormat);
		m_pic->getInfo(m_width,m_height,data,m_size);

		m_pixelFormat = m_pic->getPixelFormat();
		m_data = new ImageData(data);

		// no longer needed
		m_pic->detach();
        m_pic = nullptr;
	}
	// DDS
	else if (((UInt8)id[0] == 'D') && ((UInt8)id[1] == 'D') && ((UInt8)id[2] == 'S') &&
			 ((UInt8)id[3] == ' '))
	{
		m_pic = new DdsImg;

        m_state = m_pic->load(is, pixelFormat);
		m_pic->getInfo(m_width,m_height,data,m_size);

		m_pixelFormat = m_pic->getPixelFormat();
        //m_data = ImageData(nullptr);
	}
	else	// unknown
	{
		m_state = False;
        O3D_ERROR(E_InvalidFormat("Unsupported or invalid picture format"));
	}

	m_isDirty = False;
	return m_state;
}

// write the picture
Bool Image::save(
	const String &filename,
	FileFormat format,
	Int32 quality)
{
	if (!m_state)
		O3D_ERROR(E_InvalidPrecondition("Picture must be valid"));

	if (isCompressed())
		O3D_ERROR(E_InvalidFormat("Works only with non compressed picture"));

	if (isComplex())
		O3D_ERROR(E_InvalidFormat("Complex picture are not supported at this time"));

	Bool result;

	switch (format)
	{
		case BMP:
			result = saveRgbBmp(filename);
			break;
		case JPEG:
			result = saveJpg(filename, quality);
			break;
		case PNG:
			result = savePng(filename);
			break;
		default:
			O3D_ERROR(E_InvalidParameter("This format is not supported"));
	}

	return result;
}

// write the picture in 24bpp
Bool Image::saveRgb(
	const String &filename,
	FileFormat format,
	Int32 quality)
{
	if (!m_state)
		O3D_ERROR(E_InvalidPrecondition("Picture must be valid"));

	if (isCompressed())
		O3D_ERROR(E_InvalidFormat("Works only with non compressed picture"));

	if (isComplex())
		O3D_ERROR(E_InvalidFormat("Complex picture are not supported at this time"));

	Bool result;

    switch (format) {
		case BMP:
			result = saveRgbBmp(filename);
			break;
		case JPEG:
			result = saveRgbJpg(filename, quality);
			break;
		case PNG:
			result = saveRgbPng(filename);
			break;
		default:
			O3D_ERROR(E_InvalidParameter("This format is not supported"));
			return False;
	}

	return result;
}

// Get the size of a pixel in bits.
UInt32 Image::getBpp() const
{
    switch (m_pixelFormat) {
		case PF_RED_8:
        case PF_RED_U8:
        case PF_RED_I8:
		case PF_RGBA_DXT3:
		case PF_RGBA_DXT5:
			return 8;

		case PF_RGBA_8:
        case PF_RGBA_U8:
        case PF_RGBA_I8:
		case PF_RED_F32:
			return 32;

        case PF_RG_8:
        case PF_RG_U8:
        case PF_RG_I8:
		case PF_DEPTH_U16:
			return 16;

		case PF_RG_F32:
        case PF_RGBA_F16:
			return 64;

		case PF_RGB_8:
        case PF_RGB_U8:
        case PF_RGB_I8:
			return 24;

		case PF_RGB_F32:
			return 96;

		case PF_RGB_DXT1:
		case PF_RGBA_DXT1:
			return 4;

		case PF_RGBA_F32:
			return 128;

		case PF_DEPTH:
		case PF_DEPTH_U24:
			return 24;

		case PF_DEPTH_U32:
		case PF_DEPTH_F32:
		case PF_DEPTH_U24_STENCIL_U8:
			return 32;

		default:
			O3D_ERROR(E_InvalidParameter("Unknown pixel format"));
			return 0;
	}
}

// Get the number of components of the picture.
UInt32 Image::getNumComponents() const
{
    switch (m_pixelFormat) {
		case PF_RED_8:
        case PF_RED_U8:
        case PF_RED_I8:
        case PF_RED_16:
        case PF_RED_U16:
        case PF_RED_I16:
        case PF_RED_U32:
        case PF_RED_I32:
        case PF_RED_F16:
		case PF_RED_F32:
			return 1;

        case PF_RG_8:
        case PF_RG_U8:
        case PF_RG_I8:
        case PF_RG_16:
        case PF_RG_U16:
        case PF_RG_I16:
        case PF_RG_U32:
        case PF_RG_I32:
        case PF_RG_F16:
		case PF_RG_F32:
			return 2;

		case PF_RGB_8:
        case PF_RGB_U8:
        case PF_RGB_I8:
        case PF_RGB_U16:
        case PF_RGB_I16:
        case PF_RGB_U32:
        case PF_RGB_I32:
        case PF_RGB_F16:
		case PF_RGB_F32:
		case PF_RGB_DXT1:
			return 3;

		case PF_RGBA_8:
        case PF_RGBA_U8:
        case PF_RGBA_I8:
        case PF_RGBA_16:
        case PF_RGBA_U16:
        case PF_RGBA_I16:
        case PF_RGBA_U32:
        case PF_RGBA_I32:
        case PF_RGBA_F16:
		case PF_RGBA_F32:
		case PF_RGBA_DXT1:
		case PF_RGBA_DXT3:
		case PF_RGBA_DXT5:
			return 4;

		case PF_DEPTH:
		case PF_DEPTH_U16:
		case PF_DEPTH_U24:
		case PF_DEPTH_U32:
		case PF_DEPTH_F32:
			return 1;

        case PF_DEPTH_U24_STENCIL_U8:
        case PF_DEPTH_F32_STENCIL_U8:
			return 2;

		default:
			O3D_ERROR(E_InvalidParameter("Unknown pixel format"));
			return 0;
	}
}

// Is alpha channel
Bool Image::isAlpha() const
{
    switch (m_pixelFormat) {
//        case PF_RED_8:
//        case PF_RED_U8:
//        case PF_RED_I8:
//        case PF_RED_16:
//        case PF_RED_U16:
//        case PF_RED_I16:
//        case PF_RED_U32:
//        case PF_RED_I32:
//        case PF_RED_F16:
//        case PF_RED_F32:
        case PF_RG_8:
        case PF_RG_U8:
        case PF_RG_I8:
        case PF_RG_U16:
        case PF_RG_I16:
        case PF_RG_F16:
		case PF_RG_F32:
		case PF_RGBA_8:
        case PF_RGBA_U8:
        case PF_RGBA_I8:
        case PF_RGBA_U16:
        case PF_RGBA_I16:
        case PF_RGBA_F16:
		case PF_RGBA_F32:
		case PF_RGBA_DXT1:
		case PF_RGBA_DXT3:
		case PF_RGBA_DXT5:
			return True;

		default:
			return False;
	}
}

//! Is rgb color components.
Bool Image::isRgb() const
{
    switch (m_pixelFormat) {
		case PF_RGB_8:
        case PF_RGB_U8:
        case PF_RGB_I8:
        case PF_RGB_I16:
        case PF_RGB_U16:
        case PF_RGB_I32:
        case PF_RGB_U32:
        case PF_RGB_F16:
		case PF_RGB_F32:
		case PF_RGBA_8:
        case PF_RGBA_U8:
        case PF_RGBA_I8:
        case PF_RGBA_16:
        case PF_RGBA_U16:
        case PF_RGBA_I16:
        case PF_RGBA_U32:
        case PF_RGBA_I32:
        case PF_RGBA_F16:
		case PF_RGBA_F32:
		case PF_RGB_DXT1:
		case PF_RGBA_DXT1:
		case PF_RGBA_DXT3:
		case PF_RGBA_DXT5:
			return True;

		default:
			return False;
	}
}

// Is color picture with 8 bits unsigned integer components.
Bool Image::isColorU8() const
{
    switch (m_pixelFormat) {
		case PF_RED_8:
        case PF_RED_U8:
        case PF_RG_8:
        case PF_RG_U8:
        case PF_RGB_8:
        case PF_RGB_U8:
		case PF_RGBA_8:
        case PF_RGBA_U8:
			return True;

		default:
			return False;
	}
}

// Is color picture with 32 bits float components.
Bool Image::isColorF32() const
{
    switch (m_pixelFormat) {
		case PF_RED_F32:
		case PF_RG_F32:
		case PF_RGB_F32:
		case PF_RGBA_F32:
			return True;

		default:
			return False;
	}
}

// delete the picture
void Image::destroy()
{
	if (m_state)
	{
		// remove shared data
		if (m_pic.get())
            m_pic = nullptr;
		else if (m_data.get())
            m_data = nullptr;

		m_pixelFormat = PF_RGBA_8;

		m_width = m_height = 0;
		m_size = 0;

		m_isDirty = False;
		m_state = False;
	}
}

// Get a pixel
Rgb Image::getRgbPixel(UInt32 x, UInt32 y) const
{
	O3D_ASSERT(x < m_width);
	O3D_ASSERT(y < m_height);

	UInt32 index = x + y * m_width;
	const UInt8* data = getData();

	switch (m_pixelFormat)
	{
		case PF_RED_8:
        case PF_RED_U8:
			return o3d::rgb(data[index], 0, 0);
		case PF_RED_F32:
			return o3d::rgb(UInt8(reinterpret_cast<const Float*>(data)[index]*255.f), 0, 0);

        case PF_RG_8:
        case PF_RG_U8:
			index *= 2;
			return o3d::rgb(data[index], data[index+1], 0);
		case PF_RG_F32:
			index *= 2;
			return o3d::rgb(
					UInt8(reinterpret_cast<const Float*>(data)[index]*255.f),
					UInt8(reinterpret_cast<const Float*>(data)[index+1]*255.f),
					0);

		case PF_RGB_8:
        case PF_RGB_U8:
			index *= 3;
			return o3d::rgb(data[index], data[index+1], data[index+2]);
		case PF_RGB_F32:
			index *= 3;
			return o3d::rgb(
					UInt8(reinterpret_cast<const Float*>(data)[index]*255.f),
					UInt8(reinterpret_cast<const Float*>(data)[index+1]*255.f),
					UInt8(reinterpret_cast<const Float*>(data)[index+2]*255.f));
		case PF_RGB_DXT1:
			O3D_ASSERT(0); // TODO
			index = index >> 4;
			return o3d::rgb(0,0,0);

		case PF_RGBA_8:
        case PF_RGBA_U8:
			index *= 4;
			return o3d::rgba(data[index], data[index+1], data[index+2], data[index+3]);
		case PF_RGBA_F32:
			index *= 4;
			return o3d::rgba(
					UInt8(reinterpret_cast<const Float*>(data)[index]*255.f),
					UInt8(reinterpret_cast<const Float*>(data)[index+1]*255.f),
					UInt8(reinterpret_cast<const Float*>(data)[index+2]*255.f),
					UInt8(reinterpret_cast<const Float*>(data)[index+3]*255.f));
		case PF_RGBA_DXT1:
			O3D_ASSERT(0); // TODO
			index = index >> 4;
			return o3d::rgba(0,0,0,0);
		case PF_RGBA_DXT3:
			O3D_ASSERT(0); // TODO
			index = index >> 3;
			return o3d::rgba(0,0,0,0);
		case PF_RGBA_DXT5:
			O3D_ASSERT(0); // TODO
			index = index >> 3;
			return o3d::rgba(0,0,0,0);

		case PF_DEPTH_U16:
			index *= 2;
			return o3d::rgba(data[index], data[index+1], 0, 0);
		case PF_DEPTH:
		case PF_DEPTH_U24:
			index *= 3;
			return o3d::rgba(data[index], data[index+1], data[index+2], 0);

		case PF_DEPTH_U32:
			index *= 4;
			return o3d::rgba(data[index], data[index+1], data[index+2], data[index+3]);

		case PF_DEPTH_F32:
			index *= 4;
			return o3d::rgba(
					UInt8(reinterpret_cast<const Float*>(data)[index]*255.f),
					UInt8(reinterpret_cast<const Float*>(data)[index+1]*255.f),
					UInt8(reinterpret_cast<const Float*>(data)[index+2]*255.f),
					UInt8(reinterpret_cast<const Float*>(data)[index+3]*255.f));

		case PF_DEPTH_U24_STENCIL_U8:
			index *= 4;
			return o3d::rgba(data[index], data[index+1], data[index+2], data[index+3]);

		default:
			O3D_ERROR(E_InvalidParameter("Unknown pixel format"));
			return 0;
	}
}

// Get a pixel.
Color Image::getPixel(UInt32 x, UInt32 y) const
{
	O3D_ASSERT(x < m_width);
	O3D_ASSERT(y < m_height);

	UInt32 index = x + y * m_width;
	const UInt8* data = getData();

	switch (m_pixelFormat)
	{
		case PF_RED_8:
        case PF_RED_U8:
			return Color(data[index]/255.f, 0.f, 0.f);
		case PF_RED_F32:
			return Color(reinterpret_cast<const Float*>(data)[index], 0.f, 0.f);

        case PF_RG_8:
        case PF_RG_U8:
			index *= 2;
			return Color(data[index]/255.f, data[index+1]/255.f, 0.f);
		case PF_RG_F32:
			index *= 2;
			return Color(
					reinterpret_cast<const Float*>(data)[index],
					reinterpret_cast<const Float*>(data)[index+1],
					0.f);

		case PF_RGB_8:
        case PF_RGB_U8:
			index *= 3;
			return Color(data[index]/255.f, data[index+1]/255.f, data[index+2]/255.f);
		case PF_RGB_F32:
			index *= 3;
			return Color(
					reinterpret_cast<const Float*>(data)[index],
					reinterpret_cast<const Float*>(data)[index+1],
					reinterpret_cast<const Float*>(data)[index+2]);
		case PF_RGB_DXT1:
			O3D_ASSERT(0); // TODO
			index = index >> 4;
			return Color(0,0,0);

		case PF_RGBA_8:
        case PF_RGBA_U8:
			index *= 4;
			return Color(data[index]/255.f, data[index+1]/255.f, data[index+2]/255.f, data[index+3]/255.f);
		case PF_RGBA_F32:
			index *= 4;
			return Color(
					reinterpret_cast<const Float*>(data)[index],
					reinterpret_cast<const Float*>(data)[index+1],
					reinterpret_cast<const Float*>(data)[index+2],
					reinterpret_cast<const Float*>(data)[index+3]);
		case PF_RGBA_DXT1:
			O3D_ASSERT(0); // TODO
			index = index >> 4;
			return Color(0,0,0,0);
		case PF_RGBA_DXT3:
			O3D_ASSERT(0); // TODO
			index = index >> 3;
			return Color(0,0,0,0);
		case PF_RGBA_DXT5:
			O3D_ASSERT(0); // TODO
			index = index >> 3;
			return Color(0,0,0,0);

		case PF_DEPTH_U16:
			index *= 2;
			return Color(data[index]/255.f, data[index+1]/255.f, 0, 0);
		case PF_DEPTH:
		case PF_DEPTH_U24:
			index *= 3;
			return Color(data[index]/255.f, data[index+1]/255.f, data[index+2]/255.f, 0);

		case PF_DEPTH_U32:
			index *= 4;
			return Color(data[index]/255.f, data[index+1]/255.f, data[index+2]/255.f, data[index+3]/255.f);

		case PF_DEPTH_F32:
			index *= 4;
			return Color(
					reinterpret_cast<const Float*>(data)[index],
					reinterpret_cast<const Float*>(data)[index+1],
					reinterpret_cast<const Float*>(data)[index+2],
					reinterpret_cast<const Float*>(data)[index+3]);

		case PF_DEPTH_U24_STENCIL_U8:
			index *= 4;
			return Color(data[index]/255.f, data[index+1]/255.f, data[index+2]/255.f, data[index+3]/255.f);

		default:
			O3D_ERROR(E_InvalidParameter("Unknown pixel format"));
			return 0;
	}
}

// convert R to RGB
Bool Image::convertRtoRGB8()
{
	if (!m_state)
		O3D_ERROR(E_InvalidPrecondition("Picture must be valid"));

	if (isComplex())
		O3D_ERROR(E_InvalidFormat("Complex picture are not supported at this time"));

	if ((m_pixelFormat != PF_RED_8) && (m_pixelFormat != PF_RED_F32) && (m_pixelFormat != PF_DEPTH_F32))
		O3D_ERROR(E_InvalidFormat("Pixel format must be RED_U8, RED_F32 or DEPTH_F32"));

	if (!m_data.get() || !m_data->data || !m_width || !m_height)
		return False;

	m_size = m_width * m_height * 3;

	UInt8 *data = new UInt8[m_size];

	if (m_pixelFormat == PF_RED_8)
	{
		UInt8 *src = m_data->data;
		UInt8 *dst = data;
		UInt8 *end = dst + m_size;

		while (dst < end)
		{
			*dst++ = *src;
			*dst++ = *src;
			*dst++ = *src++;
		}
	}
	else if ((m_pixelFormat == PF_DEPTH_F32) || (m_pixelFormat == PF_RED_F32))
	{
		Float *src = (Float*)m_data->data;
		UInt8 *dst = data;
		UInt8 *end = dst + m_size;
        /*register*/ UInt8 p;

		while (dst < end)
		{
			p = (UInt8)(*src++ * 255.f);

			*dst++ = p;
			*dst++ = p;
			*dst++ = p;
		}
	}

	m_data = new ImageData(data);
	m_pixelFormat = PF_RGB_8;

	setDirty();
	return True;
}

// convert R to RGBA
Bool Image::convertRtoRGBA8()
{
	if (!m_state)
		O3D_ERROR(E_InvalidPrecondition("Picture must be valid"));

	if (isComplex())
		O3D_ERROR(E_InvalidFormat("Complex picture are not supported at this time"));

	if ((m_pixelFormat != PF_RED_8) && (m_pixelFormat != PF_RED_F32) && (m_pixelFormat != PF_DEPTH_F32))
		O3D_ERROR(E_InvalidFormat("Pixel format must be RED_U8, RED_F32 or DEPTH_F32"));

	if (!m_data.get() || !m_data->data || !m_width || !m_height)
		return False;

	m_size = m_width * m_height * 4;

	UInt8 *data = new UInt8[m_size];

	if (m_pixelFormat == PF_RED_8)
	{
		UInt8 *src = m_data->data;
		UInt8 *dst = data;
		UInt8 *end = dst + m_size;

		while (dst < end)
		{
			*dst++ = *src;
			*dst++ = *src;
			*dst++ = *src++;
			*dst++ = 255;
		}
	}
	else if ((m_pixelFormat == PF_DEPTH_F32) || (m_pixelFormat == PF_RED_F32))
	{
		Float *src = (Float*)m_data->data;
		UInt8 *dst = data;
		UInt8 *end = dst + m_size;
        /*register*/ UInt8 p;

		while (dst < end)
		{
			p = (UInt8)(*src++ * 255.f);

			*dst++ = p;
			*dst++ = p;
			*dst++ = p;
			*dst++ = 255;
		}
	}

	m_data = new ImageData(data);
	m_pixelFormat = PF_RGBA_8;

	setDirty();
	return True;
}

// convert R to RGBA
Bool Image::convertRtoRGBA8(Rgb colorKey)
{
	if (!m_state)
		O3D_ERROR(E_InvalidPrecondition("Picture must be valid"));

	if (isComplex())
		O3D_ERROR(E_InvalidFormat("Complex picture are not supported at this time"));

	if ((m_pixelFormat != PF_RED_8) && (m_pixelFormat != PF_RED_F32) && (m_pixelFormat != PF_DEPTH_F32))
		O3D_ERROR(E_InvalidFormat("Pixel format must be RED_U8, RED_F32 or DEPTH_F32"));

	if (!m_data.get() || !m_data->data || !m_width || !m_height)
		return False;

	m_size = m_width * m_height * 4;

	UInt8 *data = new UInt8[m_size];

	const UInt8 r = o3d::red(colorKey);

	if (m_pixelFormat == PF_RED_8)
	{
		UInt8 *src = m_data->data;
		UInt8 *dst = data;
		UInt8 *end = dst + m_size;

		while (dst < end)
		{
			*dst++ = *src;
			*dst++ = *src;
			*dst++ = *src;

			if (*src++ == r)
				*dst++ = 0;
			else
				*dst++ = 255;
		}
	}
	else if ((m_pixelFormat == PF_DEPTH_F32) || (m_pixelFormat == PF_RED_F32))
	{
		Float *src = (Float*)m_data->data;
		UInt8 *dst = data;
		UInt8 *end = dst + m_size;
        /*register*/ UInt8 p;

		while (dst < end)
		{
			p = (UInt8)(*src++ * 255.f);

			*dst++ = p;
			*dst++ = p;
			*dst++ = p;

			if (p == r)
				*dst++ = 0;
			else
				*dst++ = 255;
		}
	}

	m_data = new ImageData(data);
	m_pixelFormat = PF_RGBA_8;

	setDirty();
	return True;
}

// convert RGB to RGBA
Bool Image::convertRGBtoRGBA8()
{
	if (!m_state)
		O3D_ERROR(E_InvalidPrecondition("Picture must be valid"));

	if (isComplex())
		O3D_ERROR(E_InvalidFormat("Complex picture are not supported at this time"));

	if ((m_pixelFormat != PF_RGB_8) && (m_pixelFormat != PF_RGB_F32))
		O3D_ERROR(E_InvalidFormat("Pixel format must be RGB_U8 or RGB_F32"));

	if (!m_data.get() || !m_data->data || !m_width || !m_height)
		return False;


	m_size = m_width * m_height * 4;

	UInt8 *data = new UInt8[m_size];
	UInt32 i,j = 0;

	if (m_pixelFormat == PF_RGB_8)
	{
		UInt8 *src = m_data->data;

		for ( i = 0 ; i < m_size ; i += 4 )
		{
			data[i  ] = src[j];
			data[i+1] = src[j+1];
			data[i+2] = src[j+2];
			data[i+3] = 255;

			j += 3;
		}
	}
	else if (m_pixelFormat == PF_RGB_F32)
	{
		Float *src = (Float*)m_data->data;

		for ( i = 0 ; i < m_size ; i += 4 )
		{
			data[i  ] = (UInt8)(src[j]   * 255.f);
			data[i+1] = (UInt8)(src[j+1] * 255.f);
			data[i+2] = (UInt8)(src[j+2] * 255.f);
			data[i+3] = 255;

			j += 3;
		}
	}

	m_data = new ImageData(data);
	m_pixelFormat = PF_RGBA_8;

	setDirty();
	return True;
}

// convert RGB to RGBA
Bool Image::convertRGBtoRGBA8(Rgb colorKey)
{
	if (!m_state)
		O3D_ERROR(E_InvalidPrecondition("Picture must be valid"));

	if (isComplex())
		O3D_ERROR(E_InvalidFormat("Complex picture are not supported at this time"));

	if ((m_pixelFormat != PF_RGB_8) && (m_pixelFormat != PF_RGB_F32))
		O3D_ERROR(E_InvalidFormat("Pixel format must be RGB_U8 or RGB_F32"));

	if (!m_data.get() || !m_data->data || !m_width || !m_height)
		return False;

	m_size = m_width * m_height * 4;

	UInt8 *data = new UInt8[m_size];
	UInt32 i,j = 0;

	UInt8 r = o3d::red(colorKey);
	UInt8 g = o3d::green(colorKey);
	UInt8 b = o3d::blue(colorKey);

	if (m_pixelFormat == PF_RGB_8)
	{
		UInt8 *src = m_data->data;

		for ( i = 0 ; i < m_size ; i += 4 )
		{
			data[i  ] = src[j];
			data[i+1] = src[j+1];
			data[i+2] = src[j+2];
			data[i+3] = 255;

			if ((data[i  ] == r) &&
				(data[i+1] == g) &&
				(data[i+2] == b))
				data[i+3] = 0;
			else
				data[i+3] = 255;

			j += 3;
		}
	}
	else if (m_pixelFormat == PF_RGB_F32)
	{
		Float *src = (Float*)m_data->data;

		for ( i = 0 ; i < m_size ; i += 4 )
		{
			data[i  ] = (UInt8)(src[j]   * 255.f);
			data[i+1] = (UInt8)(src[j+1] * 255.f);
			data[i+2] = (UInt8)(src[j+2] * 255.f);

			if ((data[i  ] == r) &&
				(data[i+1] == g) &&
				(data[i+2] == b))
				data[i+3] = 0;
			else
				data[i+3] = 255;

			j += 3;
		}
	}

	m_data = new ImageData(data);
	m_pixelFormat = PF_RGBA_8;

	setDirty();
	return True;
}

// convert to RGBA to RGB
Bool Image::convertRGBAtoRGB8()
{
	if (!m_state)
		O3D_ERROR(E_InvalidPrecondition("Picture must be valid"));

	if (isCompressed())
		O3D_ERROR(E_InvalidFormat("Compressed picture are not supported at this time"));

	if (isComplex())
		O3D_ERROR(E_InvalidFormat("Complex picture are not supported at this time"));

	if ((m_pixelFormat != PF_RGBA_8) && (m_pixelFormat != PF_RGBA_F32))
		O3D_ERROR(E_InvalidFormat("Pixel format must be RGBA_U8 or RGBA_F32"));

	if (!m_data.get() || !m_data->data || !m_width || !m_height)
		return False;

	m_size = m_width * m_height * 3;

	UInt8 *data = new UInt8[m_size];
	UInt32 i,j = 0;

	if (m_pixelFormat == PF_RGBA_8)
	{
		UInt8 *src = m_data->data;

		for ( i = 0 ; i < m_size ; i += 3 )
		{
			data[i  ] = src[j];
			data[i+1] = src[j+1];
			data[i+2] = src[j+2];

			j += 4;
		}
	}
	else if (m_pixelFormat == PF_RGBA_F32)
	{
		Float *src = (Float*)m_data->data;

		for ( i = 0 ; i < m_size ; i += 3 )
		{
			data[i  ] = (UInt8)(src[j]   * 255.f);
			data[i+1] = (UInt8)(src[j+1] * 255.f);
			data[i+2] = (UInt8)(src[j+2] * 255.f);

			j += 4;
		}
	}

	m_data = new ImageData(data);
	m_pixelFormat = PF_RGB_8;

	setDirty();
	return True;
}

Bool Image::convertRGBtoRGB8()
{
	// TODO

	setDirty();
	return True;
}

Bool Image::convertRGBAtoRGBA8()
{
	// TODO

	setDirty();
	return True;
}

Bool Image::convertRGBAtoRGBA8(Rgb colorKey)
{
	// TODO

	setDirty();
	return True;
}

// swap Red and Blue channels for each pixels
Bool Image::swapRB()
{
	if (!m_state)
		O3D_ERROR(E_InvalidPrecondition("Picture must be valid"));

	if (isComplex())
		O3D_ERROR(E_InvalidFormat("Complex picture are not supported at this time"));

	if (isCompressed())
		O3D_ERROR(E_InvalidFormat("Compressed picture are not supported at this time"));

	if ((m_pixelFormat != PF_RGB_8) && (m_pixelFormat != PF_RGBA_8) &&
		(m_pixelFormat != PF_RGB_F32) && (m_pixelFormat != PF_RGBA_F32))
		O3D_ERROR(E_InvalidFormat("Pixel format must be RGB or RGBA"));

	if (!m_data.get() || !m_data->data || !m_width || !m_height)
		return False;

	// process a deep copy before swap
	detach();

	UInt32 numComp = getNumComponents();

	if ((m_pixelFormat == PF_RGB_8) || (m_pixelFormat == PF_RGBA_8))
	{
		UInt8 *data = m_data->data;
		UInt8 buf;

		for (UInt32 i = 0 ; i < m_size ; i += numComp)
		{
			buf = data[i];
			data[i] = data[i+2];
			data[i+2] = buf;
		}
	}
	else if ((m_pixelFormat == PF_RGB_F32) || (m_pixelFormat == PF_RGBA_F32))
	{
		Float *data = (Float*)m_data->data;
		Float buf;

		for (UInt32 i = 0 ; i < m_size ; i += numComp)
		{
			buf = data[i];
			data[i] = data[i+2];
			data[i+2] = buf;
		}
	}

	setDirty();
	return True;
}

Bool Image::convertToRGB8()
{
	switch (getNumComponents())
	{
		case 3:
			return convertRGBtoRGB8();
		case 4:
			return convertRGBAtoRGB8();
		case 1:
			return convertRtoRGB8();
		case 2:
			return False;//convertRGtoRGB8();
		default:
			return False;
	}
}

Bool Image::convertToRGBA8()
{
	switch (getNumComponents())
	{
		case 4:
			return convertRGBAtoRGBA8();
		case 3:
			return convertRGBtoRGBA8();
		case 1:
			return convertRtoRGBA8();
		case 2:
			return False;//convertRGtoRGBA8();
		default:
			return False;
	}
}

Bool Image::convertToRGBA8(Rgb colorKey)
{
	switch (getNumComponents())
	{
		case 4:
			return convertRGBAtoRGBA8(colorKey);
		case 3:
			return convertRGBtoRGBA8(colorKey);
		case 1:
			return convertRtoRGBA8(colorKey);
		case 2:
			return False;//convertRGtoRGBA8(colorKey);
		default:
			return False;
	}
}

// allocate a new picture
Bool Image::allocate(
	UInt32 cx,
	UInt32 cy,
	UInt32 numComponents)
{
	if (m_state)
		destroy();

	if ((numComponents < 1) || (numComponents > 4))
		O3D_ERROR(E_InvalidParameter("Component must be between [1..4]"));

	m_data = new ImageData(new UInt8[cx*cy*numComponents]);

	m_width = cx;
	m_height = cy;

	m_size = m_width * m_height * numComponents;
	m_state = True;

	switch (numComponents)
	{
		case 1:
			m_pixelFormat = PF_RED_8;
			break;
		case 2:
            m_pixelFormat = PF_RG_8;
			break;
		case 3:
			m_pixelFormat = PF_RGB_8;
			break;
		case 4:
			m_pixelFormat = PF_RGBA_8;
			break;
		default:
			break;
	}

	setDirty();
	return True;
}

// charge un buffer
Bool Image::loadBuffer(
	UInt32 cx,
	UInt32 cy,
	UInt32 _size,
	PixelFormat pixelFormat,
	const UInt8* buffer)
{
	if (m_state)
		destroy();

    if (buffer == nullptr)
		O3D_ERROR(E_NullPointer("Buffer data must be valid"));

	if ((cx == 0) || (cy == 0) || (_size == 0))
		O3D_ERROR(E_InvalidParameter("Invalid size"));

	// check size according to w*h*bpp
	m_pixelFormat = pixelFormat;

	UInt32 estimateSize = cx * cy * (getBpp() >> 3);
	if (estimateSize != _size) {
		m_pixelFormat = PF_RGBA_8;
		O3D_ERROR(E_InvalidFormat("Buffer size or pixel format is incorrect"));
	}

	m_data = new ImageData(new UInt8[_size]);
	memcpy(m_data->data, buffer, _size);

	m_state = True;
	m_size = _size;
	m_width = cx;
	m_height = cy;

	setDirty();
	return True;
}

// detach the shared data (process a deep copy)
void Image::detach()
{
	if (!m_state)
		return;

	if (m_pic.get() && (m_pic->getReferenceCounter() > 1))
	{
		ImgFormat *src = m_pic.get();
		m_pic = src->clone();
	}
	else if (m_data.get() && m_data->data && (m_data->getReferenceCounter() > 1))
	{
		UInt8 *src = m_data->data;
		UInt8 *data = new UInt8[m_size];
		memcpy(data, src, m_size);

		m_data = new ImageData(data);
	}
}

void Image::attach(UInt32 w, UInt32 h, UInt32 size, PixelFormat pf, UInt8* data)
{
	if (m_state)
		destroy();

    if (data == nullptr)
		O3D_ERROR(E_NullPointer("Buffer data must be valid"));

	if ((w == 0) || (h == 0) || (size == 0))
		O3D_ERROR(E_InvalidParameter("Invalid size"));

	// check size according to w*h*bpp
	m_pixelFormat = pf;

	UInt32 estimateSize = w * h * (getBpp() >> 3);
	if (estimateSize != size) {
		m_pixelFormat = PF_RGBA_8;
		O3D_ERROR(E_InvalidFormat("Buffer size or pixel format is incorrect"));
	}

	m_size = size;
	m_width = w;
	m_height = h;

	m_data = new ImageData(data);
	m_state = True;
}

// select a mipmap lvl
Bool Image::bindMipMapLvl(UInt32 lvl)
{
	if (m_pic.get())
	{
		Bool ret = m_pic->bindMipMapLvl(lvl);

		if (ret)
		{
			UInt8 *data;
			m_pic->getInfo(m_width, m_height, data, m_size);

			return True;
		}
	}
	return False;
}

// select a cubemap side
void Image::bindCubeMapSide(ImgFormat::CubeMapSide side)
{
	if (m_pic.get())
	{
		UInt8 *data;

		m_pic->bindCubeMapSide(side);
		m_pic->getInfo(m_width, m_height, data, m_size);
	}
}

// select a depth
Bool Image::bindVolumeLayer(UInt32 layer)
{
	if (m_pic.get())
	{
		Bool ret = m_pic->bindVolumeLayer(layer);

		if (ret)
		{
			UInt8 *data;
			m_pic->getInfo(m_width, m_height, data, m_size);

			return True;
		}
	}
	return False;
}
