/**
 * @file isosphere.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/object/isosphere.h"

#include "o3d/core/debug.h"
#include <math.h>

using namespace o3d;

O3D_IMPLEMENT_ABSTRACT_CLASS1(IsoSphere, ENGINE_ISO_SPHERE, Primitive)

IsoSphere::IsoSphere(
	Float radius,
	UInt32 subDiv,
	UInt32 flags) :
		Primitive(flags),
			m_radius(radius),
			m_subDiv(subDiv),
			m_optimize(False)
{
	O3D_ASSERT(radius > 0.f);

	buildIsoSphere();
}

// Nombre de plan sur la triangle sup�rieure, ou nombre de vertex � la base d'un triangle
static inline UInt32 ht(UInt32 _n)
{
	return (1 << _n) + 1;
}

// Indice du premier vertex de la rang�e k
static inline UInt32 in(UInt32 _k, UInt32 _part)
{
	return (1 - (_k == 0 ? 1 : 0) + (5*(_k-1)*_k)/2 + _part * _k);
}

// Nombre de vertex de la rang�e k
static inline UInt32 vt(UInt32 _k)
{
	return (_k+1);
}

// Gere uniquement le modulo pour s'assurer que l'indice reste dans l'intervale correct.
static inline UInt32 index(UInt32 _k, UInt32 _i)
{
	return (_k == 0 ? 0 : (in(_k, 0) + (_i - in(_k, 0)) % (5*(vt(_k)-1))));
}

static void isoSphereSubDivide(
	UInt32 _currentSubDiv,
	UInt32 _currentRow,
	UInt32 _startingIndex,
	UInt32 _part,
	UInt32 _finalSubDiv,
	Float * _pVertices);

static void isoSphereSubDivideInvert(
	UInt32 _currentSubDiv,
	UInt32 _currentRow,
	UInt32 _startingIndex,
	UInt32 _part,
	UInt32 _finalSubDiv,
	Float * _pVertices);

static void isoSphereSubDivideInvert(
	UInt32 _currentSubDiv,
	UInt32 _currentRow,
	UInt32 _startingIndex,
	UInt32 _part,
	UInt32 _finalSubDiv,
	Float * _pVertices)
{
	const UInt32 lRowStep = (ht(_finalSubDiv)-1) / (ht(_currentSubDiv)-1);
	const UInt32 lCurrentRow = _currentRow;
	const UInt32 lBaseRow = lCurrentRow - 2 * lRowStep;
	const UInt32 lCenterRow = lBaseRow + lRowStep;

	if (_currentSubDiv < _finalSubDiv)
	{
		isoSphereSubDivide(_currentSubDiv+1, lBaseRow, _startingIndex - lRowStep, _part, _finalSubDiv, _pVertices);
		isoSphereSubDivideInvert(_currentSubDiv+1, lCenterRow, _startingIndex - lRowStep, _part, _finalSubDiv, _pVertices);
		isoSphereSubDivideInvert(_currentSubDiv+1, lCenterRow, _startingIndex, _part, _finalSubDiv, _pVertices);
		isoSphereSubDivideInvert(_currentSubDiv+1, lCurrentRow, _startingIndex, _part, _finalSubDiv, _pVertices);
	}
}

// CurrentRow le plan contenant le vertex A
static void isoSphereSubDivide(
	UInt32 _currentSubDiv,
	UInt32 _currentRow,
	UInt32 _startingIndex,
	UInt32 _part,
	UInt32 _finalSubDiv,
	Float * _pVertices)
{
	const UInt32 lRowStep = (ht(_finalSubDiv)-1) / (ht(_currentSubDiv)-1);
	const UInt32 lCurrentRow = _currentRow;
	const UInt32 lBaseRow = lCurrentRow + 2 * lRowStep;

	const UInt32 lFace[3] = {	index(lCurrentRow, in(lCurrentRow, _part) + _startingIndex),
								index(lBaseRow, in(lBaseRow, _part) + _startingIndex),
								index(lBaseRow, in(lBaseRow, _part) + 2*lRowStep + _startingIndex)};

	const Float * lVA = &_pVertices[3*lFace[0]];
	const Float * lVB = &_pVertices[3*lFace[1]];
	const Float * lVC = &_pVertices[3*lFace[2]];

	const UInt32 lCenterRow = _currentRow + lRowStep;
	const UInt32 lCenterRowIndex = index(lCenterRow, lFace[1] - (in(lBaseRow, _part) - in(lCenterRow, _part)));

	Float* lNewVertexAB = &_pVertices[3*(lCenterRowIndex)];
	Float* lNewVertexBC = &_pVertices[3*index(lBaseRow, lFace[1] + lRowStep)];
	Float* lNewVertexCA = &_pVertices[3*index(lCenterRow, lCenterRowIndex + lRowStep)];

	lNewVertexAB[X] = 0.5f*(lVA[X] + lVB[X]);
	lNewVertexAB[Y] = 0.5f*(lVA[Y] + lVB[Y]);
	lNewVertexAB[Z] = 0.5f*(lVA[Z] + lVB[Z]);

	lNewVertexBC[X] = 0.5f*(lVB[X] + lVC[X]);
	lNewVertexBC[Y] = 0.5f*(lVB[Y] + lVC[Y]);
	lNewVertexBC[Z] = 0.5f*(lVB[Z] + lVC[Z]);

	lNewVertexCA[X] = 0.5f*(lVC[X] + lVA[X]);
	lNewVertexCA[Y] = 0.5f*(lVC[Y] + lVA[Y]);
	lNewVertexCA[Z] = 0.5f*(lVC[Z] + lVA[Z]);

	Float liLength;
	liLength = 1.0f/sqrtf(lNewVertexAB[X] * lNewVertexAB[X] + lNewVertexAB[Y] * lNewVertexAB[Y] + lNewVertexAB[Z] * lNewVertexAB[Z]);
	lNewVertexAB[X] *= liLength;
	lNewVertexAB[Y] *= liLength;
	lNewVertexAB[Z] *= liLength;

	liLength = 1.0f/sqrtf(lNewVertexBC[X] * lNewVertexBC[X] + lNewVertexBC[Y] * lNewVertexBC[Y] + lNewVertexBC[Z] * lNewVertexBC[Z]);
	lNewVertexBC[X] *= liLength;
	lNewVertexBC[Y] *= liLength;
	lNewVertexBC[Z] *= liLength;

	liLength = 1.0f/sqrtf(lNewVertexCA[X] * lNewVertexCA[X] + lNewVertexCA[Y] * lNewVertexCA[Y] + lNewVertexCA[Z] * lNewVertexCA[Z]);
	lNewVertexCA[X] *= liLength;
	lNewVertexCA[Y] *= liLength;
	lNewVertexCA[Z] *= liLength;

	if (_currentSubDiv < _finalSubDiv)
	{
		isoSphereSubDivide(_currentSubDiv+1, lCurrentRow, _startingIndex, _part, _finalSubDiv, _pVertices);
		isoSphereSubDivide(_currentSubDiv+1, lCurrentRow + lRowStep, _startingIndex, _part, _finalSubDiv, _pVertices);
		isoSphereSubDivide(_currentSubDiv+1, lCurrentRow + lRowStep, _startingIndex + lRowStep, _part, _finalSubDiv, _pVertices);
		isoSphereSubDivideInvert(_currentSubDiv+1, lBaseRow, _startingIndex + lRowStep, _part, _finalSubDiv, _pVertices);
	}
}

static void isoSphereSubDivideSegment(
	UInt32 _firstIndex,
	UInt32 _secondIndex,
	UInt32 _currentSubDiv,
	UInt32 _finalSubDiv,
	Float *_pVertices)
{
	if (_currentSubDiv == _finalSubDiv)
		return;

	const UInt32 lRowStep = (ht(_finalSubDiv)-1) / (ht(_currentSubDiv)-1);
	const UInt32 lCenterIndex = _firstIndex + lRowStep/2;

	const Float* lV1 = &_pVertices[3*_firstIndex];
	const Float* lV2 = &_pVertices[3*_secondIndex];

	Float* lV = &_pVertices[3*lCenterIndex];

	lV[X] = 0.5f*(lV1[X] + lV2[X]);
	lV[Y] = 0.5f*(lV1[Y] + lV2[Y]);
	lV[Z] = 0.5f*(lV1[Z] + lV2[Z]);

	Float liLength = 1.0f/sqrtf(lV1[X] * lV[X] + lV1[Y] * lV[Y] + lV1[Z] * lV[Z]);
	lV[X] *= liLength;
	lV[Y] *= liLength;
	lV[Z] *= liLength;

	isoSphereSubDivideSegment(_firstIndex, lCenterIndex, _currentSubDiv+1, _finalSubDiv, _pVertices);
	isoSphereSubDivideSegment(lCenterIndex, _secondIndex, _currentSubDiv+1, _finalSubDiv, _pVertices);
}

static void isoSphereSubDivideCenter(
	UInt32 _currentSubDiv,
	UInt32 _currentRow,
	UInt32 _startingIndex,
	UInt32 _finalSubDiv,
	Float * _pVertices)
{
	if (_currentSubDiv >= _finalSubDiv)
		return;

	const UInt32 lCurrentSubDiv = _currentSubDiv+1;
	const UInt32 lRowStep = (ht(_finalSubDiv)-1) / (ht(lCurrentSubDiv)-1);

	const UInt32 lTopRowVertexCount = ht(_finalSubDiv);
	const UInt32 lTopRow = _currentRow - lRowStep;
	const UInt32 lTopRowIndex = in(ht(_finalSubDiv)-1, 0) + (lTopRow - ht(_finalSubDiv) + 1) * 5 *(lTopRowVertexCount-1);

	//const UInt32 lBotRow = _currentRow + lRowStep; Not used
	const UInt32 lBotRowIndex = lTopRowIndex + 2*lRowStep*5*(lTopRowVertexCount-1);

	const UInt32 lCenterRow = _currentRow;
	const UInt32 lCenterRowIndex = lTopRowIndex + lRowStep*5*(lTopRowVertexCount-1);

	Float * lTopPtr = &_pVertices[3*lTopRowIndex];
	Float * lBotPtr = &_pVertices[3*lBotRowIndex];
	Float * lCenterPtr = &_pVertices[3*lCenterRowIndex];

	const UInt32 lPointCount = 5*(lTopRowVertexCount-1)/lRowStep;
	for (UInt32 k = 0 ; k < lPointCount ; ++k)
	{
		lCenterPtr[X] = 0.5f*(lTopPtr[X] + lBotPtr[X]);
		lCenterPtr[Y] = 0.5f*(lTopPtr[Y] + lBotPtr[Y]);
		lCenterPtr[Z] = 0.5f*(lTopPtr[Z] + lBotPtr[Z]);

		Float liLength = 1.0f/sqrtf(lCenterPtr[X] * lCenterPtr[X] + lCenterPtr[Y] * lCenterPtr[Y] + lCenterPtr[Z] * lCenterPtr[Z]);
		lCenterPtr[X] *= liLength;
		lCenterPtr[Y] *= liLength;
		lCenterPtr[Z] *= liLength;

		lTopPtr += ((k+1) % 2)*3*2*lRowStep;
		lBotPtr += (k%2)*3*2*lRowStep;
		lCenterPtr += 3*lRowStep;

		if (k == lPointCount - 2)
			lTopPtr = &_pVertices[3*lTopRowIndex];
	}

	for (UInt32 k = 0 ; k < lPointCount ; ++k)
	{
		if (k < 5*(lTopRowVertexCount-1)/lRowStep - 1)
		{
			isoSphereSubDivideSegment(
				lCenterRowIndex + k*lRowStep,
				lCenterRowIndex + (k+1)*lRowStep,
				_currentSubDiv+1,
				_finalSubDiv,
				_pVertices);
		}
		else
		{
			isoSphereSubDivideSegment(
				lCenterRowIndex + k*lRowStep,
				lCenterRowIndex,
				_currentSubDiv+1,
				_finalSubDiv,
				_pVertices);
		}
	}

	isoSphereSubDivideCenter(_currentSubDiv+1, lCenterRow - lRowStep/2, 0, _finalSubDiv, _pVertices);
	isoSphereSubDivideCenter(_currentSubDiv+1, lCenterRow + lRowStep/2, 0, _finalSubDiv, _pVertices);
}

void IsoSphere::buildIsoSphere()
{
	if (m_pVertices != NULL)
		O3D_ERROR(E_InvalidPrecondition(String("Vertex array is not null")));

	if (m_pIndices != NULL)
		O3D_ERROR(E_InvalidPrecondition(String("Index array is not null")));

	if (m_radius <= 0.0f)
		O3D_ERROR(E_InvalidPrecondition(String("Invalid radius value <") << m_radius << ">"));

	if ((m_subDiv == 0) && (isHalfSphere()))
		O3D_ERROR(E_InvalidPrecondition(String("Incompatible option : subdivision = 0 and half sphere <") << m_radius << ">"));

	const UInt32 n = m_subDiv; // Subdivision de la sph�re

	UInt32 lVertexCount = 0, lIndexCount = 0;

	if (isHalfSphere())
		lVertexCount = 1+5*(1 << (n-1))*((2 << n)+1); // Nombre de vertex d'une demi sph�re V(n) = 1+5.2^(n-1)*(2^(n+1)+1)
	else
		lVertexCount = 2+10*(1 << (2*n)); // Nombre de vertex V(n) = 2+10*2^(2*n)

	if (isWired())
	{
		if (n > 0)
			lIndexCount = (isHalfSphere() ? 2 : 4)*(5*((1 << (n+1)) + 3*((1 << n) - 1)*(1 << (n-1))) + 5*3*(1 << (2*n-1))) + 5*(1 << n);
		else
			lIndexCount = 60;
	}
	else
		lIndexCount = 3*20*(1 << (n << 1)) / (isHalfSphere() ? 2 : 1); // Nombre de face F(n) = 20*4^n

	// now we work with 32 bits indices array so we are not limited at 65k
	//if ((lVertexCount > 65535) || (lIndexCount > 65535))
	//	O3D_ERROR(O3D_E_InvalidPrecondition(O3DString("Too many vertices or indices")));

	// Allocation de la m�moire
	m_verticesCount = lVertexCount;   // Nombre de vertex d'une demi sph�re V(n) = 1+5.2^(n-1)*(2^(n+1)+1)
	m_indicesCount = lIndexCount;     // Nombre de face F(n) = 20*4^n

	m_pVertices = new Float[m_verticesCount*3];
	m_pIndices = new UInt32[m_indicesCount];

	// Vertex sup�rieur d'un icosahedrone de rayon 1.
	const Float lIco[] = {
		0.0f,				1.0f,				0.0f,				
		0.0f,				0.4472135955f,		+0.894427191f,				
		+0.8506508084f,		0.4472135955f,		+0.2763932023f,		
		+0.5257311121f,		0.4472135955f,		-0.7236067977f,		
		-0.5257311121f,		0.4472135955f,		-0.7236067977f,		
		-0.8506508084f,		0.4472135955f,		+0.2763932023f		 };

	if (n > 0)
	{
		// Si la subdivision est sup�rieure � 0. On rentre dans le calcul.
		memcpy((void*)m_pVertices, (const void*)lIco, 3*sizeof(Float));

		// Vertex principaux
		for (UInt32 k = 0 ; k < 5 ; ++k)
		{
			memcpy((void*)&m_pVertices[3*(in(ht(n) - 1, k))], (const void*)&lIco[3*(k+1)], 3*sizeof(Float));
		}

		const UInt32 lCenterCount = ht(n);
		const UInt32 lCenterStep = (ht(n)-1)/(ht(1)-1);
		const UInt32 lCenterIndex = in(lCenterCount - 1, 0) + 5*lCenterStep*(lCenterCount-1);
		const Float lStep = 2.0f*3.14159265f/(5*(lCenterCount-1));
		const Float lOffset = 2.0f*3.14159265f/20.0f;

		// Vertex de l'�quateur
		for (UInt32 k = 0 ; k < 5*(lCenterCount-1) ; ++k)
		{
			Float * lPtr = &m_pVertices[3*(lCenterIndex + k)];

			lPtr[0] = sinf(lOffset + k * lStep);
			lPtr[1] = 0.0f;
			lPtr[2] = cosf(lOffset + k * lStep);
		}

		// On subdivise de facon r�cursive chaque face du pentagone.
		isoSphereSubDivide(1, 0, 0, 0, n, m_pVertices);
		isoSphereSubDivide(1, 0, 0, 1, n, m_pVertices);
		isoSphereSubDivide(1, 0, 0, 2, n, m_pVertices);
		isoSphereSubDivide(1, 0, 0, 3, n, m_pVertices);
		isoSphereSubDivide(1, 0, 0, 4, n, m_pVertices);

		// On subdivise ensuite
		const UInt32 lRowStep = (ht(n)-1)/(ht(2)-1);
		isoSphereSubDivideCenter(1, ht(n)-1 + lRowStep, 0, n, m_pVertices);

		if (!isHalfSphere())
		{
			// Calcul des vertices sur la sph�re inf�rieure
			// Le principe est ici de faire la sym�trie centrale de la sph�re sup�rieure
			const UInt32 lVertexToCompute = (m_verticesCount - 5*(ht(n)-1))/2;
			const Float * lSourcePtr = m_pVertices;
			Float * lTargetPtr = m_pVertices + 3*(m_verticesCount-1);

			for (UInt32 k = 0 ; k < lVertexToCompute ; ++k, lSourcePtr += 3, lTargetPtr -= 3)
			{
				*(lTargetPtr + 0) = - *(lSourcePtr+0);
				*(lTargetPtr + 1) = - *(lSourcePtr+1);
				*(lTargetPtr + 2) = - *(lSourcePtr+2);
			}
		}
	}
	else
	{
		// Dans le cas non subdivis�, on utilise directement les points d'un icosahedron de rayon 1.
		const Float lIcoBottom[] = {
			0.0f,				-0.4472135955f,		-0.894427191f,				
			-0.8506508084f,		-0.4472135955f,		-0.2763932023f,		
			-0.5257311121f,		-0.4472135955f,		+0.7236067977f,		
			+0.5257311121f,		-0.4472135955f,		+0.7236067977f,		
			+0.8506508084f,		-0.4472135955f,		-0.2763932023f,
			0.0f,				-1.0f,				0.0f };

		memcpy((void*)m_pVertices, (const void*)lIco, sizeof(lIco));

		if (!isHalfSphere())
			memcpy((void*)(m_pVertices + 18), (const void*)lIcoBottom, sizeof(lIco));
	}

	Float * lPtrVertex = m_pVertices;
	for (UInt32 k = 3*m_verticesCount  ; k > 0 ; --k, ++lPtrVertex)
		*lPtrVertex *= m_radius;

	if (n > 0)
	{
		// Calcul des indices
		UInt32 * lIndexPtr = m_pIndices;

		if (!isWired())
		{
			// Les indices de la partie sup�rieure de la sph�re
			for (UInt32 i = 0 ; i < UInt32(1 << n) ; ++i)
			{
				const UInt32 lBaseVertexIndex = 1 - (i == 0 ? 1 : 0) + (5*(i-1)*i)/2; // Indice du premier vertex de la rang�e i
				const UInt32 lBaseNextVertexIndex = 1 + (5*i*(i+1))/2;

				UInt32 lVertexIndex = lBaseVertexIndex;
				UInt32 lNextVertexIndex = lBaseNextVertexIndex;

				const UInt32 lSegmentCount = i;
				const UInt32 lNextSegmentCount = i+1;

				const UInt32 lTotalSegmentCount = 5 * lSegmentCount;
				const UInt32 lTotalNextSegmentCount = 5 * lNextSegmentCount;

				if (i == 0)
				{
					// 5 premiers triangles au sommet de la sph�re
					for (UInt32 k = 0 ; k < 5 ; ++k, lIndexPtr += 3)
					{
						lIndexPtr[0] = 0;
						lIndexPtr[1] = 1 + k;
						lIndexPtr[2] = 1 + (k + 1) % 5;
					}
				}
				else
				{
					for (UInt32 m = 0 ; m < 5 ; ++m)
					{
						for (UInt32 k = 0 ; k < lSegmentCount ; ++k, lIndexPtr += 6)
						{
							lIndexPtr[0] = lVertexIndex + k;
							lIndexPtr[1] = lNextVertexIndex + k;
							lIndexPtr[2] = lNextVertexIndex + k + 1;

							lIndexPtr[3] = lVertexIndex + k;
							lIndexPtr[4] = lNextVertexIndex + k + 1;
							lIndexPtr[5] = lBaseVertexIndex + (lVertexIndex + k + 1 - lBaseVertexIndex) % lTotalSegmentCount;
						}

						lVertexIndex = lVertexIndex + lSegmentCount;
						lNextVertexIndex = lNextVertexIndex + lNextSegmentCount;

						lIndexPtr[0] = lBaseVertexIndex + (lVertexIndex - lBaseVertexIndex) % lTotalSegmentCount;
						lIndexPtr[1] = lNextVertexIndex - 1;
						lIndexPtr[2] = lBaseNextVertexIndex + (lNextVertexIndex - lBaseNextVertexIndex) % lTotalNextSegmentCount;

						lIndexPtr += 3;
					}
				}
			}

			// Partie centrale de la sph�re
			const UInt32 lVertexCount = 5*(1 << n);

			const UInt32 lBaseVertexIndex = 1 + (5*((1 << n)-1)*(1 << n))/2; // Indice du premier vertex de la rang�e i

			for (UInt32 i = 0 ; i < (UInt32)(1 << (n-1)) ; ++i)
			{
				const UInt32 lVertexIndex = lBaseVertexIndex + i * lVertexCount;
				const UInt32 lNextVertexIndex = lVertexIndex + lVertexCount;

				for (UInt32 k = 0 ; k < lVertexCount ; ++k, lIndexPtr += 6)
				{
					lIndexPtr[0] = lVertexIndex + k;
					lIndexPtr[1] = lNextVertexIndex + k;
					lIndexPtr[2] = lVertexIndex + (k + 1) % lVertexCount;

					lIndexPtr[3] = lVertexIndex + (k + 1) % lVertexCount;
					lIndexPtr[4] = lNextVertexIndex + k;
					lIndexPtr[5] = lNextVertexIndex + (k + 1) % lVertexCount;
				}
			}

			if (!isHalfSphere())
			{
				// Maillage sym�trique
				const UInt32 lCenterVertexCount = ht(n);
				const UInt32 lFirstCenterIndex = in(lCenterVertexCount - 1, 0) + (ht(n)-1)/2 * 5*(lCenterVertexCount-1);
				const UInt32 lLastCenterIndex = lFirstCenterIndex + 5*(lCenterVertexCount-1) - 1;

				const UInt32 * lSourceIndex = m_pIndices;
				UInt32 * lTargetIndex = m_pIndices + m_indicesCount - 1;

				for (UInt32 k = 0 ; k < m_indicesCount / 2 ; ++k, ++lSourceIndex, --lTargetIndex)
				{
					if ((*lSourceIndex < lFirstCenterIndex) || (*lSourceIndex > lLastCenterIndex))
						*lTargetIndex = m_verticesCount - 1 - *lSourceIndex;
					else
						*lTargetIndex = lFirstCenterIndex + (*lSourceIndex + 5*(lCenterVertexCount-1)/2 - lFirstCenterIndex) % (5*(lCenterVertexCount-1));
				}
			}
		}
		else // Wireframe
		{
			memset(m_pIndices, 0, m_indicesCount*sizeof(UInt32));

			// Les indices de la partie superieure de la sphere
			for (UInt32 i = 0 ; i < UInt32(1 << n) ; ++i)
			{
				const UInt32 lBaseVertexIndex = 1 - (i == 0 ? 1 : 0) + (5*(i-1)*i)/2; // Indice du premier vertex de la rang�e i
				const UInt32 lBaseNextVertexIndex = 1 + (5*i*(i+1))/2;

				UInt32 lVertexIndex = lBaseVertexIndex;
				UInt32 lNextVertexIndex = lBaseNextVertexIndex;

				const UInt32 lTriangleCount = i;
				const UInt32 lNextTriangleCount = i+1;

				for (UInt32 m = 0 ; m < 5 ; ++m)
				{
					lIndexPtr[0] = lVertexIndex;
					lIndexPtr[1] = lNextVertexIndex;

					lIndexPtr += 2;

					for (UInt32 k = 0 ; k < lTriangleCount ; ++k, lIndexPtr += 6)
					{
						lIndexPtr[0] = lVertexIndex + k;
						lIndexPtr[1] = lNextVertexIndex + k + 1;

						lIndexPtr[2] = lNextVertexIndex + k + 1;
						lIndexPtr[3] = ((m == 4) && (k == lTriangleCount-1) ? lBaseVertexIndex : lVertexIndex + k + 1);

						lIndexPtr[4] = lVertexIndex + k;
						lIndexPtr[5] = ((m == 4) && (k == lTriangleCount-1) ? lBaseVertexIndex : lVertexIndex + k + 1);
					}

					lVertexIndex += lTriangleCount;
					lNextVertexIndex += lNextTriangleCount;
				}
			}

			// Partie centrale de la sph�re
			const UInt32 lVertexCount = 5*(1 << n);

			const UInt32 lBaseVertexIndex = 1 + (5*((1 << n)-1)*(1 << n))/2; // Indice du premier vertex de la rang�e i

			for (UInt32 i = 0 ; i < (UInt32)(1 << (n-1)) ; ++i)
			{
				const UInt32 lVertexIndex = lBaseVertexIndex + i * lVertexCount;
				const UInt32 lNextVertexIndex = lVertexIndex + lVertexCount;

				for (UInt32 k = 0 ; k < lVertexCount ; ++k, lIndexPtr += 6)
				{
					lIndexPtr[0] = lVertexIndex + k;
					lIndexPtr[1] = lNextVertexIndex + k;

					lIndexPtr[2] = lNextVertexIndex + k;
					lIndexPtr[3] = lVertexIndex + (k + 1) % lVertexCount;

					lIndexPtr[4] = lVertexIndex + k;
					lIndexPtr[5] = lVertexIndex + (k + 1) % lVertexCount;
				}
			}

			const UInt32 lCenterVertexIndex = lBaseVertexIndex + (1 << (n-1)) * lVertexCount;
			const UInt32 lCenterVertexCount = 5*(1 << n);

			for (UInt32 i = 0 ; i < lCenterVertexCount ; ++i, lIndexPtr += 2)
			{
				lIndexPtr[0] = lCenterVertexIndex + i;
				lIndexPtr[1] = lCenterVertexIndex + (i + 1) % lCenterVertexCount;
			}

			if (!isHalfSphere())
			{
				const UInt32 lIndexToInvert = 2*(5*((1 << (n+1)) + 3*((1 << n) - 1)*(1 << (n-1))) + 5*3*(1 << (2*n-1)));
				const UInt32 lCenterVertexCount = ht(n);
				const UInt32 lFirstCenterIndex = in(lCenterVertexCount - 1, 0) + (ht(n)-1)/2 * 5*(lCenterVertexCount-1);
				const UInt32 lLastCenterIndex = lFirstCenterIndex + 5*(lCenterVertexCount-1) - 1;

				const UInt32 * lSourceIndex = m_pIndices;
				UInt32 * lTargetIndex = m_pIndices + m_indicesCount - 1;

				for (UInt32 k = 0 ; k < lIndexToInvert ; ++k, ++lSourceIndex, --lTargetIndex)
				{
					if ((*lSourceIndex < lFirstCenterIndex) || (*lSourceIndex > lLastCenterIndex))
						*lTargetIndex = m_verticesCount - 1 - *lSourceIndex;
					else
						*lTargetIndex = lFirstCenterIndex + (*lSourceIndex + 5*(lCenterVertexCount-1)/2 - lFirstCenterIndex) % (5*(lCenterVertexCount-1));
				}
			}
		}
	}
	else
	{
		if (isWired())
		{
			const UInt32 lIcoIndices[60] = {
			0, 1,	0, 2,	 0, 3,	0, 4,	0, 5,
			1, 2,	2, 3,	 3, 4,	4, 5,	5, 1,

			1, 8,	1, 9,	 2, 9,	2, 10,	3, 10,
			3, 6,	4, 6,	 4, 7,	5, 7,	5, 8,

			11, 6,	11, 7,	 11, 8,	11, 9,	11, 10,
			6, 7,	7, 8,	 8, 9,	9, 10,	10, 6};

			memcpy((void*)m_pIndices, (const void*)lIcoIndices, sizeof(lIcoIndices));
		}
		else
		{
			const UInt32 lIcoIndices[60] = {
			0, 1, 2,	0, 2, 3,	0, 3, 4,	0, 4, 5,	0, 5, 1,

			1, 9, 2,	2, 9, 10,	3, 2, 10,	3, 10, 6,	6, 4, 3,
			6, 7, 4,	4, 7, 5,	5, 7, 8,	8, 1, 5,	1, 8, 9,

			11, 7, 6,	11, 8, 7,	11, 9, 8,	11, 10, 9,	11, 6, 10 };

			memcpy((void*)m_pIndices, (const void*)lIcoIndices, sizeof(lIcoIndices));
		}
	}
}

