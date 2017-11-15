/**
 * @file texture2dcache.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_TEXTURE2DCACHE_H
#define _O3D_TEXTURE2DCACHE_H

#include "../scene/sceneentity.h"
#include "o3d/core/stringlist.h"
#include "o3d/core/stringmap.h"
#include "o3d/core/vector2.h"

#include "texture2d.h"
#include "texturearray2d.h"

namespace o3d {

/**
 * @brief Texture 2D cache is an asset of immutable 2d object contained into one or many
 * 2d textures.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-08-26
 */
class O3D_API Texture2DCache : public SceneEntity
{
public:

	O3D_DECLARE_DYNAMIC_CLASS(Texture2DCache)

	enum Format
	{
		FORMAT_TEXTURE_2D,
		FORMAT_TEXTURE_2D_ARRAY
	};

	/**
	 * @brief Texture2DCache
	 * @param format The format of texture to use.
	 */
	Texture2DCache(BaseObject *parent, Format format = FORMAT_TEXTURE_2D);

	//! Texture format.
	Format getFormat() const { return m_format; }

	//! Add a folder of image used for the generation of the cache.
	void addFolder(const String &path);

	//! Defines the size hint for the generated textures.
	void setSizeHint(const Vector2i &size);

	/**
	 * @brief generate Generate the texture cache.
	 * @param filtering If true, double the first and last lines and rows of each elements,
	 * in the case of elements that are not on the border of the texture, to have a
	 * correct filtering when rendering. If false no filtering can be used.
	 */
	void generate(Bool filtering);

	/**
	 * @brief bind Bind an element of the texture cache.
	 * Bind the texture if necessary according to the element id.
	 * @param id Element identifier.
	 */
	void bind(UInt32 id);

	/**
	 * @brief getElementId Get an element id according to its filename without its extension.
	 * @param name
	 * @return
	 */
	UInt32 getElementId(const String &name) const;



private:

	Format m_format;
	T_StringList m_folders;

	StringMap<UInt32> m_dictionnary;

	struct Element
	{
		Float su, sv, eu, ev;
		Float depth;            //!< only when using array of texture
		Texture *texture;
	};

	typedef std::vector<Element> T_ElementVector;
	typedef T_ElementVector::iterator IT_ElementVector;
	typedef T_ElementVector::const_iterator CIT_ElementVector;

	//! Contains the elements with textures coordinates and related texture.
	T_ElementVector m_elements;
};

} // namespace o3d

#endif // _O3D_TEXTURE2DCACHE_H

