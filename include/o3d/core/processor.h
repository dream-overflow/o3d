/**
 * @file processor.h
 * @brief Processor informations.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2007-12-01
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_PROCESSOR_H
#define _O3D_PROCESSOR_H

#include "string.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//!@class Processor
//-------------------------------------------------------------------------------------
//! Give informations about processor type and its available features (like MMX,SSE...)
//---------------------------------------------------------------------------------------
class O3D_API Processor
{
public:

	//! Default constructor
    Processor();

	//! Report informations on log file
	void reportLog();

	//! Has SSE1 support
    Bool hasSSE() const { return m_has_sse; }

	//! Has SSE2 support
    Bool hasSSE2() const { return m_has_sse2; }

	//! Has SSE3 support
    Bool hasSSE3() const { return m_has_sse3; }

	//! Has SSSE3 support
    Bool hasSSSE3() const { return m_has_ssse3; }

	//! Has SSE4_1 support
    Bool hasSSE4_1() const { return m_has_sse4_1; }

	//! Has SSE4_2 support
    Bool hasSSE4_2() const { return m_has_sse4_2; }

	//! Has MMX support
    Bool hasMMX() const { return m_has_mmx; }

	//! Has MMX ext support
    Bool hasMMXExt() const { return m_has_mmx_ext; }

	//! Has 3DNOW support
    Bool has3DNow() const { return m_has_3dnow; }

	//! Has 3DNOW ext support
    Bool has3DNowExt() const { return m_has_3dnow_ext; }

	//! Has Hyper HyperTransport Technology
    Bool isHTT() const { return m_is_htt; }

	//! Get the CPU name
    const String& getName() const { return m_cpu_name; }

	//! Get the CPU Vendor ID
    const String& getVendorID() const { return m_cpu_vendor; }

	//! Get the CPU vendor name
    String getVendorName() const;

	//! Get the CPU frequency in Mhz
    Float getSpeed() const { return m_cpu_speed; }

	//! Get the CPU model stepping
	Int32 getStepping() const { return m_stepping; }

	//! Get the CPU model
    Int32 getModel() const { return m_model; }

	//! Get the CPU family
    Int32 getFamily() const { return m_family; }

	//! Get the CPU type
    Int32 getType() const { return m_type; }

	//! Get ext informations about the CPU model
    Int32 getExtModel() const { return m_ext_model; }

	//! Get ext informations about the CPU family
    Int32 getExtFamily() const { return m_ext_family; }

private:

    Bool m_has_mmx;
    Bool m_has_mmx_ext;
    Bool m_has_3dnow;
    Bool m_has_3dnow_ext;
    Bool m_has_sse;
    Bool m_has_sse2;
    Bool m_has_sse3;
    Bool m_has_ssse3;
    Bool m_has_sse4_1;
    Bool m_has_sse4_2;
    Bool m_is_htt;

    Int32 m_stepping;
    Int32 m_model;
    Int32 m_family;
    Int32 m_type;
    Int32 m_ext_model;
    Int32 m_ext_family;

    String m_cpu_name;
    String m_cpu_vendor;

    Float m_cpu_speed;

    void doCPUID();
};

} // namespace o3d

#endif // _O3D_PROCESSOR_H

