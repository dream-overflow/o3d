/**
 * @file precompiled.h
 * @brief Base and math precompiled headers
 * @author Emmanuel Ruffio
 * @date 2007-19-09
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_PRECOMPILED_H
#define _O3D_PRECOMPILED_H

#include "o3d/core/memorydbg.h"

#ifdef O3D_USE_PCH

#include "o3d/core/base.h"
#include "o3d/core/file.h"
#include "o3d/core/filemanager.h"
#include "o3d/core/diskfile.h"
#include "o3d/core/string.h"
#include "o3d/core/filelisting.h"
#include "o3d/core/evt.h"
#include "o3d/core/dir.h"
#include "o3d/core/memfile.h"
#include "o3d/core/logger.h"
#include "o3d/core/mutex.h"
#include "o3d/core/thread.h"
#include "o3d/core/callback.h"
#include "o3d/core/task.h"
#include "o3d/core/debug.h"
#include "o3d/core/diskdir.h"
#include "o3d/core/objects.h"
#include "o3d/core/idmanager.h"
#include "o3d/core/fileinfo.h"
#include "o3d/core/diskfileinfo.h"
#include "o3d/core/memorymanager.h"
#include "o3d/core/base.h"
#include "o3d/core/stringtokenizer.h"

#include "o3d/core/box.h"
#include "o3d/core/dualquaternion.h"
#include "o3d/geom/frustum.h"
#include "o3d/geom/aabbox.h"
#include "o3d/geom/bsphere.h"
#include "o3d/geom/plane.h"
#include "o3d/core/matrix3.h"
#include "o3d/core/matrix4.h"
#include "o3d/core/quaternion.h"
#include "o3d/core/vector2.h"
#include "o3d/core/vector3.h"
#include "o3d/core/vector4.h"
#include "o3d/core/math.h"
#include "o3d/core/matharray.h"

#endif

#endif // _O3D_PRECOMPILED_H

