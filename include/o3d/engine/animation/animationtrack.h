/**
 * @file animationtrack.h
 * @brief Animations tracks.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-02-03
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_ANIMATIONTRACK_H
#define _O3D_ANIMATIONTRACK_H

#include "keyframe.h"
#include "animation.h"
#include "o3d/core/memorydbg.h"
#include "o3d/core/quaternion.h"

namespace o3d {

class SceneObject;
class Animatable;

//---------------------------------------------------------------------------------------
//! @class AnimationTrack
//-------------------------------------------------------------------------------------
//! An animation track contain a list of keyframe used to animate an object
//! with different manner. There are different kind of keyframe and consequently there
//! is as much animation track kind as keyframe (ex: linear,constant,bezier,TCB spline).
//! An animation track can use a float, a vector or a quaternion for compute the
//! animation.
//---------------------------------------------------------------------------------------
class O3D_API AnimationTrack : public Serialize
{
public:

	//! Animation track format.
	enum TrackType
	{
		TRACK_TYPE_BOOL,
		TRACK_TYPE_FLOAT,
		TRACK_TYPE_VECTOR,
		TRACK_TYPE_QUATERNION,
		TRACK_TYPE_UNDEFINED
	};

	//! Animation target.
	enum Target
	{
        TARGET_OBJECT_POS,         //!< move on all axis with a Vector3
		TARGET_OBJECT_POS_X,       //!< move on X axis with a Float
		TARGET_OBJECT_POS_Y,       //!< ... Y
		TARGET_OBJECT_POS_Z,       //!< ... Z

        TARGET_OBJECT_ROT,         //!< rotate with a Quaternion
        TARGET_OBJECT_ROT_X,       //!< rotate on X with a Float (Linear, bezier, TCB...)
        TARGET_OBJECT_ROT_Y,       //!< rotate on Y with a Float (Linear, bezier, TCB...)
        TARGET_OBJECT_ROT_Z,       //!< rotate on Z with a Float (Linear, bezier, TCB...)

        TARGET_OBJECT_SCALE,       //!< scale on all axis with a Vector3
		TARGET_OBJECT_SCALE_X,     //!< scale on X axis with a Float
		TARGET_OBJECT_SCALE_Y,     //!< ... Y
		TARGET_OBJECT_SCALE_Z,     //!< ... Z

        TARGET_MATERIAL_AMBIENT,   //!< vary ambient color with a Color
        TARGET_MATERIAL_AMBIENT_R, //!< vary ambient red color component with a Color
		TARGET_MATERIAL_AMBIENT_G, //!< ... green
		TARGET_MATERIAL_AMBIENT_B, //!< ... blue
		TARGET_MATERIAL_AMBIENT_A, //!< ... alpha

        TARGET_MATERIAL_DIFFUSE,   //!< vary diffuse color with a Color
        TARGET_MATERIAL_DIFFUSE_R, //!< vary diffuse red color component with a Color
		TARGET_MATERIAL_DIFFUSE_G, //!< ... green
		TARGET_MATERIAL_DIFFUSE_B, //!< ... blue
		TARGET_MATERIAL_DIFFUSE_A, //!< ... alpha

        TARGET_MATERIAL_SPECULAR,   //!< vary specular color with a Color
        TARGET_MATERIAL_SPECULAR_R, //!< vary specular red color component with a Color
		TARGET_MATERIAL_SPECULAR_G, //!< ... green
		TARGET_MATERIAL_SPECULAR_B, //!< ... blue
		TARGET_MATERIAL_SPECULAR_A, //!< ... alpha

		TARGET_MATERIAL_SHINE,     //!< vary shininess with a Float

		TARGET_OBJECT_DISPLAY,     //!< display/hide the target with a Float or Bool

		TARGET_BLEND_WEIGHT = 256,//!< (Float) weight of a sub-animation. sub-target = id of the sub-anim
		TARGET_BLEND_OFFSET,	  //!< (Float) offset of a sub-animation. sub-target = id of the sub-anim
		TARGET_BLEND_SCALE,	      //!< (Float)  scale of a sub-animation. sub-target = id of the sub-anim

		TARGET_UNDEFINED = 1024
	};

	//! Animation track playing mode.
	enum TrackMode
	{
		TRACK_MODE_CONSTANT,      //!< constant value (equal to first or last key)
		TRACK_MODE_LOOP,          //!< restart when at last keyframe
		TRACK_MODE_PING_PONG,      //!< go backward when at last keyframe
		TRACK_MODE_RELATIVE_LOOP,  //!< restart where the last keyframe stopped
		TRACK_MODE_UNDEFINED
	};

protected:

	T_KeyFrameList m_keyFrameList;    //!< the list of keyframes

	Target m_Target;
	UInt32 m_SubTarget;           //!< sub-target (optional)

	TrackMode m_TrackMode_Before;
	TrackMode m_TrackMode_After;

	Float m_Length;
	Float m_Time;    //!< the time corresponding to the current data

	Evaluator::Type m_evaluator_type;
	TrackType m_track_type;

	struct TrackRange
	{
		CIT_KeyFrameList First;
		CIT_KeyFrameList Last;
	};

	typedef std::vector<TrackRange> T_TrackRangeVector;
	typedef T_TrackRangeVector::iterator IT_TrackRangeVector;
	typedef T_TrackRangeVector::const_iterator CIT_TrackRangeVector;

	T_TrackRangeVector m_trackRangeVector; //! start/end track for each animation range (precomputed)

public:

	//! default constructor
	AnimationTrack()
	{
		m_Target = TARGET_UNDEFINED;
		m_SubTarget = 0;
		m_TrackMode_Before = TRACK_MODE_UNDEFINED;
		m_TrackMode_After = TRACK_MODE_UNDEFINED;
		m_Length = 0.f;

		m_evaluator_type = Evaluator::UNDEFINED;
		m_track_type = TRACK_TYPE_UNDEFINED;

		m_Time = Limits<Float>::max();
	}

	//! constructor
	AnimationTrack(
			Target target,
			UInt32 subTarget,
			TrackMode trackModeBefore,
			TrackMode trackModeAfter)
	{
		m_Target = target;
		m_SubTarget = subTarget;
		m_TrackMode_Before = trackModeBefore;
		m_TrackMode_After  = trackModeAfter;
		m_Length = 0.f;

		m_evaluator_type = Evaluator::UNDEFINED;
		m_track_type = TRACK_TYPE_UNDEFINED;

		m_Time = Limits<Float>::max();
	}

	//! destructor
	virtual ~AnimationTrack();


	//! Get the keyframe list (read only).
	inline const T_KeyFrameList& getKeyFrameList() const { return m_keyFrameList; }
	//! Get the keyframe list.
	inline T_KeyFrameList& getKeyFrameList() { return m_keyFrameList; }

	//! add/get/remove keyframe
	void addKeyFrame(KeyFrame &keyframe)      { m_keyFrameList.push_back(&keyframe);  }
	void addKeyFrameFirst(KeyFrame &keyframe) { m_keyFrameList.push_front(&keyframe); }
	void addKeyFrameLast(KeyFrame &keyframe)  { m_keyFrameList.push_back(&keyframe);  }
	void removeKeyFrame(KeyFrame &keyframe)   { m_keyFrameList.remove(&keyframe);     }

    //! Set the animation track duration.
	inline void setLength(Float length) { m_Length = length; }
    //! Get the animation track duration.
	inline Float getLength() const { return m_Length; }

	//! Set track modes.
	inline void setTrackMode(TrackMode before, TrackMode after)
	{
		m_TrackMode_Before = before;
		m_TrackMode_After  = after;
	}

	//! Set track mode before.
	inline void setTrackModeBefore(TrackMode before) { m_TrackMode_Before = before; }
	//! Set track mode after.
	inline void setTrackModeAfter(TrackMode after) { m_TrackMode_After  = after;  }

	//! Get track mode before.
	inline TrackMode getTrackModeBefore() const { return m_TrackMode_Before; }
	//! Get track mode after.
	inline TrackMode getTrackModeAfter () const { return m_TrackMode_After;  }

	//! Set target mode.
	inline void setTarget(Target target) { m_Target = target; }
	//! Get target mode.
	inline Target getTarget() const { return m_Target; }

	//! Set suntarget mode.
	inline void setSubTarget(UInt32 subTarget) { m_SubTarget = subTarget; }
	//! Get suntarget mode.
	inline UInt32 getSubTarget() const { return m_SubTarget; }

	//! Is the target is a rotation.
	inline Bool isRotationTarget() const
	{
		return (m_Target == TARGET_OBJECT_ROT);
	}

	//! Is the target is a translation.
	inline Bool isTranslationTarget() const
	{
		return ((m_Target == TARGET_OBJECT_POS)  ||
			    (m_Target == TARGET_OBJECT_POS_X) ||
				(m_Target == TARGET_OBJECT_POS_Y) ||
				(m_Target == TARGET_OBJECT_POS_Z));
	}

	//! Is the target is a scale.
	inline Bool isScaleTarget() const
	{
		return ((m_Target == TARGET_OBJECT_SCALE)  ||
			    (m_Target == TARGET_OBJECT_SCALE_X) ||
				(m_Target == TARGET_OBJECT_SCALE_Y) ||
				(m_Target == TARGET_OBJECT_SCALE_Z));
	}


	//! get track/evaluator type
	inline TrackType getType() const { return m_track_type; }
	inline Evaluator::Type getEvaluatorType() const { return m_evaluator_type; }

	//! Compute the object animation depend of time.
	virtual const void* compute(Animatable* target, Float time, UInt32 &ValSize) = 0;

	//! Draw the trajectory of the animation.
	virtual void drawTrajectory(SceneObject* object) { /* nothing is default */ }

	//! Find the nearest time according given time.
	void findKeyFrameTime(
			Animatable* target,
			Float startTime,
			Float endTime,
			Float &newStartTime,
			Float &newEndTime) const;

	//! Set the full animation range for pTarget.
	void setFullKeyRange(Animatable* target) const;

	//! Precompute animation range positions.
	void computeKeyRange(Float stime, Float etime);

	//! Use animation track range.
	void useKeyRange(Animatable* target, UInt32 id) const;

	// Serialization
	virtual Bool writeToFile(OutStream &os);
	virtual Bool readFromFile(InStream &is);

protected:

	//! find the keyframe animation depend on time. Depend of the AnimationTrackMode
	void findKeyFrame(
			Animatable* target,
			Float &time,
			KeyFrame *&keyBefore,
			KeyFrame *&keyAfter) const;
};

typedef std::list<AnimationTrack*> T_AnimationTrackList;
typedef T_AnimationTrackList::iterator IT_AnimationTrackList;
typedef T_AnimationTrackList::const_iterator CIT_AnimationTrackList;


//---------------------------------------------------------------------------------------
//! @class AnimationTrack_LinearFloat (single float)
//-------------------------------------------------------------------------------------
//! A linear float AnimationTrack
//---------------------------------------------------------------------------------------
class O3D_API AnimationTrack_LinearFloat : public AnimationTrack
{
	O3D_DEFINE_SERIALIZATION(AnimationTrack_LinearFloat)

public:

	//! default constructor
	AnimationTrack_LinearFloat() : AnimationTrack()
	{
		m_track_type = TRACK_TYPE_FLOAT;
        m_evaluator_type = Evaluator::LINEAR;
	}
	//! constructor
	AnimationTrack_LinearFloat(
			Target target,
			UInt32 subTarget,
			TrackMode trackModeBefore,
			TrackMode trackModeAfter) :
				AnimationTrack(target, subTarget, trackModeBefore, trackModeAfter)
	{
		m_track_type = TRACK_TYPE_FLOAT;
        m_evaluator_type = Evaluator::LINEAR;
	}

	//! compute the object animation depend of time
	virtual const void* compute(Animatable *target, Float time, UInt32 &valSize);

	// Serialization
	virtual Bool readFromFile(InStream &is);

private:

	Float m_Data;
};


//---------------------------------------------------------------------------------------
//! @class AnimationTrack_LinearVector (3 float values)
//-------------------------------------------------------------------------------------
//! A linear vector AnimationTrack
//---------------------------------------------------------------------------------------
class O3D_API AnimationTrack_LinearVector : public AnimationTrack
{
	O3D_DEFINE_SERIALIZATION(AnimationTrack_LinearVector)

public:

	//! default constructor
	AnimationTrack_LinearVector() : AnimationTrack()
	{
		m_track_type = TRACK_TYPE_VECTOR;
        m_evaluator_type = Evaluator::LINEAR;
	}
	//! constructor
	AnimationTrack_LinearVector(
			Target target,
			UInt32 subTarget,
			TrackMode trackModeBefore,
			TrackMode trackModeAfter) :
				AnimationTrack(target, subTarget, trackModeBefore, trackModeAfter)
	{
		m_track_type = TRACK_TYPE_VECTOR;
        m_evaluator_type = Evaluator::LINEAR;
	}

	//! compute the object animation depend of time
	virtual const void* compute(Animatable* pTarget,Float time,UInt32& ValSize);

	//! draw the trajectory of the animation
	virtual void drawTrajectory(SceneObject* pObject);

	// Serialization
	virtual Bool readFromFile(InStream &is);

private:

	Vector3 m_Data;
};


//---------------------------------------------------------------------------------------
//! @class AnimationTrack_LinearQuaternion
//-------------------------------------------------------------------------------------
//! A linear quaternion AnimationTrack
//---------------------------------------------------------------------------------------
class O3D_API AnimationTrack_LinearQuaternion : public AnimationTrack
{
	O3D_DEFINE_SERIALIZATION(AnimationTrack_LinearQuaternion)

public:

	//! default constructor
	AnimationTrack_LinearQuaternion() : AnimationTrack()
	{
		m_track_type = TRACK_TYPE_QUATERNION;
        m_evaluator_type = Evaluator::LINEAR;
	}
	//! constructor
	AnimationTrack_LinearQuaternion(Target target,UInt32 SubTarget,
			TrackMode track_mode_before, TrackMode track_mode_after) :
			AnimationTrack(target,SubTarget,track_mode_before,track_mode_after)
	{
		m_track_type = TRACK_TYPE_QUATERNION;
        m_evaluator_type = Evaluator::LINEAR;
	}

	//! compute the object animation depend of time
	virtual const void* compute(Animatable* pTarget,Float time,UInt32& ValSize);

	// Serialization
	virtual Bool readFromFile(InStream &is);

private:

	Quaternion m_Data;
};


//---------------------------------------------------------------------------------------
//! @class AnimationTrack_SmoothQuaternion
//-------------------------------------------------------------------------------------
//! A smoothed quaternion AnimationTrack
//---------------------------------------------------------------------------------------
class O3D_API AnimationTrack_SmoothQuaternion : public AnimationTrack
{
	O3D_DEFINE_SERIALIZATION(AnimationTrack_SmoothQuaternion)

public:

	//! default constructor
	AnimationTrack_SmoothQuaternion() : AnimationTrack()
	{
		m_track_type = TRACK_TYPE_QUATERNION;
        m_evaluator_type = Evaluator::SMOOTH;
	}
	//! constructor
	AnimationTrack_SmoothQuaternion(Target target,UInt32 SubTarget,
			TrackMode track_mode_before, TrackMode track_mode_after) :
			AnimationTrack(target,SubTarget,track_mode_before,track_mode_after)
	{
		m_track_type = TRACK_TYPE_QUATERNION;
        m_evaluator_type = Evaluator::SMOOTH;
	}

	//! compute the object animation depend of time
	virtual const void* compute(Animatable* pTarget,Float time,UInt32& ValSize);

	// Serialization
	virtual Bool readFromFile(InStream &is);

private:

	Quaternion m_Data;
};


//---------------------------------------------------------------------------------------
//! @class AnimationTrack_BezierFloat
//-------------------------------------------------------------------------------------
//! A bezier with single float AnimationTrack
//---------------------------------------------------------------------------------------
class O3D_API AnimationTrack_BezierFloat : public AnimationTrack
{
	O3D_DEFINE_SERIALIZATION(AnimationTrack_BezierFloat)

public:

	//! default constructor
	AnimationTrack_BezierFloat() : AnimationTrack()
	{
		m_track_type = TRACK_TYPE_FLOAT;
        m_evaluator_type = Evaluator::BEZIER;
	}
	//! constructor
	AnimationTrack_BezierFloat(Target target,UInt32 SubTarget,
			TrackMode track_mode_before, TrackMode track_mode_after) :
			AnimationTrack(target,SubTarget,track_mode_before,track_mode_after)
	{
		m_track_type = TRACK_TYPE_FLOAT;
        m_evaluator_type = Evaluator::BEZIER;
	}

	//! initialize all evaluators of the keyframe
	void initAllEvaluators();
	//! initialize the evaluator of the keyframe
	void initEvaluator(IT_KeyFrameList it);

	//! compute the object animation depend of time
	virtual const void* compute(Animatable* pTarget,Float time,UInt32& ValSize);

	// Serialization
	virtual Bool readFromFile(InStream &is);

private:

	Float m_Data;
};


//---------------------------------------------------------------------------------------
//! @class AnimationTrack_BezierVector
//-------------------------------------------------------------------------------------
//! A bezier with a vector AnimationTrack
//---------------------------------------------------------------------------------------
class O3D_API AnimationTrack_BezierVector : public AnimationTrack
{
	O3D_DEFINE_SERIALIZATION(AnimationTrack_BezierVector)

public:

	//! default constructor
	AnimationTrack_BezierVector() : AnimationTrack()
	{
		m_track_type = TRACK_TYPE_VECTOR;
        m_evaluator_type = Evaluator::BEZIER;
	}
	//! constructor
	AnimationTrack_BezierVector(Target target,UInt32 SubTarget,
			TrackMode track_mode_before, TrackMode track_mode_after) :
			AnimationTrack(target,SubTarget,track_mode_before,track_mode_after)
	{
		m_track_type = TRACK_TYPE_VECTOR;
        m_evaluator_type = Evaluator::BEZIER;
	}

	//! initialize all evaluators of the keyframe
	void initAllEvaluators();
	//! initialize the evaluator of the keyframe
	void initEvaluator(IT_KeyFrameList it);

	//! compute the object animation depend of time
	virtual const void* compute(Animatable* pTarget,Float time,UInt32& ValSize);

	//! draw the trajectory of the animation
	virtual void drawTrajectory(SceneObject* pObject);

	// Serialization
	virtual Bool readFromFile(InStream &is);

private:

	Vector3 m_Data;
};


//---------------------------------------------------------------------------------------
//! @class AnimationTrack_TCBFloat
//-------------------------------------------------------------------------------------
//! A TCB with a single float value AnimationTrack
//---------------------------------------------------------------------------------------
class O3D_API AnimationTrack_TCBFloat : public AnimationTrack
{
	O3D_DEFINE_SERIALIZATION(AnimationTrack_TCBFloat)

public:

	//! default constructor
	AnimationTrack_TCBFloat() : AnimationTrack()
	{
		m_track_type = TRACK_TYPE_FLOAT;
        m_evaluator_type = Evaluator::TCB;
	}
	//! constructor
	AnimationTrack_TCBFloat(Target target,UInt32 SubTarget,
			TrackMode track_mode_before, TrackMode track_mode_after) :
			AnimationTrack(target,SubTarget,track_mode_before,track_mode_after)
	{
		m_track_type = TRACK_TYPE_FLOAT;
        m_evaluator_type = Evaluator::TCB;
	}

	//! initialize all evaluators of the keyframe
	void initAllEvaluators();
	//! initialize the evaluator of the keyframe
	void initEvaluator(IT_KeyFrameList it);

	//! compute the object animation depend of time
	virtual const void* compute(Animatable* pTarget,Float time,UInt32& ValSize);

	// Serialization
	virtual Bool readFromFile(InStream &is);

private:

	Float m_Data;
};


//---------------------------------------------------------------------------------------
//! @class AnimationTrack_TCBVector
//-------------------------------------------------------------------------------------
//! A TCB with a 3d vector AnimationTrack
//---------------------------------------------------------------------------------------
class O3D_API AnimationTrack_TCBVector : public AnimationTrack
{
	O3D_DEFINE_SERIALIZATION(AnimationTrack_TCBVector)

public:

	//! default constructor
	AnimationTrack_TCBVector() : AnimationTrack()
	{
		m_track_type = TRACK_TYPE_VECTOR;
        m_evaluator_type = Evaluator::TCB;
	}
	//! constructor
	AnimationTrack_TCBVector(Target target,UInt32 SubTarget,
			TrackMode track_mode_before, TrackMode track_mode_after) :
			AnimationTrack(target,SubTarget,track_mode_before,track_mode_after)
	{
		m_track_type = TRACK_TYPE_VECTOR;
        m_evaluator_type = Evaluator::TCB;
	}

	//! initialize all evaluators of the keyframe
	void initAllEvaluators();
	//! initialize the evaluator of the keyframe
	void initEvaluator(IT_KeyFrameList it);

	//! compute the object animation depend of time
	virtual const void* compute(Animatable* pTarget,Float time,UInt32& ValSize);

	// Serialization
	virtual Bool readFromFile(InStream &is);

private:

	Vector3 m_Data;
};


//---------------------------------------------------------------------------------------
//! @class AnimationTrack_ConstantFloat
//-------------------------------------------------------------------------------------
//! A constant single float AnimationTrack
//---------------------------------------------------------------------------------------
class O3D_API AnimationTrack_ConstantFloat : public AnimationTrack
{
	O3D_DEFINE_SERIALIZATION(AnimationTrack_ConstantFloat)

public:

	//! default constructor
	AnimationTrack_ConstantFloat() : AnimationTrack()
	{
		m_track_type = TRACK_TYPE_FLOAT;
        m_evaluator_type = Evaluator::CONSTANT;
	}
	//! constructor
	AnimationTrack_ConstantFloat(Target target,UInt32 SubTarget,
			TrackMode track_mode_before, TrackMode track_mode_after) :
			AnimationTrack(target,SubTarget,track_mode_before,track_mode_after)
	{
		m_track_type = TRACK_TYPE_FLOAT;
        m_evaluator_type = Evaluator::CONSTANT;
	}

	//! compute the object animation depend of time
	virtual const void* compute(Animatable* pTarget,Float time,UInt32& ValSize);

	// Serialization
	virtual Bool readFromFile(InStream &is);

private:

	Float m_Data;
};


//---------------------------------------------------------------------------------------
//! @class AnimationTrack_ConstantVector
//-------------------------------------------------------------------------------------
//! A constant 3d vector AnimationTrack
//---------------------------------------------------------------------------------------
class O3D_API AnimationTrack_ConstantVector : public AnimationTrack
{
	O3D_DEFINE_SERIALIZATION(AnimationTrack_ConstantVector)

public:

	//! default constructor
	AnimationTrack_ConstantVector() : AnimationTrack()
	{
		m_track_type = TRACK_TYPE_VECTOR;
        m_evaluator_type = Evaluator::CONSTANT;
	}
	//! constructor
	AnimationTrack_ConstantVector(Target target,UInt32 SubTarget,
			TrackMode track_mode_before, TrackMode track_mode_after) :
			AnimationTrack(target,SubTarget,track_mode_before,track_mode_after)
	{
		m_track_type = TRACK_TYPE_VECTOR;
        m_evaluator_type = Evaluator::CONSTANT;
	}

	//! compute the object animation depend of time
	virtual const void* compute(Animatable* pTarget,Float time,UInt32& ValSize);

	// Serialization
	virtual Bool readFromFile(InStream &is);

private:

	Vector3 m_Data;
};


//---------------------------------------------------------------------------------------
//! @class AnimationTrack_ConstantQuaternion
//-------------------------------------------------------------------------------------
//! A constant quaternion AnimationTrack
//---------------------------------------------------------------------------------------
class O3D_API AnimationTrack_ConstantQuaternion : public AnimationTrack
{
	O3D_DEFINE_SERIALIZATION(AnimationTrack_ConstantQuaternion)

public:

	//! default constructor
	AnimationTrack_ConstantQuaternion() : AnimationTrack()
	{
		m_track_type = TRACK_TYPE_QUATERNION;
        m_evaluator_type = Evaluator::CONSTANT;
	}
	//! constructor
	AnimationTrack_ConstantQuaternion(
			Target target,
			UInt32 subTarget,
			TrackMode trackModeBefore,
			TrackMode trackModeAfter) :
				AnimationTrack(target, subTarget, trackModeBefore, trackModeAfter)
	{
		m_track_type = TRACK_TYPE_QUATERNION;
        m_evaluator_type = Evaluator::CONSTANT;
	}

	//! compute the object animation depend of time
	virtual const void* compute(Animatable* target, Float time, UInt32 &valSize);

	// Serialization
	virtual Bool readFromFile(InStream &is);

private:

	Quaternion m_Data;
};


//---------------------------------------------------------------------------------------
//! @class AnimationTrack_ConstantBool
//-------------------------------------------------------------------------------------
//! A constant 2 state value AnimationTrack
//---------------------------------------------------------------------------------------
class O3D_API AnimationTrack_ConstantBool : public AnimationTrack
{
	O3D_DEFINE_SERIALIZATION(AnimationTrack_ConstantBool)

public:

	//! default constructor
	AnimationTrack_ConstantBool() : AnimationTrack()
	{
		m_track_type = TRACK_TYPE_BOOL;
        m_evaluator_type = Evaluator::CONSTANT;
	}
	//! constructor
	AnimationTrack_ConstantBool(
			Target target,
			UInt32 subTarget,
			TrackMode trackModeBefore,
			TrackMode trackModeAfter) :
				AnimationTrack(target, subTarget, trackModeBefore, trackModeAfter)
	{
		m_track_type = TRACK_TYPE_BOOL;
        m_evaluator_type = Evaluator::CONSTANT;
	}

	//! compute the object animation depend of time
	virtual const void* compute(Animatable* target, Float time, UInt32 &valSize);

	// Serialization
	virtual Bool readFromFile(InStream &is);

private:

	Bool m_Data;
};

} // namespace o3d

#endif // _O3D_ANIMATIONTRACK_H

