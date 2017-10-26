/**
 * @file imgformat.h
 * @brief Interface for picture format.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2005-10-06
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_IMGFORMAT_H
#define _O3D_IMGFORMAT_H

#include "o3d/core/file.h"
#include "o3d/core/smartcounter.h"
#include "o3d/core/memorydbg.h"
#include "o3d/core/debug.h"

#include "imagetype.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class ImgFormat
//-------------------------------------------------------------------------------------
//! Virtual class for image data loading.
//---------------------------------------------------------------------------------------
class O3D_API ImgFormat : NonAssignable<>, public SmartCounter<ImgFormat>
{
public:

	//! Cube-map sides
	enum CubeMapSide
	{
		LEFT_SIDE = 0,    //!< X+
		RIGHT_SIDE = 1,   //!< X-
		UP_SIDE = 2,      //!< Y+
		DOWN_SIDE = 3,    //!< Y-
		FRONT_SIDE = 4,   //!< Z+
		BACK_SIDE = 5     //!< Z-
	};

	//! Default constructor.
	ImgFormat();

	//! Copy constructor.
	ImgFormat(const ImgFormat &dup);

	//! Virtual destructor. Make a call to Destroy().
	virtual ~ImgFormat();

	//! Clone
	virtual ImgFormat* clone() const = 0;

	//! Get picture informations
	void getInfo(
		UInt32 &width,
		UInt32 &height,
		UInt8 *&data,
		UInt32 &size);

	//! Destroy all
	virtual void destroy();

	//! check the first bytes of the file for see his format
    virtual Bool checkFormat(InStream &is) = 0;

	//! is the picture contain many mipmap, or it is a multi-layer (3d texture), 
	//! or it is a cube-map.
	virtual Bool isComplex() const;

	//! Is a compressed picture (ie. DXT).
	virtual Bool isCompressed() const;

	//! Get the pixel data format.
	virtual PixelFormat getPixelFormat() const = 0;

	//! Get the data ptr.
	virtual const UInt8* getData() const { return m_data; }

	//! Get the data ptr.
	virtual UInt8* getData() { return m_data; }

	//! Detach its data array. The internal data member is then NULL.
	void detach();


	//-----------------------------------------------------------------------------------
	// IO
	//-----------------------------------------------------------------------------------

    //! Load method.
    //! @param format Load with the best default pixel format.
    virtual Bool load(InStream &is) = 0;

    //! Load method.
    //! @param format Load to a specific pixel format (convert if necessary).
    virtual Bool load(InStream &is, PixelFormat pixelFormat) = 0;

    //! Save method.
    virtual Bool save(OutStream &os, PixelFormat pixelFormat) = 0;


	//-----------------------------------------------------------------------------------
	// Complex pictures
	//-----------------------------------------------------------------------------------

	//! Is there mipmaps.
	virtual Bool isMipMap() const;

	//! Is it a cube-map.
	virtual Bool isCubeMap() const;

	//! Is it a volume texture (3d).
	virtual Bool isVolumeTexture() const;

	//! Get the number of mipmap (don't count the primary level).
	//! so the total number of level is GetNumMipMapLvl() + 1
	virtual UInt32 getNumMipMapLvl() const;

	//! Get the depth size of a volume texture.
	virtual UInt32 getNumDepthLayer() const;

	//! Select a mipmap lvl. 0 mean primary level.
	virtual Bool bindMipMapLvl(UInt32 lvl);

	//! Select a cubemap side.
	virtual void bindCubeMapSide(CubeMapSide side);

	//! Select a depth for a volume texture.
	virtual Bool bindVolumeLayer(UInt32 layer);

	//! Get current bound mipmap lvl.
	virtual UInt32 getCurrentMipMapLvl() const;

	//! Get current bound cubemap side.
	virtual UInt32 getCurrentCubeMapSide() const;

	//! Get current bound depth volume layer.
	virtual UInt32 getCurrentDepthLayer() const;

protected:

	Bool m_state;         //!< true for a valid picture
	UInt32 m_width;       //!< width
	UInt32 m_height;      //!< height
	UInt8 *m_data;	      //!< picture data
	UInt32 m_size;		  //!< picture data size
};

} // namespace o3d

#endif // _O3D_IMGFORMAT_H

