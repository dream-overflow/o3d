/**
 * @file jpg.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include <stdio.h>

#include "o3d/image/precompiled.h"
#include "o3d/image/jpg.h"

#include "o3d/image/image.h"
#include "o3d/core/filemanager.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef O3D_WINDOWS
	#define XMD_H 1
#endif

#ifdef _MSC_VER
    #include <libjpeg/jpeglib.h>
    #include <libjpeg/jerror.h>
#else
	#include <jpeglib.h>
	#include <jerror.h>
#endif

#ifdef __cplusplus
}
#endif

using namespace o3d;

//---------------------------------------------------------------------------------------
// We need somes extra functions to libjpeg for add the possibility to load jpeg from
// memory... then add this ... marvelous code :-D
//---------------------------------------------------------------------------------------

// buffer input size
#define INPUT_BUF_SIZE 4096
// buffer output size
#define OUTPUT_BUF_SIZE 4096

// somes structs
typedef struct my_source_mem
{
    struct jpeg_source_mgr pub;	/* public fields */

    int	indexinmem;
    JOCTET * inmem;		    /* source stream */
    JOCTET * buffer;	    /* start of buffer */
    int lenght;		        /* Size of buffer in memory */
    boolean start_of_file;	/* have we gotten any data yet? */
} my_source_mem;

typedef struct my_source_istream
{
	struct jpeg_source_mgr pub;	/* public fields */
    InStream *is;
    Int64 init_pos;         /* initial stream position */

    JOCTET * buffer;	    /* start of buffer */
    boolean start_of_file;	/* have we gotten any data yet? */
} my_source_istream;

typedef struct my_destination_ostream
{
    struct jpeg_destination_mgr pub; /* public fields */
    OutStream *os;

    JOCTET * buffer;	    /* start of buffer */
    int lenght;		        /* Size of buffer in memory */
    boolean start_of_file;	/* have we gotten any data yet? */
} my_destination_ostream;

// fonctions
//static boolean fill_mem_input_buffer(j_decompress_ptr cinfo)
//{
//    my_source_mem* src = (my_source_mem*)cinfo->src;
//	size_t nbytes;

//    if (src->indexinmem + INPUT_BUF_SIZE > src->lenght)
//		nbytes=src->lenght-src->indexinmem;
//	else
//		nbytes = INPUT_BUF_SIZE;

//	/*if (src->indexinmem+INPUT_BUF_SIZE>((cinfo->output_width * cinfo->output_components) *
//		cinfo->output_height ))
//	{
//		nbytes = ((src->indexinmem+INPUT_BUF_SIZE) -
//		  	 ((cinfo->output_width * cinfo->output_components) *
//			  cinfo->output_height ));
//	}
//	else
//		nbytes=INPUT_BUF_SIZE;*/

//    memcpy(src->buffer, src->inmem, nbytes);
//	src->inmem += nbytes;
//	src->indexinmem += (int)nbytes;

//	/*if (nbytes <= 0)
//	{
//		if (src->start_of_file)
//			ERREXIT(cinfo, JERR_INPUT_EMPTY);
//		WARNMS(cinfo, JWRN_JPEG_EOF);
//		src->buffer[0] = (JOCTET) 0xFF;
//		src->buffer[1] = (JOCTET) JPEG_EOI;
//		nbytes = 2;
//	}*/

//	src->pub.next_input_byte = src->buffer;
//    src->pub.bytes_in_buffer = nbytes;
//	src->start_of_file = FALSE;

//    return TRUE;
//}

// fonctions
static boolean fill_is_input_buffer(j_decompress_ptr cinfo)
{
  /*  my_source_istream* src = (my_source_istream*)cinfo->src;
    size_t nbytes;

    if (INPUT_BUF_SIZE > src->is->getAvailable())
        nbytes = src->is->getAvailable();
    else
        nbytes = INPUT_BUF_SIZE;

    src->is->read(src->buffer, 1, nbytes);

    src->pub.next_input_byte = src->buffer;
    src->pub.bytes_in_buffer = nbytes;
    src->start_of_file = FALSE;*/

    my_source_istream* src = (my_source_istream*)cinfo->src;
    size_t nbytes;

    if (INPUT_BUF_SIZE > src->is->getAvailable())
        nbytes = src->is->getAvailable();
    else
        nbytes = INPUT_BUF_SIZE;

    src->is->read(src->buffer, nbytes);

    src->pub.bytes_in_buffer = nbytes;
    src->start_of_file = FALSE;

    if (src->pub.bytes_in_buffer == 0) // check for end-of-stream
    {
        // Insert a fake EOI marker
        src->buffer[0] = 0xFF;
        src->buffer[1] = JPEG_EOI;
        src->pub.bytes_in_buffer = 2;
    }
    else
    {
        src->pub.next_input_byte = src->buffer;
    }

    return TRUE;
}

//static void skip_mem_input_data(j_decompress_ptr cinfo, long num_bytes)
//{
//    my_source_mem *src = (my_source_mem*)cinfo->src;

//	if (num_bytes > 0)
//	{
//		while (num_bytes > (long) src->pub.bytes_in_buffer)
//		{
//			num_bytes -= (long) src->pub.bytes_in_buffer;
//            (void) fill_mem_input_buffer(cinfo);
//			/* note we assume that fill_input_buffer will never return FALSE,
//			 * so suspension need not be handled.
//			 */
//		}

//		src->pub.next_input_byte += (size_t) num_bytes;
//		src->pub.bytes_in_buffer -= (size_t) num_bytes;
//	}
//}

static void skip_is_input_data(j_decompress_ptr cinfo, long num_bytes)
{
    my_source_istream *src = (my_source_istream*)cinfo->src;

    if (num_bytes > 0)
    {
        while (num_bytes > (long) src->pub.bytes_in_buffer)
        {
            num_bytes -= (long) src->pub.bytes_in_buffer;
            (void) fill_is_input_buffer(cinfo);
        }

        src->pub.next_input_byte += (size_t) num_bytes;
        src->pub.bytes_in_buffer -= (size_t) num_bytes;
    }
}

//static void init_mem_source(j_decompress_ptr cinfo)
//{
//    my_source_mem *src = (my_source_mem*)cinfo->src;

//	/* We reset the empty-input-file flag for each image,
//	 * but we don't clear the input buffer.
//	 * This is correct behavior for reading a series of images from one source.
//	 */
//    src->start_of_file = TRUE;
//}

static void init_is_source(j_decompress_ptr cinfo)
{
    my_source_istream *src = (my_source_istream*)cinfo->src;

    src->is->reset(src->init_pos);
    src->start_of_file = TRUE;
}

static void init_os_destination(j_compress_ptr cinfo)
{
    my_destination_ostream *dest = (my_destination_ostream*)cinfo->dest;

    /* Allocate the output buffer --- it will be released when done with image */
    dest->buffer = (JOCTET *)
        (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
        INPUT_BUF_SIZE * sizeof(JOCTET));

    dest->pub.next_output_byte = dest->buffer;
    dest->pub.free_in_buffer = OUTPUT_BUF_SIZE;
}

static boolean empty_os_buffer(j_compress_ptr cinfo)
{
    my_destination_ostream *dest = (my_destination_ostream*)cinfo->dest;

    dest->os->writer(dest->buffer, 1, OUTPUT_BUF_SIZE);
    dest->pub.next_output_byte = dest->buffer;
    dest->pub.free_in_buffer = OUTPUT_BUF_SIZE;

    return TRUE;
}

//static void mem_term_source(j_decompress_ptr cinfo)
//{
//    //my_source_istream *src = (my_source_mem*)cinfo->src;
//}

static void in_term_source(j_decompress_ptr cinfo)
{
    my_source_istream *src = (my_source_istream*)cinfo->src;

    if (src->pub.bytes_in_buffer > 0)
       src->is->seek(-(long)src->pub.bytes_in_buffer);
}

static void out_term_destination(j_compress_ptr cinfo)
{
    my_destination_ostream *dest = (my_destination_ostream*)cinfo->dest;

    size_t nbytes = OUTPUT_BUF_SIZE - dest->pub.free_in_buffer;

    if (nbytes > 0)
        dest->os->writer(dest->buffer, 1, nbytes);
}

//static void jpeg_mem_src(j_decompress_ptr cinfo, JOCTET * memptr, int lenght)
//{
//    my_source_mem* src;

//    /* The source object and input buffer are made permanent so that a series
//     * of JPEG images can be read from the same file by calling jpeg_stdio_src
//     * only before the first one.  (If we discarded the buffer at the end of
//     * one image, we'd likely lose the start of the next one.)
//     * This makes it unsafe to use this manager and a different source
//     * manager serially with the same JPEG object.  Caveat programmer.
//     */

//    /* first time for this JPEG object? */
//    if (cinfo->src == nullptr)
//    {
//        cinfo->src = (struct jpeg_source_mgr*)(*cinfo->mem->alloc_small)(
//                (j_common_ptr) cinfo, JPOOL_PERMANENT,sizeof(my_source_mem));

//        src = (my_source_mem*) cinfo->src;

//        src->buffer = (JOCTET *)(*cinfo->mem->alloc_small) (
//                (j_common_ptr) cinfo, JPOOL_PERMANENT,INPUT_BUF_SIZE * sizeof(JOCTET));
//    }

//    src = (my_source_mem*)cinfo->src;
//    src->pub.init_source = init_mem_source;
//    src->pub.fill_input_buffer = fill_mem_input_buffer;
//    src->pub.skip_input_data = skip_mem_input_data;
//    src->pub.resync_to_restart = jpeg_resync_to_restart; /* use default method */
//    src->pub.term_source = mem_term_source;
//    src->inmem = memptr;
//    src->indexinmem = 0;
//    src->lenght = lenght;
//    src->pub.bytes_in_buffer = 0; /* forces fill_input_buffer on first read */
//    src->pub.next_input_byte = NULL; /* until buffer loaded */
//}

static void jpeg_istream_src(j_decompress_ptr cinfo, InStream *is)
{
    my_source_istream* src;

	/* first time for this JPEG object? */
    if (cinfo->src == nullptr)
	{
		cinfo->src = (struct jpeg_source_mgr*)(*cinfo->mem->alloc_small)(
                (j_common_ptr) cinfo, JPOOL_PERMANENT,sizeof(my_source_istream));

        src = (my_source_istream*)cinfo->src;

        // alloc buffer
		src->buffer = (JOCTET *)(*cinfo->mem->alloc_small) (
                (j_common_ptr) cinfo, JPOOL_PERMANENT, INPUT_BUF_SIZE * sizeof(JOCTET));
	}

    src = (my_source_istream*)cinfo->src;
    src->pub.init_source = init_is_source;
    src->pub.fill_input_buffer = fill_is_input_buffer;
    src->pub.skip_input_data = skip_is_input_data;
	src->pub.resync_to_restart = jpeg_resync_to_restart; /* use default method */
    src->pub.term_source = in_term_source;
    src->is = is;
    src->init_pos = is->getPosition();
    src->pub.bytes_in_buffer = 0;    /* forces fill_input_buffer on first read */
	src->pub.next_input_byte = NULL; /* until buffer loaded */
}

static void jpeg_ostream_dst(j_compress_ptr cinfo, OutStream *os)
{
    my_destination_ostream *dest;

    /* first time for this JPEG object? */
    if (cinfo->dest == nullptr)
    {
        cinfo->dest = (struct jpeg_destination_mgr*)(*cinfo->mem->alloc_small)(
                (j_common_ptr) cinfo, JPOOL_PERMANENT,sizeof(my_destination_ostream));

        dest = (my_destination_ostream*)cinfo->dest;

        // alloc buffer
        dest->buffer = (JOCTET *)(*cinfo->mem->alloc_small) (
                (j_common_ptr) cinfo, JPOOL_PERMANENT, INPUT_BUF_SIZE * sizeof(JOCTET));
    }

    dest = (my_destination_ostream*)cinfo->dest;
    dest->pub.init_destination = init_os_destination;
    dest->pub.empty_output_buffer = empty_os_buffer; /* use default method */
    dest->pub.term_destination = out_term_destination;
    dest->os = os;
    dest->pub.free_in_buffer = 0;
    dest->pub.next_output_byte = NULL; /* until buffer created */
}

// check the first bytes of the file for see his format
Bool Jpg::checkFormat(InStream &is)
{
	Char id[2];
    is.read(id, 2);
    is.seek(-2);

    if (((UInt8)id[0] == 0xff) && ((UInt8)id[1] == 0xd8)) {
		return True;
    }

	return False;
}

// load jpeg from file
Bool Jpg::load(InStream &is)
{
    return loadDefault(is);
}

// load jpeg from file
Bool Jpg::load(InStream &is, PixelFormat pixelFormat)
{
    switch (pixelFormat) {
		case PF_RGB_U8:
            return loadRgb24(is);
		case PF_RGBA_U8:
            return loadRgba32(is);
		default:
			O3D_ERROR(E_InvalidParameter("Unsupported convert format"));
	}
}

PixelFormat Jpg::getPixelFormat() const
{
	switch(m_bpp)
	{
		case 1:
			return PF_RED_U8;
		case 3:
			return PF_RGB_U8;
		case 4:
			return PF_RGBA_U8;
		default:
			O3D_ERROR(E_InvalidParameter("Unsupported pixel format"));
			return PF_RGBA_U8;
	}
}

Bool Jpg::loadDefault(InStream &is)
{
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;

	UInt8 *ligne;
	UInt8 *pData = NULL;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

    /* // for load managing
    if (is.getType() == File::DISK_FILE) {
		// load entirely the file with or without the managing
        UInt32 fLen = is.fileLength();
		pData = new UInt8[fLen];
        is.read(pData, 1, fLen);

		jpeg_mem_src(&cinfo,(JOCTET*)pData,(int)fLen);

		//old code
        //jpeg_stdio_src(&cinfo,((DiskFile&)file).getFile());
    } else if (is.getType() == File::MEM_FILE) {
        jpeg_mem_src(&cinfo,((MemFile&)is).getData(),((MemFile&)is).fileLength());
    } */

    jpeg_istream_src(&cinfo, &is);

	// jpeg info header
	jpeg_read_header(&cinfo, TRUE);
	jpeg_start_decompress(&cinfo);

    if (cinfo.jpeg_color_space == JCS_GRAYSCALE) {
		m_data = new UInt8[cinfo.image_height*cinfo.image_width];
		m_bpp = 1;
    } else {
		m_data = new UInt8[cinfo.image_height*cinfo.image_width*3];
		m_bpp = 3;
	}

    if (!m_data) {
        O3D_ERROR(E_InvalidAllocation("data is null"));
	}

	ligne = m_data;

	// read line by line
    while (cinfo.output_scanline < cinfo.output_height) {
		ligne = m_data + m_bpp * cinfo.output_width*cinfo.output_scanline;
		jpeg_read_scanlines(&cinfo, (JSAMPARRAY)&ligne, 1);
	}

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	deleteArray(pData);

	m_width = cinfo.image_width;
	m_height = cinfo.image_height;
	m_size = m_width * m_height * m_bpp;

	m_state = True;

	/* It's better to compile your libjpeg with the same O3D color mode
	 * so if O3D is in BGR mode then recompile libjpeg in BGR mode also
	 * you will have one more pass whose slow-down the jpeg loading.
	 * ------------------------------------------------------------------------
	 * howto setBGR(RGB) in libjpeg:
	 * edit jmorecfg.h and find for RGB_RED and set it to 2(0) then find for
	 * RGB_BLUE and set it to 0(2) that will inverse blue and red composantes.
	 * then launch the makefile (in release mode) and paste the new lib file
	 * in ./bin/lib/ (default: O3D and JPEGLIB are in the same mode)
	 */
#if (RGB_RED != 0)
	if (m_bpp == 3)
	{
		UInt8 buf;
		for (UInt32 i = 0 ; i < m_size ; i += m_bpp)
		{
			buf = m_data[i];
			m_data[i] = m_data[i+2];
			m_data[i+2] = buf;
		}
	}
#endif
	return True;
}

Bool Jpg::loadRgb24(InStream &is)
{
    return loadDefault(is);
}

Bool Jpg::loadRgba32(InStream &is)
{
    if (loadDefault(is))
		return convert24to32();

	return False;
}

// write jpeg
Bool Jpg::save(OutStream &os, PixelFormat pixelFormat)
{
	O3D_ERROR(E_InvalidOperation("Not yet implemented"));
	return False;
}

Bool Image::saveJpg(const String &filename, Int32 quality)
{
	if (!m_state)
		O3D_ERROR(E_InvalidPrecondition("Data must be non null " + filename));

	if (!m_data.get() || !m_data->data || !m_width || !m_height)
		return False;

	Image tmpPicture(*this);

	if ((tmpPicture.getPixelFormat() != PF_RGB_U8) && (tmpPicture.getPixelFormat() != PF_RED_U8))
		tmpPicture.convertToRGB8();

#if (RGB_RED != 0)
	if (tmpPicture.getNumComponents() >= 3)
		tmpPicture.swapRB();
#endif

    FileOutStream *os = FileManager::instance()->openOutStream(filename, FileOutStream::CREATE);

	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;

	UInt8 *ligne;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);

    jpeg_ostream_dst(&cinfo, os);

	// cinfo settings
	cinfo.image_width = m_width;
	cinfo.image_height = m_height;
	cinfo.input_components = tmpPicture.getNumComponents();
	cinfo.in_color_space = (tmpPicture.getNumComponents() == 1 ? JCS_GRAYSCALE : JCS_RGB);

	jpeg_set_defaults(&cinfo);

	// compression quality
	jpeg_set_quality(&cinfo, quality, TRUE/* limit to baseline-JPEG values */);

	jpeg_start_compress(&cinfo, TRUE);

	UInt8 *data = tmpPicture.m_data->data;
	ligne = data;

	while (cinfo.next_scanline<cinfo.image_height)
	{
		ligne = data + cinfo.input_components * cinfo.image_width * cinfo.next_scanline;
		jpeg_write_scanlines(&cinfo,(JSAMPARRAY)&ligne, 1);
	}

	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);

    deletePtr(os);
	return True;
}

Bool Image::saveRgbJpg(const String &filename, Int32 quality)
{
	if (!m_state)
		O3D_ERROR(E_InvalidPrecondition("Data must be non null " + filename));

	if (!m_data.get() || !m_data->data || !m_width || !m_height)
		return False;

	Image tmpPicture(*this);

	if (tmpPicture.getPixelFormat() != PF_RGB_U8)
		tmpPicture.convertToRGB8();

#if (RGB_RED != 0)
	if (tmpPicture.getNumComponents() >= 3)
		tmpPicture.swapRB();
#endif

    FileOutStream *os = FileManager::instance()->openOutStream(filename, FileOutStream::CREATE);

    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;

    UInt8 *ligne;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);

    jpeg_ostream_dst(&cinfo, os);

	// cinfo settings
	cinfo.image_width = m_width;
	cinfo.image_height = m_height;
	cinfo.input_components = tmpPicture.getNumComponents();
	cinfo.in_color_space = (tmpPicture.getNumComponents() == 1 ? JCS_GRAYSCALE : JCS_RGB);

	jpeg_set_defaults(&cinfo);

	// compression quality
	jpeg_set_quality(&cinfo, quality, TRUE/* limit to baseline-JPEG values */);

	jpeg_start_compress(&cinfo, TRUE);

	UInt8 *data = tmpPicture.m_data->data;
	ligne = data;

	while (cinfo.next_scanline<cinfo.image_height)
	{
		ligne = data + cinfo.input_components * cinfo.image_width * cinfo.next_scanline;
		jpeg_write_scanlines(&cinfo,(JSAMPARRAY)&ligne, 1);
	}

	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);

    deletePtr(os);
	return True;
}

// convert 24 to 32
Bool Jpg::convert24to32()
{
	UInt8 *exdata = new UInt8[m_size];

	// copie de l'ancien buffer pour pouvoir remplace les data de la classe
	memcpy(exdata,m_data,m_size);

	deleteArray(m_data);

	m_size = m_width * m_height * 4;
	m_data = new UInt8[m_size];

	UInt32 i,j = 0;

    for (i = 0 ; i < m_size  ; i += 4) {
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

#undef INPUT_BUF_SIZE
