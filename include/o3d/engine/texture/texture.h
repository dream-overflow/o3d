/**
 * @file texture.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_TEXTURE_H
#define _O3D_TEXTURE_H

#include "o3d/core/matrix4.h"
#include "o3d/core/smartpointer.h"
#include "o3d/core/garbagemanager.h"
#include "o3d/core/idmanager.h"
#include "o3d/core/memorydbg.h"

#include "o3d/image/image.h"

#include "gltexture.h"
#include "../enginetype.h"

#include "../scene/sceneentity.h"

namespace o3d {

/**
 * @brief Apply operation to a texture before create it.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2002-05-10
 */
class O3D_API TextureOps
{
public:

	//! Return the operation string list.
	String getOpString() const { return String(); }

    inline Bool operator< (const TextureOps&) const
	{
		return True;
	}

    inline Bool operator== (const TextureOps&) const
	{
		return True;
	}

	enum OpType
	{
		UNDEFINED_OP,
		PIXEL_TRANSFERT_OP
	};

	static const Int32 NUM_OP_TYPES = PIXEL_TRANSFERT_OP + 1;

	struct Operation
	{
	public:
		Operation() : type(UNDEFINED_OP) {}
	protected:
		UInt32 type;   //<! Operation type
	};

	class PixelTransfert : public Operation
	{
	public:

		PixelTransfert(const Color &scale, const Color &bias) :
			scaleColor(scale),
			biasColor(bias)
		{
			type = PIXEL_TRANSFERT_OP;
		}

        inline Bool operator< (const Operation&) const
		{
			//return ((scaleColor < comp.scaleColor) && (biasColor < comp.biasColor));
			return True;
		}

        inline Bool operator== (const Operation&) const
		{
			return True;
		}

		Color scaleColor;
		Color biasColor;
	};

private:

	typedef std::list<Operation*> T_Operations;
	T_Operations m_operations;
};

/**
 * @brief Texture base object.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2002-05-10
 */
class O3D_API Texture : public SceneResource, NonCopyable<>
{
public:

	O3D_DECLARE_ABSTRACT_CLASS(Texture)

    //! Filtering mode.
	enum FilteringMode
	{
		NO_FILTERING = 0,       //!< Nearest, no filtering.
		LINEAR_FILTERING,       //!< Linear filtering.
		BILINEAR_FILTERING,     //!< Bilinear + mipmap + isotropic
		BILINEAR_ANISOTROPIC,   //!< Bilinear + mipmap + anisotropic
		TRILINEAR_FILTERING,    //!< Trilinear + mipmap + isotropic
		TRILINEAR_ANISOTROPIC   //!< Trilinear + mipmap + anisotropic
	};

	//! Warp mode.
    enum WrapMode
	{
		REPEAT = 0, //!< texture repeat
		CLAMP       //!< texture clamp to edge
	};

	//! Constructor.
	Texture(BaseObject *parent, const TextureOps &operations = TextureOps());

	//! Virtual destructor.
	virtual ~Texture();

	//-----------------------------------------------------------------------------------
	// Texture generation
	//-----------------------------------------------------------------------------------

	//! Generate/regenerate the texture identifier according to the new parameters.
	//! The texture is regenerated (delete and gen) if the change is major, like change
	//! of maxLevel to a lesser maxLevel.
	//! @param format Pixel format.
	//! @param width Width in pixels.
	//! @param height Height in pixels.
	//! @param depth Depth in pixels.
	//! @param minLevel Minimal mipmap level.
	//! @param maxLevel Maximal mipmap level.
	//! @return TRUE if the texture is (re)generated, FALSE mean no change of
	//!         the texture id.
	Bool generateId(
			PixelFormat format,
			UInt32 width,
			UInt32 height,
			UInt32 depth,
			UInt32 minLevel,
			UInt32 maxLevel);

	//! Generate/regenerate the texture identifier according to the new parameters.
	//! Same as previous method, by use parameters of O3DPicture (width, height...).
	//! The texture is regenerated (delete and gen) if the change is major, like change
	//! of maxLevel to a lesser maxLevel.
	//! @param image A valid Image where to read parameters from.
	//! @param mipmaps Count any mipmaps levels.
	//! @return TRUE if the texture is (re)generated, FALSE mean no change of
	//!         the texture id.
	Bool generateId(const Image &image, Bool mipmaps);

	//! Destroy the texture.
	virtual void destroy();

	//! Delete the OpenGL texture object.
	void unload();

	//! Is the texture is valid to be bound.
	inline Bool isValid() const { return m_textureId > 0; }

	//! Add a range of texture mipmaps if the texture is already valid.
	//! @param minLevel Minimal level to generate.
	//! @param maxLevel Maximal level to generate (>0). If maxLevel is greater than
	//! log2(max(width,height)) it will stop to this limit without warning.
    virtual Bool addMipsLevels(UInt32 minLevel, UInt32 maxLevel);

	//-----------------------------------------------------------------------------------
	// General settings
	//-----------------------------------------------------------------------------------

	//! Get the filtering mode.
	inline FilteringMode getFiltering() const { return m_filtering; }
	//! Set the filtering mode.
	inline void setFiltering(FilteringMode filtering)
	{
        if (filtering != m_filtering) {
			m_filtering = filtering;
			m_updateFlags |= UPDATE_FILTERING;
		}
	}

	//! Set the warp mode.
    inline WrapMode getWrap() const { return m_wrap; }
	//! Get the warp mode.
    inline void setWrap(WrapMode warp)
	{
        if (warp != m_wrap) {
            m_wrap = warp;
            m_updateFlags |= UPDATE_WRAPMODE;
		}
	}

	//! Get the anisotropic factor.
	inline Float getAnisotropy() const { return m_anisotropy; }
	//! Set the anisotropic factor.
	inline void setAnisotropy(Float value)
	{
        if (value != m_anisotropy) {
			m_anisotropy = value;
			m_updateFlags |= UPDATE_ANISOTROPY;
		}
	}

	//! Get texture operations.
	const TextureOps& getOperations() const { return m_operations; }

	//! Get the OpenGL texture id (1 <= id <= 2^31).
	inline UInt32 getTextureId() const { return m_textureId; }

	//! Get the OpenGL texture type (TEXTURE_2D...).
	inline TextureType getTextureType() const { return m_textureType; }

	//! Get the minimal mipmap level.
	inline UInt32 getMinLevel() const { return m_minLevel; }

	//! Get the maximal mipmap level.
	inline UInt32 getMaxLevel() const { return m_maxLevel; }

	//! Get the number of mipmaps levels.
	inline UInt32 getNumMipMaps() const { return m_maxLevel - m_minLevel + 1; }

	//! Check if mipmaps level are generated for this texture.
	inline Bool isMipMaps() const
	{
        if ((m_width > 1) || (m_height > 1)) {
			return (m_maxLevel > m_minLevel);
        } else {
			return True;
        }
	}

	//! get the texture width
	inline UInt32 getWidth() const { return m_width; }

	//! get the texture height
	inline UInt32 getHeight() const { return m_height; }

	//! get the texture depth size (for 3d texture)
	inline UInt32 getDepth() const { return m_depth; }

	//! get the texture pixel format.
	inline PixelFormat getPixelFormat() const { return m_pixelFormat; }

	//! Get the OpenGL internal format.
	TextureIntFormat getGLInternalFormat() const;

	//-----------------------------------------------------------------------------------
	// Textures process
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
	// Serialization
	//-----------------------------------------------------------------------------------

    virtual Bool writeToFile(OutStream &os) override;
    virtual Bool readFromFile(InStream &is) override;

protected:

	enum UpdateFlags
	{
		UPDATE_NOTHING = 0,
		UPDATE_ANISOTROPY = 1,
		UPDATE_FILTERING = 2,
        UPDATE_WRAPMODE = 4,
        UPDATE_ALL = UPDATE_ANISOTROPY | UPDATE_FILTERING | UPDATE_WRAPMODE
	};

    UInt32 m_textureId;        //!< OpenGL texture id.
	TextureType m_textureType; //!< OpenGL texture type.

	UInt32 m_width;        //!< Width of the primary texture level.
	UInt32 m_height;       //!< Height of the primary texture level. 1 for 1d texture.
	UInt32 m_depth;        //!< Depth for a multi layer (3d) texture. 1 for 1d/2d texture.

    PixelFormat m_pixelFormat; //!< Pixel format (default:PF_RGBA_8).

    UInt32 m_minLevel;     //!< Minimal mipmap level.
	UInt32 m_maxLevel;     //!< Maximal mipmap level (default log2(max(width,height)).

	FilteringMode m_filtering; //!< Filtering mode (default:linear).
    WrapMode m_wrap;           //!< Wrap mode (default:repeat).

    Float m_anisotropy;       //!< Max anisotropy (1.0 default).
    TextureOps m_operations;  //!< Operations stack applied to the texture

	UInt32 m_updateFlags;  //!< Update flags to process at next bind

	//! Set the filtering mode to OpenGL.
	virtual void setFilteringMode() {}
	//! Set the warp mode to OpenGL.
    virtual void setWrapMode() {}
	//! Set the anisotropy level to OpenGL.
	virtual void setAnisotropyLevel() {}
};

} // namespace o3d

#endif // _O3D_TEXTURE_H
