/**
 * @file matharray.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_MATHARRAY_H
#define _O3D_MATHARRAY_H

#include "vector3.h"
#include "vector4.h"
#include "matrix3.h"
#include "matrix4.h"
#include "quaternion.h"
#include "dualquaternion.h"

#include "atomicarray.h"

namespace o3d {

/**
 * @typedef Matrix4Array
 * @date 2007-12-08
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @brief AtomicArray specialization for an array of Matrix4
 */
typedef AtomicArray<Matrix4,Float,6> Matrix4Array;

/**
 * @typedef Matrix3Array
 * @date 2007-12-08
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @brief AtomicArray specialization for an array of Matrix3
 */
typedef AtomicArray<Matrix3,Float,6> Matrix3Array;

/**
 * @typedef Vector3Array
 * @date 2007-12-08
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @brief AtomicArray specialization for an array of Vector3
 */
typedef AtomicArray<Vector3,Float,4> Vector3Array;

/**
 * @typedef Vector4Array
 * @date 2007-12-08
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @brief AtomicArray specialization for an array of Vector3
 */
typedef AtomicArray<Vector4,Float,4> Vector4Array;

/**
 * @typedef QuaternionArray
 * @date 2007-12-08
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @brief AtomicArray specialization for an array of Quaternion
 */
typedef AtomicArray<Quaternion,Float,4> QuaternionArray;

/**
 * @typedef DualQuaternionArray
 * @date 2007-12-08
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @brief AtomicArray specialization for an array of DualQuaternion
 */
typedef AtomicArray<DualQuaternion,Float,5> DualQuaternionArray;

} // namespace o3d

#endif // _O3D_MATHARRAY_H

