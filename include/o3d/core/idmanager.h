/**
 * @file idmanager.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_IDMANAGER_H
#define _O3D_IDMANAGER_H

#include "memorydbg.h"

#include "base.h"
#include <list>
#include <algorithm>

namespace o3d {

class InStream;
class OutStream;

/**
 * @brief The IDManager class
 * A manager for id. Provide an identifiers generator with recycler.
 * @date 2005-04-20
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 */
class O3D_API IDManager
{
public:

	//! @brief Default constructor
    IDManager(Int32 first = 0) :
        m_IDMax(first),
        m_IDFirst(first)
    {
    }

	//! @brief Get a free identifier
	//! @return A free identifier
	inline Int32 getID()
	{
		if (m_releasedID.empty())
		{
			return m_IDMax++;
		}
		else
		{
            // get front id, give a better (reuse the older id)
            // back can be useful for undo/redo
			Int32 id = m_releasedID.front();
            m_releasedID.pop_front();
			return id;
		}
	}

	//! View the next available identifier
    inline Int32 peekNextId() const
	{
		if (m_releasedID.empty())
			return m_IDMax;
		else
			return m_releasedID.front();
	}
        
	//! @brief Force the use of an identifier
	//! @param The id to try to use
	//! @return True if the id is available. False if it is already in use
    Bool forceID(Int32 id);

	//! @brief Release the use of an id
	//! @param id The id to release
	inline void releaseID(Int32 id)
	{
		O3D_ASSERT(id >= m_IDFirst);
		m_releasedID.push_back(id);
	}

	//! @brief Release all identifiers
	void releaseAll()
	{
		m_releasedID.clear();
		m_IDMax = m_IDFirst;
	}

	//! @brief Get the number of assigned identifiers
	//! @return The number of assigned identifiers
	inline UInt32 getNumAssignedIDs() const
	{
		return (UInt32(m_IDMax - m_IDFirst) - UInt32(m_releasedID.size()));
	}

    //! Write to file the manager state.
    Bool writeToFile(OutStream &os);

    //! Read from file the manager state.
    Bool readFromFile(InStream &is);

protected:

	Int32 m_IDMax;                   //!< Current max id
	Int32 m_IDFirst;                 //!< First used identifier
	std::list<Int32> m_releasedID;   //!< List of release ids
};

} // namespace o3d

#endif // _O3D_IDMANAGER_H
