/**
 * @file springforce.h
 * @brief A spring force
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2006-06-01
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_SPRINGFORCE_H
#define _O3D_SPRINGFORCE_H

#include "o3d/core/memorydbg.h"
#include "o3d/core/vector3.h"
#include "abcforce.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class SpringForce
//-------------------------------------------------------------------------------------
//! A spring force between a fixed point in space and the object.
//! Note: if you want to create a spring between two objects, use a constraint.
//---------------------------------------------------------------------------------------
class O3D_API SpringForce : public ABCForce
{
public:

	O3D_DECLARE_CLASS(SpringForce)

	//! default constructor
	SpringForce(
		BaseObject *pParent,
		const Vector3& Anchor = Vector3(0.f,0.f,0.f),
		Float Stiffness = 0.005f,
		Float RestLength = 1.f,
		Float Dampling = 0.f) :
			ABCForce(pParent),
			m_Stiffness(Stiffness),
			m_Dampling(Dampling),
			m_RestLength(RestLength),
			m_Anchor(Anchor)
	{}

	//! Set/Get the strength of the spring
	inline void setStiffness(Float stiffness) { m_Stiffness = stiffness; }
	inline const Float& getStiffness()const { return m_Stiffness;	}
	inline       Float& getStiffness()      { return m_Stiffness;	}

	//! Set/Get the dampling coefficient
	inline void setDampling(Float dampling) { m_Dampling = dampling; }
	inline const Float& getDampling()const { return m_Dampling;	}
	inline       Float& getDampling()      { return m_Dampling;	}

	//! Set/Get the size of the spring when it is at rest
	inline void setRestLength(Float restLength) { m_RestLength = restLength; }
	inline const Float& getRestLength()const { return m_RestLength;	}
	inline       Float& getRestLength()      { return m_RestLength;	}

	//! Set/Get the position of the anchor the object is linked to
	inline void setAnchor(const Vector3 &anchor) { m_Anchor = anchor; }
	inline const Vector3& getAnchor()const { return m_Anchor;	}
	inline       Vector3& getAnchor()      { return m_Anchor;	}


	//! process the force on the object
    virtual void processObject(class RigidBody& rigidBody) override;

	//! process the force on the particule
    //virtual void processParticule(class Particule& particule) override;

	//! serialization
    virtual Bool writeToFile(OutStream &os) override;
    virtual Bool readFromFile(InStream &is) override;

protected:

	Float m_Stiffness;   //!< the strength of the spring
	Float m_Dampling;    //!< the dampling coefficient
	Float m_RestLength;  //!< size of the spring when it is at rest
	Vector3 m_Anchor;     //!< position of the anchor the object is linked to
};

} // namespace o3d

#endif // _O3D_SPRINGFORCE_H

