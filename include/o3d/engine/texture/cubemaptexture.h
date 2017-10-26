/**
 * @file cubemaptexture.h
 * @brief A cube map texture in video card memory, all picture side must be of the same format
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2005-04-26
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_TEXTURECUBEMAP_H
#define _O3D_TEXTURECUBEMAP_H

#include "texture.h"
#include "o3d/core/task.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class CubeMapTexture
//-------------------------------------------------------------------------------------
//! A cube map texture in video card memory, all picture side must be of the same format
//---------------------------------------------------------------------------------------
class O3D_API CubeMapTexture : public Texture
{
public:

	O3D_DECLARE_CLASS(CubeMapTexture)

	//! Texture cube map side.
	enum CubeSide
	{
		SINGLE = 0,      //!< Single picture cube map.
		LEFT_SIDE = 0,   //!< X+
		RIGHT_SIDE,      //!< X-
		UP_SIDE,         //!< Y+
		DOWN_SIDE,       //!< Y-
		FRONT_SIDE,      //!< Z+
		BACK_SIDE        //!< Z-
	};

    //! Make a resource name from 6 files name. Format is "<cubemap(t1|t2|t3|t4|t5|t6)>"
    static String makeResourceName(
			const String &xp, const String &xn,
			const String &yp, const String &yn,
			const String &zp, const String &zn);

    //! Construct a cubemap texture object.
	//! @param parent Owner.
    CubeMapTexture(
            BaseObject *parent,
        const TextureOps &operations = TextureOps());

	//! Construct using a generated and valid cubemap picture.
	//! @param parent Owner.
    //! @param picture Valid 1 or 6 sides image.
	CubeMapTexture(
		BaseObject *parent,
		const Image &picture,
		const TextureOps &operations = TextureOps());

	//! Construct using 6 generated and valid pictures.
	//! @param parent Owner.
    //! @param filename Texture filename to use for the call to Load.
	CubeMapTexture(
		BaseObject *parent,
		const Image& xp, const Image& xn,
		const Image& yp, const Image& yn,
		const Image& zp, const Image& zn,
		const TextureOps &operations = TextureOps());

	//! Virtual destructor.
	virtual ~CubeMapTexture();


	//-----------------------------------------------------------------------------------
	// Processing
	//-----------------------------------------------------------------------------------

	//! set the texture parameter, such as enable extra coord for cubemap...
	virtual void set();
	//! bind an opengl texture
	virtual Bool bind();
	//! unbind the texture
	virtual void unbind();
	//! unset the texture parameter
	virtual void unSet();


	//-----------------------------------------------------------------------------------
	// Texture creation
	//-----------------------------------------------------------------------------------


	//! Create the texture using the previously loaded picture(s) or the filename(s).
	//! If the texture already exists it return false immediately.
	//! @param mipmaps If true it generate mipmaps levels if they are not available into
	//! the picture.
	//! @param unloadImg If true it unload the image data after creation.
	Bool create(Bool mipmaps, Bool unloadImg = True);

	//! clear all (memory and video card)
	virtual void destroy();

	//! Unload the pictures only.
	void unloadImage();


	//-----------------------------------------------------------------------------------
	// Picture information
	//-----------------------------------------------------------------------------------

	//! Get the associated image(s) (const version).
	inline const Image& getImage(CubeSide side) const { return m_pictures[side]; }
	//! Get the associated image(s).
	inline Image& getImage(CubeSide side) { return m_pictures[side]; }

    //! Set the associated image(s).
    inline void setImage(CubeSide side, Image &image) { m_pictures[side] = image; }

	//-----------------------------------------------------------------------------------
	// Serialization
	//-----------------------------------------------------------------------------------

	virtual Bool writeToFile(OutStream &os);
	virtual Bool readFromFile(InStream &is);

protected:

	Image m_pictures[6];    //!< The 6 pictures sides (or one)

	//! Set the filtering mode to OpenGL.
	virtual void setFilteringMode();
	//! Set the warp mode to OpenGL.
	virtual void setWrapMode();
	//! Set the anisotropy level to OpenGL.
	virtual void setAnisotropyLevel();

	void checkPictures(Bool six);
    Bool loadPicture();
};

//---------------------------------------------------------------------------------------
//! @class CubeMapTextureTask
//-------------------------------------------------------------------------------------
//! Task responsible of the loading of a cube-map texture.
//---------------------------------------------------------------------------------------
class O3D_API CubeMapTextureTask : public Task
{
public:

	//! Default constructor.
	//! @param texture Texture target.
	//! @param filename Filename of the texture to load.
	//! @param mipmaps If TRUE generate mipmaps at finalize.
	CubeMapTextureTask(
			CubeMapTexture *texture,
			const String &filename,
			Bool mipmaps);

	//! Default constructor.
	//! @param texture Texture target.
	//! @param xp Filename of the X+ texture to load.
	//! @param xn Filename of the X- texture to load.
	//! @param yp Filename of the Y+ texture to load.
	//! @param yn Filename of the Y- texture to load.
	//! @param zp Filename of the Z+ texture to load.
	//! @param zn Filename of the Z- texture to load.
	//! @param mipmaps If TRUE generate mipmaps at finalize.
	CubeMapTextureTask(
			CubeMapTexture *texture,
			const String &xp,
			const String &xn,
			const String &yp,
			const String &yn,
			const String &zp,
			const String &zn,
			Bool mipmaps);

	virtual Bool execute();

	virtual Bool finalize();

private:

	CubeMapTexture *m_texture;   //!< Cube-map texture to load with the picture.

	String m_filenames[6];  //!< Absolute filenames.
	Image m_pictures[6];    //!< Picture containers.
	Bool m_mipmaps;     //!< TRUE if use mipmaps at finalize.
};

} // namespace o3d

#endif // _O3D_TEXTURECUBEMAP_H

