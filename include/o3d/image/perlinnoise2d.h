/**
 * @file perlinnoise2d.h
 * @brief 2D Perlin Noise Generator
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2009-10-29
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_PERLINNOISE2D_H
#define _O3D_PERLINNOISE2D_H

#include "o3d/core/templatearray2d.h"
#include "o3d/image/image.h"

#include <vector>

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class PerlinNoise2d
//-------------------------------------------------------------------------------------
//! 2D Perlin Noise Generator
//! Example:
//!
//!	O3DImage lImage;
//! O3DPerlinNoise2d lPerlin;
//!	lPerlin.toImage(lImage);
//! lImage.save("PerlinGen.jpg", O3DImageJpeg);
//!
//! This is equivalent to:
//! O3DPerlinNoise2D::create(256).save("PerlinGen.jpg", O3DImageJpeg);
//---------------------------------------------------------------------------------------
class O3D_API PerlinNoise2d
{
public:

	//! The interpolation type used to generate the perlin noise
	enum InterpolationPolicy
	{
		INTERPOLATE_LINEAR = 0,		//!< Linear interpolation
		INTERPOLATE_CUBIC = 1,		//!< Cubic interpolation
		INTERPOLATE_QUINTIC = 2		//!< quintic interpolation (default)
	};

	//! Generation policy
	enum KernelPolicy
	{
		KER_CONSTANT = 0,			//!< The same kernel is used for all octaves but at different frequency
		KER_VARIABLE = 1			//!< all octaves are differents (default)
	};

	//! Boundary policy
	enum BoundaryPolicy
	{
		BOUNDARY_CLAMP = 0,			//!< default perlin noise (default)
		BOUNDARY_REPEAT = 1			//!< Ensure that the noise is connectable
	};

	//! Octave generation policy
	enum OctaveGenerationPolicy
	{
		OCTAVE_SIGNED = 0,			//!< octave values lie in [-1;1] (default)
		OCTAVE_POSITIVE				//!< Octave values lie in [0;1]
	};

	//! A constructor
	PerlinNoise2d();
	//! The copy constructor
	PerlinNoise2d(const PerlinNoise2d &);
	//! The destructor
	~PerlinNoise2d();

	//! Assignment operator
	PerlinNoise2d & operator = (const PerlinNoise2d &);

	//! Comparison operator
	Bool operator == (const PerlinNoise2d &) const;
	Bool operator != (const PerlinNoise2d &) const;

	//! Return the object state
	Bool isValid() const { return (m_size > 0) && !m_amplitudes.empty() && (m_frequencies.size() == m_amplitudes.size()); }

	//! Defines the value of amplitudes
	//! @param _amps the amplitudes. Default: 1/2^k, the length is adapted with respect to the current size
	//!        But if you change the image size, you must call again this function
	void setAmplitudes(const std::vector<Float> & _amps = DEFAULT_AMPLITUDES);
	const std::vector<Float> & getAmplitudes() const { return m_amplitudes; }

	//! Defines the frequency used to generate the noise
	//! @param _freqs the frequencies. Default: (2^k), the length is adapted with respect to the current size
	//!        But if you change the image size, you must call again this function
	void setFrequencies(const std::vector<UInt32> & _freqs = DEFAULT_FREQUENCIES);
	const std::vector<UInt32> & getFrequencies() const { return m_frequencies; }

	//! Defines the size of the image
	//! @param _size the image's size. Default: 256
	void setSize(UInt32 _size) { m_size = _size; }
	UInt32 getSize() const { return m_size; }

	//! Defines the interpolation type
	void setInterpolationPolicy(InterpolationPolicy _type) { m_interpolation = _type; }
	InterpolationPolicy getInterpolationPolicy() const { return m_interpolation; }

	//! Defines the kernel type
	void setKernelPolicy(KernelPolicy _type) { m_kernel = _type; }
	KernelPolicy getKernelPolicy() const { return m_kernel; }

	//! Defines the boundary type
	void setBoundaryPolicy(BoundaryPolicy _type) { m_boundary = _type; }
	BoundaryPolicy getBoundaryPolicy() const { return m_boundary; }

	//! Defines the octave generation policy
	void setOctaveGenerationPolicy(OctaveGenerationPolicy _type) { m_octave = _type; }
	OctaveGenerationPolicy getOctaveGenerationPolicy() const { return m_octave; }

	//! Specify the random generator seed value
	void setRandomSeed(Int32 _seed) { m_randomSeed = _seed; }
	Int32 getRandomSeed() const { return m_randomSeed; }

	//! Store the noise into a picture
	//! @param _rescale defines if the raw data must be rescaled to [0:255].
	//!        If 'false', values are clipped.
	Bool toImage(Image &, Bool _rescale = True) const;

	//! Store the noise into a buffer
	Bool toBuffer(Array2DFloat &) const;

	//! Rapidly and simply generates a perlin noise
	//! @param _size the size of the picture
	//! @param _freqs the octave's frequencies. Default: (2^k) with k in [0;m_size/2]
	//! @param _amps the octave's amplitudes
	static Image create(	UInt32 _size,
								const std::vector<UInt32> & _freqs = DEFAULT_FREQUENCIES,
								const std::vector<Float> & _amps = DEFAULT_AMPLITUDES,
								InterpolationPolicy _interp = INTERPOLATE_QUINTIC,
								KernelPolicy _kernel = KER_VARIABLE,
								OctaveGenerationPolicy _octave = OCTAVE_SIGNED,
								BoundaryPolicy _boundary = BOUNDARY_CLAMP);

	//! returns (_start*_factor^k)_k with k in [0;_size[
	static std::vector<Float> geometricSequence(UInt32 _size, Float _factor, Float _start = 1.0f);
	static std::vector<UInt32> geometricSequence(UInt32 _size, Int32 _factor, Int32 _start = 1);

	//! returns (_start/(1+k*_factor)_k with k in [0;_size[
	static std::vector<Float> harmonicSequence(UInt32 _size, Float _start, Float _factor = 1.0f);

	//! returns (_start + k*_inc)_k with k in [0;_size[
	static std::vector<UInt32> arithmeticSequence(UInt32 _size, UInt32 _start, UInt32 _inc);

private:
	
	std::vector<Float> m_amplitudes;
	std::vector<UInt32> m_frequencies;

	UInt32 m_size;

	InterpolationPolicy m_interpolation;
	KernelPolicy m_kernel;
	BoundaryPolicy m_boundary;
	OctaveGenerationPolicy m_octave;

	Int32 m_randomSeed;

public:

	static const std::vector<Float> DEFAULT_AMPLITUDES;
	static const std::vector<UInt32> DEFAULT_FREQUENCIES;
};

} // namespace o3d

#endif // _O3D_PERLINNOISE2D_H

