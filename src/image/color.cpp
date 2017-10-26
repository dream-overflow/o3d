/**
 * @file color.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/image/precompiled.h"
#include "o3d/image/color.h"

#include "o3d/core/instream.h"
#include "o3d/core/outstream.h"

#include <math.h>

using namespace o3d;

//---------------------------------------------------------------------------------------
// Color
//---------------------------------------------------------------------------------------

static inline UInt8 fromHexA0(WChar a)
{
    UInt8 r;

    if (a >= 'a')
    {
        r = (a - 'a' + 10) << 4;
    }
    else
    {
        r = (a - '0') << 4;
    }

    return r;
}

static inline UInt8 fromHexAB(WChar a, WChar b)
{
	UInt8 r;

	if (a >= 'a')
	{
		r = (a - 'a' + 10) << 4;
	}
	else
	{
		r = (a - '0') << 4;
	}

	if (b >= 'a')
	{
		r += b - 'a' + 10;
	}
	else
	{
		r += b - '0';
	}

	return r;
}

Color Color::fromHex(const String &hex)
{
	Color color;
	String value(hex);
    value.lower();

    // hex3 (no alpha)
    if (hex.length() == 3)
    {
        return Color(
                    fromHexA0(value[0]) / 255.f,
                    fromHexA0(value[1]) / 255.f,
                    fromHexA0(value[2]) / 255.f,
                    1.0f);
    }
    // hex4
    else if (hex.length() == 4)
    {
        return Color(
                    fromHexA0(value[0]) / 255.f,
                    fromHexA0(value[1]) / 255.f,
                    fromHexA0(value[2]) / 255.f,
                    fromHexA0(value[3]) / 255.f);
    }
    // hex6 (no alpha)
    else if (hex.length() == 6)
	{
		return Color(
					fromHexAB(value[0], value[1]) / 255.f,
					fromHexAB(value[2], value[3]) / 255.f,
                    fromHexAB(value[4], value[5]) / 255.f,
                    1.0f);
	}
    // hex8
	else if (hex.length() == 8)
	{
		return Color(
					fromHexAB(value[0], value[1]) / 255.f,
					fromHexAB(value[2], value[3]) / 255.f,
					fromHexAB(value[4], value[5]) / 255.f,
					fromHexAB(value[6], value[7]) / 255.f);
	}

	return color;
}

Color::Color()
{
	m_data[0] = m_data[1] = m_data[2] = m_data[3] = 0.0f;
}

Color::Color(Rgb rgb)
{
	m_data[0] = o3d::red(rgb) / 255.f;
	m_data[1] = o3d::green(rgb) / 255.f;
	m_data[2] = o3d::blue(rgb) / 255.f;
	m_data[3] = o3d::alpha(rgb) / 255.f;
}

Color::Color(const Color &src)
{
	m_data[0] = src[0];
	m_data[1] = src[1];
	m_data[2] = src[2];
	m_data[3] = src[3];
}

Color::Color(Float r, Float g, Float b, Float a)
{
	m_data[0] = r;
	m_data[1] = g;
	m_data[2] = b;
	m_data[3] = a;
}

// Initialize RGBA components.
void Color::set(Float r, Float g, Float b, Float a)
{
	m_data[0] = r;
	m_data[1] = g;
	m_data[2] = b;
	m_data[3] = a;
}

// Initialize RGBA components from integer values.
void Color::setFromInt(Int32 r, Int32 g, Int32 b, Int32 a)
{
	m_data[0] = r / 255.f;
	m_data[1] = g / 255.f;
	m_data[2] = b / 255.f;
	m_data[3] = a / 255.f;
}

// Scalar product and clamp to 1.0.
Color& Color::operator*= (Float scale)
{
	m_data[0] *= scale;
	m_data[1] *= scale;
	m_data[2] *= scale;
	m_data[3] *= scale;

	if (m_data[0] > 1.0f)
		m_data[0] = 1.0f;

	if (m_data[1] > 1.0f)
		m_data[1] = 1.0f;

	if (m_data[2] > 1.0f)
		m_data[2] = 1.0f;

	if (m_data[3] > 1.0f)
		m_data[3] = 1.0f;

	return (*this);
}

// Scalar product and clamp to 1.0 and return the new value.
Color Color::operator* (Float scale) const
{
	Color result;

	result[0] = m_data[0] * scale;
	result[1] = m_data[1] * scale;
	result[2] = m_data[2] * scale;
	result[3] = m_data[3] * scale;

	if (result[0] > 1.0f)
		result[0] = 1.0f;

	if (result[1] > 1.0f)
		result[1] = 1.0f;

	if (result[2] > 1.0f)
		result[2] = 1.0f;

	if (result[3] > 1.0f)
		result[3] = 1.0f;

	return result;
}

// duplicate
Color& Color::operator= (const Color &src)
{
	m_data[0] = src.m_data[0];
	m_data[1] = src.m_data[1];
	m_data[2] = src.m_data[2];
	m_data[3] = src.m_data[3];

	return *this;
}

// Add a color to the current color.
Color& Color::operator+= (const Color &col)
{
	m_data[0] += col.m_data[0];
	m_data[1] += col.m_data[1];
	m_data[2] += col.m_data[2];
	m_data[3] += col.m_data[3];

	return *this;
}

// Add two color and return the result as new color.
Color Color::operator+ (const Color &col) const
{
	return Color(
			m_data[0] + col.m_data[0],
			m_data[1] + col.m_data[1],
			m_data[2] + col.m_data[2],
			m_data[3] + col.m_data[3]);
}

// Mult each components separetely and return a new color.
Color Color::operator* (const Color &col) const
{
	return Color(
			m_data[0] * col.m_data[0],
			m_data[1] * col.m_data[1],
			m_data[2] * col.m_data[2],
			m_data[3] * col.m_data[3]);
}

// Mult each components and the result is stored in this.
Color& Color::operator*= (const Color &col)
{
	m_data[0] *= col.m_data[0];
	m_data[1] *= col.m_data[1];
	m_data[2] *= col.m_data[2];
	m_data[3] *= col.m_data[3];

	return *this;
}

// Set all 4 components from a float[4].
void Color::set(const Float *color)
{
	O3D_ASSERT(color);
	memcpy(m_data, color, 4*sizeof(Float));
}

// Get RGBA components in integer values.
void Color::getRgbInt(Int32 &r, Int32 &g, Int32 &b, Int32 &a) const
{
	r = (Int32)(m_data[0] * 255.f);
	g = (Int32)(m_data[1] * 255.f);
	b = (Int32)(m_data[2] * 255.f);
	a = (Int32)(m_data[3] * 255.f);
}

// Get RGBA components in float values.
void Color::getRbg(Float &r, Float &g, Float &b, Float &a) const
{
	r = m_data[0];
	g = m_data[1];
	b = m_data[2];
	a = m_data[3];
}

// Get RGBA components in an array of 4 integer values.
void Color::getRgbUInt8Array(UInt8 *col) const
{
	col[0] = (UInt8)(m_data[0] * 255.f);
	col[1] = (UInt8)(m_data[1] * 255.f);
	col[2] = (UInt8)(m_data[2] * 255.f);
	col[3] = (UInt8)(m_data[3] * 255.f);
}

//! Get the O3DRgb integer color.
Rgb Color::getRgb() const
{
	return o3d::rgba(
		(Int32)(m_data[0] * 255.f),
 		(Int32)(m_data[1] * 255.f),
 		(Int32)(m_data[2] * 255.f),
 		(Int32)(m_data[3] * 255.f));
}

//! Set from an O3DRgb integer color.
void Color::setRgb(Rgb rgb)
{
	m_data[0] = o3d::red(rgb) / 255.f;
	m_data[1] = o3d::green(rgb) / 255.f;
	m_data[2] = o3d::blue(rgb) / 255.f;
	m_data[3] = o3d::alpha(rgb) / 255.f;
}

// compare two colors and return TRUE if equals.
Bool Color::operator== (const Color &comp) const
{
	if (fabs(comp.m_data[0] - m_data[0]) > Limits<Float>::epsilon())
		return False;
	if (fabs(comp.m_data[1] - m_data[1]) > Limits<Float>::epsilon())
		return False;
	if (fabs(comp.m_data[2] - m_data[2]) > Limits<Float>::epsilon())
		return False;
	if (fabs(comp.m_data[3] - m_data[3]) > Limits<Float>::epsilon())
		return False;
	else
		return True;
}

// compare two colors and return TRUE if different.
Bool Color::operator!= (const Color &comp) const
{
	if (fabs(comp.m_data[0] - m_data[0]) > Limits<Float>::epsilon())
		return True;
	if (fabs(comp.m_data[1] - m_data[1]) > Limits<Float>::epsilon())
		return True;
	if (fabs(comp.m_data[2] - m_data[2]) > Limits<Float>::epsilon())
		return True;
	if (fabs(comp.m_data[3] - m_data[3]) > Limits<Float>::epsilon())
		return True;
	else
		return False;
}

// Serialization
Bool Color::writeToFile(OutStream &os) const
{
    os   << m_data[0]
	     << m_data[1]
		 << m_data[2]
		 << m_data[3];

	return True;
}

Bool Color::readFromFile(InStream &is)
{
    is   >> m_data[0]
		 >> m_data[1]
		 >> m_data[2]
		 >> m_data[3];

	return True;
}

// convert to a string
Color::operator String() const
{
	String temp;
	temp << String("{ ") << m_data[0] << String(", ") << m_data[1] << String(", ") << m_data[2] << String(", ") << m_data[3] << String(" }");
	return temp;
}

// Set from a string. r g b [a]. alpha is optional. Must be float.
void Color::parse(const String &str)
{
	String current = str;

	m_data[0] = m_data[1] = m_data[2] = 0.f;
	m_data[3] = 1.f;

	Int32 pos;

	for (Int32 i = 0; i < 4; ++i)
	{
		current.trimLeft(' ');

		if (current.isEmpty())
			return;

		m_data[i] = current.toFloat(0);

		pos = current.find(' ');
		if (pos == -1)
			return;

		current = current.sub(pos+1);
	}
}

