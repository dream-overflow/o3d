/**
 * @file cloudlayerperlin.cpp
 * @brief Implementation of CloudLayerPerlin.h
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2009-11-16
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/sky/cloudlayerperlin.h"
#include "o3d/engine/shader/shadermanager.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/engine/context.h"
#include "o3d/engine/object/camera.h"

#include "o3d/core/templatearray.h"
#include "o3d/core/vector3.h"

using namespace o3d;

CloudLayerPerlin::CloudLayerPerlin(BaseObject * _pParent, UInt32 _flags):
	CloudLayerBase(_pParent),
	m_dome(),
	m_perlin(),
	m_noise(),
	m_flags(CloudFlag(_flags)),
	m_scale(1.0f),
	m_time(0.0f),
	m_coveringRate(0.5),
	m_contrast(0.5),
	m_averageSize(1),
	m_lightDir(0.0f, -1.0f, 0.0f),
	m_cloudColor(1.0f, 1.0f, 1.0f),
	m_noiseParameters(13.0f, 53.0f, 151.0f, 0.5f),
	m_intensityParameters(0.005f, 0.5f, 1.0f, 0.0f),
	m_colorParameters(0.6f, 3.0f, 0.1f, 1.0f),
	m_lightParameters(0.6f, 0.4f, -3.0f, 0.5f),
	m_cloudLayerSpeed(0.0f, 0.0f),
	m_upToDate(UPDATE_PERLIN_NORMAL | UPDATE_NOISE | UPDATE_SHADER),
	m_shader(),
	m_perlinTexture(),
    m_indices(getScene()->getContext(), P_TRIANGLES, VertexBuffer::STATIC),
    m_vertices(getScene()->getContext()),
    m_texCoords(getScene()->getContext())
{
	m_perlin.setAmplitudes(PerlinNoise2d::geometricSequence(5, 0.5f, 1.0f));
	m_perlin.setFrequencies(PerlinNoise2d::geometricSequence(5, 2, 4));
	m_perlin.setSize(256);
	m_perlin.setKernelPolicy(PerlinNoise2d::KER_CONSTANT);
	m_perlin.setBoundaryPolicy(PerlinNoise2d::BOUNDARY_REPEAT);
	m_perlin.setOctaveGenerationPolicy(PerlinNoise2d::OCTAVE_POSITIVE);

	m_noise.setAmplitudes(PerlinNoise2d::geometricSequence(6, 0.5f, 0.25f));
	m_noise.setFrequencies(PerlinNoise2d::geometricSequence(6, 2, 4)); // Frequencies start at 2 because of the repeat mode
	m_noise.setSize(128);
	m_noise.setKernelPolicy(PerlinNoise2d::KER_VARIABLE);
	m_noise.setBoundaryPolicy(PerlinNoise2d::BOUNDARY_REPEAT);
	m_noise.setOctaveGenerationPolicy(PerlinNoise2d::OCTAVE_SIGNED);

	memset((void*)m_paramLocations, 0, sizeof(m_paramLocations));

	Shader * lpShading = getScene()->getShaderManager()->addShader("cloudLayer");
	O3D_ASSERT(lpShading);

	lpShading->buildInstance(m_shader);

	m_perlinTexture.setUser(this);
	m_perlinTexture = new Texture2D(this);
	m_perlinTexture->setName("Cloud texture");
	m_perlinTexture->setFiltering(Texture::BILINEAR_FILTERING);
	m_perlinTexture->setWrap(Texture::REPEAT);

	m_noiseTexture.setUser(this);
	m_noiseTexture = new Texture2D(this);
	m_perlinTexture->setName("Noise texture");
	m_noiseTexture->setFiltering(Texture::BILINEAR_FILTERING);
	m_noiseTexture->setWrap(Texture::REPEAT);

	m_normalTexture.setUser(this);
	m_normalTexture = new Texture2D(this);
	m_normalTexture->setName("Normal texture");
	m_normalTexture->setFiltering(Texture::BILINEAR_FILTERING);
	m_normalTexture->setWrap(Texture::REPEAT);
}

CloudLayerPerlin::~CloudLayerPerlin()
{
}

void CloudLayerPerlin::draw(const DrawInfo &drawInfo)
{
	if (!getActivity() || !getVisibility())
		return;

	if (!updateGeometry() || !isValid() || !m_shader.isLinked())
		return;

	Context *glContext = getScene()->getContext();

    glContext->blending().setFunc(Blending::SRC_A__ONE_MINUS_SRC_A);

	Matrix4 lModifiedModelView(glContext->modelView().get());
	lModifiedModelView.setTranslation(0.0f, 0.0f, 0.0f);

	glContext->modelView().push();
	glContext->modelView().set(lModifiedModelView);
	glContext->disableDepthWrite();
	glContext->setDepthRange(glContext->getFarDepthRange(), glContext->getFarDepthRange());
	glContext->setDepthFunc(COMP_EQUAL);

	Vector4 lColorParams = isEnabled(CLOUD_SHADING) ? m_colorParameters : Vector4(0.0f, 0.0f, 0.0f, 0.0f);
	Vector4 lLightParams = isEnabled(CLOUD_SHADOWING) ? m_lightParameters : Vector4(1.0f, 0.0f, 0.0f, 0.0f);
	Vector4 lIntensityParams = m_intensityParameters;
	const Float lPlanetRadius = isEnabled(CLOUD_OCCLUSION) ? m_dome.getRadius() - m_dome.getHeight() : 0.0f;

	if (!isEnabled(CLOUD_NOISE))
	{
		lIntensityParams[1] = 0.0f;	// No noise on cloud intensity
		lColorParams[2] = 0.0f;	// No noise on cloud color
	}

	m_shader.bindShader();

	m_shader.setConstTexture(m_paramLocations[U_TEX_CLOUD], m_perlinTexture.get(), 0);
	m_shader.setConstTexture(m_paramLocations[U_TEX_NOISE], m_noiseTexture.get(), 1);
	m_shader.setConstTexture(m_paramLocations[U_TEX_NORMAL], m_normalTexture.get(), 2);
	m_shader.setConstVector3(m_paramLocations[U_VEC3_LIGHT_DIRECTION], m_lightDir);
	m_shader.setConstVector3(m_paramLocations[U_VEC3_CLOUD_COLOR], m_cloudColor);
	m_shader.setConstVector4(m_paramLocations[U_VEC4_NOISE_PARAMS], m_noiseParameters);
	m_shader.setConstVector4(m_paramLocations[U_VEC4_INTENSITY_PARAMS], lIntensityParams);
	m_shader.setConstVector4(m_paramLocations[U_VEC4_COLOR_PARAMS], lColorParams);
	m_shader.setConstVector4(m_paramLocations[U_VEC4_LIGHT_PARAMS], lLightParams);
	m_shader.setConstMatrix4(m_paramLocations[U_MAT4_MODELVIEW], False, glContext->modelViewProjection());
	m_shader.setConstFloat(m_paramLocations[U_FLOAT_INVSCALE], 1.0f/m_scale);
	m_shader.setConstFloat(m_paramLocations[U_FLOAT_TIME], m_time);
	m_shader.setConstFloat(m_paramLocations[U_FLOAT_PLANET_RADIUS], lPlanetRadius);
	m_shader.setConstFloat(m_paramLocations[U_FLOAT_LAYER_ALTITUDE], m_dome.getHeight());
	m_shader.setConstVector2(m_paramLocations[U_VEC2_VELOCITY], m_cloudLayerSpeed);

	m_texCoords.attribute(m_paramLocations[A_VEC2_TEXCOORDS], 2, 0, 0);
	m_vertices.attribute(m_paramLocations[A_VEC4_VERTEX], 3, 0, 0);

	m_indices.draw(getScene());

	glContext->disableVertexAttribArray(m_paramLocations[A_VEC4_VERTEX]);
	glContext->disableVertexAttribArray(m_paramLocations[A_VEC2_TEXCOORDS]);

	m_shader.unbindShader();

	glContext->setDefaultDepthFunc();
	glContext->setDefaultDepthRange();
	glContext->setDefaultDepthWrite();
    glContext->blending().setDefaultFunc();

	glContext->modelView().pop();
}

void CloudLayerPerlin::setTime(Double _time)
{
	m_time = Float(_time);
}

Bool CloudLayerPerlin::project(const Vector3 & _dir, SmartArrayFloat & _target)
{
	return True;
}

Bool CloudLayerPerlin::isValid() const
{
	return m_dome.isValid() && m_perlin.isValid() && m_noise.isValid();
}

void CloudLayerPerlin::setDome(const Dome & _dome)
{
	m_dome.setRadius(_dome.getRadius());
	m_dome.setHeight(_dome.getHeight());
	m_dome.setSubDiv(_dome.getSubDiv());
	m_dome.setTextureCoordinatePolicy(_dome.getTextureCoordinatePolicy());
	m_dome.enableTextureCoordinate(_dome.isTexCoords());

	if (!m_dome.isTexCoords())
	{
		m_dome.enableTextureCoordinate(True);
		O3D_WARNING(String("Cloud layer texture coordinates was not enabled."));
	}

	if (m_dome.update())
	{
		m_vertices.create(3*m_dome.getNumVertices(), VertexBuffer::STATIC, m_dome.getVertices());
		m_texCoords.create(2*m_dome.getNumVertices(), VertexBuffer::STATIC, m_dome.getTexCoords());
		m_indices.create(m_dome.getFacesIndices(), 3*m_dome.getNumFaces());
	}
}

void CloudLayerPerlin::setPerlin(const PerlinNoise2d & _noise)
{
	m_perlin = _noise;

	if (!o3d::isPow2(m_perlin.getSize()))
	{
		m_perlin.setSize(o3d::nextPow2(m_perlin.getSize()));
		O3D_WARNING(String("Cloud layer noise rescaled to a power of 2"));
	}

	m_upToDate |= UPDATE_PERLIN_NORMAL;
}

void CloudLayerPerlin::setNoise(const PerlinNoise2d & _noise)
{
	m_noise = _noise;

	if (!o3d::isPow2(m_noise.getSize()))
	{
		m_noise.setSize(o3d::nextPow2(m_noise.getSize()));
		O3D_WARNING(String("Cloud noise rescaled to a power of 2"));
	}

	m_upToDate |= UPDATE_NOISE;
}

void CloudLayerPerlin::setAverageSize(Int32 _size)
{
	m_averageSize = _size;

	m_upToDate |= UPDATE_PERLIN_NORMAL;
}

void CloudLayerPerlin::setScale(Float _scale)
{
	m_scale = _scale;
}

void CloudLayerPerlin::setCoveringRate(Float _covering)
{
	if (_covering != m_coveringRate)
		m_upToDate |= UPDATE_PERLIN_NORMAL;

	m_coveringRate = _covering;
}

void CloudLayerPerlin::setContrast(Float _contrast)
{
	if (_contrast != m_contrast)
		m_upToDate |= UPDATE_PERLIN_NORMAL;

	m_contrast = _contrast;
}

void CloudLayerPerlin::enable(CloudFlag _flag, Bool _enable)
{
	if (_enable)
	{
		if (!isEnabled(_flag))
			m_upToDate |= UPDATE_SHADER;

		m_flags |= _flag;
	}
	else
	{
		if (isEnabled(_flag))
			m_upToDate |= UPDATE_SHADER;

		m_flags &= ~_flag;
	}
}

void CloudLayerPerlin::setFlags(UInt32 _flags)
{
	if (m_flags != _flags)
		m_upToDate |= UPDATE_SHADER;

	m_flags = _flags;
}

Bool CloudLayerPerlin::updateGeometry()
{
	if (m_upToDate == 0)
		return True;

	Bool lRet = True;

	if ((m_upToDate & UPDATE_PERLIN_NORMAL) != 0)
	{
		Array2DFloat lPerlin;
		if (m_perlin.toBuffer(lPerlin))
		{
/*			// Generates special clouds (kind of chess board)
			lPerlin.fill(0.0f);

			for (Int32 y = 0 ; y < Int32(lPerlin.width()) ; ++y)
			{
				for (Int32 x = 0 ; x < Int32(lPerlin.height()) ; ++x)
				{
					if (((y % 20) / 10 == 0) && ((x % 20) / 10 == 0))
						lPerlin(x,y) = 1.0f;
				}
			}*/

			// Min and max value of the perlin is retrieved
			Float lMin = lPerlin[0], lMax = lPerlin[0];

			for (UInt32 i = 0 ; i < lPerlin.elt() ; ++i)
			{
				lMin = o3d::min<Float>(lMin, lPerlin[i]);
				lMax = o3d::max<Float>(lMax, lPerlin[i]);
			}

			const Float lThreshold = lMax - m_coveringRate * (lMax - lMin);

			// Perlin is transformed
			for (UInt32 i = 0 ; i < lPerlin.elt() ; ++i)
			{
				float & lVal = lPerlin[i];

				if (lVal <= lThreshold)
					lVal = 0.0f;
				else
					lVal = 1.0f - expf(-m_contrast * (lVal-lThreshold));
			}

			m_perlinTexture->create(True, lPerlin.width(), lPerlin.height(), PF_RED_F32, lPerlin.getData(), PF_RED_F32);

			// Computation of the normal map (could and SHOULD be optimized (TODO))
			// 1) First the perlinTexture must be convoluted (for the moment averaged)
			//    Perlin array is convoluted with a square matrix filled with 1 whose size is (2*m_averageSize+1)
			Array2DFloat lAveraged;
			lAveraged.setSize(lPerlin.width(), lPerlin.height());
			lAveraged.fastFill(0.0f);

			const Float lIAverageSize = 1.0f / ((2*m_averageSize + 1) * (2*m_averageSize + 1));

			for (Int32 y = 0 ; y < Int32(lAveraged.width()) ; ++y)
			{
				for (Int32 x = 0 ; x < Int32(lAveraged.height()) ; ++x)
				{
					Float & lVal = lAveraged(x,y);

					if ((x >= m_averageSize) && (y >= m_averageSize) && (x < Int32(lAveraged.width()) - m_averageSize) && (y < Int32(lAveraged.width()) - m_averageSize))
					{
						for (Int32 i = x - m_averageSize ; i <= x + m_averageSize ; ++i)
						{
							for (Int32 j = y - m_averageSize ; j <= y + m_averageSize ; ++j)
							{
								lVal += lPerlin(i,j);
							}
						}
					}
					else // If (x,y) is near the boundaries, modulo must be used
					{
						for (Int32 i = x - m_averageSize ; i <= x + m_averageSize ; ++i)
						{
							Int32 li = i;

							if (li < 0)
								while (li < 0) li += lPerlin.width();
							else
								li = li % lPerlin.width();

							for (Int32 j = y - m_averageSize ; j <= y + m_averageSize ; ++j)
							{
								Int32 lj = j;

								if (lj < 0)
									while (lj < 0) lj += lPerlin.height();
								else
									lj = lj % lPerlin.height();

								lVal += lPerlin(li,lj);
							}
						}
					}

					// Finally the sum is divided by the size of the average matrix
					lVal *= lIAverageSize;
				}
			}

			// The could texture is now averaged, the normal map texture can be computed
			ArrayFloat lNormalMap;
			lNormalMap.setSize(4*lAveraged.width()*lAveraged.height());

			Float * lPtr = lNormalMap.getData();
			Float lGrad[2];

			const Int32 lWidth = Int32(lAveraged.width());
			const Int32 lHeight = Int32(lAveraged.height());
			const Int32 lSize = lWidth * lHeight;

			const Float lXStep = 1.0f / (lWidth - 1);
			const Float lYStep = 1.0f / (lHeight - 1);

			const Float lXFactor = 1.0f / (2.0f * lXStep);
			const Float lYFactor = 1.0f / (2.0f * lYStep);

			for (Int32 k = 0 ; k < lSize ; ++k, lPtr += 4)
			{
				const Int32 x = k % lAveraged.width();
				const Int32 y = k / lAveraged.width();

				// Gradient computation using second order approximation
				if ((y == 0) || (y == lHeight - 1) || (x == 0) || (x == lWidth - 1))
				{
					lGrad[0] =	lAveraged((x + 1) % lWidth			, y) - 
								lAveraged((x - 1 + lWidth) % lWidth	, y);
					lGrad[1] =	lAveraged(x, (y + 1 + lHeight) % lHeight) - 
								lAveraged(x, (y - 1 + lHeight) % lHeight);
				}
				else
				{
					lGrad[0] =	lAveraged(x + 1, y) - lAveraged(x - 1, y);
					lGrad[1] =	lAveraged(x, y + 1) - lAveraged(x, y - 1);
				}

				// Components x and y are associated to components z and x in 3D world
				lGrad[0] *= lXFactor;
				lGrad[1] *= lYFactor;

				const Float lSquareNorm = lGrad[0]*lGrad[0] + lGrad[1]*lGrad[1];

				Vector3 lGradient(lGrad[1], 0.0f, lGrad[0]); // Gradient

				// Quite strange vector since units are not homogeneous but works well
				Vector3 lVec(lGradient[X], -lSquareNorm, lGradient[Z]);

				// A more correct but costly version of a similar vector
				// Y component is a point computed by following the gradient direction. The y component will then be always negative.
                // Vector3 lVec(lGradient[1], Math::sqrt(lSquareNorm), lGradient[2]);

				lVec = lVec ^ (lVec ^ lGradient);
				// Equal to (wikipedia , Lagrange identity)
				// lVec = (lVec * lGradient) * lVec - (lVec * lVec) * lGradient;
				// Equivalent to
				// lVec = lSquareNorm * (lVec - (lSquareNorm + 1.0f) * lGradient);

				if (lVec.normInf() < 0.00001f)
				{
					if (fabs(lAveraged(x, y)) < 0.00001f)
						lVec.zero();
					else
						lVec.set(0.0f, -1.0f, 0.0f);
				}
				else
				{
					//lVec[Y] *= lAveraged(x,y); // y component is forced to 0 near cloud boundaries
					lVec.normalize();
				}

				lPtr[X] = 0.5f * (lVec[X] + 1.0f);
				lPtr[Y] = 0.5f * (lVec[Y] + 1.0f);
				lPtr[Z] = 0.5f * (lVec[Z] + 1.0f);
				lPtr[W] = 1.0f;
			}

			m_normalTexture->create(True, lPerlin.width(), lPerlin.height(), PF_RGBA_F32, lNormalMap.getData(), PF_RGBA_F32);
		}
		else
		{
			m_perlinTexture->destroy();
			lRet = False;
		}

		// The bit 'UPDATE_PERLIN_NORMAL' is set to 0
		m_upToDate &= (~UPDATE_PERLIN_NORMAL);
	}

	if ((m_upToDate & UPDATE_NOISE) != 0)
	{
		Array2DFloat lPerlin;
		if (m_noise.toBuffer(lPerlin))
		{
			// The generated noise is centered around 0 instead of 0.5.
			for (UInt32 i = 0 ; i < lPerlin.elt() ; ++i)
				lPerlin[i] += 0.5f;

			m_noiseTexture->create(True, lPerlin.width(), lPerlin.height(), PF_RED_F32, lPerlin.getData(), PF_RED_F32);
		}
		else
		{
			m_noiseTexture->destroy();
			lRet = False;
		}

		// The bit 'UPDATE_NOISE' is set to 0
		m_upToDate &= ~UPDATE_NOISE;
	}

	if ((m_upToDate & UPDATE_SHADER) != 0)
	{
		// For the moment, the same shader is used for all versions. It means that disabling options
		// does not improve performances of vertex and/or fragment programs.
        m_shader.assign("default", "default", "ALPHA_TEST_REF=0.0;ALPHA_FUNC_GREATER;");

		m_paramLocations[A_VEC4_VERTEX]				= m_shader.getAttributeLocation("a_vertex");
		m_paramLocations[A_VEC2_TEXCOORDS]			= m_shader.getAttributeLocation("a_texCoords");

		m_paramLocations[U_MAT4_MODELVIEW]			= m_shader.getUniformLocation("u_modelViewProjectionMatrix");
		m_paramLocations[U_TEX_CLOUD]				= m_shader.getUniformLocation("u_cloud");
		m_paramLocations[U_TEX_NOISE]				= m_shader.getUniformLocation("u_noise");
		m_paramLocations[U_TEX_NORMAL]				= m_shader.getUniformLocation("u_normal");
		m_paramLocations[U_VEC4_NOISE_PARAMS]		= m_shader.getUniformLocation("u_noiseParams");
		m_paramLocations[U_VEC4_INTENSITY_PARAMS]	= m_shader.getUniformLocation("u_intensityParams");
		m_paramLocations[U_VEC4_COLOR_PARAMS]		= m_shader.getUniformLocation("u_colorParams");
		m_paramLocations[U_VEC4_LIGHT_PARAMS]		= m_shader.getUniformLocation("u_lightParams");
		m_paramLocations[U_VEC3_LIGHT_DIRECTION]	= m_shader.getUniformLocation("u_lightDirection");
		m_paramLocations[U_VEC3_CLOUD_COLOR]		= m_shader.getUniformLocation("u_cloudColor");
		m_paramLocations[U_VEC2_VELOCITY]			= m_shader.getUniformLocation("u_velocity");
		m_paramLocations[U_FLOAT_INVSCALE]			= m_shader.getUniformLocation("u_invScale");
		m_paramLocations[U_FLOAT_TIME]				= m_shader.getUniformLocation("u_time");
		m_paramLocations[U_FLOAT_PLANET_RADIUS]		= m_shader.getUniformLocation("u_planetRadius");
		m_paramLocations[U_FLOAT_LAYER_ALTITUDE]	= m_shader.getUniformLocation("u_layerAltitude");

		m_upToDate &= ~UPDATE_SHADER;
	}

	return lRet;
}

