/**
 * @file animationtrack.cpp
 * @brief Implementation of AnimationTrack.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-02-03
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/animation/animationtrack.h"
#include "o3d/engine/object/primitivemanager.h"
#include "o3d/engine/scene/sceneobject.h"

#include "o3d/core/debug.h"

#include "o3d/engine/matrix.h"
#include "o3d/engine/context.h"
#include "o3d/engine/scene/scene.h"

using namespace o3d;

//---------------------------------------------------------------------------------------
// AnimationTrack
//---------------------------------------------------------------------------------------

// Destructor
AnimationTrack::~AnimationTrack()
{
	for (IT_KeyFrameList it = m_keyFrameList.begin(); it != m_keyFrameList.end(); ++it)
		deletePtr(*it);
}

// Find the keyframe animation depend on time. Depend of the TrackMode
void AnimationTrack::findKeyFrame(
		Animatable* target,
		Float &time,
		KeyFrame*& keyBefore,
		KeyFrame*& keyAfter) const
{
	Animatable::AnimatableTrack& animStatus = *target->getAnimationStatus(this);

    Float startTime = (*animStatus.First)->getTime();
	Float endTime = (*animStatus.Last)->getTime();

	// empty time
	if (startTime == endTime)
	{
		keyBefore = keyAfter = (*animStatus.First);
		return;
	}

	Int32 c = 0;

	// first get the correct time pos
    if (time < startTime)
	{
		switch (m_TrackMode_Before)
		{
		case TRACK_MODE_CONSTANT:
			// constant then always the first key
			keyBefore = keyAfter = *animStatus.First;
			return;
		case TRACK_MODE_LOOP:
			// compute the correct time pos
			while (time < startTime) { time += endTime - startTime; }
			break;
		case TRACK_MODE_PING_PONG:
			// compute the correct time pos
			while (time < startTime) { time += endTime - startTime; ++c; }
			// symmetries the time only for non pair offset (pingpong need to be symmetric)
			if ((c%2) != 0) time = startTime + endTime - time;
			break;
		default:
			O3D_WARNING("TrackMode_Before unknown");
			keyBefore = keyAfter = *animStatus.First;
			return;
		}
	}
    else if (time > endTime)
	{
		switch (m_TrackMode_Before)
		{
		case TRACK_MODE_CONSTANT:
			// constant then always the last key
			keyBefore = keyAfter = *animStatus.Last;
			return;
		case TRACK_MODE_LOOP:
			// compute the correct time pos
            while (time > endTime) { time -= endTime - startTime; }
			break;
		case TRACK_MODE_PING_PONG:
			// compute the correct time pos
            while (time > endTime) { time -= endTime - startTime; ++c; }
			// symmetries the time only for non pair offset (pingpong need to be symmetric)
			if ((c%2) != 0) time = startTime + endTime - time;
			break;
		default:
			O3D_WARNING("m_TrackMode_Before unknown");
			keyBefore = keyAfter = *animStatus.Last;
			return;
		}
	}

	// find good keyframes
	CIT_KeyFrameList nextIt;

	// forward
	if (animStatus.Time < time)
	{
		if ((time - animStatus.Time) > ((endTime - startTime) / 2))
		{
			animStatus.Current = animStatus.First;
		}

		nextIt = animStatus.Current;
        while (((*nextIt)->getTime() < time) && (nextIt != animStatus.Last))
		{
			animStatus.Current = nextIt;
			++nextIt;
		}
	}
	// backward
	else if (animStatus.Time > time)
	{
		if ((animStatus.Time - time) > ((endTime - startTime) / 2))
		{
			animStatus.Current = animStatus.First;
		}

		nextIt = animStatus.Current;
        while (((*nextIt)->getTime() > time) && (nextIt != animStatus.First))
		{
			--nextIt;
			animStatus.Current = nextIt;
		}
	}

	// store the previous time
	animStatus.Time = time;

	nextIt = animStatus.Current;
	++nextIt;

	keyBefore = *animStatus.Current;
	keyAfter = *nextIt;
}

void AnimationTrack::drawTrajectory(SceneObject*)
{
    /* nothing is default */
}

// Find the nearest keyframe time according a given time
void AnimationTrack::findKeyFrameTime(
		Animatable* target,
		Float startTime,
		Float endTime,
		Float &newStartTime,
		Float &newEndTime) const
{
	Animatable::AnimatableTrack& AnimStatut = *target->getAnimationStatus(this);

	CIT_KeyFrameList nextIt;

	KeyFrame* keyFrame;
	KeyFrame* nextKeyFrame;

	// start time
	if (startTime == 0.f)
	{
		AnimStatut.First = m_keyFrameList.begin();
		AnimStatut.Current = m_keyFrameList.begin();
		AnimStatut.Time = 0.f;

		newStartTime = 0.f;
	}
	else
	{
		// find good key frame for start time
		CIT_KeyFrameList it = m_keyFrameList.begin();
		for (; it != m_keyFrameList.end(); ++it)
		{
			keyFrame = *it;
            nextKeyFrame = nullptr;

			nextIt = it;
			++nextIt;

			if (nextIt != m_keyFrameList.end())
				nextKeyFrame = (*nextIt);

            if ((keyFrame->getTime() <= startTime) && (nextKeyFrame != nullptr) && (nextKeyFrame->getTime() > startTime))
			{
				// key found
				newStartTime = keyFrame->getTime();

				AnimStatut.First = it;
				AnimStatut.Current = it;
				AnimStatut.Time = newStartTime;
				break;
			}
		}

		if (it == m_keyFrameList.end())
		{
			AnimStatut.Time = newEndTime = newStartTime;
			AnimStatut.First = AnimStatut.Last = AnimStatut.Current = m_keyFrameList.begin();
//			O3D_ERROR(O3D_E_IndexOutOfRange("The start time is not found"));
			return;
		}
	}

	// end time
	if (endTime == 1.f)
	{
		AnimStatut.Last = m_keyFrameList.end();
		--AnimStatut.Last;

		newEndTime = 1.f;
	}
	else
	{
		// find good key frame for end time
		newEndTime = newStartTime;
		AnimStatut.Last = AnimStatut.First;

		for (CIT_KeyFrameList it = AnimStatut.First; it != m_keyFrameList.end(); ++it)
		{
			keyFrame = *it;

			if (keyFrame->getTime() >= endTime)
			{
				// key found
				newEndTime = keyFrame->getTime();

				AnimStatut.Last = it;
				break;
			}
		}
	}
}

// Set the full animation range for a target
void AnimationTrack::setFullKeyRange(Animatable *target) const
{
	Animatable::AnimatableTrack& animStatut = *target->getAnimationStatus(this);

	animStatut.Time = 0.f;
	animStatut.Current = m_keyFrameList.begin();
	animStatut.First = m_keyFrameList.begin();
	animStatut.Last  = m_keyFrameList.end();
	--animStatut.Last;
}

// Precompute animation range positioning
void AnimationTrack::computeKeyRange(Float startTime, Float endTime)
{
	TrackRange newRange;

	CIT_KeyFrameList nextIt;

	KeyFrame* keyFrame;
	KeyFrame* nextKeyFrame;

	// start time
	if (startTime == 0.f)
	{
		newRange.First = m_keyFrameList.begin();
	}
	else
	{
		// find good key frame for start time
		CIT_KeyFrameList it = m_keyFrameList.begin();
		for (; it != m_keyFrameList.end(); ++it)
		{
			keyFrame = *it;
            nextKeyFrame = nullptr;

			nextIt = it;
			++nextIt;

			if (nextIt != m_keyFrameList.end())
				nextKeyFrame = (*nextIt);

            if ((keyFrame->getTime() <= startTime) && (nextKeyFrame != nullptr) && (nextKeyFrame->getTime() > startTime))
			{
				// key found
				newRange.First = it;
				break;
			}
		}

		if (it == m_keyFrameList.end())
		{
			newRange.First = newRange.Last = m_keyFrameList.begin();
			//O3D_ERROR(O3D_E_IndexOutOfRange("The start time is not found"));
		}
	}

	// end time
	if (endTime == 1.f)
	{
		newRange.Last = m_keyFrameList.end();
		--newRange.Last;
	}
	else
	{
		// find good key frame for end time
		newRange.Last = newRange.First;

		for (CIT_KeyFrameList it = newRange.First; it != m_keyFrameList.end(); ++it)
		{
			keyFrame = *it;

			if (keyFrame->getTime() >= endTime)
			{
				// key found
				newRange.Last = it;
				break;
			}
		}
	}

	m_trackRangeVector.push_back(newRange);
}

// Use animation track range
void AnimationTrack::useKeyRange(Animatable* target, UInt32 id) const
{
	O3D_ASSERT(id < m_trackRangeVector.size());

	Animatable::AnimatableTrack& animStatut = *target->getAnimationStatus(this);
	const TrackRange &range = m_trackRangeVector[id];

	animStatut.First   = range.First;
	animStatut.Last    = range.Last;
	animStatut.Current = range.First;
}

// Serialization
Bool AnimationTrack::writeToFile(OutStream &os)
{
    os   << m_Target
		 << m_SubTarget
		 << m_Length
		 << m_TrackMode_Before
		 << m_TrackMode_After;

    os << (Int32)(m_keyFrameList.size());

	// export keyframes recursively
	for (IT_KeyFrameList it = m_keyFrameList.begin(); it != m_keyFrameList.end(); ++it)
        os << *(*it);

	return True;
}

Bool AnimationTrack::readFromFile(InStream &is)
{
	m_Time = o3d::Limits<Float>::max();

    is   >> m_Target
		 >> m_SubTarget
		 >> m_Length
		 >> m_TrackMode_Before
		 >> m_TrackMode_After;

	return True;
}

/*---------------------------------------------------------------------------------------
  class AnimationTrack_LinearFloat
---------------------------------------------------------------------------------------*/
const void* AnimationTrack_LinearFloat::compute(
		Animatable* target,
		Float time,
		UInt32& ValSize)
{
	ValSize = sizeof(m_Data);
	if (time == m_Time)
		return(&m_Data);

	// find keyframe couple
    const KeyFrameLinear<Float> *pKeyBefore = nullptr, *pKeyAfter = nullptr;
	findKeyFrame(target, time, (KeyFrame*&)pKeyBefore, (KeyFrame*&)pKeyAfter);

	Float tBefore = pKeyBefore->getTime();  // key time start
	Float tAfter  = pKeyAfter->getTime();   // key time end

	Float coef = 0.f;

	// compute the linear interpolation coef
	if (fabs(tBefore - tAfter) > o3d::Limits<Float>::epsilon())
		coef = (time - tBefore) / (tAfter - tBefore);

	// compute the animation value with coef
	m_Data = pKeyBefore->Data * (1 - coef) + pKeyAfter->Data * coef;
	m_Time = time;

	// finally return the data
	return (&m_Data);
}

Bool AnimationTrack_LinearFloat::readFromFile(InStream &is)
{
    AnimationTrack::readFromFile(is);

	UInt32 nKeys;
    is >> nKeys;

	// read all keys
	for (UInt32 i = 0 ; i < nKeys ; ++i)
	{
		KeyFrameLinear<Float>* pKeyFrame = new KeyFrameLinear<Float>(0,0);
        is >> *pKeyFrame;
		m_keyFrameList.push_back(pKeyFrame);
	}
	return True;
}

/*---------------------------------------------------------------------------------------
  class AnimationTrack_LinearVector
---------------------------------------------------------------------------------------*/
const void* AnimationTrack_LinearVector::compute(
		Animatable* target,
		Float time,
		UInt32& ValSize)
{
	ValSize = sizeof(m_Data);
	if (time == m_Time)
		return(&m_Data);

	// find keyframe couple
    const KeyFrameLinear<Vector3> *pKeyBefore = nullptr, *pKeyAfter = nullptr;
	findKeyFrame(target,time,(KeyFrame*&)pKeyBefore,(KeyFrame*&)pKeyAfter);

	Float tBefore = pKeyBefore->getTime();  // key time start
	Float tAfter  = pKeyAfter->getTime();   // key time end

	Float coef = 0.f;

	// compute the linear interpolation coef
	if (fabs(tBefore - tAfter) > o3d::Limits<Float>::epsilon())
		coef = (time - tBefore) / (tAfter - tBefore);

	// compute the animation value with coef
	m_Data = pKeyBefore->Data * (1 - coef) + pKeyAfter->Data * coef;
	m_Time = time;

	// finally return the data
	return (&m_Data);
}

void AnimationTrack_LinearVector::drawTrajectory(SceneObject *object)
{
	Context *glContext = object->getScene()->getContext();
	PrimitiveAccess primitive = object->getScene()->getPrimitiveManager()->access();

	glContext->modelView().push();

	Matrix4 mat = object->getPrevAnimationMatrix().invert() * (object->getAbsoluteMatrix());
	glContext->modelView().mult(mat);

	glContext->setPointSize(5);

	// draw first point
	Vector3 pos = ((KeyFrameLinear<Vector3>*)m_keyFrameList.front())->Data;

	primitive->setColor(1,1,1);
	primitive->beginDraw(P_POINTS);
		primitive->addVertex(pos.getData());
	primitive->endDraw();

	// draw lines and others key point
	for(IT_KeyFrameList it = m_keyFrameList.begin(); it != m_keyFrameList.end(); ++it)
	{
		KeyFrameLinear<Vector3> *pKey = (KeyFrameLinear<Vector3>*)*it;

		primitive->setColor(1.f,1.f,0.f);
		primitive->beginDraw(P_LINES);
			primitive->addVertex(pos.getData());
			primitive->addVertex(pKey->Data.getData());
		primitive->endDraw();

		pos = pKey->Data;

		primitive->setColor(1,1,1);
		primitive->beginDraw(P_POINTS);
			primitive->addVertex(pos.getData());
		primitive->endDraw();
	}

	glContext->setDefaultPointSize();

	glContext->modelView().pop();
}

Bool AnimationTrack_LinearVector::readFromFile(InStream &is)
{
    AnimationTrack::readFromFile(is);

	UInt32 nKeys;
    is >> nKeys;

	// read all keys
	for (UInt32 i = 0 ; i < nKeys ; ++i)
	{
		KeyFrameLinear<Vector3>* pKeyFrame = new KeyFrameLinear<Vector3>(0,Vector3());
        is >> *pKeyFrame;
		m_keyFrameList.push_back(pKeyFrame);
	}
	return True;
}

/*---------------------------------------------------------------------------------------
  class AnimationTrack_LinearQuaternion
---------------------------------------------------------------------------------------*/
const void* AnimationTrack_LinearQuaternion::compute(
		Animatable *target,
		Float time,
		UInt32 &ValSize)
{
	ValSize = sizeof(m_Data);
	if (time == m_Time)
		return(&m_Data);

	// find keyframe couple
    const KeyFrameLinear<Quaternion> *pKeyBefore = nullptr, *pKeyAfter = nullptr;
	findKeyFrame(target,time,(KeyFrame*&)pKeyBefore,(KeyFrame*&)pKeyAfter);

	Float tBefore = pKeyBefore->getTime();  // key time start
	Float tAfter  = pKeyAfter->getTime();   // key time end

	Float coef = 0.f;

	// compute the linear interpolation coef
	if (fabs(tBefore - tAfter) > o3d::Limits<Float>::epsilon())
		coef = (time - tBefore) / (tAfter - tBefore);

	// compute the animation value with coef (m_Data is normalized)
	pKeyBefore->Data.lerp(pKeyAfter->Data, coef, m_Data);
	m_Time = time;

	// finally return the data
	return (&m_Data);
}

Bool AnimationTrack_LinearQuaternion::readFromFile(InStream &is)
{
    AnimationTrack::readFromFile(is);

	UInt32 nKeys;
    is >> nKeys;

	// read all keys
	for (UInt32 i = 0 ; i < nKeys ; ++i)
	{
		KeyFrameLinear<Quaternion>* pKeyFrame = new KeyFrameLinear<Quaternion>(0,Quaternion());
        is >> *pKeyFrame;
		m_keyFrameList.push_back(pKeyFrame);
	}
	return True;
}

/*---------------------------------------------------------------------------------------
  class AnimationTrack_SmoothQuaternion
---------------------------------------------------------------------------------------*/
const void* AnimationTrack_SmoothQuaternion::compute(
		Animatable *pTarget,
		Float time,
		UInt32 &ValSize)
{
	ValSize = sizeof(m_Data);
	if (time == m_Time)
		return(&m_Data);

	// find keyframe couple
    const KeyFrameSmooth<Quaternion> *pKeyBefore = nullptr, *pKeyAfter = nullptr;
	findKeyFrame(pTarget,time,(KeyFrame*&)pKeyBefore,(KeyFrame*&)pKeyAfter);

	Float tBefore = pKeyBefore->getTime();  // key time start
	Float tAfter  = pKeyAfter->getTime();   // key time end

	Float coef = 0.f;

	// compute the linear interpolation coef
	if (fabs(tBefore - tAfter) > o3d::Limits<Float>::epsilon())
		coef = (time - tBefore) / (tAfter - tBefore);

	// compute the animation value with coef to sphere unit
	pKeyBefore->Data.slerp(pKeyAfter->Data, coef, m_Data);
	m_Time = time;

	// finally return the data
	return (&m_Data);
}

Bool AnimationTrack_SmoothQuaternion::readFromFile(InStream &is)
{
    AnimationTrack::readFromFile(is);

	UInt32 nKeys;
    is >> nKeys;

	// read all keys
	for (UInt32 i = 0 ; i < nKeys ; ++i)
	{
		KeyFrameSmooth<Quaternion>* pKeyFrame = new KeyFrameSmooth<Quaternion>(0,Quaternion());
        is >> *pKeyFrame;
		m_keyFrameList.push_back(pKeyFrame);
	}
	return True;
}

/*---------------------------------------------------------------------------------------
  class AnimationTrack_BezierFloat
---------------------------------------------------------------------------------------*/
void AnimationTrack_BezierFloat::initEvaluator(IT_KeyFrameList it)
{
	Float P1[2],P2[2];

	KeyFrameBezier<Float>* pKey0 = (KeyFrameBezier<Float>*) *it;
	KeyFrameBezier<Float>* pKey1;

	IT_KeyFrameList it_next(it);
	it_next++;

	if (it_next != m_keyFrameList.end())
		pKey1 = (KeyFrameBezier<Float>*)*(it_next);
	else
		pKey1 = (KeyFrameBezier<Float>*)m_keyFrameList.front();

	// compute bezier curve control points 1 and 2
	P1[X] = pKey0->TangentRight[0][X];
	P1[Y] = pKey0->Data + pKey0->TangentRight[0][Y];
	P2[X] = 1 + pKey1->TangentLeft[0][X];
	P2[Y] = pKey1->Data + pKey1->TangentLeft[0][Y];//Right[0][Y];

	// initialize evaluator
	pKey0->Evaluator[0].initialize(pKey0->Data,P1,P2,pKey1->Data);
}

void AnimationTrack_BezierFloat::initAllEvaluators()
{
	for(IT_KeyFrameList it = m_keyFrameList.begin(); it != m_keyFrameList.end(); ++it)
		initEvaluator(it);
}

const void* AnimationTrack_BezierFloat::compute(
		Animatable* target,
		Float time,
		UInt32 &ValSize)
{
	ValSize = sizeof(m_Data);
	if (time == m_Time)
		return(&m_Data);

	// find keyframe couple
    const KeyFrameBezier<Float> *pKeyBefore = nullptr, *pKeyAfter = nullptr;
	findKeyFrame(target,time,(KeyFrame*&)pKeyBefore,(KeyFrame*&)pKeyAfter);

	Float tBefore = pKeyBefore->getTime();  // key time start
	Float tAfter  = pKeyAfter->getTime();   // key time end

	Float coef = 0.f;

	// compute the linear interpolation coef
	if (fabs(tBefore - tAfter) > o3d::Limits<Float>::epsilon())
		coef = (time - tBefore) / (tAfter - tBefore);

	// compute the animation value with coef to sphere unit
	m_Data = pKeyBefore->Evaluator[0].evaluate(coef);
	m_Time = time;

	// finally return the data
	return (&m_Data);
}

Bool AnimationTrack_BezierFloat::readFromFile(InStream &is)
{
    AnimationTrack::readFromFile(is);

	UInt32 nKeys;
    is >> nKeys;

	// read all keys
	for (UInt32 i = 0 ; i < nKeys ; ++i)
	{
		KeyFrameBezier<Float>* pKeyFrame = new KeyFrameBezier<Float>(0,0);
        is >> *pKeyFrame;
		m_keyFrameList.push_back(pKeyFrame);
	}

	initAllEvaluators();

	return True;
}

/*---------------------------------------------------------------------------------------
  class AnimationTrack_BezierVector
---------------------------------------------------------------------------------------*/
void AnimationTrack_BezierVector::initEvaluator(IT_KeyFrameList it)
{
	Float P1[2],P2[2];

	KeyFrameBezier<Vector3>* pKey0 = (KeyFrameBezier<Vector3>*) *it;
	KeyFrameBezier<Vector3>* pKey1;

	IT_KeyFrameList it_next(it);
	it_next++;

	if (it_next != m_keyFrameList.end())
		pKey1 = (KeyFrameBezier<Vector3>*)*(it_next);
	else
		pKey1 = (KeyFrameBezier<Vector3>*)m_keyFrameList.front();

	// compute bezier curve control points 1 and 2
	for (UInt32 i = 0 ; i < 3 ; ++i)
	{
		P1[X] = pKey0->TangentRight[i][X];
		P1[Y] = pKey0->Data[i] + pKey0->TangentRight[i][Y];
		P2[X] = 1 + pKey1->TangentLeft[i][X];
		P2[Y] = pKey1->Data[i] + pKey1->TangentLeft[i][Y];

		// initialize evaluator
		pKey0->Evaluator[i].initialize(pKey0->Data[i],P1,P2,pKey1->Data[i]);
	}
}

void AnimationTrack_BezierVector::initAllEvaluators()
{
	for(IT_KeyFrameList it = m_keyFrameList.begin() ; it != m_keyFrameList.end() ; ++it)
		initEvaluator(it);
}

const void* AnimationTrack_BezierVector::compute(
		Animatable* target,
		Float time,
		UInt32 &ValSize)
{
	ValSize = sizeof(m_Data);
	if (time == m_Time)
		return(&m_Data);

	// find keyframe couple
    const KeyFrameBezier<Vector3> *pKeyBefore = nullptr, *pKeyAfter = nullptr;
	findKeyFrame(target,time,(KeyFrame*&)pKeyBefore,(KeyFrame*&)pKeyAfter);

	Float tBefore = pKeyBefore->getTime();  // key time start
	Float tAfter  = pKeyAfter->getTime();   // key time end

	Float coef = 0.f;

	// compute the linear interpolation coef
	if (fabs(tBefore - tAfter) > o3d::Limits<Float>::epsilon())
		coef = (time - tBefore) / (tAfter - tBefore);

	// compute animation values with coef
	m_Data[X] = pKeyBefore->Evaluator[X].evaluate(coef);
	m_Data[Y] = pKeyBefore->Evaluator[Y].evaluate(coef);
	m_Data[Z] = pKeyBefore->Evaluator[Z].evaluate(coef);
	m_Time = time;

	// finaly return the data
	return (&m_Data);
}

Bool AnimationTrack_BezierVector::readFromFile(InStream &is)
{
    AnimationTrack::readFromFile(is);

	UInt32 nKeys;
    is >> nKeys;

	// read all keys
	for (UInt32 i = 0 ; i < nKeys ; ++i)
	{
		KeyFrameBezier<Vector3>* pKeyFrame = new KeyFrameBezier<Vector3>(0,Vector3());
        is >> *pKeyFrame;
		m_keyFrameList.push_back(pKeyFrame);
	}

	initAllEvaluators();

	return True;
}

#define O3D_DRAW_TANGENT_FACTOR 0.3f

void AnimationTrack_BezierVector::drawTrajectory(SceneObject* object)
{
	Context *glContext = object->getScene()->getContext();
	PrimitiveAccess primitive = object->getScene()->getPrimitiveManager()->access();

	glContext->modelView().push();

	Matrix4 mat = object->getPrevAnimationMatrix().invert() * (object->getAbsoluteMatrix());
	glContext->modelView().mult(mat);

	// change point size and switch to simple draw mode
	glContext->setPointSize(3);

	// draw all keys
	Vector3 pos;
	Vector3 tangent;
	KeyFrameBezier<Vector3> *key;
	Bool first = True;

	for(IT_KeyFrameList it = m_keyFrameList.begin(); it != m_keyFrameList.end(); ++it)
	{
        key = (KeyFrameBezier<Vector3>*)*it;
		pos = key->Data;

		// draw the key point
		primitive->setColor(1.f,1.f,1.f);
		primitive->beginDraw(P_POINTS);
			primitive->addVertex(pos.getData());
		primitive->endDraw();

		// draw the left tangent
		if (!first)
		{
			tangent[X] = -key->TangentLeft[X][1] / key->TangentLeft[X][0];
			tangent[Y] = -key->TangentLeft[Y][1] / key->TangentLeft[Y][0];
			tangent[Z] = -key->TangentLeft[Z][1] / key->TangentLeft[Z][0];

			primitive->beginDraw(P_POINTS);
				primitive->addVertex((pos + tangent*O3D_DRAW_TANGENT_FACTOR).getData());
			primitive->endDraw();

			primitive->setColor(1.f,0.f,0.f);
			primitive->beginDraw(P_LINES);
				primitive->addVertex(pos.getData());
				primitive->addVertex((pos + tangent*O3D_DRAW_TANGENT_FACTOR).getData());
			primitive->endDraw();
		}
		else first = False;

		// draw the right tangent
		tangent[X] = key->TangentRight[X][1] / key->TangentRight[X][0];
		tangent[Y] = key->TangentRight[Y][1] / key->TangentRight[Y][0];
		tangent[Z] = key->TangentRight[Z][1] / key->TangentRight[Z][0];

		primitive->setColor(1.f,1.f,1.f);
		primitive->beginDraw(P_POINTS);
			primitive->addVertex((pos + tangent*O3D_DRAW_TANGENT_FACTOR).getData());
		primitive->endDraw();

		primitive->setColor(1.f,0.f,0.f);
		primitive->beginDraw(P_LINES);
			primitive->addVertex(pos.getData());
			primitive->addVertex((pos + tangent*O3D_DRAW_TANGENT_FACTOR).getData());
		primitive->endDraw();

		// draw the curve beetween the two points
		Vector3 data;

		primitive->setColor(1.f,1.f,0.f);
		primitive->beginDraw(P_LINE_STRIP);

		for (Float t = 0 ; t <= 1.05f ; t += 0.05f)
		{
			data[X] = key->Evaluator[X].evaluate(t);
			data[Y] = key->Evaluator[Y].evaluate(t);
			data[Z] = key->Evaluator[Z].evaluate(t);
			primitive->addVertex(data.getData());
		}
		primitive->endDraw();
	}

	// draw the last keyframe point
	key = (KeyFrameBezier<Vector3>*)m_keyFrameList.back();
	pos = key->Data;

	primitive->setColor(1.f,1.f,1.f);
	primitive->beginDraw(P_POINTS);
		primitive->addVertex(pos.getData());
	primitive->endDraw();

	// draw the last left tangent
	tangent[X] = -key->TangentLeft[X][1] / key->TangentLeft[X][0];
	tangent[Y] = -key->TangentLeft[Y][1] / key->TangentLeft[Y][0];
	tangent[Z] = -key->TangentLeft[Z][1] / key->TangentLeft[Z][0];

	primitive->beginDraw(P_POINTS);
		primitive->addVertex((pos + tangent*O3D_DRAW_TANGENT_FACTOR).getData());
	primitive->endDraw();

	primitive->setColor(Color(1.f,0.f,0.f));
	primitive->beginDraw(P_LINES);
		primitive->addVertex(pos.getData());
		primitive->addVertex((pos + tangent*O3D_DRAW_TANGENT_FACTOR).getData());
	primitive->endDraw();

	glContext->setDefaultPointSize();
	glContext->modelView().pop();
}

#undef O3D_DRAW_TANGENT_FACTOR

/*---------------------------------------------------------------------------------------
  class AnimationTrack_TCBFloat
---------------------------------------------------------------------------------------*/
void AnimationTrack_TCBFloat::initEvaluator(IT_KeyFrameList it)
{
	/* P0 is value before the first key (P0=P1 if this is the first key)
	 * P1 is value of the first key
	 * P2 is value of the second key
	 * P3 is value after the second key (P4=P3 if this is the last key)
	 */
	Float P0,P1,P2,P3;

	// init the evaluators
    KeyFrameTCB<Float>* pKey0 = nullptr;
    KeyFrameTCB<Float>* pKey1 = (KeyFrameTCB<Float>*)*it;
    KeyFrameTCB<Float>* pKey2 = nullptr;
    KeyFrameTCB<Float>* pKey3 = nullptr;

	// get the Key0
	if (it != m_keyFrameList.begin())
	{
		IT_KeyFrameList it_before(it);
		it_before--;

		pKey0 = (KeyFrameTCB<Float>*)*it_before;
	}
	else
		pKey0 = pKey1;

	// get the Key2
	IT_KeyFrameList it_next(it);
	it_next++;
	if (it_next != m_keyFrameList.end())
		pKey2 = (KeyFrameTCB<Float>*)*it_next;
	else
		pKey2 = (KeyFrameTCB<Float>*)m_keyFrameList.front();

	// get the Key3
	if (it_next != m_keyFrameList.end())
	{
		IT_KeyFrameList it_nextnext(it_next);
		it_nextnext++;

		if (it_nextnext != m_keyFrameList.end())
			pKey3 = (KeyFrameTCB<Float>*)*it_nextnext;
		else
			pKey3 = pKey2;
	}
	else
		pKey3 = pKey2;

	P0 = pKey0->Data;
	P1 = pKey1->Data;
	P2 = pKey2->Data;
	P3 = pKey3->Data;

	// init the evaluator
	pKey1->Evaluator[0].initialize(P0,P1,P2,P3,pKey1->Tension[0],pKey1->Continuity[0],pKey1->Bias[0],
		                                       pKey2->Tension[0],pKey2->Continuity[0],pKey2->Bias[0]);
}

void AnimationTrack_TCBFloat::initAllEvaluators()
{
	for(IT_KeyFrameList it = m_keyFrameList.begin() ; it != m_keyFrameList.end() ; ++it)
		initEvaluator(it);
}

const void* AnimationTrack_TCBFloat::compute(
		Animatable* target,
		Float time,
		UInt32 &ValSize)
{
	ValSize = sizeof(m_Data);
	if (time == m_Time)
		return(&m_Data);

	// find keyframe couple
    const KeyFrameTCB<Float> *pKeyBefore = nullptr, *pKeyAfter = nullptr;
	findKeyFrame(target,time,(KeyFrame*&)pKeyBefore,(KeyFrame*&)pKeyAfter);

	Float tBefore = pKeyBefore->getTime();  // key time start
	Float tAfter  = pKeyAfter->getTime();   // key time end

	Float coef = 0.f;

	// compute the linear interpolation coef
	if (fabs(tBefore - tAfter) > o3d::Limits<Float>::epsilon())
		coef = (time - tBefore) / (tAfter - tBefore);

	// compute animation values with coef
	m_Data = pKeyBefore->Evaluator[0].evaluate(coef);
	m_Time = time;

	// finally return the data
	return (&m_Data);
}

Bool AnimationTrack_TCBFloat::readFromFile(InStream &is)
{
    AnimationTrack::readFromFile(is);

	UInt32 nKeys;
    is >> nKeys;

	// read all keys
	for (UInt32 i = 0 ; i < nKeys ; ++i)
	{
		KeyFrameTCB<Float>* pKeyFrame = new KeyFrameTCB<Float>(0,0);
        is >> *pKeyFrame;
		m_keyFrameList.push_back(pKeyFrame);
	}

	initAllEvaluators();

	return True;
}

/*---------------------------------------------------------------------------------------
  class AnimationTrack_TCBVector
---------------------------------------------------------------------------------------*/
void AnimationTrack_TCBVector::initEvaluator(IT_KeyFrameList it)
{
	/* P0 is value before the first key (P0=P1 if this is the first key)
	 * P1 is value of the first key
	 * P2 is value of the second key
	 * P3 is value after the second key (P4=P3 if this is the last key)
	 */
	Float P0,P1,P2,P3;

	// init the evaluators
    KeyFrameTCB<Vector3>* pKey0 = nullptr;
    KeyFrameTCB<Vector3>* pKey1 = (KeyFrameTCB<Vector3>*)*it;
    KeyFrameTCB<Vector3>* pKey2 = nullptr;
    KeyFrameTCB<Vector3>* pKey3 = nullptr;

	// get the Key0
	if (it != m_keyFrameList.begin())
	{
		IT_KeyFrameList it_before(it);
		it_before--;

		pKey0 = (KeyFrameTCB<Vector3>*)*it_before;
	}
	else
		pKey0 = pKey1;

	// get the Key2
	IT_KeyFrameList it_next(it);
	it_next++;
	if (it_next != m_keyFrameList.end())
		pKey2 = (KeyFrameTCB<Vector3>*)*it_next;
	else
		pKey2 = (KeyFrameTCB<Vector3>*)m_keyFrameList.front();

	// get the Key3
	if (it_next != m_keyFrameList.end())
	{
		IT_KeyFrameList it_nextnext(it_next);
		it_nextnext++;

		if (it_nextnext != m_keyFrameList.end())
			pKey3 = (KeyFrameTCB<Vector3>*)*it_nextnext;
		else
			pKey3 = pKey2;
	}
	else
		pKey3 = pKey2;


	for (UInt32 i = 0 ; i < 3 ; ++i)
	{
        P0 = pKey0->Data[i];
		P1 = pKey1->Data[i];
		P2 = pKey2->Data[i];
		P3 = pKey3->Data[i];

		// init the evaluator
		pKey1->Evaluator[0].initialize(P0,P1,P2,P3,pKey1->Tension[i],pKey1->Continuity[i],pKey1->Bias[i],
			                                       pKey2->Tension[i],pKey2->Continuity[i],pKey2->Bias[i]);
	}
}

void AnimationTrack_TCBVector::initAllEvaluators()
{
	for(IT_KeyFrameList it = m_keyFrameList.begin() ; it != m_keyFrameList.end() ; ++it)
		initEvaluator(it);
}

const void* AnimationTrack_TCBVector::compute(
		Animatable* target,
		Float time,
		UInt32 &ValSize)
{
	ValSize = sizeof(m_Data);
	if (time == m_Time)
		return(&m_Data);

	// find keyframe couple
    const KeyFrameTCB<Float> *pKeyBefore=nullptr,*pKeyAfter=nullptr;
	findKeyFrame(target,time,(KeyFrame*&)pKeyBefore,(KeyFrame*&)pKeyAfter);

	Float tBefore = pKeyBefore->getTime();  // key time start
	Float tAfter  = pKeyAfter->getTime();   // key time end

	Float coef = 0.f;

	// compute the linear interpolation coef
	if (fabs(tBefore - tAfter) > o3d::Limits<Float>::epsilon())
		coef = (time - tBefore) / (tAfter - tBefore);

	// compute animation values with coef
	m_Data[X] = pKeyBefore->Evaluator[X].evaluate(coef);
	m_Data[Y] = pKeyBefore->Evaluator[Y].evaluate(coef);
	m_Data[Z] = pKeyBefore->Evaluator[Z].evaluate(coef);
	m_Time = time;

	// finally return the data
	return (&m_Data);
}

Bool AnimationTrack_TCBVector::readFromFile(InStream &is)
{
    AnimationTrack::readFromFile(is);

	UInt32 nKeys;
    is >> nKeys;

	// read all keys
	for (UInt32 i = 0 ; i < nKeys ; ++i)
	{
		KeyFrameTCB<Vector3>* pKeyFrame = new KeyFrameTCB<Vector3>(0,Vector3());
        is >> *pKeyFrame;
		m_keyFrameList.push_back(pKeyFrame);
	}

	initAllEvaluators();

	return True;
}

/*---------------------------------------------------------------------------------------
  class AnimationTrack_ConstantFloat
---------------------------------------------------------------------------------------*/
const void* AnimationTrack_ConstantFloat::compute(
		Animatable* target,
		Float time,
		UInt32 &ValSize)
{
	ValSize = sizeof(m_Data);
	if (time == m_Time)
		return(&m_Data);

	// find keyframe couple
    const KeyFrameConstant<Float> *pKeyBefore = nullptr, *pKeyAfter = nullptr;
	findKeyFrame(target,time,(KeyFrame*&)pKeyBefore,(KeyFrame*&)pKeyAfter);

	// compute animation values with coef
	m_Data = pKeyBefore->Data;
	m_Time = time;

	// finaly return the data
	return (&m_Data);
}

Bool AnimationTrack_ConstantFloat::readFromFile(InStream &is)
{
    AnimationTrack::readFromFile(is);

	UInt32 nKeys;
    is >> nKeys;

	// read all keys
	for (UInt32 i = 0 ; i < nKeys ; ++i)
	{
		KeyFrameConstant<Float>* pKeyFrame = new KeyFrameConstant<Float>(0,0);
        is >> *pKeyFrame;
		m_keyFrameList.push_back(pKeyFrame);
	}

	return True;
}

/*---------------------------------------------------------------------------------------
  class AnimationTrack_ConstantVector
---------------------------------------------------------------------------------------*/
const void* AnimationTrack_ConstantVector::compute(
		Animatable* target,
		Float time,
		UInt32 &ValSize)
{
	ValSize = sizeof(m_Data);
	if (time == m_Time)
		return(&m_Data);

	// find keyframe couple
    const KeyFrameConstant<Vector3> *pKeyBefore = nullptr, *pKeyAfter = nullptr;
	findKeyFrame(target,time,(KeyFrame*&)pKeyBefore,(KeyFrame*&)pKeyAfter);

	// compute animation values with coef
	m_Data = pKeyBefore->Data;
	m_Time = time;

	// finaly return the data
	return (&m_Data);
}

Bool AnimationTrack_ConstantVector::readFromFile(InStream &is)
{
    AnimationTrack::readFromFile(is);

	UInt32 nKeys;
    is >> nKeys;

	// read all keys
	for (UInt32 i = 0 ; i < nKeys ; ++i)
	{
		KeyFrameConstant<Vector3>* pKeyFrame = new KeyFrameConstant<Vector3>(0,Vector3());
        is >> *pKeyFrame;
		m_keyFrameList.push_back(pKeyFrame);
	}

	return True;
}

/*---------------------------------------------------------------------------------------
  class AnimationTrack_ConstantQuaternion
---------------------------------------------------------------------------------------*/
const void* AnimationTrack_ConstantQuaternion::compute(
		Animatable* pTarget,
		Float time,
		UInt32 &ValSize)
{
	ValSize = sizeof(m_Data);
	if (time == m_Time)
		return(&m_Data);

	// find keyframe couple
    const KeyFrameConstant<Quaternion> *pKeyBefore=nullptr,*pKeyAfter=nullptr;
	findKeyFrame(pTarget,time,(KeyFrame*&)pKeyBefore,(KeyFrame*&)pKeyAfter);

	// compute animation values with coef
	m_Data = pKeyBefore->Data;
	m_Time = time;

	// finaly return the data
	return (&m_Data);
}

Bool AnimationTrack_ConstantQuaternion::readFromFile(InStream &is)
{
    AnimationTrack::readFromFile(is);

	UInt32 nKeys;
    is >> nKeys;

	// read all keys
	for (UInt32 i = 0 ; i < nKeys ; ++i)
	{
		KeyFrameConstant<Quaternion>* pKeyFrame = new KeyFrameConstant<Quaternion>(0,Quaternion());
        is >> *pKeyFrame;
		m_keyFrameList.push_back(pKeyFrame);
	}

	return True;
}

/*---------------------------------------------------------------------------------------
  class AnimationTrack_ConstantBool
---------------------------------------------------------------------------------------*/
const void* AnimationTrack_ConstantBool::compute(
		Animatable* target,
		Float time,
		UInt32 &ValSize)
{
	ValSize = sizeof(m_Data);
	if (time == m_Time)
		return(&m_Data);

	// find keyframe couple
    const KeyFrameConstant<Bool> *pKeyBefore = nullptr, *pKeyAfter = nullptr;
	findKeyFrame(target,time,(KeyFrame*&)pKeyBefore,(KeyFrame*&)pKeyAfter);

	// compute animation values with coef
	m_Data = pKeyBefore->Data;
	m_Time = time;

	// finally return the data
	return (&m_Data);
}

Bool AnimationTrack_ConstantBool::readFromFile(InStream &is)
{
    AnimationTrack::readFromFile(is);

	UInt32 nKeys;
    is >> nKeys;

	// read all keys
	for (UInt32 i = 0 ; i < nKeys ; ++i)
	{
		KeyFrameConstant<Bool>* pKeyFrame = new KeyFrameConstant<Bool>(0,0);
        is >> *pKeyFrame;
		m_keyFrameList.push_back(pKeyFrame);
	}

	return True;
}
