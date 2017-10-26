/**
 * @file scatteringmodelbase.h
 * @brief 
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_SCATTERINGMODELBASE_H
#define _O3D_SCATTERINGMODELBASE_H

#include "o3d/core/baseobject.h"
#include "o3d/core/runnable.h"
#include "o3d/core/evt.h"
#include "o3d/core/thread.h"

#include "o3d/core/vector2.h"
#include "o3d/core/vector3.h"

namespace o3d {

class Dome;
class SkyObjectBase;

/**
 * @brief Base class which defines a physical model
 * @date 2008-05-09
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * Definition of a physical scattering model in an atmosphere
 */
class O3D_API ScatteringModelBase : public BaseObject, public Runnable
{
	O3D_DECLARE_ABSTRACT_CLASS_NO_ASSIGN(ScatteringModelBase)

	enum TaskResult
	{
		RESULT_UNDEFINED = 0,
		RESULT_STARTED,
		RESULT_COMPLETED,
		RESULT_ABORTED,
		RESULT_FAILED
	};

	struct SkyObjectInfos
	{
		SkyObjectBase * pObject;

		Vector2f position;
		Float distance;

		Vector3 intensity;
		Vector3 waveLength;
        Vector2f apparentAngle;				//!< Apparent angle of the object

        Vector3 luminance;					//!< Computed by the model
        Float brightness;					//!< Computed by the model
	};

	typedef std::vector<SkyObjectInfos>		T_ObjectArray;
	typedef T_ObjectArray::iterator			IT_ObjectArray;
	typedef T_ObjectArray::const_iterator	CIT_ObjectArray;

	//! Structure which contains all info needed to compute the sky color
	struct TaskData
	{
        const Dome * pDome;					//!< Geometrie de la sphere

        T_ObjectArray objectArray;			//!< Liste des objets du ciel

		Double time;						//!< Temps courant (sec)

        Vector3 camPosition;				//!< Position of the camera (m)

		Float planetRadius;					//!< Rayon de la planete (km)
		Float atmThickness;					//!< Epaisseur de l'atmosph�re (km)

		UInt32 stepIndex;					//!< Pas d'integration (km)
		Float stepFactor;					//!< Step length increase factor

		Float rayleighPhaseFunctionCoef1;	//!< Coef A in the formula : 3.0/(16.0*PI) * (A + B * Theta);
		Float rayleighPhaseFunctionCoef2;	//!< Coef B in the formula : 3.0/(16.0*PI) * (A + B * Theta);
		Float rayDensityAtGround;			//!< Density of molecule at ground level
		Float rayDensityDecFactor;			//!< Exponential decrease factor (m)
		Float rayAirIndex;					//!< Air indice
		Float rayKingFactor;				//!< King factor

		Float mieDensityAtGround;			//!< Density of particule at ground level
		Float mieDensityDecFactor;			//!< Exponential decrease factor
		Float mieCrossSection;				//!< Mie particule cross section coefficient
		Float mieAniso;						//!< Anisotropic coefficient of the particules

		Float postCoefA;					//!< Coefficient A in : Brightness = A - C*EXP(-B*Luminance)
		Float postCoefB;					//!< Coefficient B in : Brightness = A - C*EXP(-B*Luminance)
		Float postCoefC;					//!< Coefficient C in : Brightness = A - C*EXP(-B*Luminance)

		Bool colorInterpolation;
		Float colorThreshold;				//!< Threshold used to interpolate color instead of doing full computation

        SmartArrayFloat colorArray;			//!< (OUT) The array of vertex colors
        SmartArrayFloat workingArray;		//!< (INOUT) Array used to compute the vertex colors

        TaskResult result;					//!< (OUT) The result state
	};

	//! Default constructor
	explicit ScatteringModelBase(BaseObject * _pParent);
	//! The destructor
	virtual ~ScatteringModelBase();

	//! @brief Start the task
	TaskResult start(TaskData & _datas, Bool _asynch = True);
	//! @brief Stop the task
	void stop();
	//! @brief Waits for the task to end
	TaskResult wait();
	//! @brief Returns whether or not the task is started
	Bool isStarted() const;

public:

    Signal<TaskData const &> onFinish{this};

protected:

	//! @brief Thread method
	Int32 run(void*);

	//! @brief Entry function of the model
	virtual TaskResult entry(TaskData &) = 0;

	//! @brief Called periodically
	//! @return TRUE means that must quit as soon as possible
	Bool mustQuit();

private:

	// Members //
	Bool m_mustQuit;

	Thread m_thread;
};

} // namespace o3d

#endif // _O3D_SCATTERINGMODELBASE_H

