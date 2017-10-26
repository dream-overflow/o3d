/**
 * @file vector2.cpp
 * @brief 
 * @author Emmanuel Ruffio
 * @date 2006-08-08
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/vector2.h"

using namespace o3d;

// Null vector (static)

Vector2d  Vector::nullVector2d(0, 0);
Vector2f  Vector::nullVector2f(0.f, 0.f);
Vector2i  Vector::nullVector2i(0, 0);
Vector2ui Vector::nullVector2ui(0, 0);
Vector2s  Vector::nullVector2s(0, 0);
Vector2us Vector::nullVector2us(0, 0);
Vector2b  Vector::nullVector2b(0, 0);
Vector2ub Vector::nullVector2ub(0, 0);

// Specializations

template <>
inline Bool Vector2<Float>::operator!= (const Vector2<Float>& vec)const
{
	if (fabs(V[X] - vec[X]) > o3d::Limits<Float>::epsilon()) return True;
	if (fabs(V[Y] - vec[Y]) > o3d::Limits<Float>::epsilon()) return True;
	return False;
}

template <>
inline Bool Vector2<Float>::operator== (const Vector2<Float>& vec)const
{
	if (fabs(V[X] - vec[X]) > o3d::Limits<Float>::epsilon()) return False;
	if (fabs(V[Y] - vec[Y]) > o3d::Limits<Float>::epsilon()) return False;
	return True;
}

template <>
inline Bool Vector2<Double>::operator!= (const Vector2<Double>& vec)const
{
	if (fabs(V[X] - vec[X]) > o3d::Limits<Float>::epsilon()) return True;
	if (fabs(V[Y] - vec[Y]) > o3d::Limits<Float>::epsilon()) return True;
	return False;
}

template <>
inline Bool Vector2<Double>::operator== (const Vector2<Double>& vec)const
{
	if (fabs(V[X] - vec[X]) > o3d::Limits<Float>::epsilon()) return False;
	if (fabs(V[Y] - vec[Y]) > o3d::Limits<Float>::epsilon()) return False;
	return True;
}

