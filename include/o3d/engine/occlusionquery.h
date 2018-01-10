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

#include "queryobject.h"

namespace o3d {

/**
 * @brief This class is an interface for an occlusion query object.
 * @details An Occlusion query allows to know how many pixels passed the Z-Test for a set
 * of primitives. This is useful for flare/glow rendering and for visibility check.
 * Each primitives are rendered between begin() and end() call will be included in the
 * Occlusion test. After calling end(), you can use getOcclusionType() to know if some
 * primitives are occluded or visible. Use getVisibleCount() to know how many fragments
 * have passed the Z-Buffer test.
 * @note Prefers the usage of AtomicCounter that is more portable and recent.
 */
class O3D_API OcclusionQuery : public QueryObject
{
public:

    enum SamplePassType
    {
        SAMPLES_PASSED = 0x8914,      //!< Count the number of samples passed
        ANY_SAMPLES_PASSED = 0x8C2F,  //!< True if any samples passed
        ANY_SAMPLES_PASSED_CONSERVATIVE = 0x8D6A
    };

    //! Occlusion return type
	enum Result
	{
		NOT_AVAILABLE = 0,   //!< Occlusion test isn't available.
		OCCLUDED,            //!< The primitives are occluded.
		NOT_OCCLUDED         //!< The primitives are visible.
	};

	//! Constructor.
    OcclusionQuery(Context *context, SamplePassType type = SAMPLES_PASSED);
	//! Destructor.
	~OcclusionQuery();

	//! return if primitives are occluded or visible
	Result getOcclusionType();

    /**
     * @brief Get how many samples passed the Z-Test or 0 if this information isn't available.
     * When SAMPLES_PASSED is used.
     */
    inline UInt32 getVisibleCount()
	{
        // Check if we have the result, if not, try to get it
        if (getOcclusionType() != NOT_AVAILABLE) {
			return m_visibleCount;
        } else {
			return 0;
		}
	}

    /**
     * @brief I any or none of the fragments are visibles.
     * When ANY_SAMPLES_PASSED is used.
     */
    Bool isAnyVisibles()
    {
        // Check if we have the result, if not, try to get it
        if (getOcclusionType() != NOT_AVAILABLE) {
            return m_visibleCount != 0;
        } else {
            return False;
        }
    }

	//! Start the occlusion test
    void begin();
	//! end the occlusion test
    void end();

    //! Force to get the results
	void forceResults();

protected:

    SamplePassType m_samplePassType;  //!< Type of sample pass

    UInt32 m_visibleCount;    //!< Visible fragments
    Result m_occlusionType;   //!< Determine if primitives are visible or occluded
};

} // namespace o3d

#endif // _O3D_OCCLUSIONQUERY_H
