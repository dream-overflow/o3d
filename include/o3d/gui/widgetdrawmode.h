/**
 * @file widgetdrawmode.h
 * @brief Base class for drawing widgets.
 * @author RinceWind
 * @author  Frederic SCHERMA
 * @date 2006-10-11
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_WIDGETDRAWMODE_H
#define _O3D_WIDGETDRAWMODE_H

#include "o3d/engine/texture/texture2d.h"
#include "o3d/engine/material/materialprofile.h"
#include "o3d/engine/shader/shadable.h"
#include "o3d/engine/vertexbuffer.h"
#include "o3d/core/vector2.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class WidgetDrawMode
//---------------------------------------------------------------------------------------
//! Base class for drawing widgets.
//---------------------------------------------------------------------------------------
class O3D_API WidgetDrawMode : public Shadable
{
public:

	//! Type of the widget draw mode object
	enum DrawModeType
	{
		UNDEFINED_MODE,
		SIMPLE_MODE,
		COMPASS_MODE
	};

	//! data structure for widget position and size
	struct WidgetDataSet
	{
		Int32 x,y;
		Int32 width,height;

		//! default constructor
		WidgetDataSet() : x(0), y(0), width(0), height(0) {}

		//! copy constructor
		WidgetDataSet(const WidgetDataSet& dup)
		{
			x = dup.x; y = dup.y; width = dup.width; height = dup.height;
		}

		WidgetDataSet& operator= (const WidgetDataSet& dup)
		{
			x = dup.x; y = dup.y; width = dup.width; height = dup.height;
			return *this;
		}

		void set(Int32 _x, Int32 _y, Int32 _width, Int32 _height)
		{
			x = _x; y = _y; width = _width; height = _height;
		}

		void set(Int32* data)
		{
			x = data[0]; y = data[1]; width = data[2]; height = data[3];
		}
	};

	//! Default constructor.
    WidgetDrawMode(Context *context, BaseObject *parent);

	//! Copy constructor.
	WidgetDrawMode(WidgetDrawMode &dup);

	//! Virtual destructor.
	virtual ~WidgetDrawMode();

	//! Get the type of the widget mode.
	virtual DrawModeType getType() const = 0;

	//! Set the texture.
	void setTexture(Texture2D* texture);
	//! Get the texture.
	Texture2D* getTexture();

	//! Set the default widget size.
	inline void setDefaultSize(const Vector2i &size) { m_defaultSize = size; }
	//! Set the default widget size.
	inline void setDefaultSize(Int32 w, Int32 h) { m_defaultSize.set(w,h); }
	//! Get the default widget size.
	inline const Vector2i& getDefaultSize() const { return m_defaultSize; }

	//! Set the default text color.
	inline void setDefaultTextColor(const Color &color) { m_defaultTextColor = color; }
	//! Get the default text color.
	inline const Color& getDefaultTextColor() const { return m_defaultTextColor; }

	//! Set the margin.
	void setMargin(const Vector2i &topLeft, const Vector2i &bottomRight);
	//! Get the top-left margin.
	const Vector2i& getTopLeftMargin() const { return m_topLeftMargin; }
	//! Get the bottom-right margin.
	const Vector2i& getBottomRightMargin() const { return m_bottomRightMargin; }
	//! Get the left margin.
	Int32 getLeftMargin() const { return m_topLeftMargin.x(); }
	//! Get the top margin.
	Int32 getTopMargin() const { return m_topLeftMargin.y(); }
	//! Get the right margin.
	Int32 getRightMargin() const { return m_bottomRightMargin.x(); }
	//! Get the bottom margin.
	Int32 getBottomMargin() const { return m_bottomRightMargin.y(); }

	//! Get base width.
	virtual Int32 getWidgetWidth() const = 0;
	//! Get base height.
	virtual Int32 getWidgetHeight() const = 0;

	//! draw directly into the current buffer.
	virtual void draw(Int32 x, Int32 y, Int32 width, Int32 height) = 0;

	//! Define the material.
	void setMaterial(Material *material);

protected:

	VertexBufferObjf m_vertices;
	VertexBufferObjf m_texCoords;
	ElementBufferObjui m_faces;

	UInt32 m_verticesOffs;
	UInt32 m_facesOffs;

	ArrayFloat m_verticesArray;
	ArrayFloat m_texCoordsArray;
	ArrayUInt32 m_facesArray;

	MaterialProfile m_material; //!< Material profile.

	Vector2i m_defaultSize;     //!< Default widget size defined by the theme.
	Color m_defaultTextColor;   //!< Default text color.

	Vector2i m_topLeftMargin;
	Vector2i m_bottomRightMargin;

	void drawWidgetElement(WidgetDataSet widget, Int32 x, Int32 y, Int32 width, Int32 height);
	void repeatWidgetElement(WidgetDataSet widget, Int32 x, Int32 y, Int32 width, Int32 height);

public:

	//-----------------------------------------------------------------------------------
	// Shadable override
	//-----------------------------------------------------------------------------------

	O3D_SHADABLE_NO_DATA_ACCESS
	O3D_SHADABLE_NO_MISC1
	O3D_SHADABLE_NO_EXT_ARRAY

	virtual VertexProgramType getVertexProgramType() const;

	virtual void attribute(VertexAttributeArray mode, UInt32 location);
	virtual void processAllFaces(Shadable::ProcessingPass pass);
};

} // namespace o3d

#endif // _O3D_WIDGETDRAWMODE_H

