/**
 * @file texturemanager.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/glextdefines.h"
#include "o3d/engine/texture/texturemanager.h"

#include "o3d/core/filemanager.h"
#include "o3d/core/mutex.h"
#include "o3d/core/taskmanager.h"
#include "o3d/core/virtualfilelisting.h"
#include "o3d/core/stringtokenizer.h"
#include "o3d/engine/texture/texture2d.h"
#include "o3d/engine/texture/cubemaptexture.h"
#include "o3d/engine/texture/normalcubemaptex.h"
#include "o3d/engine/texture/attenuationtex3d.h"
#include "o3d/engine/glextensionmanager.h"
#include "o3d/engine/scene/scene.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(TextureManager, ENGINE_TEXTURE_LIST, SceneResourceManager)

// Default Constructor.
TextureManager::TextureManager(BaseObject *parent, const String &path) :
    SceneResourceManager(parent, path, "*.bmp|*.dds|*.gif|*.jpg|*.png|*.tga|*.o3dtex"),
	m_isAsynchronous(False),
    m_defaultTexture2D(nullptr),
    m_defaultTextureCubeMap(nullptr)
{
	m_garbageManager.setLifeTime(10000);

	Int32 i, j, c;

	// 2d default texture (a chess-board)
	Image image2d;
	image2d.allocate(64,64,4);

	UInt8 *data = image2d.getDataWrite();

	for (i = 0; i < 64; ++i)
	{
		for (j = 0; j < 64; ++j)
		{
			c = (!(i & 8) ^ !(j & 8)) * 255;

			data[(i*256) + (j*4) + 0] = (UInt8)c;
			data[(i*256) + (j*4) + 1] = (UInt8)c;
			data[(i*256) + (j*4) + 2] = (UInt8)c;
			data[(i*256) + (j*4) + 3] = (UInt8)255;
		}
	}

    m_defaultTexture2D = new Texture2D(this, image2d);
	m_defaultTexture2D->setFiltering(Texture::BILINEAR_FILTERING);
	m_defaultTexture2D->setWrap(Texture::REPEAT);
	m_defaultTexture2D->create(True, False);
    m_defaultTexture2D->setName("DefaultTexture2D");
	m_defaultTexture2D->setResourceName("<o3d::texture::2d::default>");
	m_defaultTexture2D->setPersistant(True);

    addTexture(m_defaultTexture2D);

	// cubemap default texture
	m_defaultTextureCubeMap = new CubeMapTexture(
		this, image2d, image2d, image2d, image2d, image2d, image2d);
	m_defaultTextureCubeMap->create(False, False);
    m_defaultTextureCubeMap->setName("DefaultCubeMapTexture");
	m_defaultTextureCubeMap->setResourceName("<o3d::texture::cubemap::default>");
	m_defaultTextureCubeMap->setPersistant(True);

    addTexture(m_defaultTextureCubeMap);
}

// Destructor
TextureManager::~TextureManager()
{
    removeTexture(m_defaultTexture2D);
    removeTexture(m_defaultTextureCubeMap);

	deletePtr(m_defaultTexture2D);
	deletePtr(m_defaultTextureCubeMap);

	if (!m_findMap.empty())
	{
		String message("Textures still exists into the manager:\n");

		for (IT_FindMap it = m_findMap.begin(); it != m_findMap.end(); ++it)
		{
			for (std::list<Texture*>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
			{
                message += "    |- " + (*it2)->getResourceName() + "\n";
				deletePtr(*it2);
			}
		}

		O3D_WARNING(message);
	}
}

// Delete child .
Bool TextureManager::deleteChild(BaseObject *child)
{
	if (child)
	{
		if (child->getParent() != this)
			O3D_ERROR(E_InvalidParameter("The parent child differ from this"));
		else
		{
			// is it a texture, and is it managed by this
			Texture *texture = o3d::dynamicCast<Texture*>(child);
			if (texture && (texture->getManager() == this))
			{
				deleteTexture(texture);
				return True;
			}

			// otherwise simply delete the child
			deletePtr(child);
			return True;
		}
	}
	return False;
}

Texture* TextureManager::findTexture(
        const String &resourceName,
        UInt32 type,
        Bool mipMaps)
{
   FastMutexLocker locker(m_mutex);

    // search the name into the map, next the texture given parameters into the element
    CIT_FindMap cit = m_findMap.find(resourceName);
    if (cit != m_findMap.end()) {
		// search into the list
        for (std::list<Texture*>::const_iterator it2 = cit->second.begin(); it2 != cit->second.end(); ++it2) {
			Texture *texture = *it2;

            if ((texture->getType() == type) && (texture->isMipMaps() == mipMaps)) {
				// found it ?
                O3D_LOG(Logger::INFO, "Reuse texture " + texture->getResourceName());
				return texture;
			}
		}
	}

	// maybe the asked texture was just deleted, so search it into the garbage collector
    Texture *texture = nullptr;
	m_garbageManager.remove(
            GarbageKey(resourceName, type, mipMaps),
			texture);

    locker.unlock();

	// if found, reinsert it into the manager
    if (texture) {
		addTexture(texture);
        O3D_LOG(Logger::INFO, "Ungarbage texture " + texture->getResourceName());
    }

	return texture;
}

// Manage an existing texture.
void TextureManager::addTexture(Texture *texture)
{
    if (texture->getManager() != nullptr) {
		O3D_ERROR(E_InvalidOperation("The given texture already have a manager"));
    }

    FastMutexLocker locker(m_mutex);

    // search for the texture name
    IT_FindMap it = m_findMap.find(texture->getResourceName());
    if (it != m_findMap.end()) {
		it->second.push_back(texture);
    } else {
		std::list<Texture*> entry;
		entry.push_back(texture);

        m_findMap.insert(std::make_pair(texture->getResourceName(), entry));
	}

    O3D_MESSAGE("Add texture \"" + texture->getResourceName() + "\"");

	// this is the manager and its parent
	texture->setManager(this);
	texture->setParent(this);
	texture->setId(m_IDManager.getID());
}

// Remove an existing texture from the manager.
void TextureManager::removeTexture(Texture *texture)
{
	if (texture->getManager() != this)
		O3D_ERROR(E_InvalidParameter("Texture manager is not this"));

    FastMutexLocker locker(m_mutex);

	// remove the texture object from the manager.
    IT_FindMap it = m_findMap.find(texture->getResourceName());
	if (it != m_findMap.end())
	{
		std::list<Texture*>::iterator it2 =  it->second.begin();

		// search into the list
		for (; it2 != it->second.end(); ++it2)
		{
			// found it ?
			if (*it2 == texture)
			{
				break;
			}
		}

		// if texture found erase it from the list
		if (it2 != it->second.end())
		{
			it->second.erase(it2);

            texture->setManager(nullptr);
			texture->setParent(getScene());

			m_IDManager.releaseID(texture->getId());
			texture->setId(-1);

            O3D_MESSAGE("Remove (not delete) existing texture: " + texture->getResourceName());
		}

		// erase the list if empty
		if (it->second.empty())
			m_findMap.erase(it);
	}
}

// Delete a managed texture.
void TextureManager::deleteTexture(Texture *texture)
{
	if (texture->getManager() != this)
		O3D_ERROR(E_InvalidParameter("Texture manager is not this"));

    FastMutexLocker locker(m_mutex);

    // remove the textures objects from the manager.
    IT_FindMap it = m_findMap.find(texture->getResourceName());
	if (it != m_findMap.end())
	{
		std::list<Texture*>::iterator it2 =  it->second.begin();

		// search into the list
		for (; it2 != it->second.end(); ++it2)
		{
			// found it ?
			if (*it2 == texture)
				break;
		}

		// if texture found erase it from the list
		// and adding it for a deferred deletion
		if (it2 != it->second.end())
		{
			it->second.erase(it2);
			m_garbageManager.add(GarbageKey(texture), texture);

            texture->setManager(nullptr);

			m_IDManager.releaseID(texture->getId());
			texture->setId(-1);

            O3D_MESSAGE("Delete (to GC) texture: " + texture->getResourceName());
		}

		// erase the list if empty
		if (it->second.empty())
			m_findMap.erase(it);
	}
}

// Purge immediately the garbage manager of its content.
void TextureManager::purgeGarbage()
{
    FastMutexLocker locker(m_mutex);
	m_garbageManager.destroy();
}

// add a new 2d texture/picture by a file and load it in GPU memory
Texture2D* TextureManager::addTexture2D(
	const String &filename,
	Bool mipMaps,
	const TextureOps &ops)
{
	// search for a similar texture
	Texture *texture = findTexture(
            filename,
			ENGINE_TEXTURE_2D,
			mipMaps);

	if (texture)
		return static_cast<Texture2D*>(texture);

	// new texture
    Texture2D *texture2D = new Texture2D(this, ops);
	texture2D->setResourceName(filename);

	// asynchronous loading
	if (m_isAsynchronous)
	{
		Texture2DTask *task = new Texture2DTask(
				texture2D,
                getFullFileName(filename),
				mipMaps);

		TaskManager::instance()->addTask(task);
	}
	// synchronous loading
    else
    {
        Image image(getFullFileName(filename));
        texture2D->setImage(image);

        if (!texture2D->create(mipMaps))
        {
            deletePtr(texture2D);
            return nullptr;
        }
	}

	addTexture(texture2D);
	return texture2D;
}

//! Is a 2d texture exists.
Bool TextureManager::isTexture2D(
		const String &filename,
		Bool mipMaps,
		const TextureOps &ops)
{
    String resourceName = filename;

	// search for a similar texture
	return findTexture(
            resourceName,
			ENGINE_TEXTURE_2D,
			mipMaps) != NULL;
}

// add a new normal cubemap texture and compute it, and then return it's key
NormalCubeMapTex* TextureManager::addNormalCubeMapTex(
	UInt32 size,
	Float offset)
{
    String resourceName = NormalCubeMapTex::makeResourceName(size, offset);

	// search for a similar texture
	Texture *texture = findTexture(
            resourceName,
			ENGINE_TEXTURE_NORMCUBEMAP,
			False);

	if (texture)
		return static_cast<NormalCubeMapTex*>(texture);

	// new texture
	NormalCubeMapTex *normalCubeMapTexture = new NormalCubeMapTex(
			this,
			size,
			offset);

    normalCubeMapTexture->setResourceName(resourceName);

	if (!normalCubeMapTexture->create())
	{
		deletePtr(normalCubeMapTexture);
        return nullptr;
	}

	addTexture(normalCubeMapTexture);
	return normalCubeMapTexture;
}

//! Is a normal cube-map texture exists.
Bool TextureManager::isNormalCubeMapTex(
	UInt32 size,
	Float offset)
{
    String resourceName = NormalCubeMapTex::makeResourceName(size, offset);

	return findTexture(
            resourceName,
			ENGINE_TEXTURE_NORMCUBEMAP,
			False) != NULL;
}

// add a new normal cubemap texture and compute it, and then return it's key
CubeMapTexture* TextureManager::addCubeMapTexture(
	const String &xp, const String &xn,
	const String &yp, const String &yn,
	const String &zp, const String &zn,
	Bool mipMaps,
	const TextureOps &ops)
{
    String resourceName = CubeMapTexture::makeResourceName(xp, xn, yp, yn, zp, zn);

	// search for a similar texture
	Texture *texture = findTexture(
            resourceName,
			ENGINE_TEXTURE_CUBEMAP,
			mipMaps);

	if (texture)
		return static_cast<CubeMapTexture*>(texture);

	// new texture
    CubeMapTexture *cubeMapTexture = new CubeMapTexture(this, ops);
	cubeMapTexture->setResourceName(resourceName);

    String filenames[6];
    filenames[0] = getFullFileName(xp);
    if (xn.isValid())
    {
        filenames[1] = getFullFileName(xn);
        filenames[2] = getFullFileName(yp);
        filenames[3] = getFullFileName(yn);
        filenames[4] = getFullFileName(zp);
        filenames[5] = getFullFileName(zn);
    }

	// asynchronous loading
	if (m_isAsynchronous)
	{
		CubeMapTextureTask *task = new CubeMapTextureTask(
				cubeMapTexture,
                filenames[0],
                filenames[1],
                filenames[2],
                filenames[3],
                filenames[4],
                filenames[5],
				mipMaps);

		TaskManager::instance()->addTask(task);
	}
	// synchronous loading
    else
    {
        if (filenames[CubeMapTexture::RIGHT_SIDE].isValid())
        {
            for (Int32 i = 0; i < 6; ++i)
            {
                Image image(filenames[i]);
                cubeMapTexture->setImage(CubeMapTexture::CubeSide(i), image);
            }
        }
        else
        {
            Image image(filenames[0]);
            cubeMapTexture->setImage(CubeMapTexture::SINGLE, image);
        }

        if (!cubeMapTexture->create(mipMaps))
        {
            deletePtr(cubeMapTexture);
            return nullptr;
        }
	}

	addTexture(cubeMapTexture);
	return cubeMapTexture;
}

CubeMapTexture* TextureManager::addCubeMapTexture(
	const String &filename,
	Bool mipMaps,
	const TextureOps &ops)
{
	// search for a similar texture
	Texture *texture = findTexture(
            filename,
			ENGINE_TEXTURE_CUBEMAP,
			mipMaps);

	if (texture)
		return static_cast<CubeMapTexture*>(texture);

	// new texture
    CubeMapTexture *cubeMapTexture = new CubeMapTexture(this, ops);
	cubeMapTexture->setResourceName(filename);

	// asynchronous loading
	if (m_isAsynchronous)
	{
		CubeMapTextureTask *task = new CubeMapTextureTask(
				cubeMapTexture,
                getFullFileName(filename),
				mipMaps);

		TaskManager::instance()->addTask(task);
	}
	// synchronous loading
    else
    {
        Image image(getFullFileName(filename));
        cubeMapTexture->setImage(CubeMapTexture::CubeSide(0), image);

        if (!cubeMapTexture->create(mipMaps))
        {
            deletePtr(cubeMapTexture);
            return nullptr;
        }
	}

	addTexture(cubeMapTexture);
	return cubeMapTexture;
}

// is a cube map texture exists.
Bool TextureManager::isCubeMapTexture(
	const String &xp, const String &xn,
	const String &yp, const String &yn,
	const String &zp, const String &zn,
	Bool mipMaps,
	const TextureOps &ops)
{
	// search for a similar texture
    String resourceName = CubeMapTexture::makeResourceName(xp, xn, yp, yn, zp, zn);

	return findTexture(
            resourceName,
			ENGINE_TEXTURE_CUBEMAP,
            mipMaps) != nullptr;
}

Bool TextureManager::isCubeMapTexture(
	const String &filename,
	Bool mipMaps,
	const TextureOps &ops)
{
	String absFileName = getFullFileName(filename);

	return findTexture(
			absFileName,
			ENGINE_TEXTURE_CUBEMAP,
            mipMaps) != nullptr;
}

// add a new normal cubemap texture and compute it, and then return it's key
AttenuationTex3D* TextureManager::addAttenuationTex3D(UInt32 size)
{
    String resourceName = AttenuationTex3D::makeResourceName(size);

	// search for a similar texture
	Texture *texture = findTexture(
            resourceName,
			ENGINE_TEXTURE_ATTENUATION_3D,
			False);

	if (texture)
		return static_cast<AttenuationTex3D*>(texture);

	// new texture
	AttenuationTex3D *att3dTexture = new AttenuationTex3D(this, size);

	if (!att3dTexture->create())
	{
		deletePtr(att3dTexture);
        return nullptr;
	}

	addTexture(att3dTexture);
	return att3dTexture;
}

//! Is a 3d attenuation texture exists.
Bool TextureManager::isAttenuationTex3D(UInt32 size)
{
    String resourceName = AttenuationTex3D::makeResourceName(size);

	return findTexture(
            resourceName,
			ENGINE_TEXTURE_ATTENUATION_3D,
			False);
}

// ReadTexture2D
Texture2D* TextureManager::readTexture2D(InStream &is)
{
	Texture2D* texture2D = new Texture2D(this);

	// read texture info from file
    if (!texture2D->readFromFile(is))
	{
		deletePtr(texture2D);
        return nullptr;
	}

	// default texture 2d
	if (texture2D->getResourceName() == "<o3d::texture::2d::default>")
	{
		deletePtr(texture2D);
		return m_defaultTexture2D;
	}

	// search for a similar texture
	Texture *foundTexture = findTexture(
            texture2D->getResourceName(),
			ENGINE_TEXTURE_2D,
			texture2D->isMipMaps());

	// already exists ?
	if (foundTexture)
	{
		deletePtr(texture2D);
		return static_cast<Texture2D*>(foundTexture);
	}

	// asynchronous loading
	if (m_isAsynchronous)
	{
		Texture2DTask *task = new Texture2DTask(
				texture2D,
                getFullFileName(texture2D->getResourceName()),
				texture2D->isMipMaps());

		TaskManager::instance()->addTask(task);
	}
	// synchronous loading
    else
    {
        Image image(getFullFileName(texture2D->getResourceName()));
        texture2D->setImage(image);

        if (!texture2D->create(texture2D->isMipMaps()))
        {
            deletePtr(texture2D);
            return nullptr;
        }
	}

	addTexture(texture2D);
	return texture2D;
}

NormalCubeMapTex* TextureManager::readTextureNormCubeMap(InStream &is)
{
	NormalCubeMapTex* normalCubeMapTexture = new NormalCubeMapTex(this, 0, 0.f);

	// read texture info from file
    if (!normalCubeMapTexture->readFromFile(is))
	{
		deletePtr(normalCubeMapTexture);
        return nullptr;
	}

	// search for a similar texture
	Texture *foundTexture = findTexture(
            normalCubeMapTexture->getResourceName(),
			ENGINE_TEXTURE_NORMCUBEMAP,
			False);

	// already exists ?
	if (foundTexture)
	{
		deletePtr(normalCubeMapTexture);
		return static_cast<NormalCubeMapTex*>(foundTexture);
	}

	if (!normalCubeMapTexture->create())
	{
		deletePtr(normalCubeMapTexture);
		return NULL;
	}

	addTexture(normalCubeMapTexture);
	return normalCubeMapTexture;
}

CubeMapTexture* TextureManager::readTextureCubeMap(InStream &is)
{
  	CubeMapTexture* cubeMapTexture = new CubeMapTexture(this);

	// read texture info from file
    if (!cubeMapTexture->readFromFile(is))
	{
		deletePtr(cubeMapTexture);
        return nullptr;
	}

	// default texture cube-map
	if (cubeMapTexture->getResourceName() == "<o3d::texture::cubemap::default>")
	{
		deletePtr(cubeMapTexture);
		return m_defaultTextureCubeMap;
	}

	// search for a similar texture
	Texture *foundTexture = findTexture(
            cubeMapTexture->getResourceName(),
			ENGINE_TEXTURE_CUBEMAP,
			cubeMapTexture->isMipMaps());

	// already exists ?
	if (foundTexture)
	{
		deletePtr(cubeMapTexture);
		return static_cast<CubeMapTexture*>(foundTexture);
	}

    String filenames[6];

    // get the filenames from the resource name
    if (cubeMapTexture->getResourceName().startsWith("<cubemap("))
    {
        String tmp = cubeMapTexture->getResourceName();

        tmp.remove(0, 9);
        tmp.trimRight(")>");

        StringTokenizer tokenizer(tmp, "|");
        Int32 i = 0;

        while (tokenizer.hasMoreElements())
        {
            filenames[CubeMapTexture::CubeSide(i)] = getFullFileName(tokenizer.nextElement());
        }
    }
    else
    {
        filenames[0] = getFullFileName(cubeMapTexture->getResourceName());
    }

	// asynchronous loading
	if (m_isAsynchronous)
	{
        if (filenames[CubeMapTexture::RIGHT_SIDE].isValid())
		{
			CubeMapTextureTask *task = new CubeMapTextureTask(
					cubeMapTexture,
                    filenames[CubeMapTexture::LEFT_SIDE],
                    filenames[CubeMapTexture::RIGHT_SIDE],
                    filenames[CubeMapTexture::UP_SIDE],
                    filenames[CubeMapTexture::DOWN_SIDE],
                    filenames[CubeMapTexture::FRONT_SIDE],
                    filenames[CubeMapTexture::BACK_SIDE],
					cubeMapTexture->isMipMaps());

			TaskManager::instance()->addTask(task);
		}
		else
		{
			CubeMapTextureTask *task = new CubeMapTextureTask(
					cubeMapTexture,
                    filenames[CubeMapTexture::SINGLE],
					cubeMapTexture->isMipMaps());

			TaskManager::instance()->addTask(task);
		}
	}
	// synchronous loading
    else
    {
        if (filenames[CubeMapTexture::RIGHT_SIDE].isValid())
        {
            for (Int32 i = 0; i < 6; ++i)
            {
                Image image(filenames[i]);
                cubeMapTexture->setImage(CubeMapTexture::CubeSide(i), image);
            }
        }
        else
        {
            Image image(filenames[0]);
            cubeMapTexture->setImage(CubeMapTexture::SINGLE, image);
        }

        if (!cubeMapTexture->create(cubeMapTexture->isMipMaps()))
        {
            deletePtr(cubeMapTexture);
            return nullptr;
        }
    }

	addTexture(cubeMapTexture);
	return cubeMapTexture;
}

AttenuationTex3D* TextureManager::readTextureAtt3D(InStream &is)
{
  	AttenuationTex3D *texture = new AttenuationTex3D(this, 0);

	// read texture info from file
    if (!texture->readFromFile(is))
	{
		deletePtr(texture);
        return nullptr;
	}

	// search for a similar texture
	Texture *foundTexture = findTexture(
            texture->getResourceName(),
			ENGINE_TEXTURE_ATTENUATION_3D,
			False);

	// already exists ?
	if (foundTexture)
	{
		deletePtr(texture);
		return static_cast<AttenuationTex3D*>(foundTexture);
	}

	if (!texture->create())
	{
		deletePtr(texture);
        return nullptr;
	}

	addTexture(texture);
	return texture;
}

// Enable an asynchronous texture query manager
void TextureManager::enableAsynchronous()
{
    FastMutexLocker locker(m_mutex);

	if (!m_isAsynchronous)
		m_isAsynchronous = True;
}

// Disable an asynchronous texture query manager
void TextureManager::disableAsynchronous()
{
    FastMutexLocker locker(m_mutex);

	if (m_isAsynchronous)
		m_isAsynchronous = False;
}

// Is asynchronous texture loading is enabled.
Bool TextureManager::isAsynchronous() const
{
    m_mutex.lock();
	Bool result = m_isAsynchronous;
    m_mutex.unlock();

	return result;
}

// Update the manager
void TextureManager::update()
{
    FastMutexLocker locker(m_mutex);

	// process garbage checking
	m_garbageManager.update();
}

