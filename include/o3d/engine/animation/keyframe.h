/**
 * @file keyframe.h
 * @brief A key frame is an element of an animation track.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2005-02-03
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_KEYFRAME_H
#define _O3D_KEYFRAME_H

#include "evaluator.h"
#include "o3d/core/serialize.h"
#include "o3d/core/vector3.h"
#include "o3d/core/memorydbg.h"

#include <list>

namespace o3d {

// avoid C4127 warning on VC compilers
#ifdef _MSC_VER
#pragma warning(push,3)
#endif

//---------------------------------------------------------------------------------------
//! @class KeyFrame
//-------------------------------------------------------------------------------------
//! base class for all KeyFrame.
//---------------------------------------------------------------------------------------
class O3D_API KeyFrame : public Serialize
{
public:

	//! Default constructor.
	KeyFrame(Float time = 0.f) { m_time = time; }

	//! Set the keyframe time.
	inline void setTime(Float time) { m_time = time; }
	//! Get the keyframe time.
	inline Float getTime() const {return m_time; }

	// Serialization
    virtual Bool writeToFile(OutStream &os)  { os << m_time; return True; }
    virtual Bool readFromFile(InStream &is) { is >> m_time; return True; }

protected:

	Float m_time; //!< time position of the animation keyframe (0<m_time<1)
};

typedef std::list<KeyFrame*> T_KeyFrameList;
typedef T_KeyFrameList::iterator IT_KeyFrameList;
typedef T_KeyFrameList::const_iterator CIT_KeyFrameList;


//---------------------------------------------------------------------------------------
//! @class KeyFrameLinear
//-------------------------------------------------------------------------------------
//! a linear KeyFrame
//---------------------------------------------------------------------------------------
template <class T>
class O3D_API_TEMPLATE KeyFrameLinear : public KeyFrame
{
	O3D_DEFINE_SERIALIZATION(KeyFrameLinear)

public:

	T Data; //!< data associated to the keyframe (can be a 3d vector or a single float scalar)

	//! constructor
	KeyFrameLinear<T>(Float time = 0.f, const T &data = T()): KeyFrame(time) { Data = data; }

	// Serialization
	virtual Bool writeToFile(OutStream &os)
	{
        os << m_time
           << Data;

		return True;
	}
	virtual Bool readFromFile(InStream &is)
	{
        is >> m_time
           >> Data;

		return True;
	}
};


//---------------------------------------------------------------------------------------
//! @class KeyFrameSmooth
//-------------------------------------------------------------------------------------
//! A smoothing KeyFrame
//---------------------------------------------------------------------------------------
template <class T>
class O3D_API_TEMPLATE KeyFrameSmooth : public KeyFrame
{
	O3D_DEFINE_SERIALIZATION(KeyFrameSmooth)

public:

	T Data; //!< data associated to the keyframe (can be a 3d vector or a single float scalar)

	// constructor
	KeyFrameSmooth<T>(Float time = 0.f, const T &data = T()): KeyFrame(time) { Data = data; }

	// Serialization
	virtual Bool writeToFile(OutStream &os)
	{
        os << m_time
           << Data;

		return True;
	}
	virtual Bool readFromFile(InStream &is)
	{
        is >> m_time
           >> Data;

		return True;
	}
};


//---------------------------------------------------------------------------------------
//! @class KeyFrameSmooth
//-------------------------------------------------------------------------------------
//! A constant KeyFrame
//---------------------------------------------------------------------------------------
template <class T>
class O3D_API_TEMPLATE KeyFrameConstant : public KeyFrame
{
	O3D_DEFINE_SERIALIZATION(KeyFrameConstant)

public:

	T Data; //!< data associated to the keyframe (can be a 3d vector or a single float scalar)

	// constructor
	KeyFrameConstant<T>(Float time = 0.f, const T &data = T()): KeyFrame(time) { Data = data; }

	// Serialization
	virtual Bool writeToFile(OutStream &os)
	{
        os << m_time
           << Data;

		return True;
	}
	virtual Bool readFromFile(InStream &is)
	{
        is >> m_time
           >> Data;

		return True;
	}
};


//---------------------------------------------------------------------------------------
//! @class KeyFrameBezier
//-------------------------------------------------------------------------------------
//! A bezier KeyFrame
//---------------------------------------------------------------------------------------
template <class T>
class O3D_API_TEMPLATE KeyFrameBezier : public KeyFrame
{
	O3D_DEFINE_SERIALIZATION(KeyFrameBezier)

public:

	T Data; //!< data associated to the keyframe (can be a 3d vector or a single float scalar)

	Vector2f* TangentLeft;  //  left tangent list (1 or 3)
	Vector2f* TangentRight; // right tangent list (1 or 3)

	Evaluator1D_Bezier* Evaluator;	// evaluator list (1 or 3)


	// constructor
	KeyFrameBezier<T>(Float time = 0.f, const T &data = T()): KeyFrame(time)
	{
		Data = data;
		TangentLeft  = NULL;
		TangentRight = NULL;
		Evaluator = NULL;
	}

	// destructor
	~KeyFrameBezier<T>()
	{
		if (sizeof(T) == sizeof(Vector3))
		{
			deleteArray(TangentLeft);
			deleteArray(TangentRight);
			deleteArray(Evaluator);
		}
		else if (sizeof(T) == sizeof(Float))
		{
			deletePtr(TangentLeft);
			deletePtr(TangentRight);
			deletePtr(Evaluator);
		}
	}

	// Serialization
	virtual Bool writeToFile(OutStream &os)
	{
        os << m_time
           << Data;

		// write tangents list
		if (sizeof(T) == sizeof(Vector3))
		{
            os   << TangentLeft[X]
	 			 << TangentLeft[Y]
			  	 << TangentLeft[Z]
				 << TangentRight[X]
				 << TangentRight[Y]
			 	 << TangentRight[Z];
		}
		else if (sizeof(T) == sizeof(Float))
		{
            os   << *TangentLeft
			     << *TangentRight;
		}

		return True;
	}

	virtual Bool readFromFile(InStream &is)
	{
        is >> m_time
           >> Data;

		// read tangents list
		if (sizeof(T) == sizeof(Vector3))
		{
			Evaluator    = new Evaluator1D_Bezier[3];
			TangentLeft  = new Vector2f[3];
			TangentRight = new Vector2f[3];

            is   >> TangentLeft[X]
	 			 >> TangentLeft[Y]
			  	 >> TangentLeft[Z]
				 >> TangentRight[X]
				 >> TangentRight[Y]
			 	 >> TangentRight[Z];
		}
		else if (sizeof(T) == sizeof(Float))
		{
			Evaluator    = new Evaluator1D_Bezier;
			TangentLeft  = new Vector2f;
			TangentRight = new Vector2f;

            is   >> *TangentLeft
			     >> *TangentRight;
		}

		return True;
	}
};


//---------------------------------------------------------------------------------------
//! @class KeyFrameTCB
//-------------------------------------------------------------------------------------
//! A TCB spline KeyFrame
//---------------------------------------------------------------------------------------
template <class T>
class O3D_API_TEMPLATE KeyFrameTCB : public KeyFrame
{
	O3D_DEFINE_SERIALIZATION(KeyFrameTCB)

public:

	T Data; //!< data associated to the keyframe (can be a 3d vector or a single float scalar)

	Float* Tension;     // tension list (1 or 3)
	Float* Continuity;  // continuity list (1 or 3)
	Float* Bias;        // bias list (1 or 3)

	Evaluator1D_TCB* Evaluator; // evaluator list (1 or 3)

	// constructor
	KeyFrameTCB<T>(Float time = 0.f, const T &data = T()): KeyFrame(time)
	{
		Data       = data;
		Tension    = NULL;
		Continuity = NULL;
		Bias       = NULL;
		Evaluator  = NULL;
	}
	// destructor
	~KeyFrameTCB<T>()
	{
		if (sizeof(T) == sizeof(Vector3))
		{
			deleteArray(Tension);
			deleteArray(Continuity);
			deleteArray(Bias);
			deleteArray(Evaluator);
		}
		else if (sizeof(T) == sizeof(Float))
		{
			deletePtr(Tension);
			deletePtr(Continuity);
			deletePtr(Bias);
			deletePtr(Evaluator);
		}
	}

	// Serialization
	virtual Bool writeToFile(OutStream &os)
	{
        os << m_time
           << Data;

		// write tangents list
		if (sizeof(T) == sizeof(Vector3))
		{
            os   << Tension[X]
	 			 << Tension[Y]
			  	 << Tension[Z]
				 << Continuity[X]
				 << Continuity[Y]
			 	 << Continuity[Z]
				 << Bias[X]
				 << Bias[Y]
			 	 << Bias[Z];
		}
		else if (sizeof(T) == sizeof(Float))
		{
            os   << *Tension
			     << *Continuity
				 << *Bias;
		}

		return True;
	}

	virtual Bool readFromFile(InStream &is)
	{
        is >> m_time
           >> Data;

		// read tangents list
		if (sizeof(T) == sizeof(Vector3))
		{
			Evaluator  = new Evaluator1D_TCB[3];
			Tension    = new Float[3];
			Continuity = new Float[3];
			Bias       = new Float[3];

            is   >> Tension[X]
	 			 >> Tension[Y]
			  	 >> Tension[Z]
				 >> Continuity[X]
				 >> Continuity[Y]
			 	 >> Continuity[Z]
				 >> Bias[X]
				 >> Bias[Y]
                 >> Bias[Z];
		}
		else if (sizeof(T) == sizeof(Float))
		{
			Evaluator  = new Evaluator1D_TCB;
			Tension    = new Float;
			Continuity = new Float;
			Bias       = new Float;

            is   >> *Tension
			     >> *Continuity
				 >> *Bias;
		}

		return True;
	}
};

#ifdef _MSC_VER
#pragma warning(pop)
#endif

} // namespace o3d

#endif // _O3D_KEYFRAME_H

