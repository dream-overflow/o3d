/**
 * @file color.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_COLOR_H
#define _O3D_COLOR_H

#include "o3d/core/base.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

class InStream;
class OutStream;
class String;

//! Define a 32 bit RGBA color.
typedef UInt32 Rgb;

/**
 * @brief Simple color object.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2002-04-04
 * Float version of a 4 components color.
 */
class O3D_API Color
{
public :

    //! Create a color from an hex3, hex4, hex6 or hex8 format (ie:"ffee99" or with alpha "ffee99ff").
	static Color fromHex(const String &hex);

	//! Default constructor.
	Color();

	//! Construct from an Rgb.
	Color(Rgb rgb);

	//! Copy constructor.
	Color(const Color &src);

	//! Constructor from RGBA float components.
	Color(Float r, Float g, Float b, Float a = 1.f);

	//! Destructor.
	~Color() {}

	//! Duplicate.
	Color& operator= (const Color &col);

	//! Add a color to the current color.
	Color& operator+= (const Color &col);

	//! Add two color and return the result as new color.
	Color operator+ (const Color &col) const;

	//! Mult each components separetely and return a new color.
	Color operator* (const Color &col) const;

	//! Mult each components separetely and the result is stored in this.
	Color& operator*= (const Color &col);

	//! Set all 4 components from a float[4].
	void set(const Float *color);

	//! Set a component from a float.
	inline void set(Int32 c, Float val) { m_data[c] = val; }

	//! Initialize RGBA components.
	void set(Float r, Float g, Float b, Float a = 1.f);

	//! Initialize RGBA components from integer values.
	void setFromInt(Int32 r, Int32 g, Int32 b, Int32 a = 255);

	//! Get red component in integer.
	inline Int32 redInt() const { return (Int32)(m_data[0] * 255.f); }
	//! Get red component in float.
	inline Float red() const { return m_data[0]; }
	//! Set red component from float.
	inline void red(Float r) { m_data[0] = r; }
    //! Get red component in integer.
    inline Int32 rInt() const { return (Int32)(m_data[0] * 255.f); }
    //! Get red component in float.
    inline Float r() const { return m_data[0]; }
    //! Set red component from float.
    inline void r(Float r) { m_data[0] = r; }

	//! Get green component in integer.
	inline Int32 greenInt() const { return (Int32)(m_data[1] * 255.f); }
	//! Get green component in float.
	inline Float green() const { return m_data[1]; }
	//! Set green component from float.
	inline void green(Float g) { m_data[1] = g; }
    //! Get green component in integer.
    inline Int32 gInt() const { return (Int32)(m_data[1] * 255.f); }
    //! Get green component in float.
    inline Float g() const { return m_data[1]; }
    //! Set green component from float.
    inline void g(Float g) { m_data[1] = g; }

	//! Get blue component in integer.
	inline Int32 blueInt() const { return (Int32)(m_data[2] * 255.f); }
	//! Get blue component in float.
	inline Float blue() const { return m_data[2]; }
	//! Set blue component from float.
	inline void blue(Float b) { m_data[2] = b; }
    //! Get blue component in integer.
    inline Int32 bInt() const { return (Int32)(m_data[2] * 255.f); }
    //! Get blue component in float.
    inline Float b() const { return m_data[2]; }
    //! Set blue component from float.
    inline void b(Float b) { m_data[2] = b; }

	//! Get alpha component in integer.
	inline Int32 alphaInt() const { return (Int32)(m_data[3] * 255.f); }
	//! Get alpha component in float.
	inline Float alpha() const { return m_data[3]; }
	//! Set alpha component from float.
	inline void alpha(Float a) { m_data[3] = a; }
    //! Get alpha component in integer.
    inline Int32 aInt() const { return (Int32)(m_data[3] * 255.f); }
    //! Get alpha component in float.
    inline Float a() const { return m_data[3]; }
    //! Set alpha component from float.
    inline void a(Float a) { m_data[3] = a; }

	//! Get a component.
	inline Float& operator[] (UInt32 c) { return m_data[c]; }

	//! Get a component (read only).
	inline const Float& operator[] (UInt32 c) const { return m_data[c]; }

	//! Access to data array.
	inline Float* getData() { return m_data; }

	//! Access to data array (read only).
	inline const Float* getData() const { return m_data; }

	//! Get RGBA components in integer values.
	void getRgbInt(Int32 &r, Int32 &g, Int32 &b, Int32 &a) const;

	//! Get RGBA components in float values.
	void getRbg(Float &r, Float &g, Float &b, Float &a) const;

	//! Get RGBA components in an array of 4 integer values.
	void getRgbUInt8Array(UInt8 *col) const;

	//! Get the O3DRgb integer color.
	Rgb getRgb() const;

	//! Set from an O3DRgb integer color.
	void setRgb(Rgb rgb);

	//! Scalar product and clamp to 1.0.
	Color& operator*= (Float scale);

	//! Scalar product and clamp to 1.0 and return the new value.
	Color operator* (Float scale) const;

	//! Compare two colors and return TRUE if equals.
	Bool operator== (const Color &comp) const;

	//! Compare two colors and return TRUE if different.
	Bool operator!= (const Color &comp) const;

	//! Serialize to file.
	Bool writeToFile(OutStream &os) const;
	//! Un-serialize from file.
	Bool readFromFile(InStream &is);

	//! convert to a string (useful for debugging) { r, g, b, a }
	operator String() const;

	//! Set from a string. r g b [a]. alpha is optional. Must be float.
	void parse(const String &str);

protected :

	Float m_data[4];    //!< RGBA
};

//! Define an Rgb color from RGB components and 255 alpha.
inline Rgb rgb(Int32 r, Int32 g, Int32 b)
{
	return r | (g << 8) | (b << 16) | 0xff000000;
}

//! Define an Rgb color from RGBA components.
inline Rgb rgba(Int32 r, Int32 g, Int32 b, Int32 a)
{
	return r | (g << 8) | (b << 16) | (a << 24);
}

//! Get the Red component of an Rgb color.
inline Int32 red(Rgb rgb) { return rgb & 0xff; }

//! Get the Green component of an Rgb color.
inline Int32 green(Rgb rgb) { return (rgb & 0xff00) >> 8; }

//! Get the Blue component of an Rgb color.
inline Int32 blue(Rgb rgb) { return (rgb & 0xff0000) >> 16; }

//! Get the Alpha component of an Rgb color.
inline Int32 alpha(Rgb rgb) { return (rgb & 0xff000000) >> 24; }

//! Convert to gray an Rgb color.
inline Int32 gray(Rgb rgb)
{
	return (red(rgb)*11 + green(rgb)*16 + blue(rgb)*5) / 32;
}

} // namespace o3d

#endif // _O3D_COLOR_H

