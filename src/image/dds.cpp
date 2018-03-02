/**
 * @file dds.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-08-02
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/image/precompiled.h"
#include "o3d/image/dds.h"

#include "o3d/core/filemanager.h"
#include "o3d/core/debug.h"

using namespace o3d;

// Constructor
Dds::Dds(const String& filename) :
	m_size(0),
	m_Format(PF_RGBA_8),
	m_PrimHeight(0),
	m_PrimWidth(0),
	m_Depth(0),
	m_bpp(0),
    m_pData(nullptr),
	m_NumMipMap(0),
	m_cubemap(False),
	m_volume(False),
	m_compressed(False),
	m_CurrentMipMap(0),
	m_CurrentDepth(0),
	m_CurrentSide(ImgFormat::LEFT_SIDE),
	m_Offset(0)
{
    load(filename);
}

Dds::Dds(InStream &is) :
	m_size(0),
	m_Format(PF_RGBA_8),
	m_PrimHeight(0),
	m_PrimWidth(0),
	m_Depth(0),
	m_bpp(0),
    m_pData(nullptr),
	m_NumMipMap(0),
	m_cubemap(False),
	m_volume(False),
	m_compressed(False),
	m_CurrentMipMap(0),
	m_CurrentDepth(0),
	m_CurrentSide(ImgFormat::LEFT_SIDE),
	m_Offset(0)
{
    load(is);
}

// Copy constructor
Dds::Dds(const Dds &dup) :
	m_size(dup.m_size),
	m_Format(dup.m_Format),
	m_PrimHeight(dup.m_PrimHeight),
	m_PrimWidth(dup.m_PrimWidth),
	m_Depth(dup.m_Depth),
	m_bpp(dup.m_bpp),
    m_pData(nullptr),
	m_NumMipMap(dup.m_NumMipMap),
	m_cubemap(dup.m_cubemap),
	m_volume(dup.m_volume),
	m_compressed(dup.m_compressed),
	m_CurrentMipMap(dup.m_CurrentMipMap),
	m_CurrentDepth(dup.m_CurrentDepth),
	m_CurrentSide(dup.m_CurrentSide),
	m_Offset(dup.m_Offset)
{
	if (dup.m_pData)
	{
		m_pData = new UInt8[dup.m_size];
		memcpy(m_pData,dup.m_pData,dup.m_size);
	}
}

// Destructor
Dds::~Dds()
{
	destroy();
}

// destroy all content
void Dds::destroy()
{
	m_CurrentSide = ImgFormat::LEFT_SIDE;

	m_CurrentMipMap =
		m_NumMipMap =
		m_size =
		m_CurrentDepth =
		m_PrimHeight =
		m_PrimWidth =
		m_Depth =
		m_Offset = 0;

	m_Format = PF_RGBA_8;

	m_volume = m_cubemap = m_compressed = False;
	deleteArray(m_pData);
}

// get info for size,data...
void Dds::getInfo(
	UInt32 &size,
	UInt32 &width,
	UInt32 &height,
	UInt8* &data)
{
	// compute picture sizes
	width = o3d::max((UInt32)1,(m_PrimWidth >> m_CurrentMipMap));
	height = o3d::max((UInt32)1,(m_PrimHeight >> m_CurrentMipMap));

	// compute image size
	// DXT1 RGBA (result in 4bpp)
	if ((m_Format == PF_RGB_DXT1) || (m_Format == PF_RGBA_DXT1))
	{
		size = (o3d::max((UInt32)1,(width >> 2)) * o3d::max((UInt32)1,(height >> 2))) << 3;
	}
	// dxt3 and 5 format (result in 4bpp), 3dc format (result in 3bpp)
	else if ((m_Format == PF_RGBA_DXT3) || (m_Format == PF_RGBA_DXT5))
	{
		size = (o3d::max((UInt32)1,(width >> 2)) * o3d::max((UInt32)1,(height >> 2))) << 4;
	}
	// non compressed format
	else
	{
		size = width * height * (m_bpp >> 3);
	}

	// compute the picture data offset
	UInt32 Offset = 0;
	UInt32 i;

	// Move the data ptr to the asked cube-map side. Z+ is the first so there is no offset.
	if (m_cubemap && m_CurrentSide)
	{
		UInt32 imageSize = 0;

		switch (m_Format)
		{
		case PF_RED_8:
			// 8 bits
			if (m_NumMipMap)
			{
				for (i = 0 ; i < m_NumMipMap ; ++i)
					imageSize += o3d::max((UInt32)1,(m_PrimWidth >> i)) *
						o3d::max((UInt32)1,(m_PrimHeight >> i));
			}
			else
			{
				imageSize = m_PrimWidth * m_PrimHeight;
			}
			break;

		case PF_RG_8:
		//case GL_RGB5_A1:
		//case GL_RGB5:
		//case GL_RGBA4:
			// 16 bits
			if (m_NumMipMap)
			{
				for (i = 0 ; i < m_NumMipMap ; ++i)
					imageSize += (	o3d::max((UInt32)1,(m_PrimWidth >> i)) *
							o3d::max((UInt32)1,(m_PrimHeight >> i))) * 2;
			}
			else
			{
				imageSize = m_PrimWidth * m_PrimHeight * 2;
			}
			break;

		case PF_RGB_8:
			// 24 bits
			if (m_NumMipMap)
			{
				for (i = 0 ; i < m_NumMipMap ; ++i)
					imageSize += (	o3d::max((UInt32)1,(m_PrimWidth >> i)) *
							o3d::max((UInt32)1,(m_PrimHeight >> i))) * 3;
			}
			else
			{
				imageSize = m_PrimWidth * m_PrimHeight * 3;
			}
			break;

		case PF_RGBA_8:
			// 32 bits
			if (m_NumMipMap)
			{
				for (i = 0 ; i < m_NumMipMap ; ++i)
					imageSize += (	o3d::max((UInt32)1,(m_PrimWidth >> i)) *
							o3d::max((UInt32)1,(m_PrimHeight >> i))) * 4;
			}
			else
			{
				imageSize = m_PrimWidth * m_PrimHeight * 4;
			}
			break;

		case PF_RGB_DXT1:
		case PF_RGBA_DXT1:
			// 8 bits block
			if (m_NumMipMap)
			{
				for (i = 0 ; i < m_NumMipMap ; ++i)
					imageSize += (	o3d::max((UInt32)1,(m_PrimWidth >> (i+2))) *
							o3d::max((UInt32)1,(m_PrimHeight >> (i+2)))) << 3;
			}
			else
			{
				imageSize = (	o3d::max((UInt32)1,(m_PrimWidth >> 2)) *
						o3d::max((UInt32)1,(m_PrimHeight >> 2))) << 3;
			}
			break;

		case PF_RGBA_DXT3:
		case PF_RGBA_DXT5:
			// 16 bits block
			if (m_NumMipMap)
			{
				for (i = 0 ; i < m_NumMipMap ; ++i)
					imageSize += (	o3d::max((UInt32)1,(m_PrimWidth >> (i+2))) *
							o3d::max((UInt32)1,(m_PrimHeight >> (i+2)))) << 4;
			}
			else
			{
				imageSize = (	o3d::max((UInt32)1,(m_PrimWidth >> 2)) *
						o3d::max((UInt32)1,(m_PrimHeight >> 2))) << 4;
			}
			break;

		default:
			O3D_ERROR(E_InvalidFormat("Pixel format"));
			break;
		}

		// On la taille en octets d'une image complete avec tous ses niveaux de mip map
		// Il nous reste donc a sauter les cubemaps ou depthmaps non desirés
		Offset += imageSize * UInt32(m_CurrentSide);
	}

	// Move the data ptr to the asked mipmap lvl. Level 0 is the first so there is no offset.
	if (m_NumMipMap && m_CurrentMipMap)
	{
		UInt32 skipSize = 0;

		switch (m_Format)
		{
		case PF_RED_8:
			// 8 bits
			for (i = 0 ; i < m_CurrentMipMap ; i++)
				skipSize += o3d::max((UInt32)1,(m_PrimWidth >> i)) *
					o3d::max((UInt32)1,(m_PrimHeight >> i)) *
					o3d::max((UInt32)1,(m_Depth >> i));
			break;

		case PF_RG_8:
		//case GL_RGB5_A1:
		//case GL_RGB5:
		//case GL_RGBA4:
			// 16 bits
			for (i = 0 ; i < m_CurrentMipMap ; i++)
				skipSize += o3d::max((UInt32)1,(m_PrimWidth >> i)) *
					o3d::max((UInt32)1,(m_PrimHeight >> i)) *
					o3d::max((UInt32)1,(m_Depth >> i)) * 2;
			break;

		case PF_RGB_8:
			// 24 bits
			for (i = 0 ; i < m_CurrentMipMap ; i++)
				skipSize += o3d::max((UInt32)1,(m_PrimWidth >> i)) *
					o3d::max((UInt32)1,(m_PrimHeight >> i)) *
					o3d::max((UInt32)1,(m_Depth >> i)) * 3;
			break;

		case PF_RGBA_8:
			// 32 bits
			for (i = 0 ; i < m_CurrentMipMap ; i++)
				skipSize += o3d::max((UInt32)1,(m_PrimWidth >> i)) *
					o3d::max((UInt32)1,(m_PrimHeight >> i)) *
					o3d::max((UInt32)1,(m_Depth >> i)) * 4;
			break;

		case PF_RGB_DXT1:
		case PF_RGBA_DXT1:
			// 8 bytes block
			for (i = 0 ; i < m_CurrentMipMap ; i++)
				skipSize += (	o3d::max((UInt32)1,(m_PrimWidth >> (i+2))) *
						o3d::max((UInt32)1,(m_PrimHeight >> (i+2))) *
						o3d::max((UInt32)1,(m_Depth >> (i+2)))) << 3;
			break;

		case PF_RGBA_DXT3:
		case PF_RGBA_DXT5:
			// 16 bytes block
			for (i = 0 ; i < m_CurrentMipMap ; i++)
				skipSize += (	o3d::max((UInt32)1,(m_PrimWidth >> (i+2))) *
						o3d::max((UInt32)1,(m_PrimHeight >> (i+2))) *
						o3d::max((UInt32)1,(m_Depth >> (i+2)))) << 4;
			break;

		default:
			O3D_ERROR(E_InvalidFormat("Pixel format"));
			break;
		}

		Offset += skipSize;
	}

	data = m_pData + Offset;
	m_Offset = Offset;
}


//---------------------------------------------------------------------------------------
//! @class DDSPixelFormat
//---------------------------------------------------------------------------------------

class DDSPixelFormat
{
public:

	enum DDS
	{
		DDPF_ALPHAPIXELS = 0x00000001,
		DDPF_FOURCC = 0x00000004,
		DDPF_RGB = 0x00000040
	};

	enum DXTFORMAT
	{
		NONE = 0,
		DXT1 = 1,
		DXT2 = 2,
		DXT3 = 3,
		DXT4 = 4,
		DXT5 = 5
	};

	// members --------------------------------------------------------------------------
	Bool alpha;
	Bool fourcc;
	Bool rgb;

	DXTFORMAT dxt;      // DTX compression format

	UInt32 rgbformat; // this is the total number of bits in the format. Should include
	                    // DDPF_RGB in this case. This value is usually 16, 24, or 32.
	                    // For A8R8G8B8, this value would be 32.

	UInt32 rgbmask[3];// For RGB formats, these three fields contain the masks for the
	                    // red, green, and blue channels. For A8R8G8B8, these values would be
	                    // 0x00ff0000, 0x0000ff00, and 0x000000ff respectively.

	UInt32 alphamask; // For RGB formats, this contains the mask for the alpha channel, if any.
	                    // it should include DDPF_ALPHAPIXELS in this case. For A8R8G8B8, this value
	                    // would be 0xff000000.

	// ----------------------------------------------------------------------------------

	Bool readFromFile(InStream &is)
	{
		// struct size
		UInt32 size;
        is >> size;

		if (size != 32)
			return False;

		// flags
		UInt32 flag;
        is >> flag;

		if ((flag & DDPF_ALPHAPIXELS) == DDPF_ALPHAPIXELS)
			alpha = True;
		else
			alpha = False;

		if ((flag & DDPF_FOURCC) == DDPF_FOURCC)
			fourcc = True;
		else
			fourcc = False;

		if ((flag & DDPF_RGB) == DDPF_RGB)
			rgb = True;
		else
			rgb = False;

		// dxt format (if fourcc)
		UInt8 dxtstr[4];
        is.read(dxtstr, 4);
		dxt = NONE;

		if ((dxtstr[0] == 'D') && (dxtstr[1] == 'X') && (dxtstr[2] == 'T'))
		{
			if (dxtstr[3] == '1') dxt = DXT1;
			if (dxtstr[3] == '2') dxt = DXT2;
			if (dxtstr[3] == '3') dxt = DXT3;
			if (dxtstr[3] == '4') dxt = DXT4;
			if (dxtstr[3] == '5') dxt = DXT5;
		}
		else if ((dxtstr[0] == 'R') && (dxtstr[1] == 'X') && (dxtstr[2] == 'G') && (dxtstr[3] == 'B'))
		{
			dxt = DXT5;
		}

		// dtx without DDPF_FOURCC flag
		if (!fourcc && dxt)
			return False;

		// rgb format (for uncompressed)
        is >> rgbformat;

		// rgbformat without DDPF_FOURCC flag
		if (!dxt && !rgb && rgbformat)
			return False;

		// R,G,B mask
        is.read(rgbmask, 3);

		// alpha bit mask
        is >> alphamask;

		// alphamask without DDPF_ALPHAPIXELS flag
		if (!alpha && alphamask)
			return False;

        return True;
	}

	Bool writeToFile(OutStream &os) const
	{
		// don't write
		return False;
	}
};


//---------------------------------------------------------------------------------------
//! @class SurfaceFormatHeader
//---------------------------------------------------------------------------------------
class SurfaceFormatHeader
{
public:

	enum DDSD
	{
		DDSD_CAPS = 0x00000001,
		DDSD_HEIGHT = 0x00000002,
		DDSD_WIDTH = 0x00000004,
		DDSD_PITCH = 0x00000008,
		DDSD_PIXELFORMAT = 0x00001000,
		DDSD_MIPMAPCOUNT = 0x00020000,
		DDSD_LINEARSIZE	= 0x00080000,
		DDSD_DEPTH = 0x00800000
	};

	enum CAPS1
	{
		DDSCAPS_COMPLEX = 0x00000008,
		DDSCAPS_TEXTURE = 0x00001000,
		DDSCAPS_MIPMAP = 0x00400000
	};

	enum CAPS2
	{
		DDSCAPS2_CUBEMAP = 0x00000200,
		DDSCAPS2_CUBEMAP_POSITIVEX = 0x00000400,
		DDSCAPS2_CUBEMAP_NEGATIVEX = 0x00000800,
		DDSCAPS2_CUBEMAP_POSITIVEY = 0x00001000,
		DDSCAPS2_CUBEMAP_NEGATIVEY = 0x00002000,
		DDSCAPS2_CUBEMAP_POSITIVEZ = 0x00004000,
		DDSCAPS2_CUBEMAP_NEGATIVEZ = 0x00008000,
		DDSCAPS2_VOLUME = 0x00200000
	};

private:

	UInt32 defaultFlag;

public:

	// constructor
	SurfaceFormatHeader() : defaultFlag(DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT) {}

	// members --------------------------------------------------------------------------
	DDSPixelFormat pixelformat;

	Bool compressed;
	Bool volumetexture;
	Bool mipmap;
	Bool cubemap;

	UInt32 height,width;      // main picture size
	UInt32 pitchorlinearsize; // for compressed (DDSD_LINEARSIZE) this is the total picture size
	                              // for uncompressed (DDSD_PITCH)    this is the size of a scaline

	UInt32 depth;             // for volume textures (DDSD_DEPTH) this is the depth of the volume
	UInt32 mipmapcount;       // for items with mipmap levels (DDSD_MIPMAPCOUNT) this is the total
	                              // number of levels in the mipmap chain of the main image
	// ----------------------------------------------------------------------------------

	Bool readFromFile(InStream &is)
	{
		// struct size
		UInt32 size;
        is >> size;
		if (size != 124)
			return False;

		// flags
		UInt32 flag;
        is >> flag;
		if ((flag & defaultFlag) != defaultFlag)
			return False;

		// compressed or uncompressed data
		if ((flag & DDSD_LINEARSIZE) == DDSD_LINEARSIZE) compressed = True;
		else if ((flag & DDSD_PITCH) == DDSD_PITCH)      compressed = False;
		else return False;

		// volume texture ?
		if ((flag & DDSD_DEPTH) == DDSD_DEPTH) volumetexture = True;
		else volumetexture = False;

		// is mipmap ?
		if ((flag & DDSD_MIPMAPCOUNT) == DDSD_MIPMAPCOUNT) mipmap = True;
		else mipmap = False;

		// main picture size and pitchorlinearsize
        is >> height
			>> width
			>> pitchorlinearsize
			>> depth
			>> mipmapcount;

		// reseverd (11 DWORD)
		UInt32 reserved[11];
        is.read(reserved, 11);

		// pixel format structure
        if (!pixelformat.readFromFile(is))
			return False;

		// capabilities structure 1
		UInt32 caps1;
        is >> caps1;

		// need always DDSCAPS_TEXTURE
		if ((caps1 & DDSCAPS_TEXTURE) != DDSCAPS_TEXTURE)
			return False;

		// if DDSCAPS_MIPMAP then mipmapcount must be not 0
		if (((caps1 & DDSCAPS_MIPMAP) == DDSCAPS_MIPMAP) && !mipmapcount)
			return False;

		// capabilities structure 2
		UInt32 caps2;
        is >> caps2;

		// is cubemap ?
		if ((caps2 & DDSCAPS2_CUBEMAP) == DDSCAPS2_CUBEMAP)
			cubemap = True;
		else cubemap = False;

		// if DDSCAPS_COMPLEX and not volumetexture or not mipmap
		if ((((caps1 & DDSCAPS_COMPLEX) == DDSCAPS_COMPLEX) && !volumetexture && !mipmapcount && !cubemap))
			return False;

		// DDSCAPS2_VOLUME withouth volumetexture
		if (((caps2 & DDSCAPS2_VOLUME) == DDSCAPS2_VOLUME) && !volumetexture)
			return False;

		// check if there is the 6 side of the cubemap
		if (cubemap)
		{
			if ((caps2 & DDSCAPS2_CUBEMAP_POSITIVEX) != DDSCAPS2_CUBEMAP_POSITIVEX)
				cubemap = False;
			if ((caps2 & DDSCAPS2_CUBEMAP_NEGATIVEX) != DDSCAPS2_CUBEMAP_NEGATIVEX)
				cubemap = False;
			if ((caps2 & DDSCAPS2_CUBEMAP_POSITIVEY) != DDSCAPS2_CUBEMAP_POSITIVEY)
				cubemap = False;
			if ((caps2 & DDSCAPS2_CUBEMAP_NEGATIVEY) != DDSCAPS2_CUBEMAP_NEGATIVEY)
				cubemap = False;
			if ((caps2 & DDSCAPS2_CUBEMAP_POSITIVEZ) != DDSCAPS2_CUBEMAP_POSITIVEZ)
				cubemap = False;
			if ((caps2 & DDSCAPS2_CUBEMAP_NEGATIVEZ) != DDSCAPS2_CUBEMAP_NEGATIVEZ)
				cubemap = False;
		}

		// reserved 2+1 DWORD
        is.read(reserved, 3);

		return True;
	}

	Bool writeToFile(OutStream &os) const
	{
		// don't write
		return False;
	}
};

// Read a DDS file
Bool Dds::load(const String& filename)
{
    InStream *is = FileManager::instance()->openInStream(filename);

    Bool ret = load(*is);
    deletePtr(is);

	return ret;
}

Bool Dds::load(InStream &is)
{
//	UInt8 magicValue[4+1] = { "DDS " };
	UInt8 magicCheck[4];

	SurfaceFormatHeader surfaceFormatHeader;

	// check MagicValue
    is.read(magicCheck, 4);

	if ((magicCheck[0] != 'D') || (magicCheck[1] != 'D') || (magicCheck[2] != 'S') || (magicCheck[3] != ' '))
        O3D_ERROR(E_InvalidFormat("Invalid DDS token"));

	// read Surface Format Header
    if (!surfaceFormatHeader.readFromFile(is))
        O3D_ERROR(E_InvalidFormat("Invalid DDS header"));

	// OpenGL texture format
	// compressed ?
	if (surfaceFormatHeader.pixelformat.dxt != DDSPixelFormat::NONE)
	{
		if (surfaceFormatHeader.pixelformat.dxt == DDSPixelFormat::DXT1)
		{
			if (surfaceFormatHeader.pixelformat.alpha)
				m_Format = PF_RGBA_DXT1;
			else
				m_Format = PF_RGB_DXT1;
		}

		else if ((surfaceFormatHeader.pixelformat.dxt == DDSPixelFormat::DXT2) ||
			(surfaceFormatHeader.pixelformat.dxt == DDSPixelFormat::DXT4))
        {
            O3D_ERROR(E_InvalidFormat("Unsupported surface format"));
		}

		else if (surfaceFormatHeader.pixelformat.dxt == DDSPixelFormat::DXT3)
			m_Format = PF_RGBA_DXT3;

		else if (surfaceFormatHeader.pixelformat.dxt == DDSPixelFormat::DXT5)
			m_Format = PF_RGBA_DXT5;
	}
	// 8,16,24,32 bit non compressed ?
	else if (surfaceFormatHeader.pixelformat.rgb)
	{
		switch (surfaceFormatHeader.pixelformat.rgbformat)
		{
		case 8:
			// alpha 8 only
			if ((surfaceFormatHeader.pixelformat.rgbmask[0] == 0) &&
				(surfaceFormatHeader.pixelformat.rgbmask[1] == 0) &&
				(surfaceFormatHeader.pixelformat.rgbmask[2] == 0) &&
				(surfaceFormatHeader.pixelformat.alphamask == 0xff) &&
				surfaceFormatHeader.pixelformat.alpha)

				m_Format = PF_RED_8;
			// luminance 8 only
			else if ((surfaceFormatHeader.pixelformat.rgbmask[0] == 0xff) &&
					 (surfaceFormatHeader.pixelformat.rgbmask[1] == 0) &&
					 (surfaceFormatHeader.pixelformat.rgbmask[2] == 0))
				m_Format = PF_RED_8;

			else
                O3D_ERROR(E_InvalidFormat("Invalid surface format"));
            break;

		case 16:
			if (surfaceFormatHeader.pixelformat.alpha)
			{
				// luminance alpha 16
				if ((surfaceFormatHeader.pixelformat.rgbmask[0] == 0xff00) &&
					(surfaceFormatHeader.pixelformat.rgbmask[1] == 0) &&
					(surfaceFormatHeader.pixelformat.rgbmask[2] == 0) &&
					(surfaceFormatHeader.pixelformat.alphamask == 0xff))
					m_Format = PF_RG_8;
				// RGB5 A1
				/*else if ((surfaceFormatHeader.pixelformat.rgbmask[0] == 0x7C00) &&
						 (surfaceFormatHeader.pixelformat.rgbmask[1] == 0x3E0) &&
						 (surfaceFormatHeader.pixelformat.rgbmask[2] == 0x1F) &&
						 (surfaceFormatHeader.pixelformat.alphamask == 0x8000))
					m_Format = GL_RGB5_A1;
				// RGBA4
				else if ((surfaceFormatHeader.pixelformat.rgbmask[0] == 0xF00) &&
						 (surfaceFormatHeader.pixelformat.rgbmask[1] == 0xF0) &&
						 (surfaceFormatHeader.pixelformat.rgbmask[2] == 0xF) &&
						 (surfaceFormatHeader.pixelformat.alphamask == 0xF000))
					m_Format = GL_RGBA4;*/
				else
                    O3D_ERROR(E_InvalidFormat("Invalid surface format"));
			}
			else
			{
				// RGB5
				/*if ((surfaceFormatHeader.pixelformat.rgbmask[0] == 0x7C00) &&
					(surfaceFormatHeader.pixelformat.rgbmask[1] == 0x3E0) &&
					(surfaceFormatHeader.pixelformat.rgbmask[2] == 0x1F))
					m_Format = GL_RGB5;
				else*/
                    O3D_ERROR(E_InvalidFormat("Invalid surface format"));
			}
			break;

		case 24:
				// RGB8
				if (!surfaceFormatHeader.pixelformat.alpha &&
					(surfaceFormatHeader.pixelformat.rgbmask[0] == 0xFF0000) &&
					(surfaceFormatHeader.pixelformat.rgbmask[1] == 0xFF00) &&
					(surfaceFormatHeader.pixelformat.rgbmask[2] == 0xFF))
					m_Format = PF_RGB_8;
				else
                    O3D_ERROR(E_InvalidFormat("Invalid surface format"));
			break;

		case 32:
				// RGBA8
				if (surfaceFormatHeader.pixelformat.alpha &&
					(surfaceFormatHeader.pixelformat.rgbmask[0] == 0xFF0000) &&
					(surfaceFormatHeader.pixelformat.rgbmask[1] == 0xFF00) &&
					(surfaceFormatHeader.pixelformat.rgbmask[2] == 0xFF) &&
					(surfaceFormatHeader.pixelformat.alphamask  == 0xFF000000))
					m_Format = PF_RGBA_8;
				else
                    O3D_ERROR(E_InvalidFormat("Invalid surface format"));
			break;
		}
	}
	else
	{
        O3D_ERROR(E_InvalidFormat("Invalid surface format"));
	}

	m_PrimHeight = surfaceFormatHeader.height;
	m_PrimWidth = surfaceFormatHeader.width;
	m_Depth = surfaceFormatHeader.depth;
	m_bpp = surfaceFormatHeader.pixelformat.rgbformat;
	m_volume = surfaceFormatHeader.volumetexture;
	m_cubemap = surfaceFormatHeader.cubemap;
	m_compressed = surfaceFormatHeader.compressed;
	m_NumMipMap = surfaceFormatHeader.mipmapcount - 1;

	// compute size of all texture(s) data(s)
	UInt32 tempSize = 0;
	m_size = 0;

	for (UInt32 i = 0;
		i < (surfaceFormatHeader.mipmapcount > 0 ? surfaceFormatHeader.mipmapcount : 1);
		++i)
	{
		// dxt 1 format
		if ((m_Format == PF_RGB_DXT1) || (m_Format == PF_RGBA_DXT1))
		{
			tempSize = (o3d::max((UInt32)1,(m_PrimWidth >> (i+2))) *
					o3d::max((UInt32)1,(m_PrimHeight >> (i+2))) *
					o3d::max((UInt32)1,(m_Depth >> (i+2)))) << 3;

			m_size += tempSize;
		}
		// dxt3 and 5 format
		else if ((m_Format == PF_RGBA_DXT3) || (m_Format == PF_RGBA_DXT5))
		{
			tempSize = (o3d::max((UInt32)1,(m_PrimWidth >> (i+2))) *
					o3d::max((UInt32)1,(m_PrimHeight >> (i+2))) *
					o3d::max((UInt32)1,(m_Depth >> (i+2)))) << 4;

			m_size += tempSize;
		}
		// non compressed format
		else
		{
			m_size += (o3d::max((UInt32)1,(m_PrimWidth >> i)) *
					o3d::max((UInt32)1,(m_PrimHeight >> i)) *
					o3d::max((UInt32)1,(m_Depth >> i))) *
				(surfaceFormatHeader.pixelformat.rgbformat >> 3);
		}
	}

	if (surfaceFormatHeader.cubemap)
		m_size *= 6;

	// read data
	m_pData = new UInt8[m_size];
	if (!m_pData)
        O3D_ERROR(E_InvalidAllocation("m_pData is null"));

    is.read(m_pData, m_size);

	// swap 16,24 and 32 non compressed mode
	if ((surfaceFormatHeader.pixelformat.rgbformat > 8) &&
	    (surfaceFormatHeader.pixelformat.dxt == DDSPixelFormat::NONE))
	{
		UInt32 i;
		UInt8 Swap;

		switch (m_Format)
		{
			case PF_RG_8:
				// AL to LA
				for (i = 0 ; i < m_size ; i += 2)
				{
					Swap = m_pData[i];          // (A)
					m_pData[i] = m_pData[i+1];  //  L
					m_pData[i+1] = Swap;        //  A
				}
				break;

			case PF_RGB_8:
				// BGR to RGB
				for (i = 0 ; i < m_size ; i += 3)
				{
					Swap = m_pData[i];          // (B)
					m_pData[i] = m_pData[i+2];  //  R
					m_pData[i+2] = Swap;        //  B
				}
				break;

			case PF_RGBA_8:
				// BGRA to RGBA
				for (i = 0 ; i < m_size ; i += 4)
				{
					Swap = m_pData[i];          // (B)
					m_pData[i] = m_pData[i+2];  //  R
					m_pData[i+2] = Swap;        //  B
				}
				break;

			default:
                O3D_ERROR(E_InvalidFormat("Invalid surface format header"));
				break;
		}
	}

	return True;
}


//---------------------------------------------------------------------------------------
// DdsImg
//---------------------------------------------------------------------------------------

// Copy constructor
DdsImg::DdsImg(const DdsImg &dup) :
	ImgFormat(),
    m_pDds(nullptr)
{
	if (dup.m_pDds)
	{
		m_pDds = new Dds(*dup.m_pDds);
		m_pDds->getInfo(m_size,m_width,m_height,m_data);
		m_state = True;
	}
	else
		m_state = False;
}

Bool DdsImg::checkFormat(InStream &is)
{
//	UInt8 MagicValue[4+1] = { "DDS " };
    UInt8 magicCheck[4];

	// check MagicValue
    is.read(magicCheck, 4);
    is.seek(-4);

    if ((magicCheck[0] != 'D') || (magicCheck[1] != 'D') || (magicCheck[2] != 'S') || (magicCheck[3] != ' ')) {
        return False;
    }

	return True;
}

void DdsImg::destroy()
{
	ImgFormat::destroy();
	deletePtr(m_pDds);
}

Bool DdsImg::load(InStream &is)
{
	m_pDds = new Dds;

    if (m_pDds->load(is))
	{
		m_pDds->getInfo(m_size, m_width ,m_height, m_data);
		return True;
	}

	return False;
}

Bool DdsImg::load(InStream &is, PixelFormat pixelFormat)
{
	//switch (pixelFormat)
	//{
	//	default:
			O3D_ERROR(E_InvalidParameter("Unsupported convert format"));
			return False;
	//}
}

Bool DdsImg::save(OutStream &os, PixelFormat pixelFormat)
{
	O3D_ERROR(E_InvalidOperation("Not yet implemented"));
	return False;
}

Bool DdsImg::isComplex() const
{
	return (m_pDds->isMipMap() || m_pDds->isCubeMap() || m_pDds->isVolumeTexture());
}

Bool DdsImg::isCompressed() const
{
	return m_pDds->isCompressed();
}

PixelFormat DdsImg::getPixelFormat() const
{
    return m_pDds->getPixelFormat();
}

Bool DdsImg::isMipMap() const
{
	return m_pDds->isMipMap();
}

Bool DdsImg::isCubeMap() const
{
	return m_pDds->isCubeMap();
}

Bool DdsImg::isVolumeTexture() const
{
	return m_pDds->isVolumeTexture();
}

UInt32 DdsImg::getNumMipMapLvl() const
{
	return m_pDds->getNumMipMapLvl();
}

UInt32 DdsImg::getNumDepthLayer() const
{
	return m_pDds->getNumDepthLayer();;
}

Bool DdsImg::bindMipMapLvl(UInt32 lvl)
{
	Bool ret = m_pDds->bindMipMapLvl(lvl);

	if (ret)
		m_pDds->getInfo(m_size,m_width,m_height,m_data);

	return ret;
}

void DdsImg::bindCubeMapSide(ImgFormat::CubeMapSide side)
{
	m_pDds->bindCubeMapSide(side);
	m_pDds->getInfo(m_size,m_width,m_height,m_data);
}

Bool DdsImg::bindVolumeLayer(UInt32 layer)
{
	Bool ret = m_pDds->bindVolumeLayer(layer);

	if (ret)
		m_pDds->getInfo(m_size,m_width,m_height,m_data);

	return ret;
}

UInt32 DdsImg::getCurrentMipMapLvl() const
{
	return m_pDds->getCurrentMipMapLvl();
}

UInt32 DdsImg::getCurrentCubeMapSide() const
{
	return m_pDds->getCurrentCubeMapSide();
}

UInt32 DdsImg::getCurrentDepthLayer() const
{
	return m_pDds->getCurrentDepthLayer();
}

