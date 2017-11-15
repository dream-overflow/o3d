/**
 * @file targetnode.h
 * @brief Node element into the world hierarchy that always follow a target.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2008-07-03
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3DTARGETNODE_H
#define _O3DTARGETNODE_H

#include "node.h"
#include "o3d/engine/object/mtransform.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class TargetNode
//-------------------------------------------------------------------------------------
//! Node element into the world hierarchy that always follow a target.
//---------------------------------------------------------------------------------------
class O3D_API TargetNode : public Node
{
public:

	O3D_DECLARE_CLASS(TargetNode)

	//! default constructor
    TargetNode(BaseObject *parent = nullptr);

	//! copy constructor
	TargetNode(const TargetNode &dup);

	//! destructor
	virtual ~TargetNode();

	//! duplicator
	TargetNode& operator=(const TargetNode &dup);


	//-----------------------------------------------------------------------------------
	// Target
	//-----------------------------------------------------------------------------------

	//! Set the target object
	inline void setTarget(SceneObject *target) { m_target = target; }

	//! Get the target object (read only)
	inline const SceneObject* getTarget() const { return m_target.get(); }
	//! Set the target object
	inline      SceneObject* getTarget() { return m_target.get(); }

	//! Define a minimal distance to follow the target (0.f is default)
	inline void setMinDistance(Float min) { m_minDist = min; }
	//! Define a maximal distance to follow the target (0.f is default, meaning no maximal)
	inline void setMaxDistance(Float max) { m_maxDist = max; }
	//! Define the roll (0.f is default)
	inline void setRoll(Float roll) { m_roll = roll; }

	//! Get the minimal distance
	inline Float getMinDistance() const { return m_minDist; }
	//! Get the maximal distance
	inline Float getMaxDistance() const { return m_maxDist; }
	//! Get the roll angle (in radian)
	inline Float getRoll() const { return m_roll; }


	//-----------------------------------------------------------------------------------
	// Updatable
	//-----------------------------------------------------------------------------------

	virtual void update();


	//-----------------------------------------------------------------------------------
	// Serialisation
	//-----------------------------------------------------------------------------------

	virtual Bool writeToFile(OutStream &os);
	virtual Bool readFromFile(InStream &is);

	//! Post import pass to retrieve the target
	virtual void postImportPass();

protected:

	MTransform *m_transformTarget;      //!< Default transform for following
	SmartObject<SceneObject> m_target;  //!< Target object

	Float m_minDist;      //!< Minimal distance
	Float m_maxDist;      //!< Maximal distance

	Float m_roll;         //!< Roll (rotate on direction axis)

	Int32 m_targetId;     //!< Target id for importation pass
};

} // namespace o3d

#endif // _O3D_TARGETNODE_H

