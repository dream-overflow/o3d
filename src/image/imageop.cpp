/**
 * @file imageop.cpp
 * @brief Implementation of Picture.h
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/image/precompiled.h"
#include "o3d/image/imageop.h"

#include "o3d/core/math.h"

using namespace o3d;

Bool Image::vFlip()
{
	if (!m_state)
		O3D_ERROR(E_InvalidPrecondition("Picture must be valid"));

	if (isCompressed())
		O3D_ERROR(E_InvalidFormat("Works only with non compressed picture"));

	if (isComplex())
		O3D_ERROR(E_InvalidFormat("Complex picture are not supported at this time"));

	if (!isColorU8())
		O3D_ERROR(E_InvalidFormat("Only color 8bpp format are supported"));

	if (!m_data.get() || !m_data->data || !m_width || !m_height)
		return False;

	UInt8 *exdata = m_data->data;
	UInt8 *data = new UInt8[m_size];

	UInt32 numComp = getNumComponents();

	UInt32 lx,ly;
	UInt32 ly2 = (m_height-1);	  // on inverse les ligne de haut en bas
	UInt32 x = m_width * numComp; // taille d'une ligne en octet

	if (numComp == 1)
	{
		for ( ly = 0 ; ly < m_height ; ++ly )	// pour chaque ligne de 0..sizeY
		{
			// on lit chaque (L/A) de la ligne
			for ( lx = 0 ; lx < m_width ; ++lx )
			{
				data[lx+ly*x] = exdata[lx+ly2*x];
			}

			ly2--;
		}
	}
	else if (numComp == 2)
	{
		for ( ly = 0 ; ly < m_height ; ++ly )	// pour chaque ligne de 0..sizeY
		{
			// on lit chaque (LA) de la ligne
			for ( lx = 0 ; lx < x ; lx += 2 )
			{
				data[lx+ly*x]   = exdata[lx+ly2*x];		//L
				data[lx+ly*x+1] = exdata[lx+ly2*x+1];	//A
			}

			ly2--;						// et on décrement d'une ligne
		}
	}
	else if (numComp == 3)
	{
		for ( ly = 0 ; ly < m_height ; ++ly )	// pour chaque ligne de 0..sizeY
		{
			// on lit chaque (RGB) de la ligne
			for ( lx = 0 ; lx < x ; lx += 3 )
			{
				data[lx+ly*x]   = exdata[lx+ly2*x];		//R
				data[lx+ly*x+1] = exdata[lx+ly2*x+1];	//G
				data[lx+ly*x+2] = exdata[lx+ly2*x+2];	//B
			}

			ly2--;						// et on décrement d'une ligne
		}
	}
	else if (numComp == 4)
	{
		for ( ly = 0 ; ly < m_height ; ++ly )	// pour chaque ligne de 0..sizeY
		{
			// on lit chaque (RGB) de la ligne
			for ( lx = 0 ; lx < x ; lx += 4 )
			{
				data[lx+ly*x]   = exdata[lx+ly2*x];		//R
				data[lx+ly*x+1] = exdata[lx+ly2*x+1];	//G
				data[lx+ly*x+2] = exdata[lx+ly2*x+2];	//B
				data[lx+ly*x+3] = exdata[lx+ly2*x+3];   //A
			}

			ly2--;						// et on décrement d'une ligne
		}
	}

	m_data = new ImageData(data);

	setDirty();
	return True;
}

Bool Image::hFlip()
{
	if (!m_state)
		O3D_ERROR(E_InvalidPrecondition("Picture must be valid"));

	if (isCompressed())
		O3D_ERROR(E_InvalidFormat("Works only with non compressed picture"));

	if (isComplex())
		O3D_ERROR(E_InvalidFormat("Complex picture are not supported at this time"));

	if (!isColorU8())
		O3D_ERROR(E_InvalidFormat("Only color 8bpp format are supported"));

	if (!m_data.get() || !m_data->data || !m_width || !m_height)
		return False;

	UInt8 *exdata = m_data->data;
	UInt8 *data = new UInt8[m_size];

	UInt32 numComp = getNumComponents();

	UInt32 lx,ly;
	UInt32 lx2;						// on inverse les colonnes
	UInt32 x = m_width * numComp;			// taille d'une ligne en octet

	if (numComp == 1)
	{
		for ( ly = 0 ; ly < m_height ; ++ly )	// pour chaque ligne de 0..sizeY
		{
			lx2 = x - 1;

			// on lit chaque (L/A) de la ligne
			for ( lx = 0 ; lx < m_width  ; ++lx )
			{
				data[lx+ly*x] = exdata[lx2+ly*x];		//index

				lx2--;
			}
		}
	}
	else if (numComp == 2)
	{
		for ( ly = 0 ; ly < m_height ; ++ly )	// pour chaque ligne de 0..sizeY
		{
			lx2 = x - 2;

			// on lit chaque (LA) de la ligne
			for ( lx = 0 ; lx < m_width * numComp ; lx += 2 )
			{
				data[lx+ly*x]   = exdata[lx2+ly*x];		//L
				data[lx+ly*x+1] = exdata[lx2+ly*x+1];	//A

				lx2 -= 2;
			}
		}
	}
	else if (numComp == 3)
	{
		for ( ly = 0 ; ly < m_height ; ++ly )	// pour chaque ligne de 0..sizeY
		{
			lx2 = x - 3;

			// on lit chaque (RGB) de la ligne
			for ( lx = 0 ; lx < m_width * numComp ; lx += 3 )
			{
				data[lx+ly*x]   = exdata[lx2+ly*x];		//R
				data[lx+ly*x+1] = exdata[lx2+ly*x+1];	//G
				data[lx+ly*x+2] = exdata[lx2+ly*x+2];	//B

				lx2 -= 3;
			}
		}
	}
	else if (numComp == 4)
	{
		for ( ly = 0 ; ly < m_height ; ++ly )	// pour chaque ligne de 0..sizeY
		{
			lx2 = x - 4;

			// on lit chaque (RGB) de la ligne
			for ( lx = 0 ; lx < m_width * numComp ; lx += 4 )
			{
				data[lx+ly*x]   = exdata[lx2+ly*x];		//R
				data[lx+ly*x+1] = exdata[lx2+ly*x+1];	//G
				data[lx+ly*x+2] = exdata[lx2+ly*x+2];	//B
				data[lx+ly*x+3] = exdata[lx2+ly*x+3];   //A

				lx2 -= 4;
			}
		}
	}

	m_data = new ImageData(data);

	setDirty();
	return True;
}

Bool Image::flip()
{
	if (!m_state)
		O3D_ERROR(E_InvalidPrecondition("Picture must be valid"));

	if (isCompressed())
		O3D_ERROR(E_InvalidFormat("Works only with non compressed picture"));

	if (isComplex())
		O3D_ERROR(E_InvalidFormat("Complex picture are not supported at this time"));

	if (!isColorU8())
		O3D_ERROR(E_InvalidFormat("Only color 8bpp format are supported"));

	if (!m_data.get() || !m_data->data || !m_width || !m_height)
		return False;

	UInt8 *exdata = m_data->data;
	UInt8 *data = new UInt8[m_size];

	UInt32 numComp = getNumComponents();
	UInt32 j = m_size - numComp;

	if (numComp == 1)
	{
		for (UInt32 i = 0 ; i < (UInt32)(m_width * m_height * 2) ; ++i )
		{
			data[i] = exdata[j];
			j--;
		}
	}
	else if (numComp == 2)
	{
		for (UInt32 i = 0 ; i < (UInt32)(m_width * m_height * 2) ; i += 2 )
		{
			data[i]   = exdata[j];
			data[i+1] = exdata[j+1];

			j -= 2;
		}
	}
	else if (numComp == 3)
	{
		for (UInt32 i = 0 ; i < (UInt32)(m_width * m_height * 3) ; i += 3 )
		{
			data[i]   = exdata[j];
			data[i+1] = exdata[j+1];
			data[i+2] = exdata[j+2];

			j -= 3;
		}
	}
	else if (numComp == 4)
	{
		for (UInt32 i = 0 ; i < (UInt32)(m_width * m_height * 4) ; i += 4 )
		{
			data[i]   = exdata[j];
			data[i+1] = exdata[j+1];
			data[i+2] = exdata[j+2];
			data[i+3] = exdata[j+3];

			j -= 4;
		}
	}

	m_data = new ImageData(data);

	setDirty();
	return True;
}

// create a new picture using a background color.
Bool Image::loadWithColor(
	UInt32 cx,
	UInt32 cy,
	UInt8 r,
	UInt8 g,
	UInt8 b,
	UInt8 a,
	Bool alpha)
{
	UInt32 numComponents = 3;

	if (m_state)
		destroy();

	if (alpha)
		++numComponents;

	m_data = new ImageData(new UInt8[cx*cy*numComponents]);

	// RGBA8
	if (numComponents == 4)
	{
		UInt32 len = cx * cy;
		UInt32 *buf = (UInt32*)m_data->data;

		Rgb color = o3d::rgba(r, g, b, a);
		//UInt8 col[4] = { r,g,b,a };
		//UInt32 color = *(UInt32*)&col;

		for (UInt32 i = 0; i < len; ++i)
		{
			*buf++ = color;
		}
	}
	// RGB8
	else
	{
		/*UInt8 *buf = m_data->data;
		UInt32 len = cx * cy * 3;
		for ( UInt32 i = 0 ; i < len ; i += 3)
		{
			buf[i]   = r;
			buf[i+1] = g;
			buf[i+2] = b;
		}*/

		UInt32 len = cx * cy * 3 / 4;
		UInt32 *buf = (UInt32*)m_data->data;

		Rgb color = o3d::rgba(r, g, b, r);
		//UInt8 col[4] = { r,g,b,r };
		//UInt32 color = *(UInt32*)&col;

		for (UInt32 i = 0; i < len; ++i)
		{
			*buf++ = color;
		}

		if ((cx * cy * 3) % 4)
		{
			UInt8 *last = (UInt8*)buf;
			*last++ = r;
			*last++ = g;
			*last = b;
		}
	}

	m_size = cx * cy * numComponents;
	m_width = cx;
	m_height = cy;
	m_state = True;

	switch (numComponents)
	{
		case 3:
			m_pixelFormat = PF_RGB_U8;
			break;
		case 4:
			m_pixelFormat = PF_RGBA_U8;
			break;
		default:
			break;
	}

	setDirty();
	return True;
}

// process a blending with another picture and blending scalar
Bool Image::blending(const Image& pic, Float alpha)
{
	if (!pic.m_state)
		O3D_ERROR(E_InvalidParameter("Picture must be valid"));

	if (!m_state)
		O3D_ERROR(E_InvalidPrecondition("Picture must be valid"));

	if (isCompressed() || pic.isCompressed())
		O3D_ERROR(E_InvalidFormat("Works only with non compressed picture"));

	if (isComplex() || pic.isComplex())
		O3D_ERROR(E_InvalidFormat("Complex picture are not supported at this time"));

	if (!isColorU8() || !pic.isColorU8())
		O3D_ERROR(E_InvalidFormat("Only color 8bpp format are supported"));

	// two pics with != formats
	if ((m_pixelFormat != pic.m_pixelFormat) || (m_size != pic.m_size) ||
		(m_width != pic.m_width) || (m_height != pic.m_height))
	{
		O3D_ERROR(E_InvalidFormat("The two picture must have the same format"));
	}

	if (!m_data.get() || !m_data->data || !m_width || !m_height)
		return False;

	UInt32 numComp = getNumComponents();
	detach();

	UInt8 *data = m_data->data;
	UInt8 *src2 = pic.m_data->data;

	if (numComp == 1)
	{
		for (UInt32 i = 0 ; i < (UInt32)(m_width*m_height) ; ++i)
		{
			data[i] = (UInt8)(alpha * (Float)src2[i]) + (UInt8)((1.0f-alpha) * (Float)data[i]);
		}
	}
	else if (numComp == 2)
	{
		/*if (BlendAlpha)
		{
			for (UInt32 i = 0 ; i < (UInt32)(sizeX*sizeY*2) ; i += 2)
			{
				data[i] = (UInt8)(alpha * (Float)src2[i]) + (UInt8)((1.0f-alpha) * (Float)data[i]);
				data[i+1] = (UInt8)((1.f-alpha)*255.0f);
			}
		}
		else*/
		{
			for (UInt32 i = 0 ; i < (UInt32)(m_width*m_height*2) ; i += 2)
			{
				data[i] = (UInt8)(alpha * (Float)src2[i]) + (UInt8)((1.0f-alpha) * (Float)data[i]);
			}
		}
	}
	else if (numComp == 3)
	{
		for (UInt32 i = 0 ; i < (UInt32)(m_width*m_height*3) ; i += 3)
		{
			data[i]   = (UInt8)(alpha * (Float)src2[i])   + (UInt8)((1.0f-alpha) * (Float)data[i]);
			data[i+1] = (UInt8)(alpha * (Float)src2[i+1]) + (UInt8)((1.0f-alpha) * (Float)data[i+1]);
			data[i+2] = (UInt8)(alpha * (Float)src2[i+2]) + (UInt8)((1.0f-alpha) * (Float)data[i+2]);
		}
	}
	else if (numComp == 4)
	{
		/*if (BlendAlpha)
		{
			for (UInt32 i = 0 ; i < (UInt32)(sizeX*sizeY*4) ; i += 4)
			{
				data[i]   = (UInt8)(alpha * (Float)src2[i])   + (UInt8)((1.0f-alpha) * (Float)data[i]);
				data[i+1] = (UInt8)(alpha * (Float)src2[i+1]) + (UInt8)((1.0f-alpha) * (Float)data[i+1]);
				data[i+2] = (UInt8)(alpha * (Float)src2[i+2]) + (UInt8)((1.0f-alpha) * (Float)data[i+2]);

				data[i+3] = (UInt8)((1.f-alpha)*255.0f);
			}
		}
		else*/
		{
			for (UInt32 i = 0 ; i < (UInt32)(m_width*m_height*4) ; i += 4)
			{
				data[i]   = (UInt8)(alpha * (Float)src2[i])   + (UInt8)((1.0f-alpha) * (Float)data[i]);
				data[i+1] = (UInt8)(alpha * (Float)src2[i+1]) + (UInt8)((1.0f-alpha) * (Float)data[i+1]);
				data[i+2] = (UInt8)(alpha * (Float)src2[i+2]) + (UInt8)((1.0f-alpha) * (Float)data[i+2]);
			}
		}
	}

	setDirty();
	return True;
}

// effectue une transparence avec une couleur.
Bool Image::mask(UInt8 r, UInt8 g, UInt8 b, Float alpha)
{
	if (!m_state)
		O3D_ERROR(E_InvalidPrecondition("Picture must be valid"));

	if (isCompressed())
		O3D_ERROR(E_InvalidFormat("Works only with non compressed picture"));

	if (isComplex())
		O3D_ERROR(E_InvalidFormat("Complex picture are not supported at this time"));

	if (!isColorU8())
		O3D_ERROR(E_InvalidFormat("Only color 8bpp format are supported"));

	if (!m_data.get() || !m_data->data || !m_width || !m_height)
		return False;

	UInt32 numComp = getNumComponents();
	detach();

	UInt8 *data = m_data->data;

	if (numComp == 1)
	{
		for (UInt32 i = 0 ; i < (UInt32)(m_width*m_height) ; ++i)
		{
			data[i] = (UInt8)(alpha * (Float)r) + (UInt8)((1.0f-alpha) * (Float)data[i]);
		}
	}
	else if (numComp == 2)
	{
		/*if (MaskAlpha)
		{
			for (UInt32 i = 0 ; i < (UInt32)(sizeX*sizeY*2) ; i += 2)
			{
				data[i] = (UInt8)(alpha * (Float)r) + (UInt8)((1.0f-alpha) * (Float)data[i]);
				data[i+1] = (UInt8)((1.f-alpha)*255.0f);
			}
		}
		else*/
		{
			for (UInt32 i = 0 ; i < (UInt32)(m_width*m_height*2) ; i += 2)
			{
				data[i] = (UInt8)(alpha * (Float)r) + (UInt8)((1.0f-alpha) * (Float)data[i]);
			}
		}
	}
	else if (numComp == 3)
	{
		for (UInt32 i = 0 ; i < (UInt32)(m_width*m_height*3) ; i += 3)
		{
			data[i] =   (UInt8)(alpha * (Float)r) + (UInt8)((1.0f-alpha) * (Float)data[i]);
			data[i+1] = (UInt8)(alpha * (Float)g) + (UInt8)((1.0f-alpha) * (Float)data[i+1]);
			data[i+2] = (UInt8)(alpha * (Float)b) + (UInt8)((1.0f-alpha) * (Float)data[i+2]);
		}
	}
	else if (numComp == 4)
	{
		/*if (MaskAlpha)
		{
			for (UInt32 i = 0 ; i < (UInt32)(sizeX*sizeY*4) ; i += 4)
			{
				data[i] =   (UInt8)(alpha * (Float)r) + (UInt8)((1.0f-alpha) * (Float)data[i]);
				data[i+1] = (UInt8)(alpha * (Float)g) + (UInt8)((1.0f-alpha) * (Float)data[i+1]);
				data[i+2] = (UInt8)(alpha * (Float)b) + (UInt8)((1.0f-alpha) * (Float)data[i+2]);
				data[i+3] = (UInt8)((1.f-alpha)*255.0f);
			}
		}
		else*/
		{
			for (UInt32 i = 0 ; i < (UInt32)(m_width*m_height*4) ; i += 4)
			{
				data[i] =   (UInt8)(alpha * (Float)r) + (UInt8)((1.0f-alpha) * (Float)data[i]);
				data[i+1] = (UInt8)(alpha * (Float)g) + (UInt8)((1.0f-alpha) * (Float)data[i+1]);
				data[i+2] = (UInt8)(alpha * (Float)b) + (UInt8)((1.0f-alpha) * (Float)data[i+2]);
			}
		}
	}

	setDirty();
	return True;
}

// Negative all pixels.
Bool Image::negative(Bool Alpha)
{
	if (!m_state)
		O3D_ERROR(E_InvalidPrecondition("Picture must be valid"));

	if (isCompressed())
		O3D_ERROR(E_InvalidFormat("Works only with non compressed picture"));

	if (isComplex())
		O3D_ERROR(E_InvalidFormat("Complex picture are not supported at this time"));
	
	if (!isColorU8())
		O3D_ERROR(E_InvalidFormat("Only color 8bpp format are supported"));

	if (!m_data.get() || !m_data->data || !m_width || !m_height)
		return False;

	UInt32 numComp = getNumComponents();

	detach();

	UInt8 *data = m_data->data;

	if (numComp == 1)
	{
		for (UInt32 i = 0 ; i < (UInt32)(m_width*m_height) ; ++i)
		{
			data[i] = 255 - data[i];
		}
	}
	else if (numComp == 2)
	{
		if (Alpha)
		{
			for (UInt32 i = 0 ; i < (UInt32)(m_width*m_height*2) ; i += 2)
			{
				data[i] = 255 - data[i];
				data[i+1] = 255 - data[i+1];
			}
		}
		else
		{
			for (UInt32 i = 0 ; i < (UInt32)(m_width*m_height*2) ; i += 2)
			{
				data[i] = 255 - data[i];
			}
		}
	}
	else if (numComp == 3)
	{
		for (UInt32 i = 0 ; i < (UInt32)(m_width*m_height*3) ; i += 3)
		{
			data[i]   = 255 - data[i];
			data[i+1] = 255 - data[i+1];
			data[i+2] = 255 - data[i+2];
		}
	}
	else if (numComp == 4)
	{
		if (Alpha)
		{
			for (UInt32 i = 0 ; i < (UInt32)(m_width*m_height*4) ; i += 4)
			{
				data[i]   = 255 - data[i];
				data[i+1] = 255 - data[i+1];
				data[i+2] = 255 - data[i+2];
				data[i+3] = 255 - data[i+3];
			}
		}
		else
		{
			for (UInt32 i = 0 ; i < (UInt32)(m_width*m_height*4) ; i += 4)
			{
				data[i]   = 255 - data[i];
				data[i+1] = 255 - data[i+1];
				data[i+2] = 255 - data[i+2];
			}
		}
	}

	setDirty();
	return True;
}

// generate the normal map (internal)
void Image::genNormalMap(UInt8* dst, Float scale, Bool wrap) const
{
	Int32 i0;
	Int32 j0;
	Int32 i1;
	Int32 j1;

	Int32 h00 = 0;
	Int32 h10 = 0;
	Int32 h01 = 0;

	Float dx;
	Float dy;

	Float mag;
	Float rmag;

	Int32 n;

	UInt32 numComp = getNumComponents();
	UInt8 *src = m_data->data;

	// Compute normals using differences between adjacent texels
	for (Int32 i = 0 ; i < (Int32)m_height ; ++i)
	{
		i0 = i;
		i1 = i - 1;

		if (i1 < 0)
		{
			if (wrap)
			{
				i1 = m_height - 1;
			}
			else
			{
				i0++;
				i1++;
			}
		}

		for (Int32 j = 0 ; j < (Int32)m_width ; ++j)
		{
			j0 = j;
			j1 = j - 1;

			if (j1 < 0)
			{
				if (wrap)
				{
					j1 = m_width - 1;
				}
				else
				{
					j0++;
					j1++;
				}
			}

			h00 = (Int32)src[(i0 * m_width + j0) * numComp];
			h10 = (Int32)src[(i0 * m_width + j1) * numComp];
			h01 = (Int32)src[(i1 * m_width + j0) * numComp];

			// delta
			dx = (Float)((h10 - h00) / 255.0f) * scale;
			dy = (Float)((h01 - h00) / 255.0f) * scale;

			mag = Math::sqrt(dx * dx + dy * dy + 1.0f);
			rmag = 1.0f / mag;

			n = (i * m_width + j) * 4;

			dst[n]     = (UInt8)(128 + 127 * dx * rmag);
			dst[n + 1] = (UInt8)(128 + 127 * dy * rmag);
			dst[n + 2] = (UInt8)(128 + 127 * rmag);
			dst[n + 3] = 255;
		}
	}
}

// convert this picture from height-map (grey-scale) to a RGB normal map
Bool Image::toNormalMap(Float scale, Bool wrap)
{
	if (!m_state)
		O3D_ERROR(E_InvalidPrecondition("Picture must be valid"));

	if (isCompressed())
		O3D_ERROR(E_InvalidFormat("Works only with non compressed picture"));

	if (isComplex())
		O3D_ERROR(E_InvalidFormat("Complex picture are not supported at this time"));

	if (!m_data.get() || !m_data->data || !m_width || !m_height)
		return False;

	// the destination buffer
	UInt8* data = new UInt8[m_width * m_height * 4];
	genNormalMap(data, scale, wrap);

	m_data = new ImageData(data);
	m_pixelFormat = PF_RGBA_U8;

	setDirty();
	return True;
}

// convert the picture from height-map (grey-scale) to a RGB normal map, and return
// the new picture.
Image* Image::retNormalMap(Float scale, Bool wrap)
{
	if (!m_state)
		O3D_ERROR(E_InvalidPrecondition("Picture must be valid"));

	if (isCompressed())
		O3D_ERROR(E_InvalidFormat("Works only with non compressed picture"));

	if (isComplex())
		O3D_ERROR(E_InvalidFormat("Complex picture are not supported at this time"));

	if (!m_data.get() || !m_data->data || !m_width || !m_height)
		return NULL;

	Image* nmap = new Image;

	if (!nmap->allocate(m_width, m_height, 4))
	{
		deletePtr(nmap);
		return NULL;
	}

	genNormalMap(nmap->m_data->data, scale, wrap);

	nmap->m_pixelFormat = PF_RGBA_U8;
	nmap->setDirty();

	return nmap;
}

// Convert to LuminanceAlpha
Bool Image::convertToRG8(Bool grayScale)
{
	if (m_pixelFormat == PF_RG_U8)
		return True;

	if (isColorU8())
		return convertX8toRG8(grayScale);

	if (isColorF32() || (m_pixelFormat == PF_DEPTH_F32))
		return convertXF32toRG8(grayScale);

	return False;
}

// Convert to luminance bitmap
Bool Image::convertToR8(Bool grayScale)
{
	if (m_pixelFormat == PF_RED_U8)
		return True;

	if (isColorU8())
		return convertX8toR8(grayScale);

	if (isColorF32() || (m_pixelFormat == PF_DEPTH_F32))
		return convertXF32toR8(grayScale);

	return False;
}

Bool Image::convertX8toR8(Bool grayScale)
{
	if (!m_state)
		O3D_ERROR(E_InvalidPrecondition("Picture must be valid"));

	if (m_pixelFormat == PF_RED_U8)
		return True;

	if (isCompressed())
		O3D_ERROR(E_InvalidFormat("Works only with non compressed picture"));

	if (isComplex())
		O3D_ERROR(E_InvalidFormat("Complex picture are not supported at this time"));

	if (!isColorU8())
		O3D_ERROR(E_InvalidFormat("Only color 8U format are supported"));

	if (!m_data.get() || !m_data->data || !m_width || !m_height)
		return False;

	UInt32 numComp = getNumComponents();

	UInt8 *exdata = m_data->data;

	m_size = m_width * m_height;
	UInt8 *data = new UInt8[m_size];

	UInt32 i,j = 0;

	if (numComp == 1)
	{
		// only luminance
		for ( i = 0 ; i < m_size ; ++i )
		{
			data[i] = exdata[j]; // Luminance as Luminance
			++j;
		}
	}
	// LA
	else if (numComp == 2)
	{
		for ( i = 0 ; i < m_size ; ++i )
		{
			data[i] = exdata[j]; // Luminance as Luminance
			j += 2;
		}
	}
	// RGB
	else if (numComp == 3)
	{
		if (grayScale)
		{
			for ( i = 0 ; i < m_size ; ++i )
			{
				data[i] = (exdata[j] + exdata[j+1] + exdata[j+2]) / 3; // Grayscale
				j += 3;
			}
		}
		else
		{
			for ( i = 0 ; i < m_size ; ++i )
			{
				data[i] = exdata[j]; // Red as Luminance
				j += 3;
			}
		}
	}
	// RGBA
	else if (numComp == 4)
	{
		if (grayScale)
		{
			for ( i = 0 ; i < m_size ; ++i )
			{
				data[i] = (exdata[j] + exdata[j+1] + exdata[j+2]) / 3; // Grayscale
				j += 4;
			}
		}
		else
		{
			for ( i = 0 ; i < m_size ; ++i )
			{
				data[i] = exdata[j]; // Red as Luminance
				j += 4;
			}
		}
	}

	m_data = new ImageData(data);
	m_pixelFormat = PF_RED_U8;

	setDirty();
	return True;
}

Bool Image::convertX8toRG8(Bool grayScale)
{
	if (!m_state)
		O3D_ERROR(E_InvalidPrecondition("Picture must be valid"));

	if (m_pixelFormat == PF_RG_U8)
		return True;

	UInt32 numComp = getNumComponents();

	if (isCompressed())
		O3D_ERROR(E_InvalidFormat("Works only with non compressed picture"));

	if (isComplex())
		O3D_ERROR(E_InvalidFormat("Complex picture are not supported at this time"));

	if (!isColorU8())
		O3D_ERROR(E_InvalidFormat("Only color 8U format are supported"));

	if (!m_data.get() || !m_data->data || !m_width || !m_height)
		return False;

	UInt8 *exdata = m_data->data;

	m_size = m_width * m_height * 2;
	UInt8 *data = new UInt8[m_size];

	UInt32 i,j = 0;

	if (numComp == 1)
	{
		// only luminance
		for ( i = 0 ; i < m_size ; i += 2 )
		{
			data[i] = exdata[j]; // Luminance as Luminance
			data[i+1] = 255;     // 255 as Alpha
			++j;
		}
	}
	// RGB
	else if (numComp == 3)
	{
		if (grayScale)
		{
			for ( i = 0 ; i < m_size ; i += 2 )
			{
				data[i] = (exdata[j] + exdata[j+1] + exdata[j+2]) / 3; // Grayscale as Luminance
				data[i+1] = 255;     // 255 as Alpha
				j += 3;
			}
		}
		else
		{
			for ( i = 0 ; i < m_size ; i += 2 )
			{
				data[i] = exdata[j]; // Red as Luminance
				data[i+1] = 255;     // 255 as Alpha
				j += 3;
			}
		}
	}
	// RGBA
	else if (numComp == 4)
	{
		if (grayScale)
		{
			for ( i = 0 ; i < m_size ; i += 2 )
			{
				data[i] = (exdata[j] + exdata[j+1] + exdata[j+2]) / 3; // Grayscale as Luminance
				data[i+1] = exdata[j+3];    // Alpha as Alpha
				j += 4;
			}
		}
		else
		{
			for ( i = 0 ; i < m_size ; i += 2 )
			{
				data[i] = exdata[j];        // Red as Luminance
				data[i+1] = exdata[j+3];    // Alpha as Alpha
				j += 4;
			}
		}
	}

	m_data = new ImageData(data);
	m_pixelFormat = PF_RG_U8;

	setDirty();
	return True;
}

Bool Image::convertToA8(Bool grayScale)
{
	if (m_pixelFormat == PF_RED_U8)
		return True;

	if (isColorU8())
		return convertX8toA8(grayScale);

	if (isColorF32() || (m_pixelFormat == PF_DEPTH_F32))
		return convertXF32toA8(grayScale);

	return False;
}

// Convert to Alpha bitmap
Bool Image::convertX8toA8(Bool grayScale)
{
	if (!m_state)
		O3D_ERROR(E_InvalidPrecondition("Picture must be valid"));

	if (m_pixelFormat == PF_RED_U8)
		return True;

	if (isCompressed())
		O3D_ERROR(E_InvalidFormat("Works only with non compressed picture"));

	if (isComplex())
		O3D_ERROR(E_InvalidFormat("Complex picture are not supported at this time"));
	
	if (!isColorU8())
		O3D_ERROR(E_InvalidFormat("Only color 8U format are supported"));

	if (!m_data.get() || !m_data->data || !m_width || !m_height)
		return False;

	UInt32 numComp = getNumComponents();
	UInt8 *exdata = m_data->data;

	m_size = m_width * m_height;
	UInt8 *data = new UInt8[m_size];

	UInt32 i,j = 0;

	if (numComp == 1)
	{
		// only luminance
		for ( i = 0 ; i < m_size ; ++i )
		{
			data[i] = exdata[j]; // Luminance as alpha
			++j;
		}
	}
	// LA
	else if (numComp == 2)
	{
		for ( i = 0 ; i < m_size ; ++i )
		{
			data[i] = exdata[j+1]; // Alpha as Alpha
			j += 2;
		}
	}
	// RGB
	else if (numComp == 3)
	{
		if (grayScale)
		{
			for ( i = 0 ; i < m_size ; ++i )
			{
				data[i] = (exdata[j] + exdata[j+1] + exdata[j+2]) / 3; // Grayscale as Alpha
				j += 3;
			}
		}
		else
		{
			for ( i = 0 ; i < m_size ; ++i )
			{
				data[i] = exdata[j]; // Red as Alpha
				j += 3;
			}
		}
	}
	// RGBA
	else if (numComp == 4)
	{
		for ( i = 0 ; i < m_size ; ++i )
		{
			data[i] = exdata[j+3]; // Alpha as Alpha
			j += 4;
		}
	}

	m_data = new ImageData(data);
	m_pixelFormat = PF_RED_U8;

	setDirty();
	return True;
}

Bool Image::convertXF32toA8(Bool grayScale)
{
	if (!m_state)
		O3D_ERROR(E_InvalidPrecondition("Picture must be valid"));

	if (m_pixelFormat == PF_RED_U8)
		return True;

	if (isCompressed())
		O3D_ERROR(E_InvalidFormat("Works only with non compressed picture"));

	if (isComplex())
		O3D_ERROR(E_InvalidFormat("Complex picture are not supported at this time"));

	if (!isColorF32() && (m_pixelFormat != PF_DEPTH_F32))
		O3D_ERROR(E_InvalidFormat("Only color F32 format are supported"));

	if (!m_data.get() || !m_data->data || !m_width || !m_height)
		return False;

	UInt32 numComp = getNumComponents();
	Float *exdata = (Float*)m_data->data;

	m_size = m_width * m_height;
	UInt8 *data = new UInt8[m_size];

	UInt32 i,j = 0;

	if (numComp == 1)
	{
		// only luminance
		for ( i = 0 ; i < m_size ; ++i )
		{
			data[i] = (UInt8)(exdata[j] * 255.f); // Luminance as alpha
			++j;
		}
	}
	// LA
	else if (numComp == 2)
	{
		for ( i = 0 ; i < m_size ; ++i )
		{
			data[i] = (UInt8)(exdata[j+1] * 255.f); // Alpha as Alpha
			j += 2;
		}
	}
	// RGB
	else if (numComp == 3)
	{
		if (grayScale)
		{
			for ( i = 0 ; i < m_size ; ++i )
			{
				data[i] = (UInt8)((exdata[j] + exdata[j+1] + exdata[j+2]) / 3 * 255.f); // Grayscale as Alpha
				j += 3;
			}
		}
		else
		{
			for ( i = 0 ; i < m_size ; ++i )
			{
				data[i] = (UInt8)(exdata[j] * 255.f); // Red as Alpha
				j += 3;
			}
		}
	}
	// RGBA
	else if (numComp == 4)
	{
		for ( i = 0 ; i < m_size ; ++i )
		{
			data[i] = (UInt8)(exdata[j+3] * 255.f); // Alpha as Alpha
			j += 4;
		}
	}

	m_data = new ImageData(data);
	m_pixelFormat = PF_RED_U8;

	setDirty();
	return True;
}

Bool Image::convertXF32toR8(Bool grayScale)
{
	if (!m_state)
		O3D_ERROR(E_InvalidPrecondition("Picture must be valid"));

	if (m_pixelFormat == PF_RED_U8)
		return True;

	if (isCompressed())
		O3D_ERROR(E_InvalidFormat("Works only with non compressed picture"));

	if (isComplex())
		O3D_ERROR(E_InvalidFormat("Complex picture are not supported at this time"));

	if (!isColorF32() && (m_pixelFormat != PF_DEPTH_F32))
		O3D_ERROR(E_InvalidFormat("Only color 32F format are supported"));

	if (!m_data.get() || !m_data->data || !m_width || !m_height)
		return False;

	UInt32 numComp = getNumComponents();

	Float *exdata = (Float*)m_data->data;

	m_size = m_width * m_height;
	UInt8 *data = new UInt8[m_size];

	UInt32 i,j = 0;

	if (numComp == 1)
	{
		// only luminance
		for ( i = 0 ; i < m_size ; ++i )
		{
			data[i] = (UInt8)(exdata[j] * 255.f); // Luminance as Luminance
			++j;
		}
	}
	// LA
	else if (numComp == 2)
	{
		for ( i = 0 ; i < m_size ; ++i )
		{
			data[i] = (UInt8)(exdata[j] * 255.f); // Luminance as Luminance
			j += 2;
		}
	}
	// RGB
	else if (numComp == 3)
	{
		if (grayScale)
		{
			for ( i = 0 ; i < m_size ; ++i )
			{
				data[i] = (UInt8)((exdata[j] + exdata[j+1] + exdata[j+2]) / 3 * 255.f); // Grayscale
				j += 3;
			}
		}
		else
		{
			for ( i = 0 ; i < m_size ; ++i )
			{
				data[i] = (UInt8)(exdata[j] * 255.f); // Red as Luminance
				j += 3;
			}
		}
	}
	// RGBA
	else if (numComp == 4)
	{
		if (grayScale)
		{
			for ( i = 0 ; i < m_size ; ++i )
			{
				data[i] = (UInt8)((exdata[j] + exdata[j+1] + exdata[j+2]) / 3 * 255.f); // Grayscale
				j += 4;
			}
		}
		else
		{
			for ( i = 0 ; i < m_size ; ++i )
			{
				data[i] = (UInt8)(exdata[j] * 255.f); // Red as Luminance
				j += 4;
			}
		}
	}

	m_data = new ImageData(data);
	m_pixelFormat = PF_RED_U8;

	setDirty();
	return True;
}

Bool Image::convertXF32toRG8(Bool grayScale)
{
	if (!m_state)
		O3D_ERROR(E_InvalidPrecondition("Picture must be valid"));

	if (m_pixelFormat == PF_RG_U8)
		return True;

	UInt32 numComp = getNumComponents();

	if (isCompressed())
		O3D_ERROR(E_InvalidFormat("Works only with non compressed picture"));

	if (isComplex())
		O3D_ERROR(E_InvalidFormat("Complex picture are not supported at this time"));

	if (!isColorF32() && (m_pixelFormat != PF_DEPTH_F32))
		O3D_ERROR(E_InvalidFormat("Only color 32F format are supported"));

	if (!m_data.get() || !m_data->data || !m_width || !m_height)
		return False;

	Float *exdata = (Float*)m_data->data;

	m_size = m_width * m_height * 2;
	UInt8 *data = new UInt8[m_size];

	UInt32 i,j = 0;

	if (numComp == 1)
	{
		// only luminance
		for ( i = 0 ; i < m_size ; i += 2 )
		{
			data[i] = (UInt8)(exdata[j] * 255.f); // Luminance as Luminance
			data[i+1] = 255;     // 255 as Alpha
			++j;
		}
	}
	// RGB
	else if (numComp == 3)
	{
		if (grayScale)
		{
			for ( i = 0 ; i < m_size ; i += 2 )
			{
				data[i] = (UInt8)((exdata[j] + exdata[j+1] + exdata[j+2]) / 3 * 255.f); // Grayscale as Luminance
				data[i+1] = 255;     // 255 as Alpha
				j += 3;
			}
		}
		else
		{
			for ( i = 0 ; i < m_size ; i += 2 )
			{
				data[i] = (UInt8)(exdata[j] * 255.f); // Red as Luminance
				data[i+1] = 255;     // 255 as Alpha
				j += 3;
			}
		}
	}
	// RGBA
	else if (numComp == 4)
	{
		if (grayScale)
		{
			for ( i = 0 ; i < m_size ; i += 2 )
			{
				data[i] = (UInt8)((exdata[j] + exdata[j+1] + exdata[j+2]) / 3 * 255.f); // Grayscale as Luminance
				data[i+1] = (UInt8)(exdata[j+3] * 255.f);    // Alpha as Alpha
				j += 4;
			}
		}
		else
		{
			for ( i = 0 ; i < m_size ; i += 2 )
			{
				data[i] = (UInt8)(exdata[j] * 255.f);        // Red as Luminance
				data[i+1] = (UInt8)(exdata[j+3] * 255.f);    // Alpha as Alpha
				j += 4;
			}
		}
	}

	m_data = new ImageData(data);
	m_pixelFormat = PF_RG_U8;

	setDirty();
	return True;
}

// Resize the size of the picture given a filtering mode.
Bool Image::resize(UInt32 _width, UInt32 _height, ResizeFilter _filter)
{
	if (!m_state)
		O3D_ERROR(E_InvalidPrecondition("Picture must be valid"));

	if (isCompressed())
		O3D_ERROR(E_InvalidFormat("Works only with non compressed picture"));

	if (isComplex())
		O3D_ERROR(E_InvalidFormat("Complex picture are not supported at this time"));

	if (!isColorU8())
		O3D_ERROR(E_InvalidFormat("Only color 8bpp format are supported"));

	// empty picture
	if ((_width == 0) || (_height == 0))
		return False;

	// nothing to to
	if ((_width == m_width) && (_height == m_height))
		return True;

	if (!m_data.get() || !m_data->data || !m_width || !m_height)
		return False;

	switch(_filter)
	{
		case FILTER_NEAREST:
			{
				const UInt32 oldWidth = m_width;
				const UInt32 oldHeight = m_height;

				UInt8 *pOldData = m_data->data;
				UInt32 numComp = getNumComponents();

				UInt8 *data = new UInt8[_width * _height * numComp];
				m_width = _width;
				m_height = _height;

				Float fx = Float(oldWidth) / Float(m_width);
				Float fy = Float(oldHeight) / Float(m_height);

				Float u = 0.0f, v = 0.0f;

				UInt8 * ptr = data;

				for (UInt32 y = m_height ; y > 0 ; --y, v += fy)
				{
					UInt8 * pOldPtr = &pOldData[UInt16(v) * oldWidth * numComp];

					u = 0.0f;

					for (UInt32 x = m_width ; x > 0 ; --x, u += fx, ptr += numComp)
						memcpy((void*)ptr, (const void*)&pOldPtr[numComp*UInt32(u)], numComp * sizeof(UInt8));
				}

				m_data = new ImageData(data);
			}

			setDirty();
			return True;

		default:
			return False;
	};
}

