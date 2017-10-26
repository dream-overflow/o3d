/**
 * @file precompiled.h
 * @brief Engine precompiled headers
 * @author Emmanuel Ruffio
 * @date 2007-11-23
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_ENGINEPRECOMPILEDHEADER_H
#define _O3D_ENGINEPRECOMPILEDHEADER_H

#include "o3d/core/memorydbg.h"

#ifdef O3D_USE_PCH

#include "o3d/core/precompiled.h"
#include "o3d/image/precompiled.h"
#include "o3d/engine/animation/animation.h"
#include "o3d/engine/animation/animatable.h"
#include "o3d/engine/animation/animationtrack.h"
#include "o3d/engine/hierarchy/node.h"
#include "o3d/engine/hierarchy/targetnode.h"
#include "o3d/engine/object/bones.h"
#include "o3d/engine/object/geometry.h"
#include "o3d/engine/object/facearray.h"
#include "o3d/engine/object/meshdata.h"
#include "o3d/engine/object/camera.h"
#include "o3d/engine/object/complexmesh.h"
#include "o3d/engine/object/gizmo.h"
#include "o3d/engine/object/light.h"
#include "o3d/engine/object/mesh.h"
#include "o3d/engine/object/skin.h"
#include "o3d/engine/object/sceneobject.h"
#include "o3d/engine/object/primitive.h"
#include "o3d/engine/context.h"
#include "o3d/engine/glextensionmanager.h"
#include "o3d/engine/texture/gltexture.h"
#include "o3d/engine/matrix.h"
#include "o3d/engine/vertexbuffer.h"
#include "o3d/engine/material/material.h"
#include "o3d/engine/texture/texture.h"
#include "o3d/engine/effects/specialeffects.h"
#include "o3d/engine/landscape/landscape.h"
//#include "o3d/engine/landscape/pclod/Configs.h"
//#include "o3d/engine/landscape/pclod/Terrain.h"
//#include "o3d/engine/landscape/pclod/ZoneManager.h"
#include "o3d/engine/visibility/visibilityabc.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/engine/scene/sceneio.h"
#include "o3d/engine/scene/sceneinfo.h"

#endif

#endif // _O3D_ENGINEPRECOMPILEDHEADER_H

