/**
 * @file bmp.cpp
 * @brief Ecriture des images bitmap (8,24bpp) avec ou sans palette.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-10
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/image/precompiled.h"
#include "o3d/image/bmp.h"

#include "o3d/image/image.h"
#include "o3d/core/filemanager.h"

using namespace o3d;

/*---------------------------------------------------------------------------------------
   check the first bytes of the file for see his format
---------------------------------------------------------------------------------------*/
Bool Bmp::checkFormat(InStream &is)
{
	Char id[2];
    is.read(id, 2);
    is.seek(-2);

	if ((id[0] == 'B') && (id[1] == 'M'))
		return True;

	return False;
}

PixelFormat Bmp::getPixelFormat() const
{
	switch (m_bpp)
	{
		case 3:
			return PF_RGB_U8;
		case 4:
            return PF_RGBA_U8;
		default:
			O3D_ERROR(E_InvalidFormat("Unsupported pixel format"));
			return PF_RGBA_U8;
	}
}

Bool Bmp::load(InStream &is)
{
    return loadDefault(is);
}

Bool Bmp::load(InStream &is, PixelFormat pixelFormat)
{
	switch (pixelFormat)
	{
		case PF_RGB_U8:
            return loadRgb24(is);
			break;
	
		case PF_RGBA_U8:
            return loadRgba32(is);
	
		default:
			O3D_ERROR(E_InvalidParameter("Unsupported pixel format conversion"));
	}
}

/*---------------------------------------------------------------------------------------
  Permet de charger un fichier BMP (bitmap) en conservant son format d'origine
  8 ou 24 bpp, ne lis pas le 16bpp. Renvoi True si succé
---------------------------------------------------------------------------------------*/
Bool Bmp::loadDefault(InStream &is)
{
	// check the file format
    if (!checkFormat(is))
        O3D_ERROR(E_InvalidFormat("Invalid BMP token"));

	UInt8 *_data = NULL;
	UInt8 *palette = NULL;
	UInt16 data_ofs;
	UInt32 ofs1 = 0,ofs2 = 0;
	UInt32 palcol, index;

	// lecture des information sur les données de l'images
	// informations inutiles jusqu'à l'octet 14
    is.seek(14);

    is >> data_ofs;

    is.seek(2);

    is >> m_width   // picture size
       >> m_height;

	// informations inutiles jusqu'à l'octet 28
    is.seek(2);

	// nombre de couleur de l'image
	UInt16 tmp_bpp;
    is >> tmp_bpp;
	m_bpp = tmp_bpp;

    is.seek(16);

    is >> palcol;
	if (palcol > 256) palcol = 0;
	if ((palcol == 0) && (m_bpp == 8)) palcol = 256;

	data_ofs += 14;
    is.seek(data_ofs-50);     // position devant la palette si 8bbp ou les données sinon

	if ((m_bpp == 1) || (m_bpp == 4) || (m_bpp == 8))
	{
		if (palcol == 0)
		{
			if (m_bpp == 8) palcol = 256;
			else if (m_bpp == 4) palcol = 16;
			else if (m_bpp == 1) palcol = 2;
		}

		palette = new UInt8[palcol*4];	// alloue la palette dans la classe

        is.read(palette, palcol*4);
	}

	// compute the offset
	UInt32 ofs = 0;
    // data size to load in byte
    UInt32 load = (m_height * m_width * m_bpp) >> 3;
    // len of a line in bytes
    UInt32 len = load / m_height;

    // compute the offset per line
	if (m_bpp == 24)
		ofs = len - (m_width * 3);
	else if (m_bpp == 8)
		ofs = len - m_width;
	else if (m_bpp == 4)
		ofs = len - (m_width >> 1);
	else if (m_bpp == 1)
		ofs = len - (m_width >> 3);
    else
        O3D_ERROR(E_InvalidFormat("Invalid pixel format"));

	if ((m_bpp == 8) || (m_bpp == 4) || (m_bpp == 1))
	{
		_data = new UInt8[load];
		m_data = new UInt8[m_width*m_height*3];
	}
	else
	{
		_data = new UInt8[load];
		m_data = new UInt8[m_width*m_height*(m_bpp>>3)];
	}

	if (m_bpp == 1)
	{
        is.read(_data, load);

		Int32 lx,ly = m_height-1;			// on inverse les ligne de haut en bas
		Int32 ly2 = 0,lx2;
		Int32 x = m_width * 3;
		Int32 x2 = (m_width >> 3) + ofs;
		Int32 bit;

		UInt8 select[] = { 1,2,4,8,16,32,64,128 };

		while (ly>=0)					// pour chaque ligne de sizeY..0
		{
			lx = 0;
			// on lit chaque index de la ligne
			for ( lx2 = 0 ; lx2 < (Int32)(m_width>>3) ; ++lx2 )
			{
				for ( bit = 7 ; bit >= 0 ; bit-- )
				{
					index = (_data[lx2+ly2*x2]&select[bit])>>bit;

					m_data[lx+ly*x+2] = palette[index<<2];
					m_data[lx+ly*x+1] = palette[(index<<2)+1];
					m_data[lx+ly*x]   = palette[(index<<2)+2];

					lx +=3;
				}
			}
			--ly;						// et on décrement d'une ligne
			++ly2;
		}

		deleteArray(palette);
	}
	else if (m_bpp == 4)
	{
        is.read(_data, load);

		Int32 lx,ly = m_height-1;			// on inverse les ligne de haut en bas
		Int32 ly2 = 0;
		Int32 x = m_width * 3;
		Int32 x2 = (m_width >> 1) + ofs;

		while (ly>=0)					// pour chaque ligne de sizeY..0
		{
			ofs1 = ly * x;
			ofs2 = ly2 * x2;
			// on lit chaque index de la ligne
			for ( lx = 0 ; lx < (Int32)m_width-1 ; lx += 2 )
			{
				index = (_data[ofs2]&0xf0)>>2;

				m_data[ofs1+2] = palette[index];
				m_data[ofs1+1] = palette[index+1];
				m_data[ofs1]   = palette[index+2];

				ofs1 += 3;
				index = (_data[ofs2]&0x0f)<<2;

				m_data[ofs1+2] = palette[index];
				m_data[ofs1+1] = palette[index+1];
				m_data[ofs1]   = palette[index+2];

				ofs1 += 3;
				++ofs2;
			}
			--ly;						// et on décrement d'une ligne
			++ly2;
		}

		deleteArray(palette);
	}
	else if (m_bpp == 8)	// si 8bpp color, alors on a une palette
	{
        is.read(_data, load);

		Int32 lx,ly = m_height-1;			// on inverse les ligne de haut en bas
		Int32 ly2 = 0;
		Int32 x = m_width * 3;
		Int32 x2 = m_width + ofs;

		while (ly>=0)					// pour chaque ligne de sizeY..0
		{
			ofs1 = ly * x;
			ofs2 = ly2 * x2;
			// on lit chaque index de la ligne
			for ( lx = 0 ; lx < (Int32)m_width ; ++lx )
			{
				index = _data[ofs2]<<2;

				m_data[ofs1+2] = palette[index];
				m_data[ofs1+1] = palette[index+1];
				m_data[ofs1]   = palette[index+2];

				ofs1 += 3;
				++ofs2;
			}

			--ly;						// et on décrement d'une ligne
			++ly2;
		}

		deleteArray(palette);
	}
	else if (m_bpp == 24)	// si 24bpp color
	{
		Int32 ly = m_height-1;				// on inverse les ligne de haut en bas
		Int32 ly2 = 0;
		Int32 x = m_width*3;				// taille d'une ligne en octet
		Int32 x2 = m_width*3 + ofs;

        is.read(_data, load);

		while (ly>=0)					// pour chaque ligne de sizeY..0
		{
			ofs1 = ly * x;
			ofs2 = ly2 * x2;

			for ( Int32 lx = 0 ; lx < (Int32)m_width ; ++lx )
			{
				m_data[ofs1+2] = _data[ofs2++];
				m_data[ofs1+1] = _data[ofs2++];
				m_data[ofs1]   = _data[ofs2++];

				ofs1 += 3;
			}

			--ly;						// et on décrement d'une ligne
			++ly2;
		}
	}
	else						// format de couleur non reconnue !
	{
		deleteArray(m_data);
		deleteArray(_data);

        O3D_ERROR(E_InvalidFormat("Invalid BMP format"));
	}

	deleteArray(_data);

	if ((m_bpp == 4) || (m_bpp == 8))
		m_bpp = 3;	// 3
	else if (m_bpp == 24)
		m_bpp = 3;	// 3
	else if (m_bpp == 32)
		m_bpp = 4;  // 4

	m_size = m_width * m_height * m_bpp;
	m_state = True;

	return True;
}

/*---------------------------------------------------------------------------------------
  Permet de charger un fichier BMP (bitmap) en forcant son format en 24 bpp,
  ne lis pas le 16bpp. Renvoi True si succé.
---------------------------------------------------------------------------------------*/
Bool Bmp::loadRgb24(InStream &is)
{
	// check the file format
    if (!checkFormat(is))
        O3D_ERROR(E_InvalidFormat("Invalid BMP token"));

	UInt8 *palette = NULL;
	UInt8 *_data;
	UInt16 data_ofs;
	UInt32 _palcol = 256;
	UInt32 index;
	UInt32 ofs1 = 0,ofs2 = 0;

	// lecture des information sur les données de l'images
    is.seek(14); // informations inutiles jusqu'à l'octet 14

    is >> data_ofs;

    is.seek(2);

    is >> m_width   // picture size
       >> m_height;

    is.seek(2);  // informations inutiles jusqu'à l'octet 28

	// nombre de couleur de l'image
	UInt16 tmp_bpp;
    is >> tmp_bpp;
	m_bpp = tmp_bpp;

    is.seek(16);

    is >> _palcol;
	if (_palcol > 256) _palcol = 0;
	if ((_palcol == 0) && (m_bpp == 8)) _palcol = 256;

	data_ofs += 14;
    is.seek(data_ofs-50);     // position devant la palette si 8bbp ou les données sinon

	if ((m_bpp == 1) || (m_bpp == 4) || (m_bpp == 8))
	{
		if (_palcol == 0)
		{
			if (m_bpp == 8) _palcol = 256;
			else if (m_bpp == 4) _palcol = 16;
			else if (m_bpp == 1) _palcol = 2;
		}

		palette = new UInt8[_palcol*4];

        is.read(palette, _palcol*4);
	}

    // compute the offset
    UInt32 ofs = 0;
    // data size to load in byte
    UInt32 load = (m_height * m_width * m_bpp) >> 3;
    // len of a line in bytes
    UInt32 len = load / m_height;

    // compute the offset per line
    if (m_bpp == 24)
        ofs = len - (m_width * 3);
    else if (m_bpp == 8)
        ofs = len - m_width;
    else if (m_bpp == 4)
        ofs = len - (m_width >> 1);
    else if (m_bpp == 1)
        ofs = len - (m_width >> 3);
    else
        O3D_ERROR(E_InvalidFormat("Invalid pixel format"));

	_data = new UInt8[load];
	m_data = new UInt8[m_width*m_height*3];

	if (m_bpp == 1)
	{
        is.read(_data, load);

		Int32 lx,ly = m_height-1;			// on inverse les ligne de haut en bas
		Int32 ly2 = 0,lx2;
		Int32 x = m_width * 3;
		Int32 x2 = (m_width >> 3) + ofs;
		Int32 bit;

		UInt8 select[] = { 1,2,4,8,16,32,64,128 };


		while (ly>=0)					// pour chaque ligne de sizeY..0
		{
			lx = 0;
			// on lit chaque index de la ligne
			for ( lx2 = 0 ; lx2 < (Int32)(m_width>>3) ; ++lx2 )
			{
				for ( bit = 7 ; bit >= 0 ; bit-- )
				{
					index = (_data[lx2+ly2*x2]&select[bit])>>bit;

					m_data[lx+ly*x+2] = palette[index<<2];
					m_data[lx+ly*x+1] = palette[(index<<2)+1];
					m_data[lx+ly*x]   = palette[(index<<2)+2];

					lx +=3;
				}
			}
			--ly;						// et on décrement d'une ligne
			++ly2;
		}

		deleteArray(palette);
	}
	else if (m_bpp == 4)
	{
        is.read(_data, load);

		Int32 lx,ly = m_height-1;			// on inverse les ligne de haut en bas
		Int32 ly2 = 0;
		Int32 x = m_width * 3;
		Int32 x2 = (m_width >> 1) + ofs;


		while (ly>=0)					// pour chaque ligne de sizeY..0
		{
			ofs1 = ly * x;
			ofs2 = ly2 * x2;
			// on lit chaque index de la ligne
			for ( lx = 0 ; lx < (Int32)m_width-1 ; lx += 2 )
			{
				index = (_data[ofs2]&0xf0)>>2;

				m_data[ofs1+2] = palette[index];
				m_data[ofs1+1] = palette[index+1];
				m_data[ofs1]   = palette[index+2];

				ofs1 += 3;
				index = (_data[ofs2]&0x0f)<<2;

				m_data[ofs1+2] = palette[index];
				m_data[ofs1+1] = palette[index+1];
				m_data[ofs1]   = palette[index+2];

				ofs1 += 3;
				++ofs2;
			}
			--ly;						// et on décrement d'une ligne
			++ly2;
		}

		deleteArray(palette);
	}
	else if (m_bpp == 8)	// si 8bpp color, alors on a une palette
	{
        is.read(_data, load);

		Int32 lx,ly = m_height-1;			// on inverse les ligne de haut en bas
		Int32 ly2 = 0;
		Int32 x = m_width * 3;
		Int32 x2 = m_width + ofs;

		while (ly>=0)					// pour chaque ligne de sizeY..0
		{
			ofs1 = ly * x;
			ofs2 = ly2 * x2;
			// on lit chaque index de la ligne
			for ( lx = 0 ; lx < (Int32)m_width ; ++lx )
			{
				index = _data[ofs2]<<2;

				m_data[ofs1+2] = palette[index];
				m_data[ofs1+1] = palette[index+1];
				m_data[ofs1]   = palette[index+2];

				ofs1 += 3;
				++ofs2;
			}

			--ly;						// et on décrement d'une ligne
			++ly2;
		}

		deleteArray(palette);
	}
	else if (m_bpp == 24)	// si 24bpp color
	{
        is.read(_data, load);

		Int32 ly = m_height-1;				// on inverse les ligne de haut en bas
		Int32 ly2 = 0;
		Int32 x = m_width*3;				// taille d'une ligne en octet
		Int32 x2 = (m_width*3) + ofs;

		while (ly>=0)					// pour chaque ligne de sizeY..0
		{
			ofs1 = ly * x;
			ofs2 = ly2 * x2;

			for ( Int32 lx = 0 ; lx < (Int32)m_width ; ++lx )
			{
				m_data[ofs1+2] = _data[ofs2++];
				m_data[ofs1+1] = _data[ofs2++];
				m_data[ofs1]   = _data[ofs2++];

				ofs1 += 3;
			}

			--ly;						// et on décrement d'une ligne
			++ly2;
		}
	}
	else						// format de couleur non reconnue !
	{
		deleteArray(m_data);
		deleteArray(_data);

        O3D_ERROR(E_InvalidFormat("Invalid BMP format"));
	}

	deleteArray(_data);
	m_bpp = 3;	// 3
	m_size = m_width * m_height * m_bpp;
	m_state = True;

	return True;
}

/*---------------------------------------------------------------------------------------
  Permet de charger un fichier BMP (bitmap) en forcant son format en 32 bpp,
  ne lis pas le 16bpp. Renvoi True si succé.
---------------------------------------------------------------------------------------*/
Bool Bmp::loadRgba32(InStream &is)
{
	// check the file format
    if (!checkFormat(is))
        O3D_ERROR(E_InvalidFormat("Invalid BMP token"));

	UInt8 *palette = NULL;
	UInt8 *_data;

	UInt16 data_ofs;
	UInt32 palcol = 256;
	UInt32 ofs1 = 0,ofs2 = 0;
	UInt32 index;

	// lecture des information sur les données de l'images
    is.seek(14);   // informations inutiles jusqu'à l'octet 14

    is >> data_ofs;

    is.seek(2);

    is >> m_width    // picture size
       >> m_height;

    is.seek(2);            // informations inutiles jusqu'à l'octet 28

	// nombre de couleur de l'image
	UInt16 tmp_bpp;
    is >> tmp_bpp;
	m_bpp = tmp_bpp;

    is.seek(16);

    is >> palcol;
	if (palcol > 256) palcol = 0;
	if ((palcol == 0) && (m_bpp == 8)) palcol = 256;

	data_ofs += 14;
    is.seek(data_ofs-50);     // position devant la palette si 8bbp ou les données sinon

	if ((m_bpp == 1) || (m_bpp == 4) || (m_bpp == 8))
	{
		if (palcol == 0)
		{
			if (m_bpp == 8) palcol = 256;
			else if (m_bpp == 4) palcol = 16;
			else if (m_bpp == 1) palcol = 2;
		}

		palette = new UInt8[palcol*4];	// alloue la palette dans la classe
        is.read(palette, palcol*4);
	}

    // compute the offset
    UInt32 ofs = 0;
    // data size to load in byte
    UInt32 load = (m_height * m_width * m_bpp) >> 3;
    // len of a line in bytes
    UInt32 len = load / m_height;

    // compute the offset per line
    if (m_bpp == 24)
        ofs = len - (m_width * 3);
    else if (m_bpp == 8)
        ofs = len - m_width;
    else if (m_bpp == 4)
        ofs = len - (m_width >> 1);
    else if (m_bpp == 1)
        ofs = len - (m_width >> 3);
    else
        O3D_ERROR(E_InvalidFormat("Invalid pixel format"));

	_data = new UInt8[load];
	m_data = new UInt8[m_width*m_height*4];

	if (m_bpp == 1)
	{
        is.read(_data, load);

		Int32 lx,ly = m_height-1;			// on inverse les ligne de haut en bas
		Int32 ly2 = 0,lx2;
		Int32 x = m_width * 4;
		Int32 x2 = (m_width >> 3) + ofs;
		Int32 bit;

		UInt8 select[] = { 1,2,4,8,16,32,64,128 };


		while (ly>=0)					// pour chaque ligne de sizeY..0
		{
			lx = 0;
			// on lit chaque index de la ligne
			for ( lx2 = 0 ; lx2 < (Int32)(m_width>>3) ; ++lx2 )
			{
				for ( bit = 7 ; bit >= 0 ; bit-- )
				{
					index = (_data[lx2+ly2*x2]&select[bit])>>bit;

					m_data[lx+ly*x+2] = palette[index<<2];
					m_data[lx+ly*x+1] = palette[(index<<2)+1];
					m_data[lx+ly*x]   = palette[(index<<2)+2];
					m_data[lx+ly*x+3] = palette[(index<<2)+3];

					lx +=4;
				}
			}
			--ly;						// et on décrement d'une ligne
			++ly2;
		}

		deleteArray(palette);
	}
	else if (m_bpp == 4)
	{
        is.read(_data, load);

		Int32 lx,ly = m_height-1;			// on inverse les ligne de haut en bas
		Int32 ly2 = 0;
		Int32 x = m_width * 4;
		Int32 x2 = (m_width >> 1) + ofs;

		while (ly>=0)					// pour chaque ligne de sizeY..0
		{
			ofs1 = ly * x;
			ofs2 = ly2 * x2;
			// on lit chaque index de la ligne
			for ( lx = 0 ; lx < (Int32)m_width-1 ; lx += 2 )
			{
				index = (_data[ofs2]&0xf0)>>2;

				m_data[ofs1+2]  = palette[index];
				m_data[ofs1+1] = palette[index+1];
				m_data[ofs1]   = palette[index+2];
				m_data[ofs1+3] = palette[index+3];

				ofs1 += 4;
				index = (_data[ofs2]&0x0f)<<2;

				m_data[ofs1+2] = palette[index];
				m_data[ofs1+1] = palette[index+1];
				m_data[ofs1]   = palette[index+2];
				m_data[ofs1+3] = palette[index+3];

				ofs1 += 4;
				++ofs2;
			}

			--ly;						// et on décremente d'une ligne
			++ly2;
		}

		deleteArray(palette);
	}
	else if (m_bpp == 8)	// si 8bpp color, alors on a une palette
	{
        is.read(_data, load);

		Int32 lx,ly = m_height-1;			// on inverse les ligne de haut en bas
		Int32 ly2 = 0;
		Int32 x = m_width << 2;
		Int32 x2 = m_width + ofs;

		while (ly>=0)					// pour chaque ligne de sizeY..0
		{
			ofs1 = ly * x;
			ofs2 = ly2 * x2;
			// on lit chaque index de la ligne
			for ( lx = 0 ; lx < (Int32)m_width ; ++lx )
			{
				index = _data[ofs2]<<2;

				m_data[ofs1+2] = palette[index];
				m_data[ofs1+1] = palette[index+1];
				m_data[ofs1]   = palette[index+2];
				m_data[ofs1+3] = palette[index+3];

				ofs1 += 4;
				++ofs2;
			}

			--ly;						// et on décrement d'une ligne
			++ly2;
		}

		deleteArray(palette);
	}
	else if (m_bpp == 24)	// si 24bpp color
	{
		Int32 lx,ly = m_height-1;			// on inverse les ligne de haut en bas
		Int32 ly2 = 0;
		Int32 x = m_width*4;				// taille d'une ligne en octet
		Int32 x2 = m_width*3+ofs;

        is.read(_data, load);

		while (ly>=0)					// pour chaque ligne de sizeY..0
		{
			ofs1 = ly * x;
			ofs2 = ly2 * x2;
			// on lit chaque (RGB) de la ligne
			for ( lx = 0 ; lx < (Int32)m_width ; ++lx )
			{
				m_data[ofs1+2] = _data[ofs2++];
				m_data[ofs1+1] = _data[ofs2++];
				m_data[ofs1]   = _data[ofs2++];
				m_data[ofs1+3] = 255;

				ofs1 += 4;
			}
			--ly;						// et on décrement d'une ligne
			++ly2;
		}
	}
	else						// format de couleur non reconnue !
	{
		deleteArray(m_data);
		deleteArray(_data);

        O3D_ERROR(E_InvalidFormat("Invalid BMP format"));
	}

    deleteArray(_data);
	m_bpp = 4;	// 4 octets
	m_size = m_width * m_height * m_bpp;
	m_state = True;

	return True;
}

// Write a BMP header.
static void writeBmpHeader(
		UInt16 x,
		UInt16 y,
		UInt16 bpp,
		UInt16 palcol,
        OutStream &os)
{
    os << 'B' << 'M';								// ID "BM"
    os << (UInt32)(54+palcol*4+x*y*bpp);		// taille du fichier en octet
    os << (UInt32)0;							// reserved
    os << (UInt32)54+palcol*4;				// position des donnée
    os << (UInt32)40;							// taille des donnée des infos en octet
    os << (UInt32)x << (UInt32)y;			// taille de l'image largeur hauteur
    os << (UInt16)1;							// nombre de plan (tjs 1)
    os << (UInt16)(bpp*8);					// nombre de bpp
    os << (UInt32)0;							// compression (tjs 0 == sans)
    os << (UInt32)(x*y*bpp);					// taille de l'image en octet
    os << (UInt32)92 << (UInt32)92;		// pixel par mètre
    os << (UInt32)palcol;						// nombre de couleur dans la palette
    os << (UInt32)0;							// nombre de couleurs importantes
}

Bool Bmp::save(OutStream& os, PixelFormat pixelFormat)
{
	O3D_ERROR(E_InvalidOperation("Not yet implemented"));
    return False;
}

// Write in BMP 24 bpp RGB.
Bool Image::saveRgbBmp(const String &filename)
{
	if (!m_state)
		O3D_ERROR(E_InvalidPrecondition("Data must be non null " + filename));

	if (isCompressed())
		O3D_ERROR(E_InvalidFormat("Works only with non compressed picture"));

	if (isComplex())
		O3D_ERROR(E_InvalidFormat("Complex picture are not supported at this time"));

	if (!m_data.get() || !m_data->data || !m_width || !m_height)
		return False;

	Image tmpPicture(*this);

	if (tmpPicture.isRgb())
		tmpPicture.swapRB();

	if (tmpPicture.getPixelFormat() != PF_RGB_U8)
		tmpPicture.convertToRGB8();

    FileOutStream *os = FileManager::instance()->openOutStream(filename, FileOutStream::CREATE);

    writeBmpHeader(UInt16(m_width),UInt16(m_height),3,0,*os);

    UInt32 ofs = (m_width)%4;
    UInt8 *zero = new UInt8[ofs];

	UInt16 i = UInt16(m_height);
	while (i > 0)
	{
		i--;
        os->write(&tmpPicture.m_data->data[i*m_width*3], m_width*3);

        if (ofs > 0)
            os->write(zero, ofs);
	}

    deleteArray(zero);
    deletePtr(os);
	return True;
}

