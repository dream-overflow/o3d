/**
 * @file dome.h
 * @brief Class definition of a dome generator.
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2003-02-14
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_DOME_H
#define _O3D_DOME_H

#include "primitive.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class Dome
//-------------------------------------------------------------------------------------
//! Object used to create a dome.
//---------------------------------------------------------------------------------------
class O3D_API Dome : public Primitive
{
	O3D_DECLARE_ABSTRACT_CLASS(Dome)

public:

	enum TexCoordPolicy
	{
		TEX_LATITUDE_LONGITUDE = 0,		//!< Naive approach: directly map latitude-longitude onto sphere
		TEX_PROJECTION = 1,				//!< The texture is projected on the dome with respect to the vertical (default)
		TEX_UNFOLD = 2					//!< The dome is first unfolded and then projected on the texture (best)
	};

	//! @brief Default constructor
	Dome(UInt32 _flags = 0);

	//! @brief Constructor
	//! This constructor builds a dome so that the apparent angle between two vertices
	//! of different stacks is constant. It's used for example in the sky renderer to
	//! avoid useless concentration of vertices at the horizon.
	Dome(
		Float _radius,
		Float _height,
		UInt32 _subDiv,
		UInt32 _flags = 0);

	//! A destructor
	virtual ~Dome();

	//! @brief Recompute the dome to take changes in account
	Bool update();

	//! @brief Say whether or not the data are up to date
	Bool isUpToDate() const { return m_upToDate; }

	//! @brief Specify whether or not the dome is properly defined
	Bool isValid() const;

	//-----------------------------------------------------------------------------------
	// Options
	//-----------------------------------------------------------------------------------

	//! @brief Defines the radius of the dome
	//! The radius is the radius of curvature.
	void setRadius(Float _radius, Bool _update = False);
	//! @brief Returns the radius of curvature
	Float getRadius() const { return m_radius; }

	//! @brief Defines the height of the dome
	void setHeight(Float _height, Bool _update = False);
	//! @brief Returns the height of the dome
	Float getHeight() const { return m_height; }

	//! @brief Returns the radius of the basis of the dome
	Float getBaseRadius() const;

	//! @brief Defines the subdivision level
	//! The vertex count V(N) and the index count I(N) is given by :
	//! V(N) = 2^(N+1)*(2^N + 1) + 1 with N the subdivision level
	//! I(N) = 12*4^N (case of GL_TRIANGLES)
	//! I(N) = 2^(N+1)*(6*2^N + 2) (case of GL_LINES)
	void setSubDiv(UInt32 _subDiv, Bool _update = False);
	//! @brief Returns the subdivision level
	UInt32 getSubDiv() const { return m_subDiv; }

	//! @brief Returns the number of stacks
	UInt32 getStackCount() const;

	//! @brief Returns the number of slices
	UInt32 getSliceCount(UInt32 _stacks) const;

	//! @brief Returns the number of vertices on a stack
	//! If texture coordinates are not activated, this function is equivalent to
	//! getSliceCount()
	UInt32 getVertexCount(UInt32 _stacks) const;

	//! @brief Defines how must be computed texture coordinates
	void setTextureCoordinatePolicy(TexCoordPolicy _policy);
	//! @brief Returns the way of compute texture coordinates
	TexCoordPolicy getTextureCoordinatePolicy() const { return m_texCoordPolicy; }

	//! @brief Enable/Disable texture coordinates
	//! If texture coordinates are not enabled, GetTexCoord() will always return NULL.
	//! By default, the option is enabled.
	void enableTextureCoordinate(Bool _value = True, Bool _update = False);

	//! @brief Returns a pointer to the first vertices of a stack
	//! @param _stacks is the stack index. Must be in [0; getStackCount()]
	//!                GetStackCount() is not a valid index but can be used to iterate
	//!                on all vertices without having a specific case for the last vertex.
	const Float* getVerticesAtStack(UInt32 _stacks) const;

protected:

	//! Restricted copy constructor
	Dome(const Dome &dup) : Primitive(dup) {}

	//! @brief Internal function
	void buildVertices();

	//! @brief Internal function
	void buildTexCoords();

	//! @brief Internal function
	void buildIndices();

	// Members //
	Float m_radius;
	Float m_height;
	
	UInt32 m_subDiv;

	TexCoordPolicy m_texCoordPolicy;

	Bool m_upToDate;
};

} // namespace o3d

#endif // _O3D_DOME_H

