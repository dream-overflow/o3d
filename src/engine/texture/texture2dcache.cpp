/**
 * @file texture2dcache.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"

#include "o3d/engine/texture/texture2dcache.h"
#include "o3d/engine/enginetype.h"

#include "o3d/engine/context.h"

#include "o3d/engine/scene/scene.h"
#include "o3d/engine/renderer.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(Texture2DCache, ENGINE_TEXTURE_2D_CACHE, SceneEntity)

Texture2DCache::Texture2DCache(BaseObject *parent, Texture2DCache::Format format) :
	SceneEntity(parent),
	m_format(format)
{
}

void Texture2DCache::addFolder(const String &path)
{
	m_folders.push_back(path);
}

void Texture2DCache::setSizeHint(const Vector2i &size)
{
}

void Texture2DCache::generate(Bool filtering)
{
}

void Texture2DCache::bind(UInt32 id)
{
}


UInt32 Texture2DCache::getElementId(const String &name) const
{
	return 0;
}

