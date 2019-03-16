/**
 * @file configs.h
 * @brief 
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_PCLODCONFIGS_H
#define _O3D_PCLODCONFIGS_H

#include "o3d/engine/scene/sceneentity.h"
#include "o3d/engine/shader/shader.h"

#include <map>

namespace o3d {

class Image;
class PCLODTerrain;
class Text2D;

//---------------------------------------------------------------------------------------
//! @class PCLODConfigs
//-------------------------------------------------------------------------------------
//! All parameters set for a PCLOD terrain.
//! @date 2006-08-24
//! @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
//---------------------------------------------------------------------------------------
class O3D_API PCLODConfigs : public SceneEntity
{
public:

	O3D_DECLARE_DYNAMIC_CLASS(PCLODConfigs)

	enum PCLODColormapPolicy
	{
		COLORMAP_AUTO = 0,	 //!< (Default) Generate the colormap if it does not exist on the disk
		COLORMAP_GENERATE_ALWAYS,
		COLORMAP_GENERATE_NEVER,
		COLORMAP_DONT_USE    //!< Not supported yet
	};

private:

	typedef std::map<Float, Int8> T_Curve;
	typedef T_Curve::iterator		      IT_Curve;
	typedef T_Curve::const_iterator	      CIT_Curve;

	Float m_distanceOnlyMaterial;	//!< Distance under which, only materials is used for rendering
	Float m_distanceOnlyColormap;

	String m_shadersPath;

	//! Shaders used for rendering zone with, 1, 2, 3 materials
	ShaderInstance m_shaders[3];
	ShaderInstance m_simpleShader;

	Bool m_useLog;
	Bool m_showMessage;
	Bool m_showWarning;
	Bool m_showError;

	PCLODColormapPolicy m_colormapPolicy;
	UInt8 m_colormapPrecision;
	Bool m_useColormapStaticNoise;
	Image * m_pColormapNoisePicture;
	Float m_colormapStaticNoiseFactor;

	Bool m_useWireFrame;

	UInt32 m_viewDistance;		//!< Heightmap units

	Bool m_useAsynchRefresh;
	UInt32 m_refreshFrequency;

	Bool m_useLightmapLod;
	T_Curve m_lightmapLodCurve;

	Bool m_useColormapLod;
	T_Curve m_colormapLodCurve;

	Bool m_useFrustumCulling;

	Bool m_useFrontToBack;
    //! Déplacement minimal de la caméra pour provoquer le refresh du renderManager lié au front to back rendering
    Float m_frontToBackMinViewMove;
	UInt32 m_frontToBackRefreshPeriodicity;

	Bool m_useDistance2d;

	Bool m_useLightning;
	Bool m_useSelfShadowing;
	Float m_lightMinCosAngleVariation;

    //! Contains the text2d renderer to use for debug informations
    Text2D* m_text;
	Bool m_useDebugLabel;

public:

	/* Constructors */
	PCLODConfigs(BaseObject *pParent);
	~PCLODConfigs();

	/* Accessors */

	PCLODTerrain* getTerrain();

	//! Called during the terrain initialization
	//! This function just write on the log file all parameters.
	void init();

	void setDistanceOnlyMaterial(Float _distance) { m_distanceOnlyMaterial = _distance; }
	Float getDistanceOnlyMaterial() const { return m_distanceOnlyMaterial; }

	void setDistanceOnlyColormap(Float _distance) { m_distanceOnlyColormap = _distance; }
	Float getDistanceOnlyColormap() const { return m_distanceOnlyColormap; }

	void setShadersPath(const String & _file);
	const String getShadersPath() const { return m_shadersPath; }
	Bool shadersInit() const;

	void getShader(UInt8 index, ShaderInstance & _pInstance) const;
	void getSimpleShader(ShaderInstance & _pInstance) const;

	void enableLog(Bool _state = True);
	Bool logEnabled() const;

	void showMessage(Bool _state = True);
	Bool messageShown() const;
	void showWarning(Bool _state = True);
	Bool warningShown() const;
	void showError(Bool _state = True);
	Bool errorShown() const;

	void setColormapPolicy(PCLODColormapPolicy _policy);
	PCLODColormapPolicy colormapPolicy() const;

	void setColormapPrecision(UInt8 _precision);
	UInt8 colormapPrecision() const;

	void enableColormapStaticNoise(Bool _state = True);
	Bool useColormapStaticNoise() const;

	void setColormapStaticNoise(const Image &);
	const Image * colormapStaticNoisePicture() const;

	void setColormapStaticNoiseFactor(Float _fact);
	Float colormapStaticNoiseFactor() const;

	void setViewDistance(UInt32 _distance);
	UInt32 viewDistance() const;

	void enableWireFrame(Bool _state = True);
	Bool wireFrame() const;

	void enableAsynchRefresh(Bool _state = True);
	Bool asynchRefresh() const;

	void setRefreshFrequency(UInt32 _frequency);
	UInt32 refreshFrequency() const;

	void enableLightmapLod(Bool = True);
	Bool lightmapLod() const;

	void clearLightmapCurve();
	Bool setLightmapPoint(Float, Int8);
	Bool getLightmapPoint(UInt32, Float &, Int8 &) const;
	void getLightmapPoint(Float, Int8 &) const;
	UInt32 lightmapCurvePointCount() const;

	void enableColormapLod(Bool = True);
	Bool colormapLod() const;

	void clearColormapCurve();
	Bool setColormapPoint(Float, Int8);
	Bool getColormapPoint(UInt32, Float &, Int8 &) const;
	void getColormapPoint(Float, Int8 &) const;
	UInt32 colormapCurvePointCount() const;

	void enableFrustumCulling(Bool);
	Bool frustumCulling() const;

	void enableFrontToBack(Bool);
	Bool frontToBack() const;

	void setFrontToBackMinViewMove(Float);
	Float frontToBackMinViewMove() const;

	void setFrontToBackRefreshPeriodicity(UInt32);
	UInt32 frontToBackRefreshPeriodicity() const;

	void setDistance2d(Bool);
	Bool distance2d() const;

	void enableLightning(Bool _state = True);
	Bool lightningEnabled() const;

	void enableSelfShadowing(Bool _state = True);
	Bool lightningSelfShadowing() const;

	void setLightMinAngleVariation(Float);
	Float getLightMinAngleVariation() const;
	Float getLightMinCosAngleVariation() const;

    void setText2D(Text2D *_text) { m_text = _text; }
    Text2D* getText2D() const { return m_text; }

	void enableDebugLabel(Bool _state = True);
	Bool debugLabelEnabled() const;
};

} // namespace o3d

#endif // _O3D_PCLODCONFIGS_H
