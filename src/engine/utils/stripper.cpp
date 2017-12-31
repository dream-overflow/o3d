/**
 * @file stripper.cpp
 * @brief Primitive stripper. Transform triangles into optimized triangles or triangle strips.
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2007-07-14
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/utils/stripper.h"

#undef max
#undef min

#ifdef __STL_DEBUG
	#define DISABLE_TRI_STRIPPER
#endif

#include "../../../third/nvTriStrip/NvTriStrip.h"

#ifndef DISABLE_TRI_STRIPPER
#include "../../../third/TriStripper/Include/tri_stripper.h"
using namespace triangle_stripper;
#endif

using namespace o3d;

Stripper::Stripper(StripperType _type):
	m_stripper(_type),
	m_useCache(False),
	m_cacheSize(STRIPER_DEFAULT_CACHE_SIZE),
	m_errorString()
{
}

Stripper::~Stripper()
{
}

Bool Stripper::start(T_Ui16Index _array, ResultType _type, T_Ui16ListArray & _target, T_Ui16Index * _triangles)
{
	// Verification de la taille du tableau
	if ((_array.size() / 3) * 3 != _array.size())
	{
		m_errorString = String("The array of faces has not a valid size");
		return False;
	}

	if (_array.size() == 0)
	{
		m_errorString = String("The array of faces is empty");
		return False;
	}

	switch(getStripper(_type))
	{
	case STRIPPER_NVTRISTRIP:
		{
			SetCacheSize(m_cacheSize);
			SetStitchStrips(_type == RESULT_SINGLE_TRIANGLE_STRIP);
			SetListsOnly(_type == RESULT_OPTIMIZED_TRIANGLES);

            PrimitiveGroup * pPrimGroup = nullptr;
			UInt16 primGroupCount = 0;

			if (!GenerateStrips((const UInt16*)&_array[0], (const unsigned int)_array.size(), &pPrimGroup, &primGroupCount))
			{
				m_errorString = String("Unknown error during NvTriStrip computation");
				deleteArray(pPrimGroup);
				return False;
			}

			_target.reserve(primGroupCount);

			std::vector<UInt16> triangleList;

			for (UInt32 k = 0 ; k < primGroupCount ; ++k)
			{
				if (_type == RESULT_OPTIMIZED_TRIANGLES)
				{
					O3D_ASSERT((pPrimGroup[k].numIndices/3)*3 == pPrimGroup[k].numIndices);
					_target.push_back(T_Ui16Index());
					_target.back().insert(_target.back().end(), pPrimGroup[k].indices, pPrimGroup[k].indices + pPrimGroup[k].numIndices);
				}
				else if (pPrimGroup[k].type == PT_STRIP)
				{
					_target.push_back(T_Ui16Index());
					_target.back().insert(_target.back().end(), pPrimGroup[k].indices, pPrimGroup[k].indices + pPrimGroup[k].numIndices);
				}
				else if (pPrimGroup[k].type == PT_LIST)
				{
					O3D_ASSERT((pPrimGroup[k].numIndices/3)*3 == pPrimGroup[k].numIndices);
					triangleList.insert(triangleList.end(), pPrimGroup[k].indices, pPrimGroup[k].indices + pPrimGroup[k].numIndices);
				}
			}

			if ((_type != RESULT_OPTIMIZED_TRIANGLES) && (triangleList.size() > 0))
			{
				// Si des triangles n'ont pas ete strippe, suivant le résultat souhaite, on les strippent
				// brutalement dans une nouvelle liste ou pas.
				if (_type == RESULT_TRIANGLE_STRIPS)
					_target.push_back(T_Ui16Index());
				else
					_target.back().push_back(_target.back().back());

				T_Ui16Index & lastList = _target.back();

				for (UInt32 k = 0 ; k < UInt32(triangleList.size()/3) ; ++k)
				{
					lastList.push_back(triangleList[3*k+0]);
					lastList.push_back(triangleList[3*k+0]);
					lastList.push_back(triangleList[3*k+1]);
					lastList.push_back(triangleList[3*k+2]);
					lastList.push_back(triangleList[3*k+2]);
				}
			}
			else
			{
				O3D_ASSERT(triangleList.size() == 0);
			}

			deleteArray(pPrimGroup);

			return True;
		}
		break;
	case STRIPPER_TRISTRIPPER:
		{
			if (_type == RESULT_OPTIMIZED_TRIANGLES)
			{
				m_errorString = String("Optimized triangles are not compatible with TriStripper");
				return False;
			}
#ifndef DISABLE_TRI_STRIPPER
			indices listIndices(_array.size());

			indices::iterator itTarget = listIndices.begin();

			for (T_Ui16Index::iterator it = _array.begin() ; it != _array.end() ; it++, itTarget++)
				*itTarget = UInt32(*it);

			tri_stripper stripper(listIndices);

			stripper.SetCacheSize(m_useCache ? m_cacheSize : 0);
			//stripper.SetBackwardSearch(!m_useCache);
			stripper.SetPushCacheHits(False);

			primitive_vector primitiveResult;
			stripper.Strip(&primitiveResult);

			T_Ui16Index triangleList;

			for (UInt32 k = 0 ; k < UInt32(primitiveResult.size()) ; ++k)
			{
				primitive_group & group = primitiveResult[k];

				if (_type == RESULT_SINGLE_TRIANGLE_STRIP)
				{
					if (group.Type == TRIANGLES)
					{
						triangleList.reserve(triangleList.size() + group.Indices.size());

						for (indices::iterator it = group.Indices.begin() ; it != group.Indices.end() ; it++)
							triangleList.push_back(UInt16(*it));
					}
					else
					{
						if (_target.size() > 0)
						{
							_target.back().push_back(_target.back().back());
							_target.back().push_back(UInt16(group.Indices.front()));
						}
						else
							_target.push_back(T_Ui16Index());

						_target.back().reserve(_target.size() + group.Indices.size());

						for (indices::iterator it = group.Indices.begin() ; it != group.Indices.end() ; it++)
							_target.back().push_back(UInt16(*it));
					}
				}
				else
				{
					_target.push_back(T_Ui16Index());

					_target.back().reserve(_target.size() + group.Indices.size());

					for (indices::iterator it = group.Indices.begin() ; it != group.Indices.end() ; it++)
						_target.back().push_back(UInt16(*it));
				}
			}

			if (triangleList.size() > 0)
			{
                if (_triangles == nullptr)
				{
					if (_type == RESULT_SINGLE_TRIANGLE_STRIP)
						_target.back().push_back(_target.back().back());
					else
						_target.push_back(T_Ui16Index());

					T_Ui16Index & localList = _target.back();

					for (UInt32 k = 0 ; k < triangleList.size()/3 ; ++k)
					{
						localList.push_back(triangleList[3*k + 0]);
						localList.push_back(triangleList[3*k + 0]);
						localList.push_back(triangleList[3*k + 1]);
						localList.push_back(triangleList[3*k + 2]);
						localList.push_back(triangleList[3*k + 2]);
					}
				}
				else
					*_triangles = triangleList;
			}
#else
		m_errorString = String("Invalid striper");
		return False;
#endif
		}
		break;
	default:
		m_errorString = String("Invalid striper");
		return False;
	}

	return True;
}

Bool Stripper::start(T_Ui32Index _array, ResultType _type, T_Ui32ListArray & _target, T_Ui32Index * _triangles)
{
	// Verification de la taille du tableau
	if ((_array.size() / 3) * 3 != _array.size())
	{
		m_errorString = String("The array of faces has not a valid size");
		return False;
	}

	if (_array.size() == 0)
	{
		m_errorString = String("The array of faces is empty");
		return False;
	}

	switch(getStripper(_type))
	{
	case STRIPPER_NVTRISTRIP:
		{
			m_errorString = String("UInt indices is not supported by NvTriStrip");
			return False;
		}
		break;
	case STRIPPER_TRISTRIPPER:
		{
			if (_type == RESULT_OPTIMIZED_TRIANGLES)
			{
				m_errorString = String("Optimized triangles are not compatible with TriStripper");
				return False;
			}

#ifndef DISABLE_TRI_STRIPPER
			indices listIndices(_array.size());

			indices::iterator itTarget = listIndices.begin();

			for (T_Ui32Index::iterator it = _array.begin() ; it != _array.end() ; it++, itTarget++)
				*itTarget = UInt32(*it);

			tri_stripper stripper(listIndices);

			stripper.SetCacheSize(m_useCache ? m_cacheSize : 0);
			//stripper.SetBackwardSearch(!m_useCache);
			stripper.SetPushCacheHits(False);

			primitive_vector primitiveResult;
			stripper.Strip(&primitiveResult);

			T_Ui32Index triangleList;

			for (UInt32 k = 0 ; k < UInt32(primitiveResult.size()) ; ++k)
			{
				primitive_group & group = primitiveResult[k];

				if (_type == RESULT_SINGLE_TRIANGLE_STRIP)
				{
					if (group.Type == TRIANGLES)
					{
						triangleList.reserve(triangleList.size() + group.Indices.size());

						for (indices::iterator it = group.Indices.begin() ; it != group.Indices.end() ; it++)
							triangleList.push_back(UInt32(*it));
					}
					else
					{
						if (_target.size() > 0)
						{
							_target.back().push_back(_target.back().back());
							_target.back().push_back(UInt32(group.Indices.front()));
						}
						else
							_target.push_back(T_Ui32Index());

						_target.back().reserve(_target.size() + group.Indices.size());

						for (indices::iterator it = group.Indices.begin() ; it != group.Indices.end() ; it++)
							_target.back().push_back(UInt32(*it));
					}
				}
				else
				{
					_target.push_back(T_Ui32Index());

					_target.back().reserve(_target.size() + group.Indices.size());

					for (indices::iterator it = group.Indices.begin() ; it != group.Indices.end() ; it++)
						_target.back().push_back(UInt32(*it));
				}
			}

			if (triangleList.size() > 0)
			{
                if (_triangles == nullptr)
				{
					if (_type == RESULT_SINGLE_TRIANGLE_STRIP)
						_target.back().push_back(_target.back().back());
					else
						_target.push_back(T_Ui32Index());

					T_Ui32Index & localList = _target.back();

					for (UInt32 k = 0 ; k < triangleList.size()/3 ; ++k)
					{
						localList.push_back(triangleList[3*k + 0]);
						localList.push_back(triangleList[3*k + 0]);
						localList.push_back(triangleList[3*k + 1]);
						localList.push_back(triangleList[3*k + 2]);
						localList.push_back(triangleList[3*k + 2]);
					}
				}
				else
					*_triangles = triangleList;
			}
#else
        m_errorString = String("Invalid stripper");
		return False;
#endif
		}
		break;
	default:
        m_errorString = String("Invalid stripper");
		return False;
	}

	return True;
}

// Return the stripper which will be used for computation given the current configs.
Stripper::StripperType Stripper::getStripper(ResultType _type) const
{
	if (m_stripper != STRIPPER_AUTO)
		return m_stripper;

	if ((_type == RESULT_OPTIMIZED_TRIANGLES) && (!m_useCache))
		return STRIPPER_INVALID;

	if (_type == RESULT_OPTIMIZED_TRIANGLES)
		return STRIPPER_NVTRISTRIP;

	if (_type != RESULT_OPTIMIZED_TRIANGLES)
		return STRIPPER_TRISTRIPPER;

	return STRIPPER_INVALID;
}

