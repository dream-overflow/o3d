/**
 * @file beziersurface.cpp
 * @brief This is a computed surface with bezier method.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2006-04-22
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/object/beziersurface.h"

#include "o3d/engine/context.h"
#include "o3d/engine/matrix.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/core/classfactory.h"
#include "o3d/core/debug.h"

using namespace o3d;

/*---------------------------------------------------------------------------------------
  class BezierSurface
  -------------------------------------------------------------------------------------

---------------------------------------------------------------------------------------*/

O3D_IMPLEMENT_DYNAMIC_CLASS1(BezierSurface, ENGINE_BEZIERSURFACE_MESH, ComplexMesh)

// Get the drawing type
UInt32 BezierSurface::getDrawType() const { return Scene::DRAW_BEZIER_SURFACE; }

/*---------------------------------------------------------------------------------------
  serialisation
---------------------------------------------------------------------------------------*/
Bool BezierSurface::writeToFile(OutStream &os)
{
    return ComplexMesh::writeToFile(os);
}

Bool BezierSurface::readFromFile(InStream &is)
{
    return ComplexMesh::readFromFile(is);
}

