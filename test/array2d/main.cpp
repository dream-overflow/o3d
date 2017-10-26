//---------------------------------------------------------------------------------------
//! @file main.cpp
//! Main entry of the O3DTemplateArray2d test.
//! @date 2004-01-01
//! @author Frederic SCHERMA
//-------------------------------------------------------------------------------------
//- Objective-3D Copyright (C) 2001-2009 Revolutioning Entertainment
//- mailto:contact@revolutioning.com
//- http://o3d.revolutioning.com
//-------------------------------------------------------------------------------------
// This file is part of Objective-3D.
// Objective-3D is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Objective-3D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Objective-3D.  If not, see <http://www.gnu.org/licenses/>.
//---------------------------------------------------------------------------------------

#include <o3d/core/TemplateArray2d.h>

#include <iostream>
#include <iomanip>

#include <o3d/geom/BSphere.h>
#include <o3d/geom/BCone.h>
#include <o3d/geom/AABBox.h>
#include <o3d/geom/Plane.h>

void TestBoundingConeClip()
{
	Geometry::Clipping lResult;
	const O3D_FLOAT eps = o3d::Limits<O3D_FLOAT>::epsilon();

	// =========================== Test avec une sph�re ==============================
	BSphere lSphere(Vector3(4.0f, 4.0f, 0.0f), 4.0f);

	// Cone plat
	lResult = O3DBCone(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), 1.0f, o3d::PI/4, O3D_FALSE).Clip(lSphere);
	O3D_ASSERT(lResult == Geometry::CLIP_OUTSIDE);
	lResult = O3DBCone(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), 2.0f, o3d::PI/4, O3D_FALSE).Clip(lSphere);
	O3D_ASSERT(lResult == Geometry::CLIP_INTERSECT);
	lResult = O3DBCone(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), 1.0f, o3d::PI/3, O3D_FALSE).Clip(lSphere);
	O3D_ASSERT(lResult == Geometry::CLIP_INTERSECT);

	lResult = O3DBCone(Vector3(0.0f, 0.0f, 0.0f), ! Vector3(1.0f, 1.0f, 0.0f), sqrt(16.0f+16.0f)-4.0f - eps, o3d::PI/4, O3D_FALSE).Clip(lSphere);
	O3D_ASSERT(lResult == Geometry::CLIP_OUTSIDE);
	lResult = O3DBCone(Vector3(0.0f, 0.0f, 0.0f), ! Vector3(1.0f, 1.0f, 0.0f), sqrt(16.0f+16.0f)-4.0f + eps, o3d::PI/4, O3D_FALSE).Clip(lSphere);
	O3D_ASSERT(lResult == Geometry::CLIP_INTERSECT);

	lResult = O3DBCone(Vector3(1.0f, 1.0f, 0.0f), Vector3(0.0f, -1.0f, 0.0f), 1.0f, o3d::PI/4, O3D_FALSE).Clip(lSphere);
	O3D_ASSERT(lResult == Geometry::CLIP_OUTSIDE);
	lResult = O3DBCone(Vector3(1.0f, 2.0f, 0.0f), Vector3(0.0f, -1.0f, 0.0f), 1.0f, o3d::PI/4, O3D_FALSE).Clip(lSphere);
	O3D_ASSERT(lResult == Geometry::CLIP_INTERSECT);

	lResult = O3DBCone(Vector3(0.0f, 0.0f, 0.0f), ! Vector3(1.0f, 1.0f, 0.0f), sqrt(16.0f+16.0f) + 4.0f + eps, o3d::PI/4+eps, O3D_FALSE).Clip(lSphere);
	O3D_ASSERT(lResult == Geometry::CLIP_INSIDE);
	lResult = O3DBCone(Vector3(0.0f, 0.0f, 0.0f), ! Vector3(1.0f, 1.0f, 0.0f), sqrt(16.0f+16.0f) + 4.0f + eps, o3d::PI/4-eps, O3D_FALSE).Clip(lSphere);
	O3D_ASSERT(lResult == Geometry::CLIP_INTERSECT);

	// Cone sph�rique
	lResult = O3DBCone(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), 1.0f, o3d::PI/4, O3D_TRUE).Clip(lSphere);
	O3D_ASSERT(lResult == Geometry::CLIP_OUTSIDE);
	lResult = O3DBCone(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), 2.0f, o3d::PI/4, O3D_TRUE).Clip(lSphere);
	O3D_ASSERT(lResult == Geometry::CLIP_INTERSECT);

	lResult = O3DBCone(Vector3(0.0f, 0.0f, 0.0f), ! Vector3(1.0f, 1.0f, 0.0f), sqrt(16.0f+16.0f)-4.0f - eps, o3d::PI/4, O3D_TRUE).Clip(lSphere);
	O3D_ASSERT(lResult == Geometry::CLIP_OUTSIDE);
	lResult = O3DBCone(Vector3(0.0f, 0.0f, 0.0f), ! Vector3(1.0f, 1.0f, 0.0f), sqrt(16.0f+16.0f)-4.0f - eps, o3d::PI/2, O3D_TRUE).Clip(lSphere);
	O3D_ASSERT(lResult == Geometry::CLIP_OUTSIDE);
	lResult = O3DBCone(Vector3(0.0f, 0.0f, 0.0f), ! Vector3(1.0f, 1.0f, 0.0f), sqrt(16.0f+16.0f)-4.0f + eps, o3d::PI/2, O3D_TRUE).Clip(lSphere);
	O3D_ASSERT(lResult == Geometry::CLIP_INTERSECT);

	lResult = O3DBCone(Vector3(1.0f, 1.0f, 0.0f), Vector3(0.0f, -1.0f, 0.0f), 1.0f, o3d::PI/4, O3D_TRUE).Clip(lSphere);
	O3D_ASSERT(lResult == Geometry::CLIP_OUTSIDE);
	lResult = O3DBCone(Vector3(1.0f, 2.0f, 0.0f), Vector3(0.0f, -1.0f, 0.0f), 1.0f, o3d::PI/4, O3D_TRUE).Clip(lSphere);
	O3D_ASSERT(lResult == Geometry::CLIP_INTERSECT);

	lResult = O3DBCone(Vector3(0.0f, 0.0f, 0.0f), ! Vector3(1.0f, 1.0f, 0.0f), sqrt(16.0f+16.0f) + 4.0f + eps, o3d::PI/4+eps, O3D_TRUE).Clip(lSphere);
	O3D_ASSERT(lResult == Geometry::CLIP_INSIDE);
	lResult = O3DBCone(Vector3(0.0f, 0.0f, 0.0f), ! Vector3(1.0f, 1.0f, 0.0f), sqrt(16.0f+16.0f) + 4.0f + eps, o3d::PI/4-eps, O3D_TRUE).Clip(lSphere);
	O3D_ASSERT(lResult == Geometry::CLIP_INTERSECT);

	lResult = O3DBCone(Vector3(1.0f, 1.0f, 0.0f), ! Vector3(-1.0f, -1.0f, 0.0f), 2.0f, o3d::PI/2, O3D_TRUE).Clip(lSphere);
	O3D_ASSERT(lResult == Geometry::CLIP_OUTSIDE);
	lResult = O3DBCone(Vector3(1.0f, 1.0f, 0.0f), ! Vector3(-1.0f, -1.0f, 0.0f), 2.0f, 0.75f*o3d::PI, O3D_TRUE).Clip(lSphere);
	O3D_ASSERT(lResult == Geometry::CLIP_INTERSECT);

	// ============================== Test avec un plan ==============================
	Plane lPlane(Vector3(1.0f, 0.0f, 0.0f), Vector3(2.0f, 0.0f, 0.0f));

	// Cone plat
	lResult = O3DBCone(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), 2.0f - eps, o3d::PI/4, O3D_FALSE).Clip(lPlane);
	O3D_ASSERT(lResult == Geometry::CLIP_OUTSIDE);
	lResult = O3DBCone(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), 2.0f + eps, o3d::PI/4, O3D_FALSE).Clip(lPlane);
	O3D_ASSERT(lResult == Geometry::CLIP_INTERSECT);
	lResult = O3DBCone(Vector3(4.0f, 0.0f, 0.0f), Vector3(-1.0f, 0.0f, 0.0f), 2.0f - eps, o3d::PI/4, O3D_FALSE).Clip(lPlane);
	O3D_ASSERT(lResult == Geometry::CLIP_OUTSIDE);
	lResult = O3DBCone(Vector3(4.0f, 0.0f, 0.0f), Vector3(-1.0f, 0.0f, 0.0f), 2.0f + eps, o3d::PI/4, O3D_FALSE).Clip(lPlane);
	O3D_ASSERT(lResult == Geometry::CLIP_INTERSECT);

	lResult = O3DBCone(Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), 1.0f, o3d::PI/4-eps, O3D_FALSE).Clip(lPlane);
	O3D_ASSERT(lResult == Geometry::CLIP_OUTSIDE);
	lResult = O3DBCone(Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), 1.0f, o3d::PI/4+eps, O3D_FALSE).Clip(lPlane);
	O3D_ASSERT(lResult == Geometry::CLIP_INTERSECT);
	lResult = O3DBCone(Vector3(3.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), 1.0f, o3d::PI/4-eps, O3D_FALSE).Clip(lPlane);
	O3D_ASSERT(lResult == Geometry::CLIP_OUTSIDE);
	lResult = O3DBCone(Vector3(3.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), 1.0f, o3d::PI/4+eps, O3D_FALSE).Clip(lPlane);
	O3D_ASSERT(lResult == Geometry::CLIP_INTERSECT);

	lResult = O3DBCone(Vector3(2.0f-eps, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), 1.0f, o3d::PI/4, O3D_FALSE).Clip(lPlane);
	O3D_ASSERT(lResult == Geometry::CLIP_INTERSECT);
	lResult = O3DBCone(Vector3(2.0f+eps, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), 1.0f, o3d::PI/4, O3D_FALSE).Clip(lPlane);
	O3D_ASSERT(lResult == Geometry::CLIP_OUTSIDE);

	// Cone sph�rique
	lResult = O3DBCone(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), 2.0f - eps, o3d::PI/4, O3D_TRUE).Clip(lPlane);
	O3D_ASSERT(lResult == Geometry::CLIP_OUTSIDE);
	lResult = O3DBCone(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), 2.0f + eps, o3d::PI/4, O3D_TRUE).Clip(lPlane);
	O3D_ASSERT(lResult == Geometry::CLIP_INTERSECT);

	lResult = O3DBCone(Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), sqrt(2.0f), o3d::PI/4-eps, O3D_TRUE).Clip(lPlane);
	O3D_ASSERT(lResult == Geometry::CLIP_OUTSIDE);
	lResult = O3DBCone(Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), sqrt(2.0f), o3d::PI/4+eps, O3D_TRUE).Clip(lPlane);
	O3D_ASSERT(lResult == Geometry::CLIP_INTERSECT);
	lResult = O3DBCone(Vector3(3.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), sqrt(2.0f), o3d::PI/4-eps, O3D_TRUE).Clip(lPlane);
	O3D_ASSERT(lResult == Geometry::CLIP_OUTSIDE);
	lResult = O3DBCone(Vector3(3.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), sqrt(2.0f), o3d::PI/4+eps, O3D_TRUE).Clip(lPlane);
	O3D_ASSERT(lResult == Geometry::CLIP_INTERSECT);

	lResult = O3DBCone(Vector3(2.0f-eps, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), 1.0f, o3d::PI/4, O3D_TRUE).Clip(lPlane);
	O3D_ASSERT(lResult == Geometry::CLIP_INTERSECT);
	lResult = O3DBCone(Vector3(2.0f+eps, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), 1.0f, o3d::PI/4, O3D_TRUE).Clip(lPlane);
	O3D_ASSERT(lResult == Geometry::CLIP_OUTSIDE);
	lResult = O3DBCone(Vector3(2.0f+eps, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), 1.0f, o3d::PI/2, O3D_TRUE).Clip(lPlane);
	O3D_ASSERT(lResult == Geometry::CLIP_OUTSIDE);
	lResult = O3DBCone(Vector3(2.0f+eps, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), 1.0f, o3d::PI/2+eps, O3D_TRUE).Clip(lPlane);
	O3D_ASSERT(lResult == Geometry::CLIP_INTERSECT);

	// ============================== Test avec une box ==============================
	O3DAABBox lBox(Vector3(), Vector3(2.0f, 2.0f, 2.0f));

	// Cone plat
	lResult = O3DBCone(Vector3(-3.0f - eps, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), 1.0f, o3d::PI/4, O3D_FALSE).Clip(lBox);
	O3D_ASSERT(lResult == Geometry::CLIP_OUTSIDE);
	lResult = O3DBCone(Vector3(-3.0f + eps, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), 1.0f, o3d::PI/4, O3D_FALSE).Clip(lBox);
	O3D_ASSERT(lResult == Geometry::CLIP_INTERSECT);
	lResult = O3DBCone(Vector3( 2.0f - eps, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), 1.0f, o3d::PI/4, O3D_FALSE).Clip(lBox);
	O3D_ASSERT(lResult == Geometry::CLIP_INTERSECT);
	lResult = O3DBCone(Vector3( 2.0f + eps, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), 1.0f, o3d::PI/4, O3D_FALSE).Clip(lBox);
	O3D_ASSERT(lResult == Geometry::CLIP_OUTSIDE);

	lResult = O3DBCone(Vector3( 3.0f + eps, 0.0f, 0.0f), Vector3(-1.0f, 0.0f, 0.0f), 1.0f, o3d::PI/4, O3D_FALSE).Clip(lBox);
	O3D_ASSERT(lResult == Geometry::CLIP_OUTSIDE);
	lResult = O3DBCone(Vector3( 3.0f - eps, 0.0f, 0.0f), Vector3(-1.0f, 0.0f, 0.0f), 1.0f, o3d::PI/4, O3D_FALSE).Clip(lBox);
	O3D_ASSERT(lResult == Geometry::CLIP_INTERSECT);
	lResult = O3DBCone(Vector3(-2.0f + eps, 0.0f, 0.0f), Vector3(-1.0f, 0.0f, 0.0f), 1.0f, o3d::PI/4, O3D_FALSE).Clip(lBox);
	O3D_ASSERT(lResult == Geometry::CLIP_INTERSECT);
	lResult = O3DBCone(Vector3(-2.0f - eps, 0.0f, 0.0f), Vector3(-1.0f, 0.0f, 0.0f), 1.0f, o3d::PI/4, O3D_FALSE).Clip(lBox);
	O3D_ASSERT(lResult == Geometry::CLIP_OUTSIDE);

	lResult = O3DBCone(Vector3( 3.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), 1.0f, o3d::PI/4-eps, O3D_FALSE).Clip(lBox);
	O3D_ASSERT(lResult == Geometry::CLIP_OUTSIDE);
	lResult = O3DBCone(Vector3( 3.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), 1.0f, o3d::PI/4+eps, O3D_FALSE).Clip(lBox);
	O3D_ASSERT(lResult == Geometry::CLIP_INTERSECT);

	lResult = O3DBCone(Vector3(4.0f, 0.0f, 0.0f), Vector3(-1.0f, 0.0f, 0.0f), 6.0f + eps, atan(sqrt(8.0f)/2.0f)+eps, O3D_FALSE).Clip(lBox);
	O3D_ASSERT(lResult == Geometry::CLIP_INSIDE);
	lResult = O3DBCone(Vector3(4.0f, 0.0f, 0.0f), Vector3(-1.0f, 0.0f, 0.0f), 6.0f + eps, atan(sqrt(8.0f)/2.0f)-eps, O3D_FALSE).Clip(lBox);
	O3D_ASSERT(lResult == Geometry::CLIP_INTERSECT);
	lResult = O3DBCone(Vector3(4.0f, 0.0f, 0.0f), Vector3(-1.0f, 0.0f, 0.0f), 6.0f - eps, atan(sqrt(8.0f)/2.0f)+eps, O3D_FALSE).Clip(lBox);
	O3D_ASSERT(lResult == Geometry::CLIP_INTERSECT);

	// Cone sph�rique
	lResult = O3DBCone(Vector3(-3.0f - eps, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), 1.0f, o3d::PI/4, O3D_TRUE).Clip(lBox);
	O3D_ASSERT(lResult == Geometry::CLIP_OUTSIDE);
	lResult = O3DBCone(Vector3(-3.0f + eps, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), 1.0f, o3d::PI/4, O3D_TRUE).Clip(lBox);
	O3D_ASSERT(lResult == Geometry::CLIP_INTERSECT);
	lResult = O3DBCone(Vector3( 2.0f - eps, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), 1.0f, o3d::PI/4, O3D_TRUE).Clip(lBox);
	O3D_ASSERT(lResult == Geometry::CLIP_INTERSECT);
	lResult = O3DBCone(Vector3( 2.0f + eps, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), 1.0f, o3d::PI/4, O3D_TRUE).Clip(lBox);
	O3D_ASSERT(lResult == Geometry::CLIP_OUTSIDE);

	lResult = O3DBCone(Vector3( 3.0f + eps, 0.0f, 0.0f), Vector3(-1.0f, 0.0f, 0.0f), 1.0f, o3d::PI/4, O3D_TRUE).Clip(lBox);
	O3D_ASSERT(lResult == Geometry::CLIP_OUTSIDE);
	lResult = O3DBCone(Vector3( 3.0f - eps, 0.0f, 0.0f), Vector3(-1.0f, 0.0f, 0.0f), 1.0f, o3d::PI/4, O3D_TRUE).Clip(lBox);
	O3D_ASSERT(lResult == Geometry::CLIP_INTERSECT);
	lResult = O3DBCone(Vector3(-2.0f + eps, 0.0f, 0.0f), Vector3(-1.0f, 0.0f, 0.0f), 1.0f, o3d::PI/4, O3D_TRUE).Clip(lBox);
	O3D_ASSERT(lResult == Geometry::CLIP_INTERSECT);
	lResult = O3DBCone(Vector3(-2.0f - eps, 0.0f, 0.0f), Vector3(-1.0f, 0.0f, 0.0f), 1.0f, o3d::PI/4, O3D_TRUE).Clip(lBox);
	O3D_ASSERT(lResult == Geometry::CLIP_OUTSIDE);

	lResult = O3DBCone(Vector3( 2.0f + 2.0f*eps, 0.0f, 0.0f), Vector3( 1.0f, 0.0f, 0.0f), 1.0f, o3d::PI/2, O3D_TRUE).Clip(lBox);
	O3D_ASSERT(lResult == Geometry::CLIP_OUTSIDE);
	lResult = O3DBCone(Vector3( 2.0f + 2.0f*eps, 0.0f, 0.0f), Vector3( 1.0f, 0.0f, 0.0f), 1.0f, o3d::PI/2+4.0f*eps, O3D_TRUE).Clip(lBox);
	O3D_ASSERT(lResult == Geometry::CLIP_INTERSECT);
	lResult = O3DBCone(Vector3(-2.0f - 2.0f*eps, 0.0f, 0.0f), Vector3(-1.0f, 0.0f, 0.0f), 1.0f, o3d::PI/2, O3D_TRUE).Clip(lBox);
	O3D_ASSERT(lResult == Geometry::CLIP_OUTSIDE);
	lResult = O3DBCone(Vector3(-2.0f - 2.0f*eps, 0.0f, 0.0f), Vector3(-1.0f, 0.0f, 0.0f), 1.0f, o3d::PI/2+4.0f*eps, O3D_TRUE).Clip(lBox);
	O3D_ASSERT(lResult == Geometry::CLIP_INTERSECT);
};

int main(int argc, char * argv[])
{
	TestBoundingConeClip();

	return 0;


	O3DArray2Di baseArray(400, 400);

	int buffer[] = {	0, 1, 2, 3,
						4, 5, 6, 7,
						8, 9, 10, 11,
						12, 13, 14, 15};

	memcpy((void*)&baseArray[0], (const void*)buffer, 4*4*sizeof(O3D_INT32));

	// Copy
	O3DArray2Di test0 = baseArray;
	O3D_ASSERT(test0.Width() == baseArray.Width());
	O3D_ASSERT(test0.Height() == baseArray.Height());

	for (O3D_UINT32 k = 0 ; k < test0.Elt() ; ++k) 
	{ O3D_ASSERT(test0[k] == baseArray[k]); }

	// Translation
	{
		const O3D_INT32 testNb = 8;
		const O3D_INT32 testMov[testNb][2] = { {3, 1},  {-3, 1}, {3, -1}, {-3, -1}, {3, 0}, {-3,0}, { 0, 3}, {0, -3}};
		const O3D_INT32 replaceValue = 0;

		for (O3D_UINT32 n = 0; n < 10 ; ++n)
		for (O3D_UINT32 k = 0 ; k < testNb ; ++k)
		{
			O3DArray2Di testCpy = baseArray;
			O3D_INT32 movX = testMov[k][0];
			O3D_INT32 movY = testMov[k][1];

			testCpy.FastTranslate(movX, movY, replaceValue);

		/*		for (O3D_UINT32 j = 0 ; j < testCpy.Width() ; ++j)
				for (O3D_UINT32 i = 0 ; i < testCpy.Height() ; ++i)
				{
					if ((O3D_UINT32(i - movX) < testCpy.Width()) && (O3D_UINT32(j - movY < testCpy.Height())))
					{
						O3D_ASSERT(testCpy(i,j) == baseArray(i-movX, j-movY));
					}
					else
					{
						O3D_ASSERT(testCpy(i,j) == replaceValue);
					}
				}*/
		}

			O3DArray2Di testCpy = baseArray;
			O3D_INT32 movX = 1;
			O3D_INT32 movY = 0;

		testCpy.FastTranslate(movX, movY, 0);
	}

	// Fill
	{
		O3DArray2Di lArray(10, 10, 0);
		O3DArray2Di lFastArray(10, 10, 0);

		lArray.Fill(5, 0, 0, 1, 1);
		lFastArray.FastFill(5, 0, 0, 1, 1);

		for (O3D_UINT32 j = 0 ; j < lArray.Height() ; ++j)
			for (O3D_UINT32 i = 0 ; i < lArray.Width() ; i++)
			{
				if ((i == 0) && (j == 0))
				{
					O3D_ASSERT(lArray(i,j) == 5);
					O3D_ASSERT(lFastArray(i,j) == 5);
				}
				else
				{
					O3D_ASSERT(lArray(i,j) == 0);
					O3D_ASSERT(lFastArray(i,j) == 0);
				}
			}

		lArray.Fill(8, 0, 0, lArray.Width(), lArray.Height());
		lFastArray.FastFill(8, 0, 0, lArray.Width(), lArray.Height());

		for (O3D_UINT32 k = 0 ; k < lArray.Elt() ; ++k) 
		{
			O3D_ASSERT(lArray[k] == 8);
			O3D_ASSERT(lFastArray[k] == 8);
		}
	}

	std::cout << "Fin du test" << std::endl;
	system("PAUSE");

	return 0;
}
