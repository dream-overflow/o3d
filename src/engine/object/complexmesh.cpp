/**
 * @file complexmesh.cpp
 * @brief by face.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2005-05-04
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/object/complexmesh.h"

#include "o3d/engine/context.h"
#include "o3d/engine/matrix.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/core/classfactory.h"
#include "o3d/core/debug.h"

using namespace o3d;

O3D_IMPLEMENT_ABSTRACT_CLASS1(ComplexMesh, ENGINE_COMPLEX_MESH, SceneObject)

/*---------------------------------------------------------------------------------------
  serialisation
---------------------------------------------------------------------------------------*/
Bool ComplexMesh::writeToFile(OutStream &os)
{
    return ShadableObject::writeToFile(os);
}

Bool ComplexMesh::readFromFile(InStream &is)
{
    return ShadableObject::readFromFile(is);
}

