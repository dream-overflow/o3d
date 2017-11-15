/**
 * @file perlinnoise2d.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/image/precompiled.h"
#include "o3d/image/perlinnoise2d.h"

#include "o3d/core/math.h"

#include <cmath>
#include <algorithm>

using namespace o3d;

// interpolation functions  f(0) == _v1 and f(1) == _v2
static inline Float interpolate(Float _v1, Float _v2, Float _f, PerlinNoise2d::InterpolationPolicy _type)
{
	float lCoef = 0.0;

	switch(_type)
	{
	case PerlinNoise2d::INTERPOLATE_LINEAR:
		lCoef = _f;
		break;
	case PerlinNoise2d::INTERPOLATE_CUBIC:
		lCoef = _f*_f*(3.0f - 2*_f);
		break;
	default:
		lCoef = _f*_f*_f*(10.0f + _f*(-15.0f + 6.0f*_f));
		break;
	}

	return  (1.0f-lCoef) * _v1 + lCoef * _v2;
}

static inline Float interpolate2D(Float _v00, Float _v10, Float _v01, Float _v11, Float _x, Float _y, PerlinNoise2d::InterpolationPolicy _type)
{
	return interpolate(	interpolate(_v00, _v10, _x, _type),
						interpolate(_v01, _v11, _x, _type),
						_y,
						_type);
}

// Bad noise
static inline Float random(Int32 _seed, Int32 _i)
{
	_i = _seed + _i;
	_i = (_i << 13) ^ _i;

    Float lValue = ( 1.0f - ( (_i * (_i * _i * 15731 + 789221) + 1376312589) & 0x7FFFFFFF) / 1073741824.0f);
	O3D_ASSERT((lValue >= -1.0f) && (lValue <= 1.0f));

	return lValue;
}

static inline void randomShuffle(Int32 _seed, Int32 * _pFirst, Int32 * _pLast)
{
	const Int32 lLength = (_pLast - _pFirst) / sizeof(Int32);

	for (Int32 i = 0 ; i < lLength ; ++i)
	{
		Int32 lIndex = Int32((0.5f + 0.5f * random(_seed, i)) * (lLength - 1));

		Int32 t = _pFirst[lIndex];
		_pFirst[lIndex] = _pFirst[i];
		_pFirst[i] = t;
	}
}

const std::vector<Float> PerlinNoise2d::DEFAULT_AMPLITUDES = std::vector<Float>();
const std::vector<UInt32> PerlinNoise2d::DEFAULT_FREQUENCIES = std::vector<UInt32>();

PerlinNoise2d::PerlinNoise2d():
	m_amplitudes(),
	m_frequencies(),
	m_size(256),
	m_interpolation(INTERPOLATE_QUINTIC),
	m_kernel(KER_VARIABLE),
	m_boundary(BOUNDARY_CLAMP),
	m_octave(OCTAVE_SIGNED),
	m_randomSeed(System::getMsTime())
{
	UInt32 lFreqs[] = {1,2,4,8,16,32,64,128};
	Float lAmps[] = {1.0f, 0.5f, 0.25f, 0.125f, 0.5f*0.125f, 0.25f*0.125f, 0.125f*0.125f, 0.5f*0.125f*0.125f};

	m_frequencies = std::vector<UInt32>(lFreqs, lFreqs + sizeof(lFreqs) / sizeof(UInt32));
	m_amplitudes = std::vector<Float>(lAmps, lAmps + sizeof(lAmps) / sizeof(Float));
}

PerlinNoise2d::PerlinNoise2d(const PerlinNoise2d & _which):
	m_amplitudes(_which.m_amplitudes),
	m_frequencies(_which.m_frequencies),
	m_size(_which.m_size),
	m_interpolation(_which.m_interpolation),
	m_kernel(_which.m_kernel),
	m_boundary(_which.m_boundary),
	m_octave(_which.m_octave),
	m_randomSeed(_which.m_randomSeed)
{
}

PerlinNoise2d::~PerlinNoise2d()
{
}

PerlinNoise2d & PerlinNoise2d::operator = (const PerlinNoise2d & _which)
{
	if (this == &_which)
		return *this;

	m_amplitudes = _which.m_amplitudes;
	m_frequencies = _which.m_frequencies;
	m_size = _which.m_size;
	m_interpolation = _which.m_interpolation;
	m_kernel = _which.m_kernel;
	m_boundary = _which.m_boundary;
	m_octave = _which.m_octave;
	m_randomSeed = _which.m_randomSeed;

	return *this;
}

Bool PerlinNoise2d::operator == (const PerlinNoise2d & _which) const
{
	Bool lEqual = True;

	lEqual &= m_amplitudes == _which.m_amplitudes;
	lEqual &= m_frequencies == _which.m_frequencies;
	lEqual &= m_size == _which.m_size;
	lEqual &= m_interpolation == _which.m_interpolation;
	lEqual &= m_kernel == _which.m_kernel;
	lEqual &= m_boundary == _which.m_boundary;
	lEqual &= m_octave == _which.m_octave;
	lEqual &= m_randomSeed == _which.m_randomSeed;

	return lEqual;
}

Bool PerlinNoise2d::operator != (const PerlinNoise2d & _which) const
{
	return !operator == (_which);
}

void PerlinNoise2d::setAmplitudes(const std::vector<Float> & _amps)
{
	if (&_amps == &DEFAULT_AMPLITUDES)
	{
		UInt32 lSize(o3d::nextPow2(m_size));
		Float lFactor = 2.0f;
		m_amplitudes.clear();

		while ((lSize /= 2) > 0)
			m_amplitudes.push_back(lFactor *= 0.5f);
	}
	else
		m_amplitudes = _amps;
}

void PerlinNoise2d::setFrequencies(const std::vector<UInt32> & _freqs)
{
	if (&_freqs == &DEFAULT_FREQUENCIES)
	{
		UInt32 lSize(o3d::nextPow2(m_size));
		m_frequencies.clear();

		while ((lSize /= 2) > 0)
			m_frequencies.push_back(1 << m_frequencies.size());
	}
	else
		m_frequencies = _freqs;
}

Bool PerlinNoise2d::toImage(Image & _picture, Bool _rescale) const
{
	Array2DFloat lArray;

	if (toBuffer(lArray))
	{
		_picture.allocate(m_size, m_size, 1);
		UInt8 * lpData = _picture.getDataWrite();

		if (_rescale)
		{
			// First pass to get the maximum value used to scale the data
			Float lMaxValue = lArray[0], lMinValue = lArray[0];

			for (UInt32 k = 1; k < lArray.elt() ; ++k)
			{
				lMaxValue = o3d::max<Float>(lMaxValue, lArray[k]);
				lMinValue = o3d::min<Float>(lMinValue, lArray[k]);
			}

			const Float lInvValue = (lMaxValue > lMinValue ? 1.0f/(lMaxValue - lMinValue) : 1.0f);

			for (UInt32 k = 0; k < lArray.elt() ; ++k, lpData++)
				*lpData = UInt8(floor(255.0f * lInvValue * (lArray[k] - lMinValue)));
		}
		else
		{
			for (UInt32 k = 0; k < lArray.elt() ; ++k, lpData++)
				*lpData = UInt8(o3d::max<Float>(0.0f, 255.0f * lArray[k]));
		}
	}
	else
		return False;

	return True;
}

Bool PerlinNoise2d::toBuffer(Array2DFloat & _buffer) const
{
	if (!isValid())
		return False;

	_buffer.setSize(m_size, m_size);
	_buffer.fill(0.0f);

	// Allocation of random permutation vector
	std::vector<Int32> lPermutation(m_size);

	for (std::vector<Int32>::iterator it = lPermutation.begin() ; it != lPermutation.end() ; it++)
		*it = it - lPermutation.begin();

	// Quite ugly to change the state of a global random number generator, but for the moment...
	srand(m_randomSeed);

	//RandomShuffle(m_randomSeed, &lPermutation.front(), &lPermutation.back());
	std::random_shuffle(lPermutation.begin(), lPermutation.end());

	// The following line avoids one call to the modulo function
	lPermutation.push_back(lPermutation.front());

	// Allocation of a vector containing uniformly distributed random values in [0:1]
	std::vector<Float> lRandValues(m_size);

	switch(m_octave)
	{
	case OCTAVE_POSITIVE:
		for (std::vector<Float>::iterator it = lRandValues.begin() ; it != lRandValues.end() ; it++)
			*it = Float(rand()) / RAND_MAX;
//			*it = o3d::abs<Float>(Random(m_randomSeed, it - lRandValues.begin()));
		break;
	default:
		for (std::vector<Float>::iterator it = lRandValues.begin() ; it != lRandValues.end() ; it++)
			*it = 2.0f * Float(rand()) / RAND_MAX - 1.0f;
//			*it = Random(m_randomSeed, it - lRandValues.begin());
	}

	const Int32 lOctaveCount = o3d::min<Int32>(m_amplitudes.size(), m_frequencies.size());

	for (Int32 k = 0 ; k < lOctaveCount ; ++k)
	{
		const Int32 lFreq = Int32(o3d::max<UInt32>(0, m_frequencies[k]));
		const Float lAmp = m_amplitudes[k];

		for (UInt32 j = 0 ; j < m_size; ++j)
		{
			for (UInt32 i = 0 ; i < m_size; ++i)
			{
				Float x = lFreq * float(i) / (m_size);
				Float y = lFreq * float(j) / (m_size);

				Float rx = x - floor(x);
				Float ry = y - floor(y);

				Int32 ix = Int32(floor(x));
				Int32 iy = Int32(floor(y));

				Int32 i00 = lPermutation[(ix + lPermutation[iy])			% m_size];
				Int32 i10 = lPermutation[(ix + 1 + lPermutation[iy])		% m_size];
				Int32 i01 = lPermutation[(ix + lPermutation[iy + 1])		% m_size];
				Int32 i11 = lPermutation[(ix + 1 + lPermutation[iy + 1])	% m_size];

				if (m_boundary == BOUNDARY_REPEAT)
				{
					if ((ix+1 == lFreq) || (iy+1 == lFreq))
					{
						if ((ix+1 == lFreq) && (iy+1 == lFreq))
						{
							i10 = lPermutation[(0 + lPermutation[iy])		];
							i01 = lPermutation[(ix + lPermutation[0])		% m_size];
							i11 = lPermutation[(0 + lPermutation[0])		];
						}
						else if (ix+1 == lFreq)
						{
							i10 = lPermutation[(0 + lPermutation[iy])		];
							i11 = lPermutation[(0 + lPermutation[iy + 1])	];
						}
						else // iy+1 == lFreq
						{
							i01 = lPermutation[(ix + lPermutation[0])		% m_size];
							i11 = lPermutation[(ix + 1 + lPermutation[0])	% m_size];
						}
					}
				}

				_buffer(i,j) += lAmp * interpolate2D(	lRandValues[i00],
														lRandValues[i10],
														lRandValues[i01],
														lRandValues[i11],
														rx,
														ry,
														m_interpolation);
			}
		}

		if (m_kernel == KER_VARIABLE)
			//RandomShuffle(k*m_randomSeed, &lPermutation.front(), &lPermutation.back());
			std::random_shuffle(lRandValues.begin(), lRandValues.end());
	}

	return True;
}

Image PerlinNoise2d::create(	UInt32 _size,
										const std::vector<UInt32> & _freqs,
										const std::vector<Float> & _amps,
										InterpolationPolicy _interp,
										KernelPolicy _kernel,
										OctaveGenerationPolicy _octave,
										BoundaryPolicy _boundary)
{
	Image lImage;

	PerlinNoise2d lPerlin;
	lPerlin.setSize(_size);
	lPerlin.setFrequencies(_freqs);
	lPerlin.setAmplitudes(_amps);
	lPerlin.setInterpolationPolicy(_interp);
	lPerlin.setKernelPolicy(_kernel);
	lPerlin.setBoundaryPolicy(_boundary);
	lPerlin.setOctaveGenerationPolicy(_octave);
	lPerlin.setRandomSeed(System::getMsTime());
	lPerlin.toImage(lImage);

	return lImage;
}

std::vector<Float> PerlinNoise2d::geometricSequence(UInt32 _size, Float _factor, Float _start)
{
	std::vector<Float> lSeq(_size);

	for (std::vector<Float>::iterator it = lSeq.begin() ; it != lSeq.end() ; it++, _start *= _factor)
		*it = _start;

	return lSeq;
}

std::vector<UInt32> PerlinNoise2d::geometricSequence(UInt32 _size, Int32 _factor, Int32 _start)
{
	std::vector<UInt32> lSeq(_size);

	for (std::vector<UInt32>::iterator it = lSeq.begin() ; it != lSeq.end() ; it++, _start *= _factor)
		*it = _start;

	return lSeq;
}

std::vector<Float> PerlinNoise2d::harmonicSequence(UInt32 _size, Float _start, Float _factor)
{
	std::vector<Float> lSeq(_size);

	for (std::vector<Float>::iterator it = lSeq.begin() ; it != lSeq.end() ; it++)
		*it = _start/(1.0f + (it - lSeq.begin()) * _factor);

	return lSeq;
}

std::vector<UInt32> PerlinNoise2d::arithmeticSequence(UInt32 _size, UInt32 _start, UInt32 _inc)
{
	std::vector<UInt32> lSeq(_size);

	for (std::vector<UInt32>::iterator it = lSeq.begin() ; it != lSeq.end() ; it++, _start += _inc)
		*it = _start;

	return lSeq;
}

