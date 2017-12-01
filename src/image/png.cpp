/**
 * @file png.cpp
 * @brief Loading/Writing of PNG pictures (portable network graphic).
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-01-23
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/image/precompiled.h"
#include "o3d/image/png.h"
#include "o3d/image/image.h"
#include "o3d/core/filemanager.h"

#include <stdlib.h>
#include <memory.h>

using namespace o3d;

#define PNG_ASSEMBLER_CODE_SUPPORTED
#define PNG_BYTES_TO_CHECK 8

#ifdef _MSC_VER
    #include <libpng/png.h>
#else
    #include <png.h>
#endif

#ifdef _MSC_VER
#pragma warning (disable: 4611)			// disable VC++'s warning for the setjmp
#endif

// png man
// http://www.libpng.org/pub/png/libpng-1.0.3-manual.html

static void error_handler(png_struct*, const char *error)
{
	O3D_ERROR(E_InvalidResult(String(error)));
}

static void warning_handler(png_struct*, const char *warning)
{
    // O3D_ERROR(E_InvalidResult(String(warning)));
    O3D_WARNING(String(warning));
}

static void _ReadProc(png_struct *src, png_byte *data, png_size_t size)
{
    InStream *file = (InStream*)png_get_io_ptr(src);
    file->reader(data, 1, (UInt32)size);
}

static void _WriteProc(png_struct *src, png_byte *data, png_size_t size)
{
    OutStream *file = (OutStream*)png_get_io_ptr(src);
    file->writer(data, 1, (UInt32)size);
}

static void _FlushProc(png_struct *src)
{
    OutStream *file = (OutStream*)png_get_io_ptr(src);
    file->flush();
}

// check the first bytes of the file for see his format
Bool Png::checkFormat(InStream &is)
{
	Char id[6];
    is.read(id, 6);
    is.seek(-6);

	if (((UInt8)id[0] == 0x89) && ((UInt8)id[1] == 'P') && ((UInt8)id[2] == 'N') &&
	    ((UInt8)id[3] == 'G')  && ((UInt8)id[4] == 0x0d)&& ((UInt8)id[5] == 0x0a))
		return True;

	return False;
}

PixelFormat Png::getPixelFormat() const
{
	switch (m_bpp)
	{
		case 1:
			return PF_RED_U8;
		case 2:
			return PF_RG_U8;
		case 3:
			return PF_RGB_U8;
		case 4:
			return PF_RGBA_U8;
		default:
			O3D_ERROR(E_InvalidFormat("Unsupported pixel format"));
			return PF_RGBA_U8;
	}
}

// Load a PNG file
Bool Png::load(InStream &is)
{
    return loadDefault(is);
}

Bool Png::load(InStream &is, PixelFormat pixelFormat)
{
	switch (pixelFormat)
	{
		case PF_RGB_U8:
            return loadRgb24(is);
		case PF_RGBA_U8:
            return loadRgba32(is);
		default:
			O3D_ERROR(E_InvalidParameter("Unsupported convert format"));
	}
}

Bool Png::loadDefault(InStream &is)
{
	// check the file format
    if (!checkFormat(is))
        O3D_ERROR(E_InvalidFormat("Invalid PNG token"));

	UInt8 png_check[PNG_BYTES_TO_CHECK];

	png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;

    png_bytepp row_pointers = NULL;

	//curpos = 0;
	m_palette = NULL;

	//***********************************************************************************
	// first stape: read header info

	// read signature
    is.reader(png_check, 1, PNG_BYTES_TO_CHECK);

	// check signature
	if (png_sig_cmp(png_check,(png_size_t)0, PNG_BYTES_TO_CHECK) != 0)
	{
        O3D_ERROR(E_InvalidFormat("Invalid PNG check signature"));
	}

	/********************************************************************/
	// could pass pointers to user-defined error handlers instead of NULL
	png_ptr = png_create_read_struct(
			PNG_LIBPNG_VER_STRING,
			(png_voidp)error_handler,
			error_handler,warning_handler);

	if (!png_ptr)
        O3D_ERROR(E_InvalidAllocation("png_ptr"));

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
        O3D_ERROR(E_InvalidAllocation("info_ptr"));
	}

    png_set_read_fn(png_ptr, (png_voidp)&is, _ReadProc);

	// must be called in every function that calls a PNG-reading libpng function
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		png_destroy_read_struct(&png_ptr,&info_ptr,NULL);
        O3D_ERROR(E_InvalidFormat("Invalid PNG format"));
	}

	/********************************************************************/
	// already read the 8 signature bytes
	png_set_sig_bytes(png_ptr, PNG_BYTES_TO_CHECK);

	// read the IHDR chunk
	png_read_info(png_ptr,info_ptr);

	/* alternatively, could make separate calls to png_get_image_width(), etc., but want
	 * bit_depth and color_type for later [don't care about compression_type and
	 * filter_type => NULLs]
	 */
	png_uint_32 width,height;
    Int32 bitperpel, color_type, interlace_type;

    png_get_IHDR(png_ptr,info_ptr, &width, &height, &bitperpel, &color_type, &interlace_type, NULL, NULL);

	m_width = (UInt32)width;
	m_height = (UInt32)height;

	// don't support >8bits color
	if (bitperpel == 16)
		png_set_strip_16(png_ptr);

	// expand color type
	switch (color_type)
	{
		case PNG_COLOR_TYPE_RGB:
			m_bpp = 3;
			break;

		case PNG_COLOR_TYPE_RGB_ALPHA:
			m_bpp = 4;
			break;

		case PNG_COLOR_TYPE_PALETTE:
			// Expand palette images to the full 8 bits from 2 or 4 bits/pixel
			if ((bitperpel == 2) || (bitperpel == 4))
			{
				png_set_packing(png_ptr);
				bitperpel = 8;
			}
			m_bpp = 1;
			break;

		case PNG_COLOR_TYPE_GRAY:
			// Expand grayscale images to the full 8 bits from 2 or 4 bits/pixel
			if ((bitperpel == 2) || (bitperpel == 4))
			{
				png_set_expand(png_ptr);
				bitperpel = 8;
			}
			m_bpp = 1;
			break;

		case PNG_COLOR_TYPE_GRAY_ALPHA:
			// Expand grayscale images to the full 8 bits from 2 or 4 bits/pixel
			if ((bitperpel == 2) || (bitperpel == 4))
			{
				png_set_expand(png_ptr);
				bitperpel = 8;
			}
            //png_set_gray_to_rgb(png_ptr); this can convert to RGB
            m_bpp = 2;
			break;

		default:
			png_destroy_read_struct(&png_ptr,&info_ptr,NULL);
            O3D_ERROR(E_InvalidFormat("Unsupported PNG color type"));
	}

	//***********************************************************************************
	// second step: read background color

    if ((color_type != PNG_COLOR_TYPE_RGB_ALPHA) && (color_type != PNG_COLOR_TYPE_GRAY_ALPHA))
	{
		png_color_16 pBackground = { 0, 255, 255, 255, 0 };
		png_color_16 *image_background;

		if (png_get_bKGD(png_ptr,info_ptr,&image_background))
			png_set_background(png_ptr,image_background,PNG_BACKGROUND_GAMMA_FILE,1,1.0);
		else
			png_set_background(png_ptr,&pBackground,PNG_BACKGROUND_GAMMA_SCREEN,0,1.0);
	}

	// if this image has transparency, store the trns values

	png_bytep trans               = NULL;
	Int32 num_trans           = 0;
	png_color_16p trans_values    = NULL;
	/*png_uint_32 transparent_value = */png_get_tRNS(png_ptr,info_ptr,&trans,&num_trans,&trans_values);

	// unlike the example in the libpng documentation, we have *no* idea where
	// this file may have come from--so if it doesn't have a file gamma, don't
	// do any correction ("do no harm")

	// PC is 1.0,
	// Mac is 1.45,
	// SGI defaults to 1.7
	Double gamma = 0;
	Double screen_gamma = 2.2;

#ifdef __APPLE__
	screen_gamma /= 1.45;
#endif

	// comment for ignore gamma
	if (png_get_gAMA(png_ptr,info_ptr,&gamma))
		png_set_gamma(png_ptr,screen_gamma,gamma);

	// All transformations have been registered; now update info_ptr data
	png_read_update_info(png_ptr,info_ptr);

	// Create data and write the bitmap header
	// set up the data palette, if needed
	Int32 palette_entries;
	png_colorp png_palette;

	switch (color_type)
	{
		case PNG_COLOR_TYPE_RGB:
			//png_set_invert_alpha(png_ptr);
			m_data = new UInt8[width*height*3];
			break;

		case PNG_COLOR_TYPE_RGB_ALPHA:
			//png_set_invert_alpha(png_ptr);
			m_data = new UInt8[width*height*4];
			break;

		case PNG_COLOR_TYPE_PALETTE:
			m_data = new UInt8[width*height];
			png_get_PLTE(png_ptr,info_ptr,&png_palette,&palette_entries);

			// store the palette table
			if (png_get_valid(png_ptr,info_ptr,PNG_INFO_tRNS))
			{
				m_palette = new UInt8[palette_entries*4];
				m_palbpp = 4;

				// test palette
				if (!m_palette)
				{
					png_destroy_read_struct(&png_ptr,&info_ptr,NULL);
                    O3D_ERROR(E_InvalidAllocation("palette"));
				}

				// store the palette
				for (Int32 i = 0; i < palette_entries; i++)
				{
					m_palette[i*3]   = png_palette[i].red;
					m_palette[i*3+1] = png_palette[i].green;
					m_palette[i*3+2] = png_palette[i].blue;

					if (i<num_trans)
						m_palette[i*3+3] = trans[i];
				}
			}
			else
			{
				m_palette = new UInt8[palette_entries*3];
				m_palbpp = 3;

				// test palette
				if (!m_palette)
				{
					png_destroy_read_struct(&png_ptr,&info_ptr,NULL);
                    O3D_ERROR(E_InvalidAllocation("palette"));
				}

				// store the palette
				for (Int32 i = 0; i < palette_entries; i++)
				{
					m_palette[i*3]   = png_palette[i].red;
					m_palette[i*3+1] = png_palette[i].green;
					m_palette[i*3+2] = png_palette[i].blue;
				}

			}
			break;

		case PNG_COLOR_TYPE_GRAY:
			m_data = new UInt8[width*height];
			break;

		case PNG_COLOR_TYPE_GRAY_ALPHA:
            //png_set_invert_alpha(png_ptr);
            m_data = new UInt8[width*height*2];
			break;

		default:
			break;
	}

	// test data
	if (!m_data)
	{
		png_destroy_read_struct(&png_ptr,&info_ptr,NULL);

		deleteArray(m_palette);

        O3D_ERROR(E_InvalidAllocation("data"));
	}

	// set the individual row_pointers to point at the correct offsets
	row_pointers = (png_bytepp)O3D_MALLOC(height*sizeof(png_bytep));

	if (!row_pointers)
	{
		png_destroy_read_struct(&png_ptr,&info_ptr,NULL);

		deleteArray(m_palette);
		deleteArray(m_data);

        O3D_ERROR(E_InvalidAllocation("row_pointers"));
	}

	// read in the bitmap bits via the pointer table
	for (png_uint_32 k = 0; k < height; ++k)
		row_pointers[k] = (png_byte*)&m_data[width*m_bpp*k];

	png_read_image(png_ptr,row_pointers);

	// cleanup
	if (row_pointers)
		O3D_FREE(row_pointers);

	png_read_end(png_ptr,info_ptr);

	if (png_ptr)
		png_destroy_read_struct(&png_ptr,&info_ptr,(png_infopp)NULL);

	m_size = m_width * m_height * m_bpp;
	m_state = True;

	if ((m_palbpp != 0) && (m_palette != NULL))
		return convert8to24();

	return True;
}

Bool Png::loadRgb24(InStream &is)
{
    if (!loadDefault(is))
		return False;

	if (m_bpp == 4)
		return convert32to24();
    else if (m_bpp == 2)
        return convert16to24();
    else if (m_bpp == 1)
		return convert8to24();

	return True;
}

Bool Png::loadRgba32(InStream &is)
{
    if (!loadDefault(is))
		return False;

	if (m_bpp == 3)
		return convert24to32();
    else if (m_bpp == 2)
        return convert16to32();
    else if (m_bpp == 1)
		return convert8to32();

	return True;
}

/*---------------------------------------------------------------------------------------
  Permet de convertir le format 24bbp au format 32bpp
  Renvoie True si succé.
  ---------------------------------------------------------------------------------------*/
Bool Png::convert24to32()
{
	UInt8 *exdata = new UInt8[m_size];

	// copie de l'ancien buffer pour pouvoir remplace les data de la classe
	memcpy(exdata,m_data,m_size);

	deleteArray(m_data);

	m_size = m_width * m_height * 4;
	m_data = new UInt8[m_size];

	UInt32 i,j = 0;

	for ( i = 0 ; i < m_size  ; i += 4 )
	{
		m_data[i  ] = exdata[j];
		m_data[i+1] = exdata[j+1];
		m_data[i+2] = exdata[j+2];
		m_data[i+3] = 255;

		j += 3;
	}

	deleteArray(exdata);

	m_bpp = 4;
	return True;
}

/*---------------------------------------------------------------------------------------
  Permet de convertir le format 32bbp au format 24bpp
  Renvoie True si succé.
  ---------------------------------------------------------------------------------------*/
Bool Png::convert32to24()
{
	UInt8 *exdata = new UInt8[m_size];

	// copie de l'ancien buffer pour pouvoir remplace les data de la classe
	memcpy(exdata,m_data,m_size);

	deleteArray(m_data);

	m_size = m_width * m_height * 3;

	m_data = new UInt8[m_size];

	UInt32 i,j = 0;

	for ( i = 0 ; i < (UInt32)(m_width * m_height * 4)  ; i += 4 )
	{
		m_data[j  ] = exdata[i];
		m_data[j+1] = exdata[i+1];
		m_data[j+2] = exdata[i+2];

		j += 3;
	}

	deleteArray(exdata);

	m_bpp = 3;
	return True;
}

/*---------------------------------------------------------------------------------------
  Permet de convertir le format 1bbp au format 3bpp, la palette est détruite
  Renvoie True si succé.
  ---------------------------------------------------------------------------------------*/
Bool Png::convert8to24()
{
	UInt8 *exdata = new UInt8[m_size];

	// copie de l'ancien buffer pour pouvoir remplace les data de la classe
	memcpy(exdata,m_data,m_size);

	deleteArray(m_data);

	m_size = m_width * m_height * 3;
	m_data = new UInt8[m_size];

	UInt32 i,j = 0;

	if (m_palbpp == 3)
	{
		for ( i = 0 ; i < m_size ; i += 3 )
		{
			m_data[i  ] = m_palette[exdata[j]*3];
			m_data[i+1] = m_palette[exdata[j]*3+1];
			m_data[i+2] = m_palette[exdata[j]*3+2];

			++j;
		}
	}
	else if (m_palbpp == 4)
	{
		for ( i = 0 ; i < m_size ; i += 3 )
		{
			m_data[i  ] = m_palette[(exdata[j]<<2)];
			m_data[i+1] = m_palette[(exdata[j]<<2)+1];
			m_data[i+2] = m_palette[(exdata[j]<<2)+2];

			++j;
		}
	}

	deleteArray(m_palette);
	deleteArray(exdata);

	m_bpp = 3;
	return True;
}

/*---------------------------------------------------------------------------------------
  Permet de convertir le format 8bbp au format 32bpp, la palette est détruite
  Renvoi True si succé.
  ---------------------------------------------------------------------------------------*/
Bool Png::convert8to32()
{
	UInt32 index;

	UInt8 *exdata = new UInt8[m_size];

	// copie de l'ancien buffer pour pouvoir remplace les data de la classe
	memcpy(exdata,m_data,m_size);

	deleteArray(m_data);

	m_size = m_width * m_height * 4;
	m_data = new UInt8[m_size];

	UInt32 i,j = 0;

	if (m_palbpp == 3)
	{
		for ( i = 0 ; i < m_size  ; i += 4 )
		{
			index = exdata[j] * 3;

			m_data[i  ] = m_palette[index];
			m_data[i+1] = m_palette[index+1];
			m_data[i+2] = m_palette[index+2];
			m_data[i+3] = 255;

			++j;
		}
	}
	else if (m_palbpp == 4)
	{
		for ( i = 0 ; i < m_size  ; i += 4 )
		{
			index = exdata[j]<<2;

			m_data[i  ] = m_palette[index];
			m_data[i+1] = m_palette[index+1];
			m_data[i+2] = m_palette[index+2];
			m_data[i+3] = m_palette[index+3];

			++j;
		}
	}

	deleteArray(m_palette);
	deleteArray(exdata);

	m_bpp = 4;
	return True;
}

Bool Png::convert16to24()
{
    UInt8 *exdata = m_data;

    m_size = m_width * m_height * 3;
    m_data = new UInt8[m_size];

    UInt32 i,j = 0;

    for ( i = 0 ; i < m_size ; i += 3 )
    {
        m_data[i  ] = exdata[j];
        m_data[i+1] = exdata[j];
        m_data[i+2] = exdata[j];

        j += 2;
    }

    deleteArray(exdata);

    m_bpp = 3;
    return True;
}

Bool Png::convert16to32()
{
    UInt8 *exdata = m_data;

    m_size = m_width * m_height * 4;
    m_data = new UInt8[m_size];

    UInt32 i,j = 0;

    for ( i = 0 ; i < m_size ; i += 4 )
    {
        m_data[i  ] = exdata[j];
        m_data[i+1] = exdata[j];
        m_data[i+2] = exdata[j];
        m_data[i+3] = exdata[j+1];

        j += 2;
    }

    deleteArray(exdata);

    m_bpp = 4;
    return True;
}

// Write a PNG file in grayscale or rgb8 compressed format.
Bool Png::save(OutStream &os, PixelFormat pixelFormat)
{
	O3D_ERROR(E_InvalidOperation("Not yet implemented"));
	return False;
}

// Write a PNG file in grayscale or rgb8 compressed format.
Bool Image::savePng(const String &filename)
{
	if (!m_state)
		O3D_ERROR(E_InvalidPrecondition("Data must be non null " + filename));

	if (isCompressed())
		O3D_ERROR(E_InvalidFormat("Works only with non compressed picture"));

	if (isComplex())
		O3D_ERROR(E_InvalidFormat("Complex picture are not supported at this time"));

	if (!m_data.get() || !m_data->data || !m_width || !m_height)
		return False;

	Image tmpPicture(*this);

    FileOutStream *os = FileManager::instance()->openOutStream(filename, FileOutStream::CREATE);

	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;

	png_bytepp row_pointers = NULL;

    // initialize
    png_ptr = png_create_write_struct(
            PNG_LIBPNG_VER_STRING,
            (png_voidp)error_handler,
            error_handler,warning_handler);

	if (!png_ptr)
        O3D_ERROR(E_InvalidResult("Cannot write PNG structure"));

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
        O3D_ERROR(E_InvalidResult("Cannot create PNG info structure"));

	if (setjmp(png_jmpbuf(png_ptr)))
        O3D_ERROR(E_InvalidResult("Error during PNG init_io"));

    png_set_write_fn(png_ptr, (png_voidp)os, _WriteProc, _FlushProc);

	// write header
    if (setjmp(png_jmpbuf(png_ptr)))
        O3D_ERROR(E_InvalidResult("Error during writing PNG header"));

	png_byte color_type;
	png_byte bit_depth = 8;

	UInt32 width = tmpPicture.getWidth();
	UInt32 height = tmpPicture.getHeight();
	int bpp = (int)tmpPicture.getNumComponents();

	switch (tmpPicture.getNumComponents())
	{
		case 1:
			if (tmpPicture.getPixelFormat() != PF_RED_U8)
				tmpPicture.convertToR8();
			color_type = PNG_COLOR_TYPE_GRAY;
			break;
		case 2:
			if (tmpPicture.getPixelFormat() != PF_RG_U8)
				tmpPicture.convertToRG8();
			color_type = PNG_COLOR_TYPE_GRAY_ALPHA;
			break;
		case 3:
			if (tmpPicture.getPixelFormat() != PF_RGB_U8)
				tmpPicture.convertToRGB8();
			color_type = PNG_COLOR_TYPE_RGB;
			break;
		case 4:
			if (tmpPicture.getPixelFormat() != PF_RGBA_U8)
				tmpPicture.convertToRGBA8();
			color_type = PNG_COLOR_TYPE_RGB_ALPHA;
			break;
		default:
            O3D_ERROR(E_InvalidParameter("Unsupported color type"));
			break;
	}

	png_set_IHDR(
			png_ptr,
			info_ptr,
			width,
			height,
		    bit_depth,
		    color_type,
		    PNG_INTERLACE_NONE,
		    PNG_COMPRESSION_TYPE_BASE,
		    PNG_FILTER_TYPE_BASE);

    png_write_info(png_ptr, info_ptr);

	// write bytes
	if (setjmp(png_jmpbuf(png_ptr)))
        O3D_ERROR(E_InvalidResult("Error during writing PNG image data"));

	// set the individual row_pointers to point at the correct offsets
	row_pointers = (png_bytepp)O3D_MALLOC(tmpPicture.getHeight() * sizeof(png_bytep));

	if (!row_pointers)
	{
		png_destroy_write_struct(&png_ptr, &info_ptr);
        O3D_ERROR(E_InvalidAllocation("row_pointers"));
	}

	const UInt8 *data = tmpPicture.getData();

	// read in the bitmap bits via the pointer table
	for (png_uint_32 k = 0; k < height; ++k)
		row_pointers[k] = (png_byte*)&data[width*bpp*k];

	png_write_image(png_ptr, row_pointers);

	// end write
	if (setjmp(png_jmpbuf(png_ptr)))
		O3D_ERROR(E_InvalidResult("Error during writing PNG end of file"));

	// cleanup
	if (row_pointers)
		O3D_FREE(row_pointers);

    png_write_end(png_ptr, NULL);

	if (png_ptr)
		png_destroy_write_struct(&png_ptr, &info_ptr);

    deletePtr(os);
	return True;
}

// Write a PNG File in rgb8 compressed format.
Bool Image::saveRgbPng(const String &filename)
{
	if (!m_state)
		O3D_ERROR(E_InvalidPrecondition("Data must be non null " + filename));

	if (isCompressed())
		O3D_ERROR(E_InvalidFormat("Works only with non compressed picture"));

	if (isComplex())
		O3D_ERROR(E_InvalidFormat("Complex picture are not supported at this time"));

	if (!m_data.get() || !m_data->data || !m_width || !m_height)
		return False;

	Image tmpPicture(*this);

	if ((tmpPicture.getPixelFormat() != PF_RGB_U8) && (tmpPicture.getPixelFormat() != PF_RGBA_U8))
	{
		if (tmpPicture.isAlpha())
			tmpPicture.convertToRGBA8();
		else
			tmpPicture.convertToRGB8();
	}

    FileOutStream *os = FileManager::instance()->openOutStream(filename, FileOutStream::CREATE);

	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;

	png_bytepp row_pointers = NULL;

	// initialize
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!png_ptr)
        O3D_ERROR(E_InvalidResult("Cannot write PNG structure"));

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
        O3D_ERROR(E_InvalidResult("Cannot create PNG info structure"));

	if (setjmp(png_jmpbuf(png_ptr)))
        O3D_ERROR(E_InvalidResult("Error during PNG init_io"));

    png_set_write_fn(png_ptr, (png_voidp)os, _WriteProc, _FlushProc);

	// write header
	if (setjmp(png_jmpbuf(png_ptr)))
        O3D_ERROR(E_InvalidResult("Error during writing PNG header"));

	png_byte color_type;
	png_byte bit_depth = 8;

	UInt32 width = tmpPicture.getWidth();
	UInt32 height = tmpPicture.getHeight();
	int bpp = (int)tmpPicture.getNumComponents();

	switch (tmpPicture.getNumComponents())
	{
		case 3:
			//if (tmpPicture.getPixelFormat() != O3D_RGB_U8)
			//	tmpPicture.convertTo(O3D_RGB_U8);
			color_type = PNG_COLOR_TYPE_RGB;
			break;
		case 4:
			//if (tmpPicture.getPixelFormat() != O3D_RGBA_U8)
			//	tmpPicture.convertTo(O3D_RGBA_U8);
			color_type = PNG_COLOR_TYPE_RGB_ALPHA;
			break;
		default:
            O3D_ERROR(E_InvalidParameter("Unsupported color type"));
			break;
	}

	png_set_IHDR(
			png_ptr,
			info_ptr,
			width,
			height,
		    bit_depth,
		    color_type,
		    PNG_INTERLACE_NONE,
		    PNG_COMPRESSION_TYPE_BASE,
		    PNG_FILTER_TYPE_BASE);

	png_write_info(png_ptr, info_ptr);

	// write bytes
	if (setjmp(png_jmpbuf(png_ptr)))
        O3D_ERROR(E_InvalidResult("Error during writing PNG image data"));

	// set the individual row_pointers to point at the correct offsets
	row_pointers = (png_bytepp)O3D_MALLOC(tmpPicture.getHeight() * sizeof(png_bytep));

	if (!row_pointers)
	{
		png_destroy_write_struct(&png_ptr, &info_ptr);
        O3D_ERROR(E_InvalidAllocation("row_pointers"));
	}

	const UInt8 *data = tmpPicture.getData();

	// read in the bitmap bits via the pointer table
	for (png_uint_32 k = 0; k < height; ++k)
		row_pointers[k] = (png_byte*)&data[width*bpp*k];

	png_write_image(png_ptr, row_pointers);

	// end write
	if (setjmp(png_jmpbuf(png_ptr)))
		O3D_ERROR(E_InvalidResult("Error during writing PNG end of file"));

	// cleanup
	if (row_pointers)
		O3D_FREE(row_pointers);

	png_write_end(png_ptr, NULL);

	if (png_ptr)
		png_destroy_write_struct(&png_ptr, &info_ptr);

    deletePtr(os);
	return True;
}

#ifdef _MSC_VER
#pragma warning (default: 4611)
#endif
