/**
 * @file materialtechnique.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/texture/texture2d.h"
#include "o3d/engine/texture/cubemaptexture.h"
#include "o3d/engine/material/materialprofile.h"
#include "o3d/engine/material/materialtechnique.h"
#include "o3d/engine/alphapipeline.h"
#include "o3d/engine/object/facearrayvisitor.h"
#include "o3d/engine/context.h"
#include "o3d/engine/scene/scene.h"

using namespace o3d;

// Constructor.
MaterialTechnique::MaterialTechnique(MaterialProfile *profile) :
	m_profile(profile),
	m_lodIndex(0),
	m_valid(False)
{
	O3D_ASSERT(profile);
}

// Destructor.
MaterialTechnique::~MaterialTechnique()
{
	// delete any pass
	size_t numPasses = m_pass.size();

	for (size_t i = 0; i < numPasses; ++i)
		deletePtr(m_pass[i]);
}

// Copy operator.
MaterialTechnique& MaterialTechnique::operator= (const MaterialTechnique &dup)
{
	// copy passes
	UInt32 numPasses = static_cast<UInt32>(dup.m_pass.size());
	setNumPass(numPasses);

	for (UInt32 i = 0; i < numPasses; ++i)
		*m_pass[i] = *dup.m_pass[i];

	// other
	m_lodIndex = dup.m_lodIndex;
	m_valid = False;

	return *this;
}

// Is the technique supported by the hardware.
Bool MaterialTechnique::isTechniqueSupported() const
{
	size_t numPasses = m_pass.size();

	for (size_t i = 0; i < numPasses; ++i)
		if (!m_pass[i]->isPassSupported())
			return False;

	return True;
}

// Define the number of pass.
void MaterialTechnique::setNumPass(UInt32 numPass)
{
	// delete more passes
	if (m_pass.size() > static_cast<size_t>(numPass))
	{
		for (size_t i = numPass; i < m_pass.size(); ++i)
			deletePtr(m_pass[i]);
	}

	size_t oldSize = m_pass.size();
	m_pass.resize(numPass);

	// add more passes
	if (oldSize < static_cast<size_t>(numPass))
	{
		for (size_t i = oldSize; i < numPass; ++i)
			m_pass[i] = new MaterialPass(this);
	}
}

// Get the pass at a specific index (read only).
const MaterialPass& MaterialTechnique::getPass(UInt32 index) const
{
	if (static_cast<size_t>(index) >= m_pass.size())
		O3D_ERROR(E_IndexOutOfRange("Pass index"));
	
	return *m_pass[index];
}

// Get the pass at a specific index.
MaterialPass& MaterialTechnique::getPass(UInt32 index)
{
	if (static_cast<size_t>(index) >= m_pass.size())
		O3D_ERROR(E_IndexOutOfRange("Pass index"));

	return *m_pass[index];
}

// Set ambient color for any passes.
void MaterialTechnique::setAmbient(const Color &color)
{
	UInt32 numPasses = static_cast<UInt32>(m_pass.size());

	for (UInt32 i = 0; i < numPasses; ++i)
		m_pass[i]->setAmbient(color);
}

// Set ambient color for any passes.
void MaterialTechnique::setDiffuse(const Color &color)
{
	UInt32 numPasses = static_cast<UInt32>(m_pass.size());

	for (UInt32 i = 0; i < numPasses; ++i)
		m_pass[i]->setDiffuse(color);
}

// Set ambient color for any passes.
void MaterialTechnique::setSpecular(const Color &color)
{
	UInt32 numPasses = static_cast<UInt32>(m_pass.size());

	for (UInt32 i = 0; i < numPasses; ++i)
		m_pass[i]->setSpecular(color);
}

// Set ambient color for any passes.
void MaterialTechnique::setSelfIllumination(const Color &color)
{
	UInt32 numPasses = static_cast<UInt32>(m_pass.size());

	for (UInt32 i = 0; i < numPasses; ++i)
		m_pass[i]->setSelfIllumination(color);
}

// Set ambient color for any passes.
void MaterialTechnique::setShine(Float exponent)
{
	UInt32 numPasses = static_cast<UInt32>(m_pass.size());

	for (UInt32 i = 0; i < numPasses; ++i)
		m_pass[i]->setShine(exponent);
}

// Set ambient color for any passes.
void MaterialTechnique::setSpecularExponent(Float exponent)
{
	UInt32 numPasses = static_cast<UInt32>(m_pass.size());

	for (UInt32 i = 0; i < numPasses; ++i)
		m_pass[i]->setSpecularExponent(exponent);
}

// Set ambient color for any passes.
void MaterialTechnique::setReflectivity(Float reflectivity)
{
	UInt32 numPasses = static_cast<UInt32>(m_pass.size());

	for (UInt32 i = 0; i < numPasses; ++i)
		m_pass[i]->setReflectivity(reflectivity);
}

// Set ambient color for any passes.
void MaterialTechnique::setTransparency(Float transparency)
{
	UInt32 numPasses = static_cast<UInt32>(m_pass.size());

	for (UInt32 i = 0; i < numPasses; ++i)
		m_pass[i]->setTransparency(transparency);
}

// Set ambient color for any passes.
void MaterialTechnique::setBumpOffset(const Vector2f &offset)
{
	UInt32 numPasses = static_cast<UInt32>(m_pass.size());

	for (UInt32 i = 0; i < numPasses; ++i)
		m_pass[i]->setBumpOffset(offset);
}

// Set the culling mode for any passes.
void MaterialTechnique::setCullingMode(CullingMode mode)
{
	UInt32 numPasses = static_cast<UInt32>(m_pass.size());

	for (UInt32 i = 0; i < numPasses; ++i)
		m_pass[i]->setCullingMode(mode);
}

// Set the blending mode for any passes.
void MaterialTechnique::setBlendingFunc(Blending::FuncProfile func)
{
	UInt32 numPasses = static_cast<UInt32>(m_pass.size());

	for (UInt32 i = 0; i < numPasses; ++i)
        m_pass[i]->setBlendingFunc(func);
}

// Set if faces are sorted and displayed with the alpha-pipeline for any passes.
void MaterialTechnique::setSorted(Bool state)
{
	UInt32 numPasses = static_cast<UInt32>(m_pass.size());

	for (UInt32 i = 0; i < numPasses; ++i)
		m_pass[i]->setSorted(state);
}

// Set double side drawing mode status for any passes.
void MaterialTechnique::setDoubleSide(Bool state)
{
	UInt32 numPasses = static_cast<UInt32>(m_pass.size());

	for (UInt32 i = 0; i < numPasses; ++i)
		m_pass[i]->setDoubleSide(state);
}

// Set the depth buffer test status for any passes.
void MaterialTechnique::setDepthTest(Bool state)
{
	UInt32 numPasses = static_cast<UInt32>(m_pass.size());

	for (UInt32 i = 0; i < numPasses; ++i)
		m_pass[i]->setDepthTest(state);
}

// Set the depth buffer write status for any passes.
void MaterialTechnique::setDepthWrite(Bool state)
{
	UInt32 numPasses = static_cast<UInt32>(m_pass.size());

	for (UInt32 i = 0; i < numPasses; ++i)
		m_pass[i]->setDepthWrite(state);
}

// Set the depth range to normal (false) or infinite (true) for any passes.
void MaterialTechnique::setInfiniteDepthRange(Bool state)
{
	UInt32 numPasses = static_cast<UInt32>(m_pass.size());

	for (UInt32 i = 0; i < numPasses; ++i)
		m_pass[i]->setInfiniteDepthRange(state);
}

// Set the alpha test status for any passes.
void MaterialTechnique::setAlphaTest(Bool state)
{
	UInt32 numPasses = static_cast<UInt32>(m_pass.size());

	for (UInt32 i = 0; i < numPasses; ++i)
		m_pass[i]->setAlphaTest(state);
}

// Set the alpha test function mode and ref value for any passes.
void MaterialTechnique::setAlphaTestFunc(Comparison func, Float ref)
{
	UInt32 numPasses = static_cast<UInt32>(m_pass.size());

	for (UInt32 i = 0; i < numPasses; ++i)
		m_pass[i]->setAlphaTestFunc(func, ref);
}

// Clear any loaded technique data (inverse of Compile).
void MaterialTechnique::clear()
{
	size_t numPasses = m_pass.size();

	for (size_t i = 0; i < numPasses; ++i)
		m_pass[i]->clear();

	m_valid = False;
}

// Build any pass and material of the technique.
void MaterialTechnique::prepareAndCompile(Shadable &shadable)
{
	size_t numPasses = m_pass.size();

	for (size_t i = 0; i < numPasses; ++i)
		m_pass[i]->prepareAndCompile(shadable);

	m_valid = True;
}

// Process the material technique to a shadable object.
void MaterialTechnique::processMaterial(
		Shadable &shadable,
		Shadowable *shadowable,
		Pickable *pickable,
		const DrawInfo &drawInfo)
{
	if (m_valid)
	{
		size_t numPasses = m_pass.size();

		Bool sorted = False;

		for (size_t i = 0; i < numPasses; ++i)
		{
			// process alpha pipeline once if necessary
			if (m_pass[i]->needsSorting() && !sorted && (drawInfo.pass == DrawInfo::AMBIENT_PASS))
			{
				sortFaces(shadable);
				sorted = True;
			}
			// process non sorted material passes
			else
				m_pass[i]->processMaterial(shadable, shadowable, pickable, drawInfo);
		}
	}
}

// Process the material technique to a shadable object only for sorted material pass.
void MaterialTechnique::processAlphaPipeline(
		Shadable &shadable,
		const DrawInfo &drawInfo)
{
	if (m_valid)
	{
		size_t numPasses = m_pass.size();

		for (size_t i = 0; i < numPasses; ++i)
		{
			// process sorted material passes only
			if (m_pass[i]->needsSorting())
                m_pass[i]->processMaterial(shadable, nullptr, nullptr, drawInfo);
		}
	}
}

// Process the face sorting using the alpha pipeline.
void MaterialTechnique::sortFaces(Shadable &shadable)
{
	AlphaPipeline *alphaPipeline = m_profile->getScene()->getAlphaPipeline();

	// perform the geometry initialization before use vertices and faces
	shadable.processAllFaces(Shadable::PREPARE_GEOMETRY);

	VertexElement *verticesElement = shadable.getVertexElement(V_VERTICES_ARRAY);
	FaceArray *faceArray = shadable.getFaceArray();
	if (!verticesElement || !faceArray)
		return;

	if (faceArray->getNumElements() == 0)
		return;

	// transformation matrix
	Matrix4 modelView = m_profile->getScene()->getContext()->modelView().get();

	// vertices
	const Float *vertices = verticesElement->lockArray(0, 0);

	// triangles
	FaceArrayVisitor triangles(faceArray);

	Float zaverage;		// z average of the face
	UInt32 a, b, c;

	for (FaceArrayIterator it = triangles.begin(); it != triangles.end(); ++it)
	{
		a = it.a * 3;
		b = it.b * 3;
		c = it.c * 3;

		// compute the Z average position of this face
		zaverage = modelView.transformOnZ(vertices[a], vertices[a+1], vertices[a+2])
				 + modelView.transformOnZ(vertices[b], vertices[b+1], vertices[b+2])
				 + modelView.transformOnZ(vertices[c], vertices[c+1], vertices[c+2]);

		// add this face to the alpha pipeline
		alphaPipeline->addFace(&shadable, this, it.a, it.b, it.c, zaverage);
	}

	verticesElement->unlockArray();
}

Bool MaterialTechnique::writeToFile(OutStream &os) const
{
    os << m_lodIndex;

	// passes
	UInt32 numPasses = static_cast<UInt32>(m_pass.size());
    os << numPasses;

	for (UInt32 i = 0; i < numPasses; ++i)
        os << *m_pass[i];

	return True;
}

Bool MaterialTechnique::readFromFile(InStream &is)
{
    is >> m_lodIndex;

	UInt32 numPasses;
    is >> numPasses;

	setNumPass(numPasses);

	for (UInt32 i = 0; i < numPasses; ++i)
        is >> *m_pass[i];

	return True;
}
