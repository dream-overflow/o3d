/**
 * @file imagetype.h
 * @brief Enumerations for image.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2012-12-15
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_IMAGETYPE_H
#define _O3D_IMAGETYPE_H

namespace o3d {

enum ImageObjectType
{
    ENGINE_IMAGE = 0x06000000          //!< image
};

//! RGBA components for colors
enum ColorCoord
{
	R = 0, //!< Red component
	G = 1, //!< Green component
	B = 2, //!< Blue component
	A = 3  //!< Alpha component
};

//! Image, pixel buffer, and texture formats.
enum PixelFormat
{
    PF_RED_8 = 0,   //!< 8 bits (unsigned integer) red only (filterable, renderable)
    PF_RED_U8,      //!< 8 bits unsigned integer red only.
    PF_RED_I8,      //!< 8 bits signed integer red only.
    PF_RED_SN8,     //!< 8 bits signed normalized red only.
    PF_RED_16,      //!< 16 bits (unsigned integer) red only (not on ES).
    PF_RED_U16,     //!< 16 bits unsigned integer red only.
    PF_RED_I16,     //!< 16 bits signed integer red only.
    PF_RED_SN16,    //!< 16 bits signed normalized red only.
    PF_RED_U32,     //!< 32 bits unsigned intege rred only.
    PF_RED_I32,     //!< 32 bits signed integer red only.
    PF_RED_F16,     //!< 16 bits float red only.
    PF_RED_F32,     //!< 32 bits float red only.

    PF_RG_8,        //!< 8 bits unsigned integer red green (filterable, renderable).
    PF_RG_U8,       //!< 8 bits (unsigned integer) red green.
    PF_RG_I8,       //!< 8 bits signed integer red green.
    PF_RG_SN8,      //!< 8 bits signed nomralized red green.
    PF_RG_16,       //!< 16 bits (unsigned integer) red green (not on ES).
    PF_RG_U16,      //!< 16 bits unsigned integer red green.
    PF_RG_I16,      //!< 16 bits signed integer red green.
    PF_RG_SN16,     //!< 16 bits signed normalized red green
    PF_RG_U32,      //!< 32 bits unsigned integer red green.
    PF_RG_I32,      //!< 32 bits signed integer red green.
    PF_RG_F16,      //!< 16 bits float per channel red green.
    PF_RG_F32,      //!< 32 bits float per channel red green.

    PF_RGB_8,       //!< 8 bits (unsigned integer) red green blue (filterable, renderable).
    PF_RGB_U8,      //!< 8 bits unsigned integer red green blue.
    PF_RGB_I8,      //!< 8 bits signed integer red green blue.
    PF_RGB_SN8,     //!< 8 bits signed nomralized red green blue.
    PF_RGB_U16,     //!< 16 bits unsigned integer red green blue.
    PF_RGB_I16,     //!< 16 bits signed integer red green blue.
    PF_RGB_SN16,    //!< 16 bits signed nomralized red green blue.
    PF_RGB_U32,     //!< 32 bits unsigned integer red green blue.
    PF_RGB_I32,     //!< 32 bits signed integer red green blue.
    PF_RGB_F16,     //!< 16 bits float per channel red green blue.
    PF_RGB_F32,     //!< 32 bits float per channel red green blue.

    PF_RGBA_8,      //!< 8 bits (unsigned integer) red green blue alpha (filterable, renderable).
    PF_RGBA_U8,     //!< 8 bits unsigned integer red green blue alpha.
    PF_RGBA_I8,     //!< 8 bits signed integer red green blue alpha.
    PF_RGBA_SN8,    //!< 8 bits signed nomralized red green blue alpha.
    PF_RGBA_16,     //!< 16 bits (unsigned integer) red green blue alpha (not on ES).
    PF_RGBA_U16,    //!< 16 bits unsigned integer red green blue alpha.
    PF_RGBA_I16,    //!< 16 bits signed integer red green blue alpha.
    PF_RGBA_SN16,   //!< 16 bits signed nomralized red green blue alpha.
    PF_RGBA_U32,    //!< 32 bits unsigned integer red green blue alpha.
    PF_RGBA_I32,    //!< 32 bits signed integer red green blue alpha.
    PF_RGBA_F16,    //!< 16 bits float per channel red green blue alpha.
    PF_RGBA_F32,    //!< 32 bits float per channel red green blue alpha.

    PF_DEPTH,       //!< 16 bits unsigned integer Depth.
    PF_DEPTH_U16,   //!< 16 bits unsigned integer Depth.
    PF_DEPTH_U24,   //!< 24 bits unsigned integer Depth.
    PF_DEPTH_U32,   //!< 32 bits unsigned integer Depth.
	PF_DEPTH_F32,   //!< 32 bits float Depth.
    PF_DEPTH_U24_STENCIL_U8,  //!< 24 bits unsigned integer Depth + 8 bits Stencil.
    PF_DEPTH_F32_STENCIL_U8,  //!< 32 bits float Depth + 8 bits unsigned integer Stencil

	PF_RGB_DXT1,    //!< 4 bits per pixels Red Green Blue compressed.
	PF_RGBA_DXT1,   //!< 4 bits per pixels Red Green Blue Alpha compressed.
	PF_RGBA_DXT3,   //!< 8 bits per pixels Red Green Blue Alpha compressed.
    PF_RGBA_DXT5,   //!< 8 bits per pixels Red Green Blue Alpha compressed.

    // @todo COMPRESSED_RED RG RGB RGBA SRGB_ALPHA RGTC1 RGTC2 BPTC ...

    PF_STENCIL_U1,   //!< 1 bits unsigned integer Stencil
    PF_STENCIL_U4,   //!< 4 bits unsigned integer Stencil
    PF_STENCIL_U8,   //!< 8 bits unsigned integer Stencil
    PF_STENCIL_U16,  //!< 16 bits unsigned integer Stencil
};

} // namespace o3d

#endif // _O3D_IMAGETYPE_H
