/**
 * @file dds.h
 * @brief Loading of DDS textures (24,32 bpp) mipmap, DXT1,3,5. Cubemap, volume texture.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-08-02
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_DDS_H
#define _O3D_DDS_H

#include "o3d/core/file.h"
#include "o3d/core/smartcounter.h"
#include "o3d/core/memorydbg.h"
#include "imgformat.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class Dds
//-------------------------------------------------------------------------------------
//! Loading of DDS textures (24,32 bpp) mipmap, DXT1,3,5. Cubemap, volume texture.
//---------------------------------------------------------------------------------------
class Dds : NonAssignable<>
{
public:

	//! Default constructor
    Dds(InStream &is);

	//! Construct from a filename
	Dds(const String& filename = "");

	//! destructor
	virtual ~Dds();

	//! Copy constructor
	Dds(const Dds &dup);

	//! load a DDS from an opened file
    Bool load(InStream &is);

	//! load a DDS from a filename
	Bool load(const String& filename);

	//! destroy all content
	void destroy();

	//! get info for size,data...
	void getInfo(UInt32 &size, UInt32 &width, UInt32 &height, UInt8* &data);

	//! get pixel format
	inline PixelFormat getPixelFormat() const { return m_Format; }

	//! get if its a mipmap texture
	inline Bool isMipMap() const { return m_NumMipMap; }

	//! get if its a cubemap texture
	inline Bool isCubeMap() const { return m_cubemap; }

	//! get if its a volume texture
	inline Bool isVolumeTexture() const { return m_volume; }

	//! for volume texture get depth
	inline UInt32 getNumDepthLayer() const { return m_Depth; }

	//! is a compressed DDS
	inline Bool isCompressed() const { return m_compressed; }

	//! get size for the selected texture
	UInt32 getSize() const;

	//! select a mipmap lvl
	inline Bool bindMipMapLvl(UInt32 lvl)
	{
		if (lvl < m_NumMipMap+1)
		{
			m_CurrentMipMap = lvl;
			return True;
		}
		return False;
	}

	//! get current mipmap lvl
	inline UInt32 getCurrentMipMapLvl() const { return m_CurrentMipMap; }

	//! get current cubemap side
	inline UInt32 getCurrentCubeMapSide() const { return m_CurrentSide; }

	//! get current depth volume layer
	inline UInt32 getCurrentDepthLayer() const { return m_CurrentDepth; }

	//! select a cubemap side
	inline void bindCubeMapSide(ImgFormat::CubeMapSide side) { m_CurrentSide = side; }

	//! select a depth
	Bool bindVolumeLayer(UInt32 layer)
	{
        if (layer < m_Depth)
		{
			m_CurrentDepth = layer;
			return True;
		}
		return False;
	}

	//! get num of mipmap (don't count the primary texture)
	//! then the real number of texture is GetNumMipMapLvl + 1
	inline UInt32 getNumMipMapLvl() const { return m_NumMipMap; }

	//! get current mipmap height
	inline UInt32 getHeight() const
	{
		return o3d::max((UInt32)1,(m_PrimHeight >> m_CurrentMipMap));
	}

	//! get current mipmap width
	inline UInt32 getWidth() const
	{
		return o3d::max((UInt32)1,(m_PrimWidth >> m_CurrentMipMap));
	}

	//! get current mipmap depth
	inline UInt32 getDepth() const
	{
		return o3d::max((UInt32)1,(m_Depth >> m_CurrentMipMap));
	}

	//! get current mipmap data
	inline UInt8* getData() { return m_pData + m_Offset; }

private:

	UInt32 m_size;        //!< primary size

	PixelFormat m_Format;  //!< pixel format

	UInt32 m_PrimHeight, m_PrimWidth; //!< size of the primary texture
	UInt32 m_Depth;
	UInt32 m_bpp;
	UInt8 *m_pData;     //!< all datas

	UInt32 m_NumMipMap; //!< number of mipmap lvl
	Bool m_cubemap;     //!< is contain a cubemap
	Bool m_volume;      //!< is contain a volume texture
	Bool m_compressed;  //!< is a compressed picture

	UInt32 m_CurrentMipMap;       //!< current bound mipmap
	UInt32 m_CurrentDepth;        //!< current bound depth
	ImgFormat::CubeMapSide m_CurrentSide;   //!< current bound size

	UInt32 m_Offset;    //!< offset to the current mipmap or cubemap or ...
};


//---------------------------------------------------------------------------------------
//! @class DdsImg
//-------------------------------------------------------------------------------------
//! DDS ImgFormat wrapper
//---------------------------------------------------------------------------------------
class O3D_API DdsImg : public ImgFormat
{
public:

	//! Default constructor
    DdsImg() :
        ImgFormat(),
        m_pDds(nullptr)
    {

    }

	//! Copy constructor
	DdsImg(const DdsImg &dup);

	virtual ImgFormat* clone() const { return (ImgFormat*)new DdsImg(*this); }

	virtual void destroy();

	virtual Bool isComplex() const;
	virtual Bool isCompressed() const;

    virtual Bool checkFormat(InStream &is);

	virtual PixelFormat getPixelFormat() const;

	//-----------------------------------------------------------------------------------
	// Reading
	//-----------------------------------------------------------------------------------

    virtual Bool load(InStream &is);
    virtual Bool load(InStream &is, PixelFormat pixelFormat);
    virtual Bool save(OutStream &os, PixelFormat pixelFormat);

	//-----------------------------------------------------------------------------------
	// Complex pictures
	//-----------------------------------------------------------------------------------

	virtual Bool isMipMap() const;
	virtual Bool isCubeMap() const;
	virtual Bool isVolumeTexture() const;

	virtual UInt32 getNumMipMapLvl() const;
	virtual UInt32 getNumDepthLayer() const;

	virtual Bool bindMipMapLvl(UInt32 lvl);
	virtual void bindCubeMapSide(ImgFormat::CubeMapSide side);
	virtual Bool bindVolumeLayer(UInt32 layer);

	virtual UInt32 getCurrentMipMapLvl() const;
	virtual UInt32 getCurrentCubeMapSide() const;
	virtual UInt32 getCurrentDepthLayer() const;

protected:

	Dds *m_pDds;
};

} // namespace o3d

#endif // _O3D_DDS_H

