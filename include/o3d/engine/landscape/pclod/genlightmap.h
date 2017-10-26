/**
 * @file genlightmap.h
 * @brief TODO
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2006-08-24
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_GENLIGHTMAP_H
#define _O3D_GENLIGHTMAP_H

#include "o3d/core/base.h"
#include "o3d/core/templatearray2d.h"

namespace o3d {

class Vector3;
class Callback;

class PCLODLight;
class Vector3;
class PCLODLightmap;

class O3D_API GenLightMap {

public:

	/* This function builds the normal map of a set of points.
	 * - The 2d array is an array of float.
	 * - The function returns an array of vector3d, stored as an array of floats. The vectors are not normalized.
	 * - If the target buffer is null, the function will allocate the memory. If it's not null, the target buffer
		 is considered to be wide enough to store all the values.
	 * You can pass a call back to this function which will be regularly called with the current progress (int)
	 * in argument. If the callback returns a value different from 0, the function will exit.
	 * Note that the function will destroy the callback you provided. */
	static void buildLinearNormalMapInfiniteLight(const Float *,
												   const UInt32,
												   const UInt32,
												   Float,
												   Float * &,
												   Callback * = NULL);

	/* This function is just provided for convenience.
	 * - It allows to specify borders to compute the lightmaps.
	 * - The borders is an array of size 2*(W+H)+4 */
	static void buildLinearNormalMapInfiniteLightBorders(const Float *,
												   const UInt32,
												   const UInt32,
												   Float,
												   Float * &,
												   const Float *,
												   Callback * = NULL);

	/* This function normalize a normal map */
	static void normalizeNormalMap(Float *, const UInt32, const UInt32);

	/* The function return the lightmap associated to a normal map of size W * H.
	 * - The lightmap this function returns is (W-1)*(H-1) */
	static void buildLinearLightmap(const Vector3 &, const Float *, const UInt32, const UInt32, Float * &, UInt32 &, UInt32 &);

	/* Build a shadow map from a 2d array of points. The light source is supposed to be at an infinite distance,
	 * and the light direction vector is contained in the XZ plan.
	 * Arguments*:
	 * - the 2d array of points
	 * - space between each points
	 * - the slope of the light direction in the world coordinate system
	 * The function returns a 2D array of float whose cell are egal to 0.0f or 1.0f. */
	static void buildLinearShadowmapInfiniteLightX(const Array2DFloat &, Float, Float, Array2DFloat &);

	/* Performance with a 257*257 heightmap:
	 * DEBUG: about 200 shadowmap/sec on a P4 2.5Ghz
	 * RELEASE : about 2000 shadowmap/sec */

	/* On suppose pour le moment, dir[X] >> 1 et dir[Y] >= 0 */
	static void buildShadowmapInfiniteLight(const Array2DFloat &, Float, const Vector3 &, Array2DFloat &, Callback * = NULL);

	static void buildShadowMapFromDirectionalLight(PCLODLight *, const Vector3 &, const std::vector<PCLODLightmap*> &);
};

} // namespace o3d

#endif // _O3D_GENLIGHTMAP_H

