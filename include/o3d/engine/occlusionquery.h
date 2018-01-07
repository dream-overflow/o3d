/**
 * @file occlusionquery.h
 * @brief This class is an interface for an occlusion query object.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2002-08-08
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_OCCLUSIONQUERY_H
#define _O3D_OCCLUSIONQUERY_H

#include "o3d/core/base.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

class Context;

/**
 * @brief  This class is an interface for an occlusion query object.
 * @details An Occlusion query allows to know how many pixels passed the Z-Test for a set
 * of primitives. This is useful for flare/glow rendering and for visibility check.
 *
 * Usage:
 *
 * Each primitives are rendered between begin() and end() call will be included in the
 * Occlusion test. After calling end(), you can use getOcclusionType() to know if some
 * primitives are occluded or visible. Use getVisibleCount() to know how many fragments
 * have passed the Z-Buffer test.
 */
class O3D_API OcclusionQuery
{
public:

	//! occlusion return type
	enum Result
	{
		NOT_AVAILABLE = 0,   //!< Occlusion test isn't available.
		OCCLUDED,            //!< The primitives are occluded.
		NOT_OCCLUDED         //!< The primitives are visible.
	};

	//! Constructor.
    OcclusionQuery(Context *context);
	//! Destructor.
	~OcclusionQuery();

	//! Get the occlusion query OpenGL identifier.
	inline UInt32 getId()const { return m_id; }

	//! return if primitives are occluded or visible
	Result getOcclusionType();

	//! get how many samples passed the Z-Test or 0 if this information isn't available.
	inline UInt32 getVisibleCount()
	{
		// Check if we have the result
		// If not, try to get it
        if (getOcclusionType() != NOT_AVAILABLE) {
			return m_visibleCount;
        } else {
			return 0;
		}
	}

	//! Start the occlusion test
    void begin();
	//! end the occlusion test
    void end();
	//! Force the driver to get the results
	void forceResults();

protected:

    UInt32 m_id;              //!< Id of this occlusion query object
    UInt32 m_visibleCount;    //!< Visible fragments
    Result m_occlusionType;   //!< Determine if primitives are visible or occluded
    Context *m_context;       //!< Related context
};

} // namespace o3d

#endif // _O3D_OCCLUSIONQUERY_H
