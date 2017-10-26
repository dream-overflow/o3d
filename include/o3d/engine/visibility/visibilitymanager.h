/**
 * @file visibilitymanager.h
 * @brief Visibility manager for a scene.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2009-05-15
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_VISIBILITYMANAGER_H
#define _O3D_VISIBILITYMANAGER_H

#include "o3d/core/memorydbg.h"
#include "o3d/core/baseobject.h"
#include "o3d/core/templatearray.h"
#include "o3d/engine/scene/sceneentity.h"

namespace o3d {

class VisibilityABC;
class SceneObject;
class DrawInfo;

//---------------------------------------------------------------------------------------
//! @class VisibilityManager
//-------------------------------------------------------------------------------------
//! Visibility manager for a scene.
//---------------------------------------------------------------------------------------
class O3D_API VisibilityManager : public SceneEntity
{
public:

	//! Global visibility controller type
	enum VisibilityType
	{
		DISTANCE,         //!< Visibility basic (by distance).
		OCTREE,
		QUADTREE          //!< Default
	};

	O3D_DECLARE_DYNAMIC_CLASS(VisibilityManager)

	//! Default constructor.
	//! @param parent Parent object.
	VisibilityManager(BaseObject *parent);

	//! Virtual destructor.
	virtual ~VisibilityManager();


	//-----------------------------------------------------------------------------------
	// Global settings
	//-----------------------------------------------------------------------------------

	//! Define the global visibility controller type.
	//! @param halfSize Half size in number of zone.
	//! @param zoneSize Size of a zone.
	void setGlobal(VisibilityType type, UInt32 halfSize, Float zoneSize);

	//! Get the type of the global visibility controller.
	inline VisibilityType getGlobal() const { return m_global; }

	//! Set max visibility distance and set usage to defined max distance (don't use camera zFar).
	inline void setMaxDistance(Float max)
	{
		m_maxDistance = max;
		m_useMaxDistance = True;
		m_useMaxZFar = False;
	}

	//! Get the max distance (return the user defined max distance or the active camera zFar).
	Float getMaxDistance() const;

	//! Use the zFar active camera as max distance.
	inline void useZFarMaxDistance() { m_useMaxZFar = m_useMaxDistance = True; }

	//! Use the defined max distance.
	inline void useDefinedMaxDistance() { m_useMaxDistance = True; m_useMaxZFar = False; }

	//! Enable max distance.
	inline void enableMaxDistance() { m_useMaxDistance = True;  }
	//! Disable max distance.
	inline void disableMaxDistance() { m_useMaxDistance = False; }

	//! Is max distance enable.
	inline Bool isMaxDistance() const { return m_useMaxDistance; }

	//! Is max distance is the camera zFar.
	inline Bool isMaxDistanceByZFar() const { return m_useMaxZFar; }


	//-----------------------------------------------------------------------------------
	// Process
	//-----------------------------------------------------------------------------------

	//! Add an object.
	void addObject(SceneObject *object);

	//! Update an object when it move.
	void updateObject(SceneObject *object);

	//! Remove an object.
	void removeObject(SceneObject *object);

	//! Process the visibility determination.
	void processVisibility();

	//! Draw all visible objects.
	void draw(const DrawInfo &drawInfo);

	//! Add an object to the drawing list.
	inline void addObjectToDraw(SceneObject* object) { m_drawList.push(object); }

protected:

	VisibilityType m_global;        //!< Type of the global visibility controller.

	VisibilityABC *m_globalController; //!< Global dynamic visibility controller.

	Float m_maxDistance;      //!< Max distance of visible objects.
	Bool m_useMaxDistance;    //!< Is the max distance is used (enable by default).
	Bool m_useMaxZFar;        //!< Max distance used is zFar value of the current camera (this is default).

	TemplateArray<SceneObject*> m_drawList; //!< Object draw list.
};

} // namespace o3d

#endif // _O3D_VISIBILITYMANAGER_H

