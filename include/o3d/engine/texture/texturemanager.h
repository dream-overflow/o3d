/**
 * @file texturemanager.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_TEXTUREMANAGER_H
#define _O3D_TEXTUREMANAGER_H

#include "texture.h"

namespace o3d {

class Texture2D;
class AttenuationTex3D;
class NormalCubeMapTex;
class CubeMapTexture;
class Task;

/**
 * @brief Engine texture manager for sharable texture and garbage collection.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2010-01-04
 */
class O3D_API TextureManager : public SceneResourceManager, NonCopyable<>
{
public:

	O3D_DECLARE_DYNAMIC_CLASS(TextureManager)

	//! Default Constructor.
	TextureManager(BaseObject *parent, const String &path = "");

	//! Virtual destructor.
	virtual ~TextureManager();

    virtual Bool deleteChild(BaseObject *child) override;

	//! Insert an existing texture in the manager.
	//! @note Once the texture is inserted into the manager, it become its parent.
	void addTexture(Texture *texture);

	//! Remove an existing texture from the manager.
	//! The texture is not pushed in the garbage manager, it is simply removed of this manager.
	void removeTexture(Texture *texture);

	//! Delete an existing texture from the manager. It is stored temporarily in the
	//! garbage manager, before to be really deleted if the texture is not reused
	//! before a defined time.
	void deleteTexture(Texture *texture);

	//! Purge immediately the garbage manager of its content.
	void purgeGarbage();

	//! Get the default texture 2d (a chess-board) in bilinear filtering, and repeat mode.
	inline Texture2D* getDefaultTexture2D() const { return m_defaultTexture2D; }

	//! Get the default texture cubemap (a 6 sides chess-board).
	inline CubeMapTexture* getDefaultCubeMapTexture() const { return m_defaultTextureCubeMap; }


	//---------------------------------------------------------------------------------------
	// Factory
	//---------------------------------------------------------------------------------------

	//
	// Texture 2D
	//

	//! Create/Return a 2d texture given a file name and load it in OpenGL.
	//! @param filename Picture filename to load.
	//! @param mipMaps Load/generate mipmaps levels.
	//! @param operations A set of texture operation at texture creation.
	//! @return A new or an existing texture 2d object.
	//! @note The manager becomes the parent of the texture.
	Texture2D* addTexture2D(
		const String &filename,
		Bool mipMaps,
		const TextureOps &operations = TextureOps());

	//! Is a 2d texture exists.
	//! @param filename Picture filename to search for.
	//! @param mipMaps Load/generate mipmaps levels.
	//! @param operations A set of texture operation at texture creation.
	//! @return TRUE of a texture of this format is found.
	Bool isTexture2D(
		const String &filename,
		Bool mipMaps,
		const TextureOps &operations = TextureOps());

	//
	// Normal Cube Map Texture
	//

	//! Create/Return a normalization cube map texture given a file name and
	//! load it in OpenGL.
	//! If an identical texture exists it return it directly.
	//! Only linear filtering is supported.
	//! @param size Size of the cube-map.
	//! @param offset Offset of normals vectors.
	//! @return A new or existing texture object.
	//! @note The manager becomes the parent of the texture.
	NormalCubeMapTex* addNormalCubeMapTex(UInt32 size, Float offset);

	//! Is a normalization cube map texture exists.
	//! @param size Size of the cube-map.
	//! @param offset Offset of normals vectors.
	//! @return TRUE of a texture of this format is found.
	Bool isNormalCubeMapTex(UInt32 size, Float offset);

	//
	// Cube Map Texture
	//

	//! Create/Return a cube map texture given 6 file names and load it in OpenGL.
	//! If an identical texture exists it return it directly.
	//! @param mipMaps Load/generate mipmaps levels.
	//! @param operations A set of texture operation at texture creation.
	//! @return A new or an existing texture cube-map object.
	//! @note The manager becomes the parent of the texture.
	CubeMapTexture* addCubeMapTexture(
		const String &xp, const String &xn,
		const String &yp, const String &yn,
		const String &zp, const String &zn,
		Bool mipMaps,
		const TextureOps &operations = TextureOps());

	//! Create/Return a cube map texture given a file name and load it in OpenGL.
	//! If an identical texture exists it return it directly.
	//! @param mipMaps Load/generate mipmaps levels.
	//! @param operations A set of texture operation at texture creation.
	//! @return A new or an existing texture cube-map object.
	//! @note The manager becomes the parent of the texture.
	CubeMapTexture* addCubeMapTexture(
		const String& cubemap,
		Bool mipMaps,
		const TextureOps &operations = TextureOps());

	//! Is a map texture, given 6 file names, exists.
	//! @param mipMaps Load/generate mipmaps levels.
	//! @param operations A set of texture operation at texture creation.
	//! @return TRUE of a texture of this format is found.
	Bool isCubeMapTexture(
		const String &xp,const String &xn,
		const String &yp,const String &yn,
		const String &zp,const String &zn,
		Bool mipMaps,
		const TextureOps &operations = TextureOps());

	//! Is a cube map texture, given a file name, exists.
	//! @param cubemap A file containing 6 sides of a cube-map.
	//! @param mipMaps Load/generate mipmaps levels.
	//! @param operations A set of texture operation at texture creation.
	//! @return TRUE of a texture of this format is found.
	Bool isCubeMapTexture(
		const String &cubemap,
		Bool mipMaps,
		const TextureOps &operations = TextureOps());

	//
	// 3D Attenuation Texture
	//

	//! Create/Return a 3d attenuation texture and load it in OpenGL.
	//! If an identical texture exists it return it directly. If a similar texture is found,
	//! it create a new one and share the data (picture and OpenGL identifier).
	//! Mipmaps are not supported. Only linear filtering is supported.
	//! @param size Size of the 3d texture.
	//! @return A new or existing 3d texture object.
	//! @note The manager becomes the parent of the texture.
	AttenuationTex3D* addAttenuationTex3D(UInt32 size);

	//! Is a 3d attenuation texture exists.
	//! @param size Size of the 3d texture.
	//! @return TRUE of a texture of this format is found.
	Bool isAttenuationTex3D(UInt32 size);


	//-----------------------------------------------------------------------------------
	// File factory
	//-----------------------------------------------------------------------------------

	//! Create/Return a 2d texture according to the informations read into the file.
	//! @note The manager becomes the parent of the texture.
    Texture2D* readTexture2D(InStream &is);

	//! Create/Return a normalization cube-map texture according to the informations read
	//! into the file.
	//! @note The manager becomes the parent of the texture.
    NormalCubeMapTex* readTextureNormCubeMap(InStream &is);

	//! Create/Return a cube-map texture according to the informations read into the file.
	//! @note The manager becomes the parent of the texture.
    CubeMapTexture* readTextureCubeMap(InStream &is);

	//! Create/Return a 3d attenuation texture according to the informations read
	//! into the file.
	//! @note The manager becomes the parent of the texture.
    AttenuationTex3D* readTextureAtt3D(InStream &is);


	//-----------------------------------------------------------------------------------
	// Texture loading mode
	//-----------------------------------------------------------------------------------

	//! Enable an asynchronous texture loading.
	void enableAsynchronous();

	//! Disable an asynchronous texture loading.
	void disableAsynchronous();

	//! Is asynchronous texture loading enabled.
	Bool isAsynchronous() const;

	//! Update the manager.
	void update();

protected:

	struct GarbageKey
	{
        String resourceName;
        UInt32 type;
        Bool isMipMaps;

		GarbageKey() :
                    type(0),
                    isMipMaps(False)
		{
		}

		GarbageKey(
                const String &_resourceName,
				UInt32 type,
				Bool mipMaps) :
                    resourceName(_resourceName),
                    type(type),
                    isMipMaps(mipMaps)
		{
		}

		GarbageKey(Texture *texture)
		{
            O3D_ASSERT(texture != nullptr);

            resourceName = texture->getResourceName();
            type = texture->getType();
            isMipMaps = texture->isMipMaps();
		}

		GarbageKey(const GarbageKey &dup) :
            resourceName(dup.resourceName),
            type(dup.type),
            isMipMaps(dup.isMipMaps)
		{
		}

		inline GarbageKey& operator= (const GarbageKey &dup)
		{
            resourceName = dup.resourceName;
            type = dup.type;
            isMipMaps = dup.isMipMaps;

			return *this;
		}

		inline Bool operator==(const GarbageKey &cmp) const
		{
            return ((resourceName == cmp.resourceName) &&
                    (type == cmp.type) &&
                    (isMipMaps == cmp.isMipMaps));
		}

		inline Bool operator<(const GarbageKey &cmp) const
		{
            if (resourceName < cmp.resourceName)
				return True;
            if (type < cmp.type)
				return True;
            if (isMipMaps < cmp.isMipMaps)
				return True;
			return False;
		}
	};

	//! Manage removed texture objects.
	GarbageManager<GarbageKey, Texture*> m_garbageManager;

	Bool m_isAsynchronous;  //!< is texture use asynchronous loading

	IDManager m_IDManager;

	typedef std::map<String, std::list<Texture*> > T_FindMap;

	typedef T_FindMap::iterator IT_FindMap;
	typedef T_FindMap::const_iterator CIT_FindMap;

	T_FindMap m_findMap;

	Texture2D *m_defaultTexture2D;
	CubeMapTexture *m_defaultTextureCubeMap;

    Texture* findTexture(
            const String &resourceName,
            UInt32 type,
            Bool mipMaps);
};

} // namespace o3d

#endif // _O3D_TEXTUREMANAGER_H
