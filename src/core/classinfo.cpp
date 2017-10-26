/**
 * @file classinfo.cpp
 * @brief Implementation of ClassInfo.h
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2009-10-10
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/classinfo.h"
#include "o3d/core/classfactory.h"

using namespace o3d;

ClassInfo* ClassInfo::ms_first = nullptr;

// Destructor
ClassInfo::~ClassInfo()
{
	// remove this object from the linked list of all class info: if we don't
	// do it, loading/unloading a DLL containing static ClassInfo objects is
	// not going to work
	if (this == ms_first)
	{
		ms_first = m_next;
	}
	else
	{
		ClassInfo *info = ms_first;
		while (info)
		{
			if (info->m_next == this)
			{
				info->m_next = m_next;
				break;
			}

			info = info->m_next;
		}
	}

    //unRegisterClass();
}

// registers the class
void ClassInfo::registerClass()
{
    ClassFactory::registerClassInfo(m_className, this);
}

void ClassInfo::unRegisterClass()
{
	ClassFactory::unRegisterClassInfo(m_className);
}

