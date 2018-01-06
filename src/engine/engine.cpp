/**
 * @file engine.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/engine.h"

#include "o3d/core/filemanager.h"
#include "o3d/engine/renderer.h"
#include "o3d/engine/context.h"
#include "o3d/engine/primitive/primitivemanager.h"
#include "o3d/engine/shader/shadermanager.h"
#include "o3d/engine/texture/texturemanager.h"
#include "o3d/engine/animation/animationmanager.h"
#include "o3d/engine/object/meshdatamanager.h"
#include "o3d/engine/utils/framemanager.h"
#include "o3d/engine/viewportmanager.h"

using namespace o3d;

Engine::Engine(const String &rootPath, Renderer *renderer) :
    BaseObject(nullptr),  // no parent
    m_renderer(nullptr),
    m_context(nullptr),
    //m_audio(nullptr),
    //m_gui(nullptr),
    m_viewPortManager(nullptr),
    m_primitiveManager(nullptr),
    m_shaderManager(nullptr),
    m_textureManager(nullptr),
    m_animationManager(nullptr),
    m_meshDataManager(nullptr),
    m_frameManager(nullptr)
{
    m_topLevelParent = static_cast<BaseObject*>(this);
    m_name = "Engine";

    // set the renderer
    setRenderer(renderer);

    m_rootPath = FileManager::instance()->getFullFileName(rootPath);

    m_shaderManager            = new ShaderManager(this, "shaders");

    m_textureManager	       = new TextureManager(this, m_rootPath + "/textures");
    m_meshDataManager	       = new MeshDataManager(this, m_rootPath + "/models");
    m_viewPortManager	       = new ViewPortManager(this);
    m_animationManager	       = new AnimationManager(this, m_rootPath + "/animations");
    m_frameManager             = new FrameManager;
    m_primitiveManager         = new PrimitiveManager(this);
}

Engine::~Engine()
{
    deletePtr(m_animationManager);
    deletePtr(m_meshDataManager);
    deletePtr(m_shaderManager);
    deletePtr(m_viewPortManager);
    deletePtr(m_primitiveManager);
    deletePtr(m_textureManager);

    deletePtr(m_frameManager);

    // remove the renderer
    if (m_renderer)
    {
        m_renderer->releaseIt();
        m_renderer = nullptr;
    }

    m_context = nullptr;
}

void Engine::setRenderer(Renderer *renderer)
{
    if (!renderer || !renderer->isValid())
        O3D_ERROR(E_InvalidParameter("Renderer must be valid"));

    if (!renderer->getContext())
        O3D_ERROR(E_InvalidParameter("Renderer context must be valid"));

    if (m_renderer)
        m_renderer->releaseIt();

    m_renderer = renderer;
    m_context = renderer->getContext();

    m_renderer->useIt();
    m_renderer->setCurrent();

    // add apply default parameters from the context
    getContext()->setAllParameters();
}

void Engine::update()
{
    // compute the duration since the last update
    m_frameManager->computeFrameDuration();

    // deferred deletion (this is not the best place to delete texture, glcontext, better deletion mechanism...)
    m_textureManager->update();
    m_meshDataManager->update();
    m_animationManager->update();
}

