/**
 * @file image.h
 * @brief texture (3d).
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-09
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_IMAGE_H
#define _O3D_IMAGE_H

#include "o3d/core/debug.h"
#include "o3d/core/smartpointer.h"
#include "o3d/core/memorydbg.h"
#include "dds.h"
#include "color.h"
#include "imgformat.h"
#include "imagetype.h"

namespace o3d {

enum ResizeFilter
{
	FILTER_NEAREST = 0
};

class InStream;
class OutStream;

//---------------------------------------------------------------------------------------
//! @class Image
//-------------------------------------------------------------------------------------
//! An image can contains a simple or complex image with mipmap, cubemap, or
//! volume texture (3d).
//!
//! Supported formats for reading are TGA, DDS, BMP, GIF, PNG and JPG.
//! Supported formats for writing are BMP, PNG and JPG.
//!
//! It allow loading, saving, pixel format conversion, flipping, mirroring,
//! mask operation and blending.
//!
//! About shadow and deep copy. Image data are passed by shallow copy on duplication.
//! detach allow to make a deep copy of the image data.
//! Any change done to the image create a new image data, so that the others copies
//! are not modified.
//!
//! Any conversion works on unsigned 8 bits and float 32 R(A), RG(RA), RBG, RGBA
//! and float 32 depth pixel formats. Exception for complex images (mipmap, cubemap, 3d).
//! There is actually no conversions from/to compressed pixel formats.
//! Some pixels format are not supported for conversion : any unsigned depth(stencil),
//! and DXT compressed. Also there is actually no unsigned 8 to float 32 pixel formats
//! conversions.
//---------------------------------------------------------------------------------------
class O3D_API Image : public SmartCounter<Image>
{
public:

	//! Supported picture formats
	enum FileFormat
	{
		BMP,           //!< BMP format (windows bitmap)
		TGA,           //!< TGA format (targa)
		JPEG,          //!< JPG format (Jpeg)
		PNG,           //!< PNG format (portable network graphic)
		GIF,           //!< GIF format (graphic interchange format)
		DDS            //!< DDS format (direct x)
	};

	//! Default constructor
	Image();

	//! Load a picture given its filename
	Image(const String &filename);

	//! Load a picture given its filename
	Image(const String &filename, PixelFormat pixelFormat);

	//! Load a picture from a file at current file position.
    Image(InStream &is);

	//! Load a picture from a file at current file position.
    Image(InStream &is, PixelFormat pixelFormat);

	//! Copy constructor
	Image(const Image &pic);

	//! Destructor
	virtual ~Image();

	//! Destroy the picture object
	void destroy();

	//! Create a new picture of cx*cy size.
	//! @param numComponents number of color components (1,2,3,4)
	//! Pixel format is set to R_U8 for 1 component, RG_U8, RGB_U8
	//! and RGBA_U8 respectively.
	Bool allocate(UInt32 cx, UInt32 cy, UInt32 numComponents);

	//! Create a new picture of cx*cy size given an existing buffer.
	//! @param pixelFormat Pixel format of the data.
	//! @param size Size of the buffer in bytes.
	Bool loadBuffer(
		UInt32 width,
		UInt32 height,
		UInt32 size,
		PixelFormat pixelFormat,
		const UInt8 *data);

	//! Duplicate
	Image& operator= (const Image& pic);

	//! detach the shared data (process a deep copy)
	//! @note This is done automatically when trying to modify the picture.
	void detach();

	//! Attach an external valid data array.
	void attach(UInt32 w, UInt32 h, UInt32 size, PixelFormat pf, UInt8* data);

	//-----------------------------------------------------------------------------------
	// Reading
	//-----------------------------------------------------------------------------------

    //! Load a picture from a stream. Automatically detect the format.
	//! @param file A file opened in reading to load the picture from.
	//! @note pixel format lesser than 8bits are converted into 8bits (1bpp)
    Bool load(InStream &is);

	//! Load a picture given its filename. Automatically detect the format.
	//! @param filename A valid filename of the picture to load.
	//! @note pixel format lesser than 8bits are converted into 8bits (1bpp)
	Bool load(const String &filename);

	//! Load a picture from a file at current file position. Automatically detect the format.
	//! @param file A file opened in reading to load the picture from.
	//! @param format Define the loading conversion mode.
	//! @note pixel format lesser than 8bits are converted into 8bits (1bpp)
    Bool load(InStream &is, PixelFormat pixelFormat);

	//! Load a picture given its filename. Automatically detect the format.
	//! @param filename A valid filename of the picture to load.
	//! @param format Define the loading conversion mode.
	//! @note pixel format lesser than 8bits are converted into 8bits (1bpp)
	Bool load(const String &filename, PixelFormat pixelFormat);


	//-----------------------------------------------------------------------------------
	// Writing
	//-----------------------------------------------------------------------------------

	//! Write the picture using the most direct pixel format.
	//! If the picture filename is empty, so this method will set it.
	//! @param filename Output picture filename
	//! @param format One of the valid supported format.
	//! @param quality quality for loss compression format (JPEG only)
	Bool save(const String &filename, FileFormat format, Int32 quality = 255);

	//! Write a picture in 3 or 4 bytes per pixel (24/32bits). If the picture filename is empty,
	//! so this method will set it. Use of 4 bytes if the image have an alpha channel and
	//! if the format support it (PNG only).
	//! @param filename Output picture filename
	//! @param format One of the valid supported format.
	//! @param quality quality for loss compression format (JPEG only)
	Bool saveRgb(
		const String &filename,
		FileFormat format,
		Int32 quality = 255);


	//-----------------------------------------------------------------------------------
	// Operations
	//-----------------------------------------------------------------------------------

	//! Swap Red and Blue values for each pixels of the current selected mipmap lvl.
	//! @note Does not works with compressed data like DXT.
	Bool swapRB();

	//! Convert current format to RGB888 format of the current selected mipmap lvl.
	Bool convertToRGB8();

	//! Convert current format to RGBA8888 format of the current selected mipmap lvl.
	Bool convertToRGBA8();

	//! Convert current format to RGBA8888 format of the current selected mipmap lvl,
	//! using a color mask for transparency if necessary
	//! @param colorKey Color used for transparency. Each pixel which have the value of
	//! colorKey will have an alpha value of 0.
	Bool convertToRGBA8(Rgb colorKey);

	//-----------------------------------------------------------------------------------
	// Complex bitmaps
	//-----------------------------------------------------------------------------------

	//! is the picture contain many mipmap, or it is a multi-layer (3d texture),
	//! or it is a cube-map.
	inline Bool isComplex() const
	{
		if (m_pic.get())
			return m_pic->isComplex();
		return False;
	}

	//! Is there mipmaps.
	inline Bool isMipMap() const
	{
		if (m_pic.get())
			return m_pic->isMipMap();
		return False;
	}

	//! Is it a cube-map.
	inline Bool isCubeMap() const
	{
		if (m_pic.get())
			return m_pic->isCubeMap();
		return False;
	}

	//! Is it a volume texture (3d).
	inline Bool isVolumeTexture() const
	{
		if (m_pic.get())
			return m_pic->isVolumeTexture();
		return False;
	}

	//! Get the number of mipmap (don't count the primary level).
	//! so the total number of level is GetNumMipMapLvl() + 1
	inline UInt32 getNumMipMapLvl() const
	{
		if (m_pic.get())
			return m_pic->getNumMipMapLvl();
		return 0;
	}

	//! Get the depth size of a volume texture.
	inline UInt32 getNumDepthLayer() const
	{
		if (m_pic.get())
			return m_pic->getNumDepthLayer();
		return 0;
	}

	//! Select a mipmap lvl. 0 mean primary level.
	Bool bindMipMapLvl(UInt32 lvl);

	//! Select a cubemap side.
	void bindCubeMapSide(ImgFormat::CubeMapSide side);

	//! Select a depth for a volume texture.
	Bool bindVolumeLayer(UInt32 layer);

	//! Get current bound mipmap lvl.
	inline UInt32 getCurrentMipMapLvl() const
	{
		if (m_pic.get())
			return m_pic->getCurrentMipMapLvl();
		return 0;
	}

	//! Get current bound cubemap side.
	inline UInt32 getCurrentCubeMapSide() const
	{
		if (m_pic.get())
			return m_pic->getCurrentCubeMapSide();
		return 0;
	}

	//! Get current bound depth volume layer.
	inline UInt32 getCurrentDepthLayer() const
	{
		if (m_pic.get())
			return m_pic->getCurrentDepthLayer();
		return 0;
	}

	//! Is a compressed picture (ie. DXT)
	inline Bool isCompressed() const
	{
		if (m_pic.get())
			return m_pic->isCompressed();
		return False;
	}


	//-----------------------------------------------------------------------------------
	// Accessors
	//-----------------------------------------------------------------------------------

	//! Is the object contain a picture.
	inline Bool isValid() const { return m_state; }

	//! Is the picture is empty
	inline Bool isEmpty() const { return !m_state; }

	//! Get the size of the data.
	//! @note Depending of the current bound layer/mipmap/cubeside.
	inline UInt32 getSize() const { return m_size; }

	//! Get the pixel format.
	inline PixelFormat getPixelFormat() const { return m_pixelFormat; }

	//! Bitmap data accessor (const version).
	//! @note Depending of the current bound layer/mipmap/cubeside.
	inline const UInt8* getData() const
	{
		if (m_pic.get())
			return m_pic->getData();
		else if (m_data.get())
			return m_data->data;
		else
            return nullptr;
	}

	//! Bitmap data accessor for write. Set it modified.
	//! @note Depending of the current bound layer/mipmap/cubeside. This method doesn't
	//! perform detach. Then take care to call a detach or not depending of what you are
	//! doing.
	inline UInt8* getDataWrite()
	{
		setDirty();

		if (m_pic.get())
			return m_pic->getData();
		else if (m_data.get())
			return m_data->data;
		else
            return nullptr;
	}

	//! Get a pixel.
	Rgb getRgbPixel(UInt32 x, UInt32 y) const;
	//! Get a pixel.
	Color getPixel(UInt32 x, UInt32 y) const;

	//! Get the width of the picture.
	//! @note Depending of the current bound layer/mipmap/cubeside.
	inline UInt32 getWidth() const { return m_width; }

	//! Get the height of the picture.
	//! @note Depending of the current bound layer/mipmap/cubeside.
	inline UInt32 getHeight() const { return m_height; }

	//! Get the pitch of the picture.
	//! @note Depending of the current bound layer/mipmap/cubeside.
	inline UInt32 getPitch() const { return (m_width * getBpp()) >> 3; }

	//! Get the size of a pixel in bits.
	UInt32 getBpp() const;

	//! Get the number of components of the picture.
	UInt32 getNumComponents() const;

    //! Is the picture have changed.
    inline Bool isDirty() const { return m_isDirty; }

	//! Is alpha channel.
	//! @note RED only channel returns FALSE.
	Bool isAlpha() const;

	//! Is rgb color components.
	Bool isRgb() const;

	//! Is color picture with 8 bits unsigned integer components.
    //! It includes PF_RED_8, PF_RG_8, PF_RGB_8, PF_RGBA_8 and U8 variantes.
	Bool isColorU8() const;
	//! Is color picture with 32 bits float components.
    //! It includes PF_RED_F32, PF_RG_F32, PF_RGB_F32, PF_RGBA_F32.
	Bool isColorF32() const;

	//-----------------------------------------------------------------------------------
	// Additional picture operations (defined in PictureOp.cpp)
	//----------------------------------------------------------------------------------

	//! Horizontal flip (on X axis).
	//! @note Depending of the current bound layer/mipmap/cubeside.
	//! @warning Doesn't works with compressed data.
	Bool hFlip();

	//! Vertical flip (on Y axis).
	//! @note Depending of the current bound layer/mipmap/cubeside.
	//! @warning Doesn't works with compressed data.
	Bool vFlip();

	//! Flip on the two axis (XY).
	//! @note Depending of the current bound layer/mipmap/cubeside.
	//! @warning Doesn't works with compressed data.
	Bool flip();

	//! Created a bitmap with a specified background color. Destroy the all content.
	Bool loadWithColor(
		UInt32 cx,
		UInt32 cy,
		UInt8 r,
		UInt8 g,
		UInt8 b,
		UInt8 a = 255,
		Bool alpha = True);

	//! blend with a picture (alpha=1 picture not modified, 0 dest picture)
	//! @note Depending of the current bound layer/mipmap/cubeside.
	//! @warning Doesn't works with compressed data.
	Bool blending(const Image &pic, Float alpha);

	//! blend with a color (alpha=1 picture not modified, 0 dest picture)
	//! @note Depending of the current bound layer/mipmap/cubeside.
	//! @warning Doesn't works with compressed data.
	Bool mask(UInt8 r, UInt8 g, UInt8 b, Float alpha);

	//! Invert all pixels color.
	//! @param Alpha If true then negative the alpha canal too
	//! @note Depending of the current bound layer/mipmap/cubeside.
	//! @warning Doesn't works with compressed data.
	Bool negative(Bool alpha = False);

	//! Convert picture from 1/2/3/4 components to a 2 components RG88 (Luminance/Alpha) bitmap.
	//! Use of the Alpha canal for set if existing, otherwise set it to 255.
	//! For 2 components picture that have a format different of Luminance Alpha, simply reinterpret it.
	//! @param grayScale If false, take red as luminance, else convert color to grayscale.
	//! This parameter is useful only for picture with color channels.
	//! @note Depending of the current bound layer/mipmap/cubeside.
	//! @warning Doesn't works with compressed data.
	Bool convertToRG8(Bool grayScale = False);

	//! Convert picture from 1/2/3/4 components to a 1 component RED8 (Luminance) bitmap.
	//! For 2 components, it only take the first channnel as luminance.
	//! For 3/4 components, it convert each pixel into luminance depending
	//! of the parameter grayScale.
	//! @param grayScale If false, take red as luminance, else convert color to grayscale.
	//! This parameter is useful only for picture with color channels.
	//! @note Depending of the current bound layer/mipmap/cubeside.
	//! @warning Doesn't works with compressed data.
	Bool convertToR8(Bool grayScale = False);

	//! Convert picture from 1/2/3/4 components to a 1 component Alpha 8 bitmap.
	//! Pixel format is set to RED_U8.
	//! For 2/4 components, it only take the alpha channnel as alpha.
	//! For 3 components, it convert each pixel into grayscale and set it as alpha.
	//! @param grayScale If false, take red as luminance, else convert color to grayscale.
	//! This parameter is useful only for picture without alpha channel.
	//! @note Depending of the current bound layer/mipmap/cubeside.
	//! @warning Doesn't works with compressed data.
	Bool convertToA8(Bool grayScale = False);

	//! Resize the picture by using the specified filter.
	//! @note Depending of the current bound layer/mipmap/cubeside.
	//! @warning Doesn't works with compressed data.
	Bool resize(UInt32 _width, UInt32 _height, ResizeFilter _filter = FILTER_NEAREST);

	//! Convert this picture from heightmap (geyscale) to a RGB normal map.
	//! @note Depending of the current bound layer/mipmap/cubeside.
	//! @warning Doesn't works with compressed data.
	Bool toNormalMap(Float scale, Bool wrap);

	//! convert the picture from heightmap (geyscale) to a RGB normal map, and return
	//! the new picture.
	//! @note Depending of the current bound layer/mipmap/cubeside.
	//! @warning Doesn't works with compressed data.
	Image* retNormalMap(Float scale, Bool wrap);

protected:

	//! Structure that contain sharable data
	struct ImageData : public SmartCounter<ImageData>
	{
		ImageData(UInt8 *ptr) :
			SmartCounter<Image::ImageData>(),
			data(ptr)
		{
		}

		ImageData(const ImageData &dup) :
			SmartCounter<Image::ImageData>(dup),
			data(dup.data)
		{
		}

		virtual ~ImageData() { deleteArray(data); }

		UInt8 *data;
	};

	Bool m_state;         //!< true for a valid picture

	UInt32 m_width;       //! picture width in pixel
	UInt32 m_height;      //! picture height in pixel

	PixelFormat m_pixelFormat;     //!< Picture data pixel format

    SmartPtr<ImgFormat> m_pic;     //!< we remember it only for complex bitmap (ex:DDS), otherwise null
	SmartPtr<ImageData> m_data;    //!< picture data

	UInt32 m_size;      //!< picture data size

    Bool m_isDirty;     //!< true mean the picture was modified

	//-----------------------------------------------------------------------------------
	// Conversion methods
	//-----------------------------------------------------------------------------------

	//! Convert picture from R/DEPTH U8/F32 to RGB888 of the current selected mipmap lvl.
	//! @note Does not works with compressed data like DXT.
	Bool convertRtoRGB8();

	//! Convert picture from R/DEPTH U8/F32 to RGBA8888 of the current selected mipmap lvl.
	//! @note Does not works with compressed data like DXT.
	Bool convertRtoRGBA8();

	//! Convert picture from RGB U8/F32 to RGBA8888 of the current selected mipmap lvl.
	//! @note Does not works with compressed data like DXT.
	Bool convertRGBtoRGBA8();

	//! Convert picture from RGBA U8/F32 to RGB888 of the current selected mipmap lvl.
	//! @note Does not works with compressed data like DXT.
	Bool convertRGBAtoRGB8();

	//! Convert picture from R/DEPTH to RGBA8888 of the current selected mipmap lvl.
	//! @param colorKey Color used for transparency. Each pixel which have the value of
	//! colorKey will have an alpha value of 0.
	//! @note Does not works with compressed data like DXT.
	Bool convertRtoRGBA8(Rgb colorKey);

	//! Convert picture from RGB to RGBA8888 of the current selected mipmap lvl.
	//! @param colorKey Color used for transparency. Each pixel which have the value of
	//! colorKey will have an alpha value of 0.
	//! @note Does not works with compressed data like DXT.
	Bool convertRGBtoRGBA8(Rgb colorKey);

	//! Convert any unsigned 8 format to R8.
	Bool convertX8toA8(Bool grayScale);
	//! Convert any unsigned 8 format to R8.
	Bool convertX8toR8(Bool grayScale);
	//! Convert any unsigned 8 format to RG8.
	Bool convertX8toRG8(Bool grayScale);
	//! Convert any float 32 format to R8.
	Bool convertXF32toA8(Bool grayScale);
	//! Convert any float 32 format to R8.
	Bool convertXF32toR8(Bool grayScale);
	//! Convert any float 32 format to RG8.
	Bool convertXF32toRG8(Bool grayScale);

	//! Convert any RGB format to RGB8.
	Bool convertRGBtoRGB8();
	//! Convert any RGBA format to RGBA8.
	Bool convertRGBAtoRGBA8();
	//! Convert any RGBA format to RGBA8.
	Bool convertRGBAtoRGBA8(Rgb colorKey);

	//-----------------------------------------------------------------------------------
	// Writings methods
	//-----------------------------------------------------------------------------------

	//! Write a BMP in RGB888 format.
	Bool saveRgbBmp(const String &filename);

	//! Write a JPEG file in grayscale or RGB888 compressed format.
	Bool saveJpg(const String &filename, Int32 quality);

	//! Write a JPEG file in RGB888 compressed format.
	Bool saveRgbJpg(const String &filename, Int32 quality);

	//! Write a PNG file in grayscale or RA88 or RGB888 or RGBA8888 compressed format.
	Bool savePng(const String &filename);

	//! Write a PNG File in RGB888 or RGBA8888 compressed format.
	Bool saveRgbPng(const String &filename);

	//! Determine the OpenGL format from the internal format.
	void putFormat();

	//! Normal map map generation.
	void genNormalMap(UInt8* nmap, Float scale, Bool wrap) const;

	//! Picture is not same as originally created/loaded
	inline void setDirty() { m_isDirty = True; }
};

} // namespace o3d

#endif // _O3D_IMAGE_H

