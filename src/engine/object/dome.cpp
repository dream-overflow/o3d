/**
 * @file dome.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/object/dome.h"

#include "o3d/core/vector3.h"

#include <math.h>

using namespace o3d;

static inline UInt32 getVertexCount(const Dome * _pDome, Int32 _k)
{
	if (_k == (1 << _pDome->getSubDiv()))
		return 1;
	else
		return UInt32(4*((1 << _pDome->getSubDiv()) - _k) + (_pDome->isTexCoords() ? 1 : 0));
}

static inline UInt32 getFirstIndexPart(const Dome * _pDome, Int32 _k, Int32 _part)
{
	return UInt32(2*_k*((1 << (_pDome->getSubDiv()+1)) - _k + 1) + (_pDome->isTexCoords() ? _k : 0) + (_part * getVertexCount(_pDome, _k)) / 4);
}

static inline UInt32 clampIndex(const Dome * _pDome, Int32 _k, UInt32 _index)
{
	if (_pDome->isTexCoords())
		return _index;

	if (_index == getFirstIndexPart(_pDome, _k+1, 0))
		return getFirstIndexPart(_pDome, _k, 0);
	else
		return _index;
}

O3D_IMPLEMENT_ABSTRACT_CLASS1(Dome, ENGINE_DOME, Primitive)

Dome::Dome(UInt32 _flags):
	Primitive(_flags),
	m_radius(0.0f),
	m_height(0.0f),
	m_subDiv(0),
	m_texCoordPolicy(TEX_PROJECTION),
	m_upToDate(True)
{
}

Dome::Dome(Float _radius, Float _height, UInt32 _subDiv, UInt32 _flags):
	Primitive(_flags),
	m_radius(_radius),
	m_height(_height),
	m_subDiv(_subDiv),
	m_texCoordPolicy(TEX_PROJECTION),
	m_upToDate(False)
{
	update();
	m_upToDate = True;
}

//! A destructor
Dome::~Dome()
{
}

void Dome::buildVertices()
{
	const Int32 lStacks = getStackCount();
	const UInt32 lVertexCount = (1 << (m_subDiv+1))*((1 << m_subDiv) + 1) + 1 + (isTexCoords() ? lStacks - 1 : 0);
	const Int32 lSlices = 4*(1 << m_subDiv) + (isTexCoords() ? 1 : 0);

	if (m_verticesCount != lVertexCount)
	{
		deleteArray(m_pVertices);
		deleteArray(m_pTexCoords);

		m_verticesCount = lVertexCount;
		m_pVertices = new Float[3*m_verticesCount];

		if (isTexCoords())
			m_pTexCoords = new Float[2*m_verticesCount];
	}

	Float * lPtr = m_pVertices;

	for (Int32 k = 0 ; k < lStacks-1 ; ++k)
	{
		const Float lBeta = 0.5f * o3d::PI * Float(k)/(lStacks-1);
		const Float lAlpha = lBeta + asinf((m_radius-m_height)/m_radius * cosf(lBeta));
		const Float lCosRadius = m_radius * cosf(lAlpha);
		const Float lSinRadius = m_radius * sinf(lAlpha) - (m_radius - m_height);

		for (Int32 i = 0 ; i < lSlices - 4*k ; ++i, lPtr += 3)
		{
			const Float lAngle = 2.0f * o3d::PI * Float(i)/(lSlices - 4*k - (isTexCoords() ? 1 : 0));

			lPtr[Z] = cosf(lAngle) * lCosRadius;
			lPtr[X] = sinf(lAngle) * lCosRadius;
			lPtr[Y] = lSinRadius;
		}
	}

	lPtr[Z] = 0.0f;
	lPtr[X] = 0.0f;
	lPtr[Y] = m_height;

	if (isTexCoords())
	{
		switch(m_texCoordPolicy)
		{
		case TEX_LATITUDE_LONGITUDE:
			{
				const UInt32 lStackCount = getStackCount();

				Float * lCoordPtr = m_pTexCoords;
				const Float * lVertexPtr = getVertices();

				for (UInt32 k = 0 ; k < lStackCount - 1 ; ++k)
				{
					const UInt32 lVertexCount = getVertexCount(k);
					const Float lISliceCount = 1.0f / (lVertexCount - 1);
					const Float lV = 1.0f - 2.0f/o3d::PI * asinf(lVertexPtr[Y] / Vector3(lVertexPtr).length());

					Float lU = 1.0f;

					for (UInt32 i = lVertexCount ; i > 0 ; --i, lCoordPtr += 2, lU -= lISliceCount)
					{
						lCoordPtr[0] = lU;
						lCoordPtr[1] = lV;
					}

					lVertexPtr += 3 * lVertexCount;
				}

				lCoordPtr[0] = 0.0f;
				lCoordPtr[1] = 0.0f;
			}
			break;
		case TEX_UNFOLD:
			{
				const Float lIConst = 1.0f / acos((m_radius - m_height) / m_radius);

				Float * lCoordPtr = m_pTexCoords;
				const Float * lVertexPtr = getVertices();

				for (UInt32 k = 0 ; k < getNumVertices() ; ++k, lVertexPtr += 3, lCoordPtr += 2)
				{
					Float u = lVertexPtr[Z];
					Float v = lVertexPtr[X];
                    Float lINorm = Math::sqrt(u*u + v*v);
					lINorm = (lINorm > 0.0f ? 1.0f / lINorm : 0.0f);
					u *= lINorm;
					v *= lINorm;

					Float dist = lVertexPtr[Z] * u + lVertexPtr[X] * v;
					Float height = lVertexPtr[Y];
					Float f = 0.5f * atan(dist / (height + m_radius - m_height)) * lIConst;

					lCoordPtr[0] = 0.5f + f * u;
					lCoordPtr[1] = 0.5f + f * v;
				}
			}
			break;
		default:
			{
				const Float lIBaseRadius = 1.0f / getBaseRadius();

				Float * lCoordPtr = m_pTexCoords;
				const Float * lVertexPtr = getVertices();

				for (UInt32 k = 0 ; k < getNumVertices() ; ++k, lVertexPtr += 3, lCoordPtr += 2)
				{
					lCoordPtr[0] = 0.5f + lVertexPtr[Z] * lIBaseRadius;
					lCoordPtr[1] = 0.5f + lVertexPtr[X] * lIBaseRadius;
				}
			}
			break;
		}
	}
}

void Dome::buildIndices()
{
	const Int32 lStacks = (1 << m_subDiv) + 1;

	if (isWired())
	{
		const UInt32 lLineCount = (1 << m_subDiv) * (6*(1 << m_subDiv) + 2);

		if (2*lLineCount != m_indicesCount)
		{
			deleteArray(m_pIndices);

			m_indicesCount = 2*lLineCount;
			m_pIndices = new UInt32[m_indicesCount];
		}

		UInt32 * lPtr = m_pIndices;

		for (Int32 k = 0 ; k < lStacks-1 ; ++k)
		{
			for (Int32 q = 0 ; q < 4 ; ++q)
			{
				const UInt32 lFirstIndex = ::getFirstIndexPart(this, k, q);
				const UInt32 lFirstIndexNext = ::getFirstIndexPart(this, k+1, q);
				const Int32 lVertexCount = (::getVertexCount(this, k)- (isTexCoords() ? 1 : 0))/4;

				lPtr[0] = lFirstIndex;
				lPtr[1] = lFirstIndexNext;

				lPtr += 2;

				for (Int32 s = 0 ; s < lVertexCount-1 ; ++s)
				{
					lPtr[0] = lFirstIndex + s;
					lPtr[1] = clampIndex(this, k, lFirstIndex + s+1);
					lPtr += 2;

					lPtr[0] = clampIndex(this, k, lFirstIndex + s+1);
					lPtr[1] = lFirstIndexNext + s;
					lPtr += 2;

					lPtr[0] = clampIndex(this, k, lFirstIndex + s+1);
					lPtr[1] = clampIndex(this, k+1, lFirstIndexNext + s + 1);
					lPtr += 2;
				}

				lPtr[0] = lFirstIndex + lVertexCount - 1;
				lPtr[1] = clampIndex(this, k, lFirstIndex + lVertexCount);
				lPtr += 2;
			}			
		}
	}
	else
	{
		const UInt32 lTriangleCount = 4*(1 << m_subDiv) * (1 << m_subDiv);

		if (3*lTriangleCount != m_indicesCount)
		{
			deleteArray(m_pIndices);

			m_indicesCount = 3*lTriangleCount;
			m_pIndices = new UInt32[m_indicesCount];
		}

		UInt32 * lPtr = m_pIndices;

		for (Int32 k = 0 ; k < lStacks-1 ; ++k)
		{
			for (Int32 q = 0 ; q < 4 ; ++q)
			{
				const UInt32 lFirstIndex = ::getFirstIndexPart(this, k, q);
				const UInt32 lFirstIndexNext = ::getFirstIndexPart(this, k+1, q);
				const Int32 lVertexCount = (::getVertexCount(this, k)- (isTexCoords() ? 1 : 0))/4;

				for (Int32 s = 0 ; s < lVertexCount-1 ; ++s)
				{
					lPtr[0] = lFirstIndex + s;
					lPtr[1] = clampIndex(this, k+1, lFirstIndexNext + s);
					lPtr[2] = clampIndex(this, k, lFirstIndex + s+1);

					lPtr += 3;

					lPtr[0] = clampIndex(this, k, lFirstIndex + s+1);
					lPtr[1] = lFirstIndexNext + s;
					lPtr[2] = clampIndex(this, k+1, lFirstIndexNext + s+1);

					lPtr += 3;
				}

				lPtr[0] = clampIndex(this, k, lFirstIndex + lVertexCount - 1);
				lPtr[1] = clampIndex(this, k+1, lFirstIndexNext + lVertexCount - 1);
				lPtr[2] = clampIndex(this, k, lFirstIndex + lVertexCount);

				lPtr += 3;
			}
		}
	}
}

Bool Dome::update()
{
	if (isValid())
	{
		if (!isUpToDate())
		{
			buildVertices();
			buildIndices();

			m_upToDate = True;
		}

		return True;
	}
	else
		return False;
}

//! @brief Specify wether or not the dome is properly defined
Bool Dome::isValid() const
{
	return ((m_radius > 0.0f) && (m_height > 0.0f) && (m_subDiv > 0));
}

void Dome::setRadius(Float _radius, Bool _update)
{
	m_radius = _radius;
	m_upToDate = False;

	if (_update)
		update();
}

void Dome::setHeight(Float _height, Bool _update)
{
	m_height = _height;
	m_upToDate = False;

	if (_update)
		update();
}

Float Dome::getBaseRadius() const
{
	const Float lT = m_radius-m_height;

    return Math::sqrt(m_radius*m_radius - lT*lT);
}

void Dome::setSubDiv(UInt32 _subDiv, Bool _update)
{
	m_subDiv = _subDiv;
	m_upToDate = False;

	if (_update)
		update();
}

UInt32 Dome::getStackCount() const
{
	return ((1 << m_subDiv) + 1);
}

UInt32 Dome::getSliceCount(UInt32 _stacks) const
{
	if (_stacks >= getStackCount())
		return 0;
	else if (_stacks == getStackCount() - 1)
		return 1;
	else
		return (4*(1<< m_subDiv) - (_stacks << 2));
}

UInt32 Dome::getVertexCount(UInt32 _stacks) const
{
	if (_stacks >= getStackCount())
		return 0;
	else if (_stacks == getStackCount() - 1)
		return 1;
	else
		return (4*(1<< m_subDiv) - (_stacks << 2)) + (isTexCoords() ? 1 : 0);
}

void Dome::setTextureCoordinatePolicy(TexCoordPolicy _policy)
{
	m_upToDate = (m_texCoordPolicy == _policy);

	m_texCoordPolicy = _policy;
}

void Dome::enableTextureCoordinate(Bool _value, Bool _update)
{
	if (_value != isTexCoords())
	{
		if (_value)
			m_capacities |= GEN_TEX_COORDS;
		else
			m_capacities &= ~GEN_TEX_COORDS;

		m_upToDate = False;

		if (_update)
			update();
	}
}

const Float* Dome::getVerticesAtStack(UInt32 _stacks) const
{
	if (!isUpToDate() || (_stacks > getStackCount()))
		return NULL;
	
	UInt32 lIndex = 0;
	for (UInt32 k = 0 ; k < _stacks ; ++k)
		lIndex += ::getVertexCount(this, k);

	return m_pVertices + 3*lIndex;
}

