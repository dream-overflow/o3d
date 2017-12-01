/**
 * @file worldlabel.h
 * @brief Base class used to create label you can put in the object space.
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2008-04-29
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_WORLDLABEL_H
#define _O3D_WORLDLABEL_H

#include "../scene/sceneobject.h"

#include "o3d/core/vector2.h"
#include "o3d/core/vector4.h"

namespace o3d {

/**
 * @brief Draw a front label, at a 3d position into the scene, using size relative distance
 * from camera or constant. The label always stay in front of the camera, and can be or not
 * alway visible.
 */
class O3D_API WorldLabel : public SceneObject
{
public:

	O3D_DECLARE_ABSTRACT_CLASS(WorldLabel)

	//! default contructor
	WorldLabel(BaseObject * _pParent);

	//! copy contructor
	WorldLabel(const WorldLabel & _dup);

	//! Get the drawing type
	virtual UInt32 getDrawType() const;

	//! destructor
	virtual ~WorldLabel();

	//! duplicator
	WorldLabel& operator=(const WorldLabel & _dup);

	//! Nothing to update
	virtual void update() {}

	//! @brief Prepare the drawing of the label
	virtual void draw(const DrawInfo &drawInfo);

	//! @brief Draw the content of the label
	virtual void drawContent() = 0;

	//-----------------------------------------------------------------------------------
	// Functions
	//-----------------------------------------------------------------------------------

	//! @brief Defines the coordinate the user wants to use
	//! This function allows you to change the units used during the rendering stage.
	//! In all cases, the origin is at the top left. The right edge coordinate is
	//! <screenSize[X]> with pixel coordinate or <1.0> with relative coordinate.
	//! @param _value must be FALSE if you want to use pixel coordinate, otherwise,
	//!        relative coordinate will be used.
	void enableRelativeCoordinate(Bool _value) { m_relativeCoordinate = _value; }

	//! @brief Returns the coordinate currently used
	Bool isRelativeCoordinate() const { return m_relativeCoordinate; }

	//! @brief Enable/Disable the clipping
	void enableClipping(Bool _value) { m_clipping = _value; }

	//! @brief Returns the clipping state
	Bool isClipping() const { return m_clipping; }

	//! @brief Defines an optional border (in pixels) used by the clipping
	void setClippingBorderSize(Int32 _value) { m_borderSize = _value; }

	//! @brief Returns the border size in pixels
	Int32 getClippingBorderSize() const { return m_borderSize; }

	//! @brief Set the label's size
	//! @param _size contains respectively the width and the height in pixels
	void setScreenSize(const Vector2ui & _size) { m_screenSize = _size; }

	//! @brief Set the label's size
	//! @param _width the width in pixels
	//! @param _height the height in pixels
	void setScreenSize(UInt32 _width, UInt32 _height)
	{
		m_screenSize[X] = _width;
		m_screenSize[Y] = _height;
	}

	//! @brief Returns the size of the label in pixels
	const Vector2ui & getScreenSize() const { return m_screenSize; }

	//! @brief Returns the width in pixels of the label
	UInt32 getScreenWidth() const { return m_screenSize[X]; }

	//! @brief Returns the height in pixels of the label
	UInt32 getScreenHeight() const { return m_screenSize[Y]; }

	//! @brief Defines the equation used to compute the label size
	//! This function allows you to define four parameters used to control the
	//! evolution of the label's size with the distance to the camera.
	//! @param _eqn contains the three coefficients A, B, C, D used in the formula:
	//!        f(z) = A/max(1,B + C.z + D.z^2)
	void setSizeEquation(const Vector4 & _eqn) { m_eqnCoefficient = _eqn; }

	//! @brief Returns the vector containing the equation coefficients.
	const Vector4 & getSizeEquation() const { return m_eqnCoefficient; }

	//! @brief Set the equation to the constant function f(d) = 1.
	void setConstantSize() { setSizeEquation(Vector4(1.0f, 0.0f, 0.0f, 0.0f)); }

	//! @brief Set the depth policy
	//! @param _value must be TRUE if you wants the label to be always visible if no
	//!        other objects are drawn in front of the depth buffer. FALSE means that
	//!        the label is written in the zbuffer with its real depth value.
	void enableShowAlways(Bool _value) { m_showAlways = _value; }

	//! @brief Return the depth policy
	Bool isShowAlways() const { return m_showAlways; }

	//! @brief Defines the distance of visibility
	//! This functions allows you to defines some bounds used to constraint the rendering
	//! of this object. If the distance between the label and the active camera is not in
	//! these bounds, the object won't be drawn. If you want to have the best performance,
	//! it would be better to remove this object from the Hierarchy tree instead of
	//! leaving it in the rendering list.
	//! @param _bounds contains respectively the near and the far bounds.
	void setDistanceBounds(const Vector2f & _bounds) { m_distanceBounds = _bounds; }

	//! @brief Returns the distance bounds
	const Vector2f getDistanceBounds() const { return m_distanceBounds; }

	//! @brief Defines the near bound
	void setNearDistanceBound(Float _value) { m_distanceBounds[0] = _value; }

	//! @brief Returns the near bound
	Float getNearDistanceBound() const { return m_distanceBounds[0]; }

	//! @brief Defines the far bound
	void setFarDistanceBound(Float _value) { m_distanceBounds[1] = _value; }

	//! @brief Returns the far bound
	Float getFarDistanceBound() const { return m_distanceBounds[1]; }

	//-----------------------------------------------------------------------------------
	// Serialisation
	//-----------------------------------------------------------------------------------

	virtual Bool writeToFile(OutStream &os);
	virtual Bool readFromFile(InStream &is);

protected:

	//! @brief Internal function
	//! You can override the default behavior by inherited from this class.
	virtual Float computeSizeFactor(Float);

	// Members //
	Vector2ui m_screenSize;
	Vector4 m_eqnCoefficient;	 //!< Coefficients

	Vector2f m_distanceBounds;	 //!< Defines the visibility range
	Bool m_showAlways;

	Bool m_relativeCoordinate;

	Bool m_clipping;
	Int32 m_borderSize;
};

} // namespace o3d

#endif // _O3D_WORLDLABEL_H
