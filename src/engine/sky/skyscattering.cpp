/**
 * @file skyscattering.cpp
 * @brief 
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/sky/skyscattering.h"

#include "o3d/engine/sky/skyobjectbase.h"
#include "o3d/engine/sky/scatteringmodeldefault.h"

#include "o3d/engine/shader/shadermanager.h"
#include "o3d/engine/matrix.h"
#include "o3d/engine/context.h"
#include "o3d/engine/object/camera.h"
#include "o3d/engine/scene/scene.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(SkyScattering, ENGINE_SKY_SCATTERING, SkyBase)

//! Default constructor
SkyScattering::T_SkyDatas::T_SkyDatas(Context *context):
    vertexColorBuffer(context),
	time(0.0),
	changed(False),
	skyObjectArray()
{
}

//! Default constructor
SkyScattering::SkyScattering(BaseObject * _pParent):
	SkyBase(_pParent),
	m_skyDome(),
    m_indexBuffer(getScene()->getContext()),
    m_vertexBuffer(getScene()->getContext()),
    m_currentData(getScene()->getContext()),
    m_forecastData(getScene()->getContext()),
	m_objectArray(),
	m_objectArrayChanged(False),
	m_cloudArray(),
	m_currentTime(0.0),
	m_requestTime(0.0),
	m_taskDatasList(),
	m_taskDatasGarbage(),
	m_datas(),
	m_pTask(new ScatteringModelDefault(this)),
	m_shader1(),
	m_shader2(),
	m_useForecast(True),
	m_timeStep(4.0),
	m_useAsynch(True)
{
	setDrawable(True);
	setUpdatable(True);

	m_skyDome.setSubDiv(3);
	m_skyDome.enableTextureCoordinate(False, False);

	// Initialisation des donnees
	m_datas.pDome = &m_skyDome;						//!< Geometrie de la sphere

	m_datas.time = 0.0;								//!< Temps courant (sec)
	m_datas.camPosition.set(0.0f, 0.0f, 0.0f);		//!< Position of the camera (m)

	m_datas.planetRadius = 6400.0f;					//!< Rayon de la planete (km)
	m_datas.atmThickness = 40.0f;					//!< Epaisseur de l'atmosphere (km)

	m_datas.stepIndex = 4;							//!< Pas d'integration (km)
	m_datas.stepFactor = 2.0f;						//!< Step length increase factor

	m_datas.rayleighPhaseFunctionCoef1 = 1.0f;		//!< Coef A in the formula : 3.0/(16.0*PI) * (A + B * Theta);
	m_datas.rayleighPhaseFunctionCoef2 = 1.0f;		//!< Coef B in the formula : 3.0/(16.0*PI) * (A + B * Theta);
	m_datas.rayDensityAtGround = 2.545E25f;			//!< Density of molecule at ground level
	m_datas.rayDensityDecFactor = 8400.0f;			//!< Exponential decrease factor (m)
	m_datas.rayAirIndex = 1.0003f;					//!< Air indice
	m_datas.rayKingFactor = 1.035f;					//!< King factor

	m_datas.mieDensityAtGround = 30000.0E6f;		//!< Density of particle at ground level
	m_datas.mieDensityDecFactor = 1200.0f;			//!< Exponential decrease factor
	m_datas.mieCrossSection = 3E-15f;				//!< Mie particle cross section coefficient
	m_datas.mieAniso = 0.7f;						//!< Anisotropic coefficient of the particles

	m_datas.postCoefA = 1.0f;						//!< Coefficient A in : Brightness = A - C*EXP(-B*Luminance)
	m_datas.postCoefB = 1.0f;						//!< Coefficient B in : Brightness = A - C*EXP(-B*Luminance)
	m_datas.postCoefC = 1.0f;						//!< Coefficient C in : Brightness = A - C*EXP(-B*Luminance)

	m_datas.colorInterpolation = True;
	m_datas.colorThreshold = 0.02f;					//!< Threshold used to interpolate color instead of doing full computation

	m_datas.result = ScatteringModelBase::RESULT_UNDEFINED;
}

//! The destructor
SkyScattering::~SkyScattering()
{
	deletePtr(m_pTask);
}

//! Called after the renderer configuration
void SkyScattering::init()
{
	if (isInit())
		O3D_ERROR(E_InvalidOperation(String("SkyScattering : Attempt to initialize twice the renderer")));

	// Creation du dome
	m_skyDome.setRadius(m_datas.planetRadius, False);
	m_skyDome.setHeight(m_datas.atmThickness, True);

	// Initialisation des buffers de couleur
	ScatteringModelDefault lModel(this);

	m_datas.result = ScatteringModelBase::RESULT_UNDEFINED;
	computeTime(m_requestTime, False);
	m_taskDatasList.push_back(m_datas);
	m_currentData.changed = True;

	if (m_useForecast)
	{
		m_datas.colorArray = SmartArrayFloat();
		m_datas.result = ScatteringModelBase::RESULT_UNDEFINED;
		computeTime(m_requestTime + m_timeStep, False);
		m_taskDatasList.push_back(m_datas);

		m_datas.colorArray = SmartArrayFloat();
		m_datas.result = ScatteringModelBase::RESULT_UNDEFINED;
		computeTime(m_requestTime + 2*m_timeStep, False);
		m_taskDatasList.push_back(m_datas);

		m_forecastData.changed = True;
	}

	m_datas.colorArray = SmartArrayFloat();
	m_datas.result = ScatteringModelBase::RESULT_UNDEFINED;

	// Initialisation des shaders
	Shader * lpShader = getScene()->getShaderManager()->addShader("skyScattering");
	O3D_ASSERT(lpShader);

	lpShader->buildInstance(m_shader1);
	lpShader->buildInstance(m_shader2);

    m_shader1.assign("noforecast", "default");
    m_shader2.assign("forecast", "default");

	// Initialisation des vbos
	m_indexBuffer.create(3*m_skyDome.getNumFaces(), VertexBuffer::STATIC, m_skyDome.getFacesIndices());
	m_vertexBuffer.create(3*m_skyDome.getNumVertices(), VertexBuffer::STATIC, m_skyDome.getVertices());
	m_currentData.vertexColorBuffer.create(3*m_skyDome.getNumVertices(), VertexBuffer::STATIC, m_taskDatasList.front().colorArray.getData());
	m_currentData.time = m_requestTime;

	if (m_useForecast)
	{
		m_forecastData.vertexColorBuffer.create(3*m_skyDome.getNumVertices(), VertexBuffer::STATIC, (++m_taskDatasList.begin())->colorArray.getData());
		m_forecastData.time = m_requestTime + m_timeStep;
	}

	// Evenements
	if (m_useAsynch)
        m_pTask->onFinish.connect(this, &SkyScattering::onTaskFinish, EvtHandler::CONNECTION_ASYNCH);
	else
        m_pTask->onFinish.connect(this, &SkyScattering::onTaskFinish, EvtHandler::CONNECTION_SYNCH);
}

//! Returns whether or not the renderer is initialized
Bool SkyScattering::isInit() const
{
	return (m_skyDome.isValid());
}

//! Call when the sky must be drawn
void SkyScattering::draw(const DrawInfo &drawInfo)
{
    if (!getActivity() || !getVisibility()) {
		return;
    }

    if (drawInfo.pass != DrawInfo::AMBIENT_PASS) {
        // only in ambient pass
        return;
    }

	if (isInit())
	{
		Context *glContext = getScene()->getContext();

		setUpModelView();

		Matrix4 lModelView;
		lModelView = glContext->modelView().get();

		Matrix4 lModifiedModelView(lModelView);
		lModifiedModelView.setTranslation(0.0f, 0.0f, 0.0f);

		glContext->modelView().set(lModifiedModelView);

		glContext->disableDepthWrite();

		Int32 lAttribVertexLocation = 0;
		Int32 lAttribColorLocation = 0;
		Int32 lAttribNextColorLocation = 0;
		Int32 lUniCoefLocation = 0;
		Int32 lUniModelViewProjectionMatrix = 0;

        if (m_forecastData.isValid()) {
			lAttribVertexLocation = m_shader2.getAttributeLocation("a_vertex");
			lAttribColorLocation = m_shader2.getAttributeLocation("VertexColor");
			lAttribNextColorLocation = m_shader2.getAttributeLocation("NextVertexColor");

			lUniModelViewProjectionMatrix = m_shader2.getUniformLocation("u_modelViewProjectionMatrix");
			lUniCoefLocation = m_shader2.getUniformLocation("uCoef");

			Float lCoef = 0.0f;

			if (m_forecastData.time != m_currentData.time)
				lCoef = Float((m_currentTime - m_currentData.time)/(m_forecastData.time - m_currentData.time));

			m_shader2.bindShader();
			m_shader2.setConstVector2(lUniCoefLocation, Vector2f(1.0f - lCoef, lCoef));
			m_shader2.setConstMatrix4(lUniModelViewProjectionMatrix, False, glContext->modelViewProjection());

			m_vertexBuffer.attribute(lAttribVertexLocation, 3, 0, 0);
			m_currentData.vertexColorBuffer.attribute(lAttribColorLocation, 3, 0, 0);
			m_forecastData.vertexColorBuffer.attribute(lAttribNextColorLocation, 3, 0, 0);
        } else {
			lAttribVertexLocation = m_shader1.getAttributeLocation("a_vertex");
			lAttribColorLocation = m_shader1.getAttributeLocation("VertexColor");

			lUniModelViewProjectionMatrix = m_shader1.getUniformLocation("u_modelViewProjectionMatrix");

			m_shader1.bindShader();
			m_shader1.setConstMatrix4(lUniModelViewProjectionMatrix, False, glContext->modelViewProjection());

			m_vertexBuffer.attribute(lAttribVertexLocation, 3, 0, 0);
			m_currentData.vertexColorBuffer.attribute(lAttribColorLocation, 3, 0, 0);
		}

		m_indexBuffer.bindBuffer();
        getScene()->drawElementsUInt32(P_TRIANGLES, m_indexBuffer.getCount(), nullptr);
		m_indexBuffer.unbindBuffer();

        if (m_forecastData.isValid()) {
			glContext->disableVertexAttribArray(lAttribVertexLocation);
			glContext->disableVertexAttribArray(lAttribColorLocation);
			glContext->disableVertexAttribArray(lAttribNextColorLocation);
			m_shader2.unbindShader();
        } else {
			glContext->disableVertexAttribArray(lAttribVertexLocation);
			glContext->disableVertexAttribArray(lAttribColorLocation);
			m_shader1.unbindShader();
		}

		// Rendering of cloud objects
		glContext->setDefaultDepthWrite();

		glContext->setDepthRange(glContext->getFarDepthRange(), glContext->getFarDepthRange());
		glContext->setDepthFunc(COMP_EQUAL);

		Matrix4 lTranslation;

        for (IT_SkyObjectArray it = m_objectArray.begin() ; it != m_objectArray.end() ; it++) {
			glContext->modelView().push();

			ScatteringModelBase::IT_ObjectArray itCurData = m_currentData.skyObjectArray.begin();
			ScatteringModelBase::IT_ObjectArray itForData = m_forecastData.skyObjectArray.begin();

            for ( ; itCurData != m_currentData.skyObjectArray.end() ; itCurData++) {
                if (itCurData->pObject == it->get()) {
					break;
                }
			}

            for ( ; itForData != m_forecastData.skyObjectArray.end() ; itForData++) {
                if (itForData->pObject == it->get()) {
					break;
                }
			}

			// Si l'objet a ete ajoute depuis le dernier calcul, on ne trouvera aucune donnee sur lui.
			// Donc on l'affiche pas.
            if ((itForData == m_forecastData.skyObjectArray.end()) && (itCurData == m_currentData.skyObjectArray.end())) {
				glContext->modelView().pop();
				continue;
			}

			Vector3 lObjectPosition;
			SkyObjectBase::CoordinateType lCoordinate;
			Vector2f lAngle;
			Float lBrightness = 0.0f;
			Vector3 lLuminance;

			if ((itForData != m_forecastData.skyObjectArray.end()) && (m_currentData.time != m_forecastData.time))
			{
				const Float lCoef = Float((m_currentTime - m_currentData.time) / (m_forecastData.time - m_currentData.time));

				lBrightness = (1.0f - lCoef) * itCurData->brightness + lCoef * itForData->brightness;
				lLuminance = itCurData->luminance * (1.0f - lCoef) + itForData->luminance * lCoef;
			}
			else
			{
				lBrightness = itCurData->brightness;
				lLuminance = itCurData->luminance;
			}

			(*it)->getApparentAngle(m_currentTime, lAngle);
			(*it)->getPosition(m_currentTime, lObjectPosition, lCoordinate);

			while (lObjectPosition[1] >= o3d::PI)
				lObjectPosition[1] -= 2.0f * o3d::PI;

			while (lObjectPosition[1] < -o3d::PI)
				lObjectPosition[1] += 2.0f * o3d::PI;

			const Float lAlpha = lObjectPosition[0];
			const Float lBeta = lObjectPosition[1];

			const Float lCosBeta = cosf(lBeta);
			const Float lSinBeta = sinf(lBeta);
			const Float lCosAlpha = cosf(lAlpha);
			const Float lSinAlpha = sinf(lAlpha);

			if ((lBeta + 0.5f*lAngle[1] > 0.0f) && (lBeta - 0.5f*lAngle[1] < o3d::PI))
			{
				const Float lFov = getScene()->getActiveCamera()->getFov();
				const Float lDist = 0.5f*(getScene()->getActiveCamera()->getZfar() + getScene()->getActiveCamera()->getZnear());
				const Float lScale = tanf(lAngle[0] * 180.0f / lFov) * lDist;

				Vector3 lBase0(lCosBeta * lSinAlpha, lSinBeta, lCosBeta * lCosAlpha);
				Vector3 lBase1(lCosAlpha, 0.0f, -lSinAlpha);
				Vector3 lBase2(lBase0 ^ lBase1);
				Vector3 lBase3(lSinAlpha*lCosBeta, lSinBeta, lCosBeta*lCosAlpha);

				lBase0 *= lScale;
				lBase1 *= lScale;
				lBase2 *= lScale;
				lBase3 *= lDist;

				Matrix4 lBase;
				lBase.setX(lBase1);
				lBase.setY(lBase2);
				lBase.setZ(lBase0);
				lBase.setTranslation(lBase3);
				lBase.setData(3, 3, 1.0f);

				glContext->modelView().mult(lBase);

				(*it)->setBrightness(lBrightness);
				(*it)->setLuminance(lLuminance);
				(*it)->draw(drawInfo);
			}

			glContext->modelView().pop();
		}

		glContext->setDefaultDepthFunc();
		glContext->setDefaultDepthRange();

		glContext->modelView().set(lModelView);

		// Rendering of cloud layers
		for (IT_CloudLayerArray it = m_cloudArray.begin() ; it != m_cloudArray.end() ; it++)
		{
			it->get()->setTime(m_currentTime);
			it->get()->draw(drawInfo);
		}
	}
}

void SkyScattering::update()
{
	if (isInit())
	{
		// Si l'utilisateur a demander de changer le temps, c'est ici que l'on prend en compte
		// la demande
		if ((m_requestTime != m_currentTime) || !m_currentData.isValid())
		{
			switch(m_taskDatasList.size())
			{
			case 1:
				if (m_currentData.changed) //(m_taskDatasList.front().time != m_colorTimeValue)
				{
						m_currentData.vertexColorBuffer.create(
								m_taskDatasList.front().colorArray.getNumElt(),
								VertexBuffer::STATIC,
								m_taskDatasList.front().colorArray.getData());

					m_currentData.time = m_taskDatasList.front().time;
					m_currentData.skyObjectArray = m_taskDatasList.front().objectArray;
					m_currentData.changed = False;

					// If a sky object was removed during the computation, we must remove it
					// from the list, since this list must always be synchronized with "m_objectArray"
					removeRemovedObject(m_currentData.skyObjectArray);
				}

				if (m_forecastData.isValid())
				{
					m_forecastData.vertexColorBuffer.release();
					m_forecastData.time = m_taskDatasList.front().time;
					m_forecastData.skyObjectArray.clear();
					m_forecastData.changed = False;
				}

				m_currentTime = m_taskDatasList.front().time;

				break;
			case 2:
			case 3:
				{
					ScatteringModelBase::TaskData * lpFirst = &m_taskDatasList.front();
					ScatteringModelBase::TaskData * lpSecond = (++m_taskDatasList.begin()).operator ->();

					if (m_currentData.changed)//(lpFirst->time != m_colorTimeValue)
					{
						m_currentData.vertexColorBuffer.create(
								lpFirst->colorArray.getNumElt(),
								VertexBuffer::STATIC,
								lpFirst->colorArray.getData());

						m_currentData.time = lpFirst->time;
						m_currentData.skyObjectArray = lpFirst->objectArray;
						m_currentData.changed = False;

						removeRemovedObject(m_currentData.skyObjectArray);
					}

					if (m_forecastData.changed) //(lpSecond->time != m_forecastColorTimeValue)
					{
						m_forecastData.vertexColorBuffer.create(
								lpSecond->colorArray.getNumElt(),
								VertexBuffer::STATIC,
								lpSecond->colorArray.getData());

						m_forecastData.time = lpSecond->time;
						m_forecastData.skyObjectArray = lpSecond->objectArray;
						m_forecastData.changed = False;

						removeRemovedObject(m_forecastData.skyObjectArray);
					}

					m_currentTime = o3d::max<Double>(lpFirst->time, o3d::min<Double>(lpSecond->time, m_requestTime));
				}
				break;
			}
		}

		// Si les resultats de la tache precedente ont ete traite...
		// Dans tous les cas, on ne fait rien si il y a deja une tache en cours.
		if (m_datas.result == ScatteringModelBase::RESULT_UNDEFINED)
		{
			const Double & lFirstTime = m_taskDatasList.front().time;
			const Double & lLastTime = m_taskDatasList.back().time;

			// Si la liste d'objet a change
			if (m_objectArrayChanged && !m_useForecast)
			{
				keepTaskDatas(0);
				computeTime(m_requestTime, m_useAsynch);

				m_objectArrayChanged = False;
			}
			// Si on est hors domaine
			else if ((m_requestTime > lLastTime) || (m_requestTime < lFirstTime))
			{
				// On regarde si ca vaut le coup de calculer l'instant suivant pour faire de l'interpolation
				if ((m_useForecast) && (m_requestTime > lLastTime) && (m_requestTime < lLastTime + m_timeStep))
				{
					keepTaskDatas(1);
					computeTime(lLastTime + m_timeStep, m_useAsynch);
				}
				else
				{
					keepTaskDatas(0);
					computeTime(m_requestTime, m_useAsynch);
				}
			}
			else if (m_useForecast)
			{
				// En temps normal, la pile contient trois elements, mais il se peut que certain
				// ait ete detruit si le temps a ete modifie de facon plus ou moins anormal.
				switch(m_taskDatasList.size())
				{
				case 1: // Means that lFirstTime == lLastTime == m_requestTime
					computeTime(lLastTime + m_timeStep, m_useAsynch);
					break;
				case 2:
					computeTime(lLastTime + m_timeStep, m_useAsynch);
					break;
				case 3:
					if (m_requestTime > (++m_taskDatasList.begin())->time)
					{
						keepTaskDatas(2, True); // On garde les deux derniers
						computeTime(lLastTime + m_timeStep, m_useAsynch);
					}
					break;
				}
			}
		}
	}
}

//! Add an object
void SkyScattering::addObject(SkyObjectBase* _pObject)
{
    if (_pObject == nullptr)
		O3D_ERROR(E_InvalidOperation(String("SkyScattering : Attempt to add an invalid object")));

	if (findObject(_pObject))
		O3D_ERROR(E_InvalidOperation(String("SkyScattering : Attempt to add twice the same object")));

	m_objectArray.push_back(SmartObject<SkyObjectBase>(this, _pObject));

	m_objectArrayChanged = True;
}

//! Remove an object
void SkyScattering::removeObject(SkyObjectBase* _pObject)
{
    if (_pObject == nullptr)
		O3D_ERROR(E_InvalidOperation(String("SkyScattering : Attempt to remove an invalid object")));

	for (IT_SkyObjectArray it = m_objectArray.begin() ; it != m_objectArray.end() ; it++)
		if (it->get() == _pObject)
		{
			m_objectArray.erase(it);
			m_objectArrayChanged = True;

			return;
		}

	O3D_ERROR(E_InvalidOperation("SkyScattering : The object does not exist"));
}

//! Find an object
Bool SkyScattering::findObject(SkyObjectBase * _pObject) const
{
	for (CIT_SkyObjectArray it = m_objectArray.begin() ; it != m_objectArray.end() ; it++)
		if (it->get() == _pObject)
			return True;

	return False;
}

//! Add a cloud layer
void SkyScattering::addCloudLayer(CloudLayerBase* _layer)
{
    if (_layer == nullptr)
		O3D_ERROR(E_InvalidOperation(String("SkyScattering : Attempt to add an invalid cloud layer")));

	if (findCloudLayer(_layer))
		O3D_ERROR(E_InvalidOperation(String("SkyScattering : Attempt to add twice the same cloud layer")));

	m_cloudArray.push_back(SmartObject<CloudLayerBase>(this, _layer));
}

//! Remove a cloud layer
void SkyScattering::removeCloudLayer(CloudLayerBase* _layer)
{
    if (_layer == nullptr)
		O3D_ERROR(E_InvalidOperation(String("SkyScattering : Attempt to remove an invalid cloud layer")));

	for (IT_CloudLayerArray it = m_cloudArray.begin() ; it != m_cloudArray.end() ; it++)
		if (it->get() == _layer)
		{
			m_cloudArray.erase(it);
			return;
		}

	O3D_ERROR(E_InvalidOperation("SkyScattering : The cloud layer does not exist"));
}

//! Find a cloud layer
Bool SkyScattering::findCloudLayer(CloudLayerBase* _layer) const
{
	for (CIT_CloudLayerArray it = m_cloudArray.begin() ; it != m_cloudArray.end() ; it++)
		if (it->get() == _layer)
			return True;

	return False;
}

//! Defines the current time of the virtual world
void SkyScattering::setTime(Double _value)
{
	m_requestTime = _value;
}

//! Returns the current time of the virtual world
Double SkyScattering::getTime()
{
	return m_currentTime;
}

//! @brief Defines the dome precision used to generate the geometry
void SkyScattering::setDomePrecision(UInt32 _precision)
{
	if (isInit())
		O3D_ERROR(E_InvalidOperation(String("Sky : Attempt to alter the dome"
			"precision but the renderer is already initialized")));

	m_skyDome.setSubDiv(_precision);
}

//! @brief Defines the radius of the planet (in km)
void SkyScattering::setPlanetRadius(Float _radius)
{
	if (isInit())
		O3D_ERROR(E_InvalidOperation(String("Sky : Attempt to alter the planet"
			"radius but the renderer is already initialized")));

	m_datas.planetRadius = _radius;
}

//! @brief Defines the thickness of the atmosphere (in km)
void SkyScattering::setAtmosphereThickness(Float _thickness)
{
	if (isInit())
		O3D_ERROR(E_InvalidOperation(String("Sky : Attempt to alter the atmosphere"
			"thickness but the renderer is already initialized")));

	m_datas.atmThickness = _thickness;
}

//! @brief Defines the phase function of molecules
void SkyScattering::setMoleculePhaseFunctionCoefficients(const Vector2f & _coefs)
{
	if (isInit())
		O3D_ERROR(E_InvalidOperation(String("Sky : Attempt to change a parameter"
			"but the renderer is already initialized")));

	m_datas.rayleighPhaseFunctionCoef1 = _coefs[0];
	m_datas.rayleighPhaseFunctionCoef2 = _coefs[1];
}

//! @brief Defines the density evolution of molecules in the atmosphere
void SkyScattering::setMoleculeDensity(const Vector2f _coefs)
{
	if (isInit())
		O3D_ERROR(E_InvalidOperation(String("Sky : Attempt to change a parameter"
			"but the renderer is already initialized")));

	m_datas.rayDensityAtGround = _coefs[0];
	m_datas.rayDensityDecFactor = _coefs[1];
}

//! @brief Defines some property of the gas
void SkyScattering::setAtmGazProperty(const Vector2f _coefs)
{
	if (isInit())
		O3D_ERROR(E_InvalidOperation(String("Sky : Attempt to change a parameter"
			"but the renderer is already initialized")));

	m_datas.rayAirIndex = _coefs[0];
	m_datas.rayKingFactor = _coefs[1];
}

//! @brief Defines the density evolution of particles in the atmosphere
void SkyScattering::setParticuleDensity(const Vector2f _coefs)
{
	if (isInit())
		O3D_ERROR(E_InvalidOperation(String("Sky : Attempt to change a parameter"
			"but the renderer is already initialized")));

	m_datas.mieDensityAtGround = _coefs[0];
	m_datas.mieDensityDecFactor = _coefs[1];
}

//! @brief Defines the particle cross section coefficient
void SkyScattering::setParticuleCrossSection(Float _coef)
{
	if (isInit())
		O3D_ERROR(E_InvalidOperation(String("Sky : Attempt to change a parameter"
			"but the renderer is already initialized")));

	m_datas.mieCrossSection = _coef;
}

//! @brief Defines the anisotropic coefficient of particles
void SkyScattering::setAnisotropicCoefficient(Float _aniso)
{
	if (isInit())
		O3D_ERROR(E_InvalidOperation(String("Sky : Attempt to change a parameter"
			"but the renderer is already initialized")));

	m_datas.mieAniso = _aniso;
}

//! @brief Defines the post operation coefficients
void SkyScattering::setPostOperationCoefficients(const Vector3 & _coefs)
{
	if (isInit())
		O3D_ERROR(E_InvalidOperation(String("Sky : Attempt to change a parameter"
			"but the renderer is already initialized")));

	m_datas.postCoefA = _coefs[0];
	m_datas.postCoefB = _coefs[1];
	m_datas.postCoefC = _coefs[2];
}

//-----------------------------------------------------------------------------------
// Model optimization options
//-----------------------------------------------------------------------------------

//! @brief Defines the first step length used by numerical integration
void SkyScattering::setIntegrationStepIndex(UInt32 _stepIndex)
{
	if (isInit())
		O3D_ERROR(E_InvalidOperation(String("Sky : Attempt to change a parameter"
			"but the renderer is already initialized")));

	m_datas.stepIndex = _stepIndex;
}

//! @brief Defines the integration factor
void SkyScattering::setIntegrationStepFactor(Float _stepFactor)
{
	if (isInit())
		O3D_ERROR(E_InvalidOperation(String("Sky : Attempt to change a parameter"
			"but the renderer is already initialized")));

	m_datas.stepFactor = _stepFactor;
}

//! @brief Enable/Disable color interpolation
void SkyScattering::enableColorInterpolation(Bool _state)
{
	if (isInit())
		O3D_ERROR(E_InvalidOperation(String("Sky : Attempt to change a parameter"
			"but the renderer is already initialized")));

	m_datas.colorInterpolation = _state;
}

//! @brief Defines the color threshold (optimisation)
void SkyScattering::setColorThreshold(Float _threshold)
{
	if (isInit())
		O3D_ERROR(E_InvalidOperation(String("Sky : Attempt to change a parameter"
			"but the renderer is already initialized")));

	m_datas.colorThreshold = _threshold;
}

//! @brief Enable color forecasting
void SkyScattering::enableForecast(Bool _value)
{
	if (isInit())
		O3D_ERROR(E_InvalidOperation(String("Sky : Attempt to change the forecast"
			" option state but the renderer is already initialized")));

	m_useForecast = _value;
}

//! @brief Defines the time step
void SkyScattering::setTimeStep(Double _value)
{
	if (isInit())
		O3D_ERROR(E_InvalidOperation(String("Sky : Attempt to change the time step"
			" step but the renderer is already initialized")));

	m_timeStep = _value;
}

//! @brief Enable/Disable asynchronous computation
void SkyScattering::enableAsync(Bool _value)
{
	if (isInit())
		O3D_ERROR(E_InvalidOperation(String("Sky : Attempt to enable/disable the "
			"asynchronous computation but the renderer is already initialized")));

	m_useAsynch = _value;
}


//-----------------------------------------------------------------------------------
// Serialization
//-----------------------------------------------------------------------------------
Bool SkyScattering::writeToFile(OutStream &os)
{
	return False;
}

Bool SkyScattering::readFromFile(InStream &is)
{
	return False;
}

//-----------------------------------------------------------------------------------
// Debugging
//-----------------------------------------------------------------------------------

//! @brief Returns the time bounds
Vector3 SkyScattering::getComputedTimeBounds() const
{
	Vector3 lBounds;
	Int32 k = 0;

	for (CIT_VertexDataList it = m_taskDatasList.begin() ; (it != m_taskDatasList.end()) && (k < 3) ; it++, ++k)
		lBounds[k] = Float(it->time);

	return lBounds;
}

//! @brief Returns the current time bounds
Vector2f SkyScattering::getCurrentTimeBounds() const
{
	return Vector2f(Float(m_currentData.time), Float(m_forecastData.time));
}

//-----------------------------------------------------------------------------------
// Internal
//-----------------------------------------------------------------------------------

void SkyScattering::computeTime(Double _value, Bool _asynch)
{
	O3D_ASSERT(m_datas.result == ScatteringModelBase::RESULT_UNDEFINED);

	m_datas.objectArray.resize(m_objectArray.size());

	ScatteringModelBase::IT_ObjectArray itTarget = m_datas.objectArray.begin();
	Vector3 lVec3;
	Vector2f lVec2;
	SkyObjectBase::CoordinateType lCoordinate = SkyObjectBase::COORDINATE_RELATIVE;

	for (IT_SkyObjectArray it = m_objectArray.begin() ; it != m_objectArray.end() ; it++, itTarget++)
	{
		itTarget->pObject = it->get();

		(*it)->getPosition(_value, lVec3, lCoordinate);

		itTarget->position.set(lVec3[0], lVec3[1]);
		itTarget->distance = lVec3[2];

		(*it)->getIntensity(_value, lVec3);
		itTarget->intensity = lVec3;

		(*it)->getApparentAngle(_value, lVec2);
		itTarget->apparentAngle = lVec2;

		(*it)->getWaveLength(_value, lVec3);
		itTarget->waveLength = lVec3;
	}

	m_datas.time = _value;
	m_pTask->start(m_datas, _asynch);
}

void SkyScattering::keepTaskDatas(Int32 _count, Bool _fromBack)
{
	O3D_ASSERT(_count <= Int32(m_taskDatasList.size()));

	const size_t lCount = size_t(_count);

	if (_fromBack)
	{
		const Double lTime = (m_taskDatasList.empty() ? 0.0 : m_taskDatasList.front().time);

		while (m_taskDatasList.size() > lCount)
		{
			m_taskDatasGarbage.push_back(m_taskDatasList.front());
			m_taskDatasList.pop_front();
		}

		if (!m_taskDatasList.empty() && (m_taskDatasList.front().time != lTime))
		{
			m_currentData.changed = True;
			m_forecastData.changed = True;
		}
	}
	else
	{
		while (m_taskDatasList.size() > lCount)
		{
			m_taskDatasGarbage.push_back(m_taskDatasList.back());
			m_taskDatasList.pop_back();
		}
	}
}

void SkyScattering::removeRemovedObject(ScatteringModelBase::T_ObjectArray & _array)
{
	ScatteringModelBase::IT_ObjectArray it = _array.begin();

	while (it != _array.end())
	{
		IT_SkyObjectArray itObject = m_objectArray.begin();

		for ( ; itObject != m_objectArray.end() ; itObject++)
		{
			if (it->pObject == itObject->get())
				break;
		}

		if (itObject != m_objectArray.end())
			it++;
		else
			it = _array.erase(it);
	}
}

//-----------------------------------------------------------------------------------
// Events
//-----------------------------------------------------------------------------------
void SkyScattering::onTaskFinish(ScatteringModelBase::TaskData const &)
{
	if (!m_taskDatasGarbage.empty())
	{
		m_taskDatasList.push_back(m_taskDatasGarbage.front());
		m_taskDatasGarbage.pop_front();
	}
	else
		m_taskDatasList.push_back(ScatteringModelBase::TaskData());

	switch(m_taskDatasList.size())
	{
	case 1:	m_currentData.changed = True;	break;
	case 2:	m_forecastData.changed = True;	break;
	}

	SmartArrayFloat lArray = m_taskDatasList.back().colorArray;

	m_taskDatasList.back() = m_datas;
	m_datas.colorArray = lArray;

	m_datas.result = ScatteringModelBase::RESULT_UNDEFINED;
}

