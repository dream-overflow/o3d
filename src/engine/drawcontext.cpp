/**
 * @file drawcontext.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/context.h"
#include "o3d/engine/drawcontext.h"
#include "o3d/engine/scene/scene.h"

using namespace o3d;

// Constructor.
DrawContext::DrawContext(Context *glContext) :
    m_context(glContext),
	m_cullingMode(CULLING_BACK_FACE),
	m_isSorted(False),
    m_blendingFunc(Blending::DISABLED),
    m_blendingEquation(Blending::FUNC_ADD),
	m_doubleSide(False),
	m_depthTest(True),
	m_depthWrite(True),
	m_infiniteDepthRange(False)
{
    O3D_ASSERT(m_context);
}

// Copy constructor.
DrawContext::DrawContext(const DrawContext &dup) :
	m_context(dup.m_context),
	m_cullingMode(dup.m_cullingMode),
	m_isSorted(dup.m_isSorted),
    m_blendingFunc(dup.m_blendingFunc),
    m_blendingEquation(dup.m_blendingEquation),
	m_doubleSide(dup.m_doubleSide),
	m_depthTest(dup.m_depthTest),
	m_depthWrite(dup.m_depthWrite),
	m_infiniteDepthRange(dup.m_infiniteDepthRange)
{

}

// Destructor.
DrawContext::~DrawContext()
{

}

// Copy operator.
DrawContext& DrawContext::operator= (const DrawContext &dup)
{
	m_cullingMode = dup.m_cullingMode;
	m_isSorted = dup.m_isSorted;

    m_blendingFunc = dup.m_blendingFunc;
    m_blendingEquation = dup.m_blendingEquation;

	m_doubleSide = dup.m_doubleSide;
	m_depthTest = dup.m_depthTest;
	m_depthWrite = dup.m_depthWrite;
	m_infiniteDepthRange = dup.m_infiniteDepthRange;

	return *this;
}

// Apply immediately the draw content.
void DrawContext::apply()
{
	if (!m_context)
		O3D_ERROR(E_InvalidPrecondition("The Context must be defined"));

	if (m_depthTest)
		m_context->enableDepthTest();
	else
		m_context->disableDepthTest();

	if (m_depthWrite)
		m_context->enableDepthWrite();
	else
		m_context->disableDepthWrite();

	if (m_doubleSide || m_context->isOverrideDoubleSided())
		m_context->enableDoubleSide();
	else
		m_context->disableDoubleSide();

	if (m_infiniteDepthRange)
		m_context->setInfiniteDepthRange();
	else
		m_context->setDefaultDepthRange();

    m_context->blending().setEquation(m_blendingEquation);
    m_context->blending().setFunc(m_blendingFunc);

	m_context->setCullingMode(m_cullingMode);
}

// reset to default supplementary parameters
void DrawContext::resetParameters()
{
	m_context->setDefaultDepthTest();
	m_context->setDefaultDepthWrite();
	m_context->setDefaultDoubleSide();
	m_context->setDefaultDepthRange();
	m_context->setDefaultCullingMode();

    m_context->blending().setDefaultFunc();
    m_context->blending().setDefaultEquation();
}

// Write content to file.
Bool DrawContext::writeToFile(OutStream &os) const
{
    os   << m_cullingMode
	     << m_isSorted
         << m_blendingFunc
         << m_blendingEquation
	     << m_doubleSide
	     << m_depthTest
	     << m_depthWrite
	     << m_infiniteDepthRange;

	return True;
}

// Read content from file.
Bool DrawContext::readFromFile(InStream &is)
{
    is   >> m_cullingMode
	     >> m_isSorted
         >> m_blendingFunc
         >> m_blendingEquation
	     >> m_doubleSide
	     >> m_depthTest
	     >> m_depthWrite
	     >> m_infiniteDepthRange;

	return True;
}

