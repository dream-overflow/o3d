/**
 * @file texture2d.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_TEXTURE2D_H
#define _O3D_TEXTURE2D_H

#include "texture.h"
#include "o3d/core/task.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

/**
 * @brief Two dimensional texture object.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2014-03-16
 */
class O3D_API Texture2D : public Texture
{
public:

	O3D_DECLARE_CLASS(Texture2D)

    //! Construct a texture object.
    Texture2D(BaseObject *parent, const TextureOps &operations = TextureOps());

	//! Construct using a generated and valid picture.
	//! @param parent Owner.
    //! @param picture Valid picture. This does not defines the resourcee name which is
    //! used by materials and some others components.
	Texture2D(
		BaseObject *parent,
		const Image &picture,
		const TextureOps &operations = TextureOps());

	//! Destructor.
	virtual ~Texture2D();


	//-----------------------------------------------------------------------------------
	// Texture creation
	//-----------------------------------------------------------------------------------

	//! Create an empty texture given a size and a format in the way to be filled by an FBO.
	//! If the texture already exists it return false immediately.
	//! @param pixelFormat can be one of the PixelFormat enum.
	//! @param if true the texture is not unbound, in others words it is kept as current
	//! @param mipmaps If true it generate mipmaps levels.
	Bool create(
		Bool mipmaps,
		UInt32 width,
		UInt32 height,
		PixelFormat pixelFormat,
		Bool dontUnbind = False);

	//! Create a texture from a float buffer.
	//! If the texture already exists it return false immediately.
	//! Use Update to replace the data.
	//! @param pixelFormat Internal texture pixel format, can be one of the PixelFormat enum.
    //! @param buffer data or null. Size of the data is defined by w*h*bufferFormat.bpp.
	//! @param bufferFormat Pixel format of the buffer.
	//! @param mipmaps If true it generate mipmaps levels.
	Bool create(
		Bool mipmaps,
		UInt32 width,
		UInt32 height,
		PixelFormat pixelFormat,
		const void *buffer,
		PixelFormat bufferFormat,
		Bool dontUnbind = False);

	//! Create the texture using the previously loaded picture or the filename.
	//! If the texture already exists it return false immediately.
	//! @param mipmaps If true it generate mipmaps levels if they are not available into
	//! the picture.
	//! @param unloadPicture If true it unload the picture data after creation.
	Bool create(Bool mipmaps, Bool unloadImage = True);

	//! Update the texture data. The texture data must have the same dimensions as current.
	//! It will have the same number of mipmap levels.
	//! @param bufferFormat Pixel format of the buffer. Defines the data type and format.
	//! @return False if the texture does not exists.
	Bool update(const void *buffer, PixelFormat bufferFormat, Bool dontUnbind = False);

	//! Resize the texture (min mipmap level). The texture content is not kept.
	void resize(UInt32 width, UInt32 height, Bool dontUnbind = False);

	//! Destroy the OpenGL texture, and the picture if it exists.
	virtual void destroy();

	//! Unload the image if it is valid.
	void unloadImage();

	virtual Bool addMipsLevels(UInt32 minLevel, UInt32 maxLevel);


	//-----------------------------------------------------------------------------------
	// Processing
	//-----------------------------------------------------------------------------------

	//! Set the texture parameter, such as enable extra coord for cubemap...
	virtual void set();
	//! Bind an opengl texture
	virtual Bool bind();
	//! unbind the texture
	virtual void unbind();
	//! Unset the texture parameter
	virtual void unSet();


	//-----------------------------------------------------------------------------------
    // Image informations
	//-----------------------------------------------------------------------------------

	//! Get the associated picture (const version).
	inline const Image& getImage() const { return m_picture; }
	//! Get the associated picture (for modifications).
	inline Image& getImage() { return m_picture; }

    /**
     * @brief Change the associated picture.
     */
    void setImage(Image &image);


	//-----------------------------------------------------------------------------------
	// Serialization
	//-----------------------------------------------------------------------------------

	virtual Bool writeToFile(OutStream &os);
	virtual Bool readFromFile(InStream &is);

protected:

	Image m_picture;				//!< The picture if created from a picture.

    //! Set the filtering mode to theL texture object.
	virtual void setFilteringMode();
    //! Set the wrap mode to the texture object.
    virtual void setWrapMode();
    //! Set the anisotropy level to the texture object.
	virtual void setAnisotropyLevel();
};

/**
 * @brief Task responsible of the loading of a 2D texture.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2014-03-16
 */
class O3D_API Texture2DTask : public Task
{
public:

	//! Default constructor.
	//! @param texture Texture target.
	//! @param filename Filename of the texture to load.
	//! @param mipmaps If TRUE generate mipmaps at finalize.
	Texture2DTask(
			Texture2D *texture,
			const String &filename,
			Bool mipmaps);

	virtual Bool execute();

	virtual Bool finalize();

private:

	Texture2D *m_texture;  //!< Texture 2d to load with the picture.

	String m_filename;     //!< Absolute filename.
	Image m_picture;       //!< Picture container.
    Bool m_mipmaps;        //!< TRUE if use mipmaps at finalize.
};

} // namespace o3d

#endif // _O3D_TEXTURE2D_H
