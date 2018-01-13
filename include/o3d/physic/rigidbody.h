/**
 * @file rigidbody.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_RIGIDBODY_H
#define _O3D_RIGIDBODY_H

#include "o3d/core/base.h"
#include "o3d/geom/geometry.h"
#include "physicentity.h"
#include "physicmodel.h"
#include "o3d/engine/scene/sceneobject.h"
#include "o3d/core/quaternion.h"
#include "o3d/core/memorydbg.h"

#include <vector>

namespace o3d {

/**
 * @brief A rigid body entity used by object that want to have rigid body physic computation
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2006-05-30
 */
class O3D_API RigidBody : public PhysicEntity
{
public:

	O3D_DECLARE_CLASS(RigidBody)

	//! default constructor
    RigidBody(SceneObject *object = nullptr, Bool assignToObject = True);

	//! destructor
	virtual ~RigidBody();

	//-----------------------------------------------------------------------------------
	// physic management
	//-----------------------------------------------------------------------------------

    inline Bool enablePhysicComputations()
    {
        Bool old = m_isPhysic;
        m_isPhysic = True;
        return old;
    }
    inline Bool disablePhysicComputations()
    {
        Bool old = m_isPhysic;
        m_isPhysic = False;
        return old;
    }
    inline Bool togglePhysicComputations()
    {
        Bool old = m_isPhysic;
        m_isPhysic = !m_isPhysic;
        return old;
    }

    inline Bool isUsingPhysic() const { return m_isPhysic; }

	//-----------------------------------------------------------------------------------
	// rigid body parameters
	//-----------------------------------------------------------------------------------

	//! get the mass
    inline Float getMass() const { return m_mass; }

	//! get the inverse of the mass
	inline Float getInvMass()
	{
        if (m_mass < o3d::Limits<Float>::epsilon()) {
			return 1.f / o3d::Limits<Float>::epsilon();
        } else {
            return 1 / m_mass;
        }
	}

	//! set the mass
    inline void setMass(const Float mass)
	{
        m_mass = mass;
		// re-compute parameters depending on the mass
        if (m_mass > o3d::Limits<Float>::epsilon()) {
            m_speed = m_P / m_mass;
        }
	}

	//! get the inertia body matrix
    inline const Matrix3& getIBody() const { return m_IBody; }

	//! set the inertia body matrix
	inline void setIBody(const Matrix3& IBody)
	{
		m_IBody = IBody;
		// compute parameters depending on IBody
		m_IBodyInv = IBody.invert();
        m_IWorld   = m_rotMatrix * m_IBody * (m_rotMatrix.transposeTo());
		m_IWorldInv = m_IWorld.invert();
        m_omega     = m_IWorld * m_L;
	}
	//! get the inverted inertia body matrix
    inline const Matrix3& getIBodyInv() const { return m_IBodyInv; }

    //! get the center of gravity
    inline const Vector3& getGravityCenter() const { return m_gravityCenter; }
    //! set the center of gravity
    inline void setGravityCenter(Vector3& gravityCenter) { m_gravityCenter = gravityCenter; }

    //! get the bounce coef
    inline Float getBounce() const { return m_bounce; }
    //! set the bounce coef
    inline void setBounce(Float bounce) { m_bounce = bounce; }

    //! get the friction coef
    inline Float getFriction() const { return m_friction; }
    //! set the friction coef
    inline void setFriction(Float friction) { m_friction = friction; }

    //! get the position
    inline const Vector3& getPosition() const { return m_pos; }
    //! set the position
    inline void setPosition(const Vector3 pos)
	{
        m_pos = pos;
		// reset the position
		computeWorldToBody();
	}

    //! get the rotation
    inline const Quaternion& getRotation() const { return m_rot; }
    //! set the rotation
    inline void setRotation(const Quaternion& rot)
	{
        m_rot = rot;
		// reset the orientation
        m_rotMatrix = m_rot.toMatrix3();
		computeWorldToBody();
	}

    //! get the quantity of movement p=m*v
    inline const Vector3& getP() const { return m_P; }
    //! set the quantity of movement p=m*v
	inline void setP(const Vector3& P)
	{
		m_P = P;
		// re-compute the parameters depending on P
        m_speed = m_P / m_mass;
	}

    //! get the linear momentum
    inline const Vector3& getL() const { return m_L; }
    //! set the linear momentum
	inline void setL(const Vector3& L) {	m_L = L; }

	//! get the inverted inertia world space rotation matrix
    inline const Matrix3& getIWorldInv() const { return m_IWorldInv; }

	//! get the inertia world space rotation matrix
    inline const Matrix3& getIWorld() const { return m_IWorld; }

    //! get the speed of the object in world frame
    inline const Vector3& getSpeed() const { return m_speed; }
    //! set the speed of the object in world frame
	inline void setSpeed(const Vector3& speed)
	{
        m_speed = speed;
		// compute variable depending on speed
        m_P = m_speed * m_mass;
	}

    //! get the instantaneous speed rotation axis
    inline const Vector3& getOmega() const { return m_omega; }
    //! set the instantaneous speed rotation axis
	inline void setOmega(const Vector3& Omega)
	{
        m_omega = Omega;
		// compute variable depending on omega
        m_L = m_IWorldInv * m_omega;
	}

	//! get the transformation matrix from world frame to body frame
    inline const Matrix4& getWorldToBody() const { return m_worldToBody; }
	//! get the previous transformation matrix from world frame to body frame
    inline const Matrix4& getWorldToBodyOld() const { return m_worldToBodyOld; }

	//! compute the speed of a given point on the rigid body with a radius vector
	//! from center of gravity to point on the rigid body
    inline Vector3 computeSpeed(const Vector3& radius) const { return m_speed + (m_omega ^ radius); }

	//! compute the position of a given point on the rigid body with a radius vector
	//! from center of gravity to point on the rigid body
    inline Vector3 computePos(const Vector3& radius) const { return m_worldToBody * radius; }

	//! get the rotation matrix of the rigid body
    inline const Matrix3& getRotationMatrix() const { return m_rotMatrix; }

    //! get the local force manager of this object (read-only)
    inline const ForceManager* getForceManager() const { return m_forceManager; }
    //! get the local force manager of this object
    inline ForceManager* getForceManager() { return m_forceManager; }
    //! set the local force manager of this object
    inline void setForceManager(ForceManager* pForceManager) { m_forceManager = pForceManager; }

    virtual void updatePhysicVerlet(Double t, Double dt) override;
    virtual void updatePhysicEuler(Double t, Double dt) override;
    virtual void updatePhysicRK4(Double t, Double dt, UInt32 numStepRK4) override;

	//! process a manager of force
	virtual void processForce(ForceManager& ForceManager);

    //! set the collision detection model
    inline void setPhysicModel(PhysicModel* pPhysicModel) { m_physicModel = pPhysicModel; }
    //! get the collision detection model
    inline PhysicModel* getPhysicModel() { return m_physicModel; }

	//-----------------------------------------------------------------------------------
	// accumulators management
	//-----------------------------------------------------------------------------------

	//! add a force to the accumulator
    inline void addForce(const Vector3& force) { m_forceAccumulator += force; }
	//! add a torque to the accumulator
    inline void addTorque(const Vector3& torque) { m_torqueAccumulator += torque; }

	//! add a force to the accumulator
    inline void addForceImpulse(const Vector3& force) { m_forceImpulseAccumulator += force; }
	//! add a torque to the accumulator
    inline void addTorqueImpulse(const Vector3& torque) { m_torqueImpulseAccumulator += torque; }

	//! get the force accumulator
    inline const Vector3& getForceAccumulator() const { return m_forceAccumulator; }

	//! get the torque accumulator
    inline const Vector3& getTorqueAccumulator() const { return m_torqueAccumulator; }

	//! get the force impulse accumulator
    inline const Vector3& getForceImpulseAccumulator() const { return m_forceImpulseAccumulator; }

	//! get the torque impulse accumulator
    inline const Vector3& getTorqueImpulseAccumulator() const { return m_torqueImpulseAccumulator; }

	//! Reset force and impulse accumulators
    virtual void resetAccumulators() override;

	//-----------------------------------------------------------------------------------
	// mass management
	//-----------------------------------------------------------------------------------

	//! Set up mass and inertia to represent a sphere
    void setUpMassSphere(Float density, Float radius);
	//! Set up mass and inertia to represent a box
    void setUpMassBox(Float density, Vector3 dim);
    //! Set up mass and inertia to represent a box
    void setUpMassBox(Float density, Float dimX, Float dimY, Float dimZ);
	//! Set up mass and inertia to represent a cylinder aligned on the Z axis
    void setUpMassCylinder(Float density, Float height, Float radius, UInt32 axis = Z);

	//! Auto adjust a mass for a volume
    void adjustMass(Float newMass);

	//! Given mass parameters for some object, adjust them to represent the object displaced
	//! by Dir relative to the body frame.
	//!
	//! If the body is translated by 'a' relative to its point of reference, the new inertia
  	//! about the point of reference is :
	//!
	//!   I + mass*(crossmat(c)^2 - crossmat(c+a)^2)
	//!
	//! Where c is the existing center of mass and I is the old inertia
    void translateMass(Vector3 dir);

	//! Given mass parameters for some object, adjust them to represent the object rotated
	//! by Rot relative to the body frame.
	//!
	//! If the body is rotated by `R' relative to its point of reference, the new inertia
	//! about the point of reference is :
	//!
	//!  R * I * R'
	//!
	//! where I is the old inertia.
    void rotateMass(Matrix3 rot);

	//-----------------------------------------------------------------------------------
	// physic entity method
	//-----------------------------------------------------------------------------------

	//! Compute the world to body matrix transformation whose be used by the O3DObject
	void computeWorldToBody();
	//! Compute some variables
	void computeVariables();
	//! Set the position and rotation of the rigid body to by in sync with those of the object
	void synchronizeWithObject();

    // serialisation

    virtual Bool writeToFile(OutStream &os) override;
    virtual Bool readFromFile(InStream &is) override;

protected:

    SceneObject* m_object;   //!< the object affected by the physic
    Bool m_isPhysic;         //!< is physic computation enabled

    Vector3 m_curPos;        //!< position at current time
    Quaternion m_curRot;     //!< rotation at current time

    Vector3 m_curP;          //!< quantity of movement at current time
    Vector3 m_curL;          //!< linear moment at current time

    Vector3 m_pos;           //!< position of the object in world coordinates
    Quaternion m_rot;        //!< rotation of the object in world coordinates with no translations

	Vector3 m_P;             //!< quantity of move of the object in world coordinates (p=m*v)
	Vector3 m_L;             //!< linear moment (omega=I*L)

    Matrix3 m_rotMatrix;     //!< the matrix of rotation of the object (matrix version of the quaternion)
	Matrix3 m_IWorld;        //!< the inertia matrix in world space : Iworld = Rotation*Ibody*transp(Rotation)
	Matrix3 m_IWorldInv;     //!< inverse of the inertia matrix in world space : Iworld = Rotation*Ibody*transp(Rotation)

    Vector3 m_speed;         //!< speed of the center of mass of the object : speed = p/m
    Vector3 m_omega;         //!< instantaneous rotation : omega=I*L

    Matrix4 m_worldToBody;   //!< transformation matrix (copy of the object matrix)
    Matrix4 m_worldToBodyOld;//!< previous transformation matrix (for deflexion)

	Matrix3 m_IBody;         //!< the inertia matrix in body space
	Matrix3 m_IBodyInv;      //!< inverse of the inertia matrix in body space

    Float m_mass;            //!< the mass of the object
    Vector3 m_gravityCenter; //!< the center of gravity, in body frame

    Float m_bounce;         //!< bouncing coef
    Float m_friction;       //!< friction coef

    Vector3 m_forceAccumulator;     //!< forces on the object
    Vector3 m_torqueAccumulator;    //!< torques on the object

    Vector3 m_forceImpulseAccumulator;  //!< translation impulse on the object
    Vector3 m_torqueImpulseAccumulator; //!< rotation impulse on the object

    ForceManager* m_forceManager;  //!< an additional force manager used to store forces specific to this object
    PhysicModel* m_physicModel;    //!< model used for collision detection
};

typedef std::list<RigidBody*> T_RigidBodyList;
typedef T_RigidBodyList::iterator IT_RigidBodyList;
typedef T_RigidBodyList::const_iterator CIT_RigidBodyList;

typedef std::vector<RigidBody*> T_RigidBodyVector;
typedef T_RigidBodyVector::iterator	IT_RigidBodyVector;
typedef T_RigidBodyVector::const_iterator CIT_RigidBodyVector;

} // namespace o3d

#endif // _O3D_RIGIDBODY_H
