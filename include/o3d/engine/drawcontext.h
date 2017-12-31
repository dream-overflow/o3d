/**
 * @file drawcontext.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_DRAWCONTEXT_H
#define _O3D_DRAWCONTEXT_H

#include "scene/sceneentity.h"
#include "blending.h"

namespace o3d {

class Context;

/**
 * @brief Define a set of OpenGL drawing context options.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2010-01-07
 */
class O3D_API DrawContext
{
public:

	//! Constructor.
    //! @param glContext Related valid OpenGL context.
    DrawContext(Context *glContext);

	//! Copy constructor.
    //! @param dup DrawContext to copy from.
	//! @note Copy the m_glContext pointer.
	DrawContext(const DrawContext &dup);

	//! Destructor.
	~DrawContext();

	//! Copy operator.
    //! @param dup DrawContext to copy from.
	//! @note Does not copy the m_glContext pointer.
	DrawContext& operator= (const DrawContext &dup);

	//! Apply immediately the draw content.
	void apply();

	//! Reset the parameters to default from the Context.
	void resetParameters();

	//-----------------------------------------------------------------------------------
	// Parameters
	//-----------------------------------------------------------------------------------

	//! Set the culling mode.
	inline void setCullingMode(CullingMode mode) { m_cullingMode = mode; }
	//! Get the culling mode.
	inline CullingMode getCullingMode() const { return m_cullingMode; }

    //! Set the blending function profile.
    inline void setBlendingFunc(Blending::FuncProfile func) { m_blendingFunc = func; }
    //! Get the blending function profile.
    inline Blending::FuncProfile getBlendingFunc() const { return m_blendingFunc; }

    //! Set the blending equations.
    inline void setBlendingEquation(Blending::Equation eq) { m_blendingEquation = eq; }
    //! Get the blending equations.
    inline Blending::Equation getBlendingEquation() const { return m_blendingEquation; }

	//! Set if faces are sorted and displayed with the alpha-pipeline.
	inline void setSorted(Bool state) { m_isSorted = state; }
	//! Get the state of the face sorting (using the alpha-pipeline).
	inline Bool getSorted() { return needsSorting(); }
	//! Is face sorting is asked by this shader.
	inline Bool needsSorting() { return m_isSorted; }

	//! Set double side drawing mode status.
	inline void setDoubleSide(Bool state) { m_doubleSide = state; }
	//! Get double side drawing mode status.
	inline Bool getDoubleSide() const { return m_doubleSide; }
	//! Enable the double side drawing mode.
	inline void enableDoubleSide() { m_doubleSide = True; }
	//! Disable the double side drawing mode.
	inline void disableDoubleSide() { m_doubleSide = False; }

	//! Set the depth buffer test status.
	inline void setDepthTest(Bool state) { m_depthTest = state; }
	//! Get the depth buffer test status.
	inline Bool getDepthTest()const { return m_depthTest; }
	//! Enable the depth buffer test.
	inline void enableDepthTest() { m_depthTest = True; }
	//! Disable the depth buffer test.
	inline void disableDepthTest() { m_depthTest = False; }

	//! Set the depth buffer write status.
	inline void setDepthWrite(Bool state) { m_depthWrite = state; }
	//! Get the depth buffer write status.
	inline Bool getDepthWrite() const { return m_depthWrite; }
	//! Enable the depth buffer write.
	inline void enableDepthWrite() { m_depthWrite = True; }
	//! Disable the depth buffer writer.
	inline void disableDepthWrite() { m_depthWrite = False; }

	//! Set the depth range to normal (false) or infinite (true).
	inline void setInfiniteDepthRange(Bool state) { m_infiniteDepthRange = state; }
	//! Get the depth range mode.
	inline Bool getInfiniteDepthRange() const { return m_infiniteDepthRange; }
	//! Set to infinite depth range.
	inline void enableInfiniteDepthRange() { m_infiniteDepthRange = True; }
	//! Set to normal depth range.
	inline void disableInfiniteDepthRange() { m_infiniteDepthRange = False; }

	//-----------------------------------------------------------------------------------
	// Serialization
	//-----------------------------------------------------------------------------------

	//! Write content to file.
	Bool writeToFile(OutStream &os) const;
	//! Read content from file.
	Bool readFromFile(InStream &is);

private:

    Context *m_context;         //!< Related OpenGL context.

    CullingMode m_cullingMode;  //!< Face culling mode.

    Bool m_isSorted;			//!< Face sorting (use alpha pipeline).

    Blending::FuncProfile m_blendingFunc;   //!< Blending function.
    Blending::Equation m_blendingEquation;  //!< Blending equation.

	Bool m_doubleSide;          //!< Draw the 2 sides.
    Bool m_depthTest;           //!< Enable depth test.
    Bool m_depthWrite;          //!< Enable depth write.
	Bool m_infiniteDepthRange;  //!< Enable infinite depth range.
};

} // namespace o3d

#endif // _O3D_DRAWCONTEXT_H
