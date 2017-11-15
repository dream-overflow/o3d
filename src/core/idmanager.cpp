/**
 * @file idmanager.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/idmanager.h"
#include "o3d/core/instream.h"
#include "o3d/core/outstream.h"

using namespace o3d;

Bool IDManager::forceID(Int32 id)
{
    if (id < m_IDMax)
    {
        std::list<Int32>::iterator it = std::find<std::list<Int32>::iterator, Int32>(
                    m_releasedID.begin(),
                    m_releasedID.end(),
                    id);

        if (it == m_releasedID.end())
            return False;
        else
            m_releasedID.erase(it);

        return True;
    }
    else
    {
        for (Int32 tmpId = m_IDMax; tmpId < id; ++tmpId)
        {
            m_releasedID.push_back(tmpId);
        }

        m_IDMax = id+1;

        return True;
    }
}

Bool IDManager::writeToFile(OutStream &os)
{
    os << m_IDMax
       << m_IDFirst;

    // size and garbage content
    UInt32 size = (UInt32)m_releasedID.size();
    os << size;

    for (Int32 id : m_releasedID)
    {
        os << id;
    }

    return True;
}

Bool IDManager::readFromFile(InStream &is)
{
    is >> m_IDMax
       >> m_IDFirst;

    Int32 id;
    UInt32 size;
    is >> size;

    m_releasedID.clear();

    for (UInt32 i = 0; i < size; ++i)
    {
        is >> id;
        m_releasedID.push_back(id);
    }

    return True;
}

