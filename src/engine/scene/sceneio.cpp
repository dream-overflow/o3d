/**
 * @file sceneio.cpp
 * @brief Implementation of SceneIO.h
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2008-07-02
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/scene/sceneio.h"

#include "o3d/engine/material/materialprofile.h"
#include "o3d/engine/object/camera.h"
#include "o3d/engine/object/gizmo.h"
#include "o3d/engine/object/skin.h"
#include "o3d/engine/object/complexmesh.h"
#include "o3d/engine/object/light.h"
#include "o3d/engine/object/bones.h"
#include "o3d/engine/animation/animationplayer.h"
#include "o3d/engine/effect/specialeffects.h"
#include "o3d/physic/abcforce.h"
#include "o3d/physic/rigidbody.h"
#include "o3d/audio/sndsource.h"
#include "o3d/audio/sndlistener.h"

using namespace o3d;

Bool SceneIO::isIO(const SceneObject &object) const
{
	// inherit from Skin (before Mesh)
	if (object.getTypeOf(&Skin::ms_classInfo))
		return get(SKIN);

	// inherit from Mesh
	if (object.getTypeOf(&Mesh::ms_classInfo))
		return get(MESH);

	// inherit from ComplexMesh
	if (object.getTypeOf(&ComplexMesh::ms_classInfo))
		return get(COMPLEX_MESH);

	// inherit from Light
	if (object.getTypeOf(&Light::ms_classInfo))
		return get(LIGHT);

	// inherit from Camera
	if (object.getTypeOf(&Camera::ms_classInfo))
		return get(CAMERA);

	// inherit from Gizmo
	if (object.getTypeOf(&Gizmo::ms_classInfo))
		return get(GIZMO);

	// inherit from Bones (before Node)
	if (object.getTypeOf(&Bones::ms_classInfo))
		return get(BONES);

	// inherit from Node
	if (object.getTypeOf(&Node::ms_classInfo))
		return get(NODES);

	// inherit from AnimationPlayer
	if (object.getTypeOf(&AnimationPlayer::ms_classInfo))
		return get(ANIMATION_PLAYER);

	// inherit from Animation
	if (object.getTypeOf(&Animation::ms_classInfo))
		return get(ANIMATION);

	// inherit from SpecialEffect
	if (object.getTypeOf(&SpecialEffects::ms_classInfo))
		return get(SPECIAL_EFFECT);

	// inherit from SndSource
	if (object.getTypeOf(&SndSource::ms_classInfo))
		return get(SND_SOURCE);

	// inherit from SndListener
	if (object.getTypeOf(&SndListener::ms_classInfo))
		return get(SND_LISTENER);

	// inherit from ABCForce
	if (object.getTypeOf(&ABCForce::ms_classInfo))
		return get(PHYSIC);

	// inherit from RigidBody
	if (object.getTypeOf(&RigidBody::ms_classInfo))
		return get(PHYSIC);

	// inherit from PhysicModel
	if (object.getTypeOf(&PhysicModel::ms_classInfo))
		return get(PHYSIC);

	// inherit from Material
	if (object.getTypeOf(&Material::ms_classInfo))
		return get(MATERIAL);

	// inherit from MaterialProfile
	if (object.getTypeOf(&MaterialProfile::ms_classInfo))
		return get(MATERIAL);

	return False;
}

// serialization
Bool SceneIO::writeToFile(OutStream &os)
{
    os << m_setting;
	return True;
}

Bool SceneIO::readFromFile(InStream& is)
{
    is >> m_setting;
	return True;
}

