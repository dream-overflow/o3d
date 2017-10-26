/**
 * @file tga.cpp
 * @brief Load of TGA picture (8,24,32 bpp) support of palette and RLE compression.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @author  Romain LOCCI
 * @date 2001-12-23
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/image/precompiled.h"
#include "o3d/image/tga.h"

#include "o3d/core/instream.h"
#include "o3d/core/outstream.h"

using namespace o3d;

//---------------------------------------------------------------------------------------
// check the first bytes of the file for see his format
//---------------------------------------------------------------------------------------
Bool Tga::checkFormat(InStream &is)
{
	Char id[3];
    is.read(id, 3);
    is.seek(-3);

	 if ((id[2] == 1) || (id[2] == 2) || (id[2] == 3) || (id[2] == 9) || (id[2] == 10) || (id[2] == 11))
		 return True;

	return False;
}

PixelFormat Tga::getPixelFormat() const
{
	switch (m_bpp)
	{
		case 1:
			return PF_RED_U8;
		case 3:
			return PF_RGB_U8;
		case 4:
            return PF_RGBA_U8;
		default:
			O3D_ERROR(E_InvalidFormat("Unsupported pixel format"));
			return PF_RGBA_U8;
	}
}

Bool Tga::load(InStream &is)
{
    return loadDefault(is);
}

Bool Tga::load(InStream &is, PixelFormat pixelFormat)
{
	switch (pixelFormat)
	{
	case PF_RGB_U8:
        return loadRgb24(is);
	case PF_RGBA_U8:
        return loadRgba32(is);
	default:
		O3D_ERROR(E_InvalidParameter("Unsupported convert format"));
		return False;
	}
}

Bool Tga::save(OutStream &os, PixelFormat pixelFormat)
{
	O3D_ERROR(E_InvalidOperation("Not yet implemented"));
	return False;
}

//---------------------------------------------------------------------------------------
// Permet de charger un fichier Tga en conservant son format d'origine 8 ou 24 bpp,
// ne lis pas le 16bpp. Renvoi True si succé.
//---------------------------------------------------------------------------------------
Bool Tga::loadDefault(InStream &is)
{
	// check the file format
    if (!checkFormat(is))
        O3D_ERROR(E_InvalidFormat("Invalid TGA token"));

	UInt8 *_data = NULL;
	UInt8 *palette = NULL;

	UInt32 palcol = 0;
	UInt32 palbpp = 0;

	UInt32 ofs = 0;
	UInt8 buf;
	Bool top,left;
	UInt8 idSize;

    m_seekPos = is.getPosition();

    is >> idSize;                 // taille de l'id (0 si pas)
	idSize += 18;

	//lecture de la taille en X et Y et du nombre de bits par pixels
    is.reset(m_seekPos+12);

	// picture size
	UInt16 u_short;
    is >> u_short; m_width = u_short;  //largeur de l'image
    is >> u_short; m_height = u_short;  //hauteur de l'image

    is >> buf; m_bpp = buf;      //bits par pixels
    is >> buf; buf &= 0x30;    //ordre d'apparition des pixels

	top = left = False;

	if ((buf == 0x20) || (buf == 0x30))
		top = True;

	if ((buf == 0x00) || (buf == 0x20))
		left = True;

	if(m_bpp == 16)		//mode de couleur non supporté
        O3D_ERROR(E_InvalidFormat("Unsuported TGA color format"));

    is.reset(m_seekPos+2);
    is >> buf; //lecture du type de compression

    is.reset(m_seekPos+idSize);

	m_data = new UInt8[m_width*m_height*(m_bpp>>3)];

	//si le fichier contient une palette
	if((buf == 1) || (buf == 9))
	{
		UInt8 c;

        is.reset(m_seekPos+5L);    //se place devant les informations de la palette
        is >> u_short; palcol = u_short;  //lit le nombre de couleurs dans la palette
        is >> c; palbpp = c >> 3;         //lit le nombre de composantes pour chaque couleur

		palette = new UInt8[palcol*palbpp];

        is.reset(m_seekPos+idSize);
        is.read(palette, palcol*palbpp);	//lecture de la palette
	}

	//alloue et charge le buffer index
    _data = new UInt8[is.getAvailable()];

    UInt32 load = is.getAvailable();
    is.read(_data, load);

	//image non compressé avec palette
	if(buf == 1)
	{
		Int32 x,y;
		UInt32 m,n;

		for(y=((top == True) ? 0 : (m_height-1));((top == True) ? (y<(Int32)m_height) : (y>-1));
			y+=((top == True) ? 1 : -1))
			{
				x=((left == True) ? 0 : ((Int32)m_width-1));

				while(((left == True) ? (x<(Int32)m_width) : (x>-1)))
				{
					m = y*(Int32)m_width*3 + x*3; n = _data[ofs]*palbpp;
					m_data[m+2] = palette[n];
					m_data[m+1] = palette[n + 1];
					m_data[m]   = palette[n + 2];

					++ofs;
					x += ((left == True) ? 1 : -1);
				}
			}

		m_bpp = 3;
	}

	//image non compressé sans palette (RGB)
	else if(buf == 2)
	{
		Int32 x,y;
		UInt32 m;

		for(y=((top == True) ? 0 : ((Int32)m_height-1));((top == True) ? (y<(Int32)m_height) : (y>-1));
			y+=((top == True) ? 1 : -1))
			{
				x=((left == True) ? 0 : ((Int32)m_width-1));

				while(((left == True) ? (x<(Int32)m_width) : (x>-1)))
				{
					m = y*m_width*(m_bpp>>3) + x*(m_bpp>>3);

					m_data[m+2] = _data[ofs++];
					m_data[m+1] = _data[ofs++];
					m_data[m]   = _data[ofs++];

					if(m_bpp == 32)
						m_data[m+3] = _data[ofs++];

					x += ((left == True) ? 1 : -1);
				}
			}

		if (m_bpp == 32)
			m_bpp = 4;
		else
			m_bpp = 3;
	}

	//image non compressé en mode de gris
	else if(buf == 3)
	{
		Int32 x,y;
		UInt32 m;

		for(y=((top == True) ? 0 : ((Int32)m_height-1));((top == True) ? (y<(Int32)m_height) : (y>-1));
			y+=((top == True) ? 1 : -1))
			{
				x=((left == True) ? 0 : ((Int32)m_width-1));

				while(((left == True) ? (x<(Int32)m_width) : (x>-1)))
				{
					m = y*(Int32)m_width*3 + x*3;
					m_data[m] = m_data[m + 1] = m_data[m + 2] = _data[ofs++];
					x += ((left == True) ? 1 : -1);
				}
			}

		m_bpp = 3;
	}

	//image compressé (RLE) avec palette
	else if(buf == 9)
	{
		Int32 y;
		UInt32 pos;
		ofs = 0;

		for ( y = 0 ; y<(Int32)m_height ; ++y )
		{
			Int32 len = (Int32)m_width;

			if (top) pos = y * (Int32)m_width * 3;
			else     pos = ((Int32)m_height - y - 1 ) * (Int32)m_width * 3;

			while (len > 0)
			{
				UInt8 packedheader = _data[ofs++];
				UInt8 packedsize = 1 + (packedheader & 0x7f);

				if(packedheader & 0x80)
				{
					for (Int32 i = 0 ; i < packedsize ; ++i)
					{
						UInt32 n = _data[ofs]*palbpp;

						m_data[pos+i*3+2] = palette[n];
						m_data[pos+i*3+1] = palette[n+1];
						m_data[pos+i*3]   = palette[n+2];
					}
					++ofs;
				}
				else
				{
					for (Int32 i = 0 ; i < packedsize ; ++i)
					{
						UInt32 n = _data[ofs+i]*palbpp;

						m_data[pos+i*3+2] = palette[n];
						m_data[pos+i*3+1] = palette[n+1];
						m_data[pos+i*3]   = palette[n+2];
					}
					ofs += packedsize;
				}
				pos += packedsize * 3;
				len -= packedsize;
			}
		}

		m_bpp = 3;

		// si right coded, pour optimiser le code vu que ce cas n'est pas courant, on inverse
		if (!left)
		{
			m_size = m_width * m_height * 3;
			hFlip();
		}
	}

	//image compressé (RLE) sans palette (RGB)
	else if(buf == 10)
	{
		Int32 y;
		UInt32 pos;
		ofs = 0;

		for ( y = 0 ; y<(Int32)m_height ; ++y )
		{
			Int32 len = (Int32)m_width;

			if (top) pos = y * (Int32)m_width * (m_bpp>>3);
			else     pos = ((Int32)m_height - y - 1 ) * (Int32)m_width * (m_bpp>>3);

			while (len > 0)
			{
				UInt8 packedheader = _data[ofs++];
				UInt8 packedsize = 1 + (packedheader & 0x7f);

				if(packedheader & 0x80)
				{
					for (Int32 i = 0 ; i < packedsize ; ++i)
					{
						UInt32 ofs_dst = pos + i*(m_bpp>>3);

						m_data[ofs_dst+2] = _data[ofs];
						m_data[ofs_dst+1] = _data[ofs+1];
						m_data[ofs_dst]   = _data[ofs+2];

						if (m_bpp==32)
							m_data[ofs_dst+3] = _data[ofs+3];
					}
					ofs += (m_bpp>>3);
				}
				else
				{
					for (Int32 i = 0 ; i < packedsize ; ++i)
					{
						UInt32 ofs_dst = pos + i*(m_bpp>>3);
						UInt32 ofs_src = ofs + i*(m_bpp>>3);

						m_data[ofs_dst+2] = _data[ofs_src];
						m_data[ofs_dst+1] = _data[ofs_src+1];
						m_data[ofs_dst]   = _data[ofs_src+2];

						if (m_bpp==32)
							m_data[ofs_dst+3] = _data[ofs_src+3];
					}
					ofs += packedsize * (m_bpp>>3);
				}

				pos += packedsize * (m_bpp>>3);
				len -= packedsize;
			}
		}

		if (m_bpp == 32)
			m_bpp = 4;
		else
			m_bpp = 3;

		// si right coded, pour optimiser le code vu que ce cas n'est pas courant, on inverse
		if (!left)
		{
			m_size = m_width * m_height * (m_bpp>>3);
			hFlip();
		}
	}

	//image compressé (RLE) en mode de gris
	else if(buf == 11)
	{
		Int32 y;
		UInt32 pos;
		ofs = 0;

		for ( y = 0 ; y<(Int32)m_height ; ++y )
		{
			Int32 len = (Int32)m_width;

			if (top) pos = y * (Int32)m_width * 3;
			else     pos = ((Int32)m_height - y - 1 ) * (Int32)m_width * 3;

			while (len > 0)
			{
				UInt8 packedheader = _data[ofs++];
				UInt8 packedsize = 1 + (packedheader & 0x7f);

				if(packedheader & 0x80)
				{
					memset(m_data+pos,_data[ofs++],3*packedsize);
				}
				else
				{
					for (Int32 i = 0 ; i < packedsize ; ++i)
						memset(m_data+pos+i*3,_data[ofs+i],3);
					ofs += packedsize;
				}
				pos += packedsize * 3;
				len -= packedsize;
			}
		}

		m_bpp = 3;

		// si right coded, pour optimiser le code vu que ce cas n'est pas courant, on inverse
		if (!left)
		{
			m_size = m_width * m_height * 3;
			hFlip();
		}
	}

	//si le fichier ne contient pas de données
	else
	{
		deleteArray(palette);
		deleteArray(m_data);
		deleteArray(_data);

        O3D_ERROR(E_InvalidFormat("Invalid TGA color format"));
	}

	deleteArray(palette);
	deleteArray(_data);

	m_size = m_width * m_height * m_bpp;

	m_state = True;
	return True;
}

/*---------------------------------------------------------------------------------------
  Permet de charger un fichier TGA en forcant son format en 24 bpp, ne lis pas le 16bpp
  Renvoi True si succé
---------------------------------------------------------------------------------------*/
Bool Tga::loadRgb24(InStream &is)
{
	// check the file format
    if (!checkFormat(is))
        O3D_ERROR(E_InvalidFormat("Invalid TGA token"));

	UInt32 palcol = 0;
	UInt32 palbpp = 0;

	UInt8* _data = NULL;
	UInt8 *palette = NULL;

	UInt32 ofs = 0;
	UInt8 buf;
	Bool top,left;
	UInt8 idSize;

    m_seekPos = is.getPosition();

    is >> idSize;	// taille de l'id (0 si pas)
	idSize += 18;

	//lecture de la taille en X et Y et du nombre de bits par pixels
    is.reset(m_seekPos+12);

	UInt16 u_short;
    is >> u_short; m_width = u_short;               //largeur de l'image
    is >> u_short; m_height = u_short;               //hauteur de l'image

    is >> buf; m_bpp = buf;                         //bits par pixels
    is >> buf; buf &= 0x30;                       //ordre d'apparition des pixels
	top = left = False;

	if((buf == 0x20) || (buf == 0x30)) top = True;
	if((buf == 0x00) || (buf == 0x20)) left = True;

	if(m_bpp == 16)		//mode de couleur non supporté
        O3D_ERROR(E_InvalidFormat("Invalid TGA color format"));

    is.reset(m_seekPos+2);
    is >> buf;         //lecture du type de compression
    is.reset(m_seekPos+idSize);

	m_data = new UInt8[m_width*m_height*3];

    is.reset(m_seekPos+idSize);

	//si le fichier contient une palette
	if((buf == 1) || (buf == 9))
	{
		UInt8 c;

        is.reset(m_seekPos+5L);    //se place devant les informations de la palette
        is >> u_short; palcol = u_short;  //lit le nombre de couleurs dans la palette
        is >> c; palbpp = c >> 3;         //lit le nombre de composantes pour chaque couleur

		palette = new UInt8[palcol*palbpp];

        is.reset(m_seekPos+idSize);
        is.read(palette, palcol*palbpp);	//lecture de la palette
	}

	//alloue et charge le buffer index
    _data = new UInt8[is.getAvailable()];

    UInt32 load = is.getAvailable();
    is.read(_data, load);

	//image non compressé avec palette
	if(buf == 1)
	{
		Int32 x,y;
		UInt32 m,n;

		for(y=((top == True) ? 0 : (m_height-1));((top == True) ? (y<(Int32)m_height) : (y>-1));
			y+=((top == True) ? 1 : -1))
			{
				x=((left == True) ? 0 : ((Int32)m_width-1));

				while(((left == True) ? (x<(Int32)m_width) : (x>-1)))
				{
					m = y*(Int32)m_width*3 + x*3; n = _data[ofs]*palbpp;
					m_data[m+2] = palette[n];
					m_data[m+1] = palette[n + 1];
					m_data[m]   = palette[n + 2];

					++ofs;
					x += ((left == True) ? 1 : -1);
				}
			}
	}

	//image non compressé sans palette (RGB)
	else if(buf == 2)
	{
		Int32 x,y;
		UInt32 m;

		for(y=((top == True) ? 0 : ((Int32)m_height-1));((top == True) ? (y<(Int32)m_height) : (y>-1));
			y+=((top == True) ? 1 : -1))
			{
				x=((left == True) ? 0 : ((Int32)m_width-1));

				while(((left == True) ? (x<(Int32)m_width) : (x>-1)))
				{
					m = y*(Int32)m_width*3 + x*3;

					m_data[m+2] = _data[ofs++];
					m_data[m+1] = _data[ofs++];
					m_data[m]   = _data[ofs++];
					if(m_bpp == 32) ++ofs;

					x += ((left == True) ? 1 : -1);
				}
			}
	}

	//image non compressé en mode de gris
	else if(buf == 3)
	{
		Int32 x,y;
		UInt32 m;

		for(y=((top == True) ? 0 : ((Int32)m_height-1));((top == True) ? (y<(Int32)m_height) : (y>-1));
			y+=((top == True) ? 1 : -1))
			{
				x=((left == True) ? 0 : ((Int32)m_width-1));

				while(((left == True) ? (x<(Int32)m_width) : (x>-1)))
				{
					m = y*(Int32)m_width*3 + x*3;
					m_data[m] = m_data[m + 1] = m_data[m + 2] = _data[ofs++];
					x += ((left == True) ? 1 : -1);
				}
			}
	}

	//image compressé (RLE) avec palette
	else if(buf == 9)
	{
		Int32 y;
		UInt32 pos;
		ofs = 0;

		for ( y = 0 ; y<(Int32)m_height ; ++y )
		{
			Int32 len = (Int32)m_width;

			if (top) pos = y * (Int32)m_width * 3;
			else     pos = ((Int32)m_height - y - 1 ) * (Int32)m_width * 3;

			while (len > 0)
			{
				UInt8 packedheader = _data[ofs++];
				UInt8 packedsize = 1 + (packedheader & 0x7f);

				if(packedheader & 0x80)
				{
					for (Int32 i = 0 ; i < packedsize ; ++i)
					{
						UInt32 n = _data[ofs]*palbpp;

						m_data[pos+i*3+2] = palette[n];
						m_data[pos+i*3+1] = palette[n+1];
						m_data[pos+i*3]   = palette[n+2];
					}
					++ofs;
				}
				else
				{
					for (Int32 i = 0 ; i < packedsize ; ++i)
					{
						UInt32 n = _data[ofs+i]*palbpp;

						m_data[pos+i*3+2] = palette[n];
						m_data[pos+i*3+1] = palette[n+1];
						m_data[pos+i*3]   = palette[n+2];
					}
					ofs += packedsize;
				}
				pos += packedsize * 3;
				len -= packedsize;
			}
		}

		// si right coded, pour optimiser le code vu que ce cas n'est pas courant, on inverse
		if (!left)
		{
			m_bpp = 3;
			m_size = m_width * m_height * 3;
			hFlip();
		}
	}

	//image compressé (RLE) sans palette (RGB)
	else if(buf == 10)
	{
		Int32 y;
		UInt32 pos;
		ofs = 0;

		for ( y = 0 ; y<(Int32)m_height ; ++y )
		{
			Int32 len = (Int32)m_width;

			if (top) pos = y * (Int32)m_width * 3;
			else     pos = ((Int32)m_height - y - 1 ) * (Int32)m_width * 3;

			while (len > 0)
			{
				UInt8 packedheader = _data[ofs++];
				UInt8 packedsize = 1 + (packedheader & 0x7f);

				if(packedheader & 0x80)
				{
					for (Int32 i = 0 ; i < packedsize ; ++i)
					{
						UInt32 ofs_dst = pos + i*3;

						m_data[ofs_dst+2] = _data[ofs];
						m_data[ofs_dst+1] = _data[ofs+1];
						m_data[ofs_dst]   = _data[ofs+2];
					}
					ofs += (m_bpp>>3);
				}
				else
				{
					for (Int32 i = 0 ; i < packedsize ; ++i)
					{
						UInt32 ofs_dst = pos + i*3;
						UInt32 ofs_src = ofs + i*(m_bpp>>3);

						m_data[ofs_dst+2] = _data[ofs_src];
						m_data[ofs_dst+1] = _data[ofs_src+1];
						m_data[ofs_dst]   = _data[ofs_src+2];
					}
					ofs += packedsize * (m_bpp>>3);
				}
				pos += packedsize * 3;
				len -= packedsize;
			}
		}

		// si right coded, pour optimiser le code vu que ce cas n'est pas courant, on inverse
		if (!left)
		{
			m_bpp = 3;
			m_size = m_width * m_height * 3;
			hFlip();
		}

	}

	//image compressé (RLE) en mode de gris
	else if(buf == 11)
	{
		Int32 y;
		UInt32 pos;
		ofs = 0;

		for ( y = 0 ; y<(Int32)m_height ; ++y )
		{
			Int32 len = (Int32)m_width;

			if (top) pos = y * (Int32)m_width * 3;
			else     pos = ((Int32)m_height - y - 1 ) * (Int32)m_width * 3;

			while (len > 0)
			{
				UInt8 packedheader = _data[ofs++];
				UInt8 packedsize = 1 + (packedheader & 0x7f);

				if(packedheader & 0x80)
				{
					memset(m_data+pos,_data[ofs++],3*packedsize);
				}
				else
				{
					for (Int32 i = 0 ; i < packedsize ; ++i)
						memset(m_data+pos+i*3,_data[ofs+i],3);
					ofs += packedsize;
				}
				pos += packedsize * 3;
				len -= packedsize;
			}
		}

		// si right coded, pour optimiser le code vu que ce cas n'est pas courant, on inverse
		if (!left)
		{
			m_bpp = 3;
			m_size = m_width * m_height * 3;
			hFlip();
		}
	}

	//si le fichier ne contient pas de données
	else
	{
		deleteArray(palette);
		deleteArray(m_data);
		deleteArray(_data);

        O3D_ERROR(E_InvalidFormat("Invalid TGA color format"));
	}

	deleteArray(palette);
	deleteArray(_data);

	m_bpp = 3;
	m_size = m_width * m_height * 3;

	m_state = True;
	return True;
}

/*---------------------------------------------------------------------------------------
  Permet de charger un fichier TGA en forcant son format en 32 bpp, ne lis pas le 16bpp
  Renvoi True si succé.
---------------------------------------------------------------------------------------*/
Bool Tga::loadRgba32(InStream &is)
{
	// check the file format
    if (!checkFormat(is))
        O3D_ERROR(E_InvalidFormat("Invalid TGA token"));

	UInt32 palcol = 0;
	UInt32 palbpp = 0;

	UInt8 *_data = NULL;
	UInt8 *palette = NULL;

	UInt32 ofs = 0;
	UInt8 buf;
	Bool top,left;
	UInt8 idSize;

    m_seekPos = is.getPosition();

    is >> idSize;		// taille de l'id (0 si pas)
	idSize += 18;

	//lecture de la taille en X et Y et du nombre de bits par pixels
    is.reset(m_seekPos+12);

	UInt16 u_short;
    is >> u_short; m_width = u_short;               //largeur de l'image
    is >> u_short; m_height = u_short;               //hauteur de l'image

    is >> buf; m_bpp = buf;                         //bits par pixels
    is >> buf; buf &= 0x30;                       //ordre d'apparition des pixels
	top = left = False;

	if((buf == 0x20) || (buf == 0x30)) top = True;
	if((buf == 0x00) || (buf == 0x20)) left = True;

	if(m_bpp == 16)		//mode de couleur non supporté
        O3D_ERROR(E_InvalidFormat("Invalid TGA color format"));

    is.reset(m_seekPos+2);
    is >> buf;              //lecture du type de compressage
    is.reset(m_seekPos+idSize);

	m_data = new UInt8[m_width*m_height*4];

	//si le fichier contient une palette
	if((buf == 1) || (buf == 9))
	{
		UInt8 c;

        is.reset(m_seekPos+5L);    //se place devant les informations de la palette
        is >> u_short; palcol = u_short;  //lit le nombre de couleurs dans la palette
        is >> c; palbpp = c >> 3;         //lit le nombre de composantes pour chaque couleur

		palette = new UInt8[palcol*palbpp];

        is.reset(m_seekPos+idSize);    //debut des donnée de la palette
        is.read(palette, palcol*palbpp);     //lecture de la palette
	}

	//alloue et charge le buffer index
    _data = new UInt8[is.getAvailable()];

    UInt32 load = is.getAvailable();
    is.read(_data, load);

	//image non compressé avec palette
	if(buf == 1)
	{
		Int32 x,y;
		UInt32 m,n;

		for(y=((top == True) ? 0 : ((Int32)m_height-1));((top == True) ? (y<(Int32)m_height) : (y>-1));
			y+=((top == True) ? 1 : -1))
			{
				x=((left == True) ? 0 : ((Int32)m_width-1));

				while(((left == True) ? (x<(Int32)m_width) : (x>-1)))
				{
					m = y*(Int32)m_width*4 + x*4; n = _data[ofs]*palbpp;

					m_data[m+2] = palette[n];
					m_data[m+1] = palette[n + 1];
					m_data[m]   = palette[n + 2];
					m_data[m+3] = 255;

					if(palbpp == 4)
						m_data[m+3] = palette[n + 3];

					++ofs;
					x+=((left == True) ? 1 : -1);
				}
			}
	}

	//image non compressé sans palette (RGB)
	else if(buf == 2)
	{
		Int32 x,y;
		UInt32 m;

		for(y=((top == True) ? 0 : ((Int32)m_height-1));((top == True) ? (y<(Int32)m_height) : (y>-1));
			y+=((top == True) ? 1 : -1))
			{
				x=((left == True) ? 0 : ((Int32)m_width-1));

				while(((left == True) ? (x<(Int32)m_width) : (x>-1)))
				{
					m = y*(Int32)m_width*4 + x*4;

					m_data[m+2] = _data[ofs++];
					m_data[m+1] = _data[ofs++];
					m_data[m]   = _data[ofs++];
					m_data[m+3] = 255;

					if(m_bpp == 32)
						m_data[m+3] = _data[ofs++];

					x += ((left == True) ? 1 : -1);
				}
			}
	}

	//image non compressé en mode de gris
	else if(buf == 3)
	{
		Int32 x,y;
		UInt32 m;

		for(y=((top == True) ? 0 : ((Int32)m_height-1));((top == True) ? (y<(Int32)m_height) : (y>-1));
			y+=((top == True) ? 1 : -1))
			{
				x=((left == True) ? 0 : ((Int32)m_width-1));

				while(((left == True) ? (x<(Int32)m_width) : (x>-1)))
				{
					m = y*(Int32)m_width*4 + x*4;
					m_data[m] = m_data[m + 1] = m_data[m + 2] = m_data[m + 3] = _data[ofs++];
					x+=((left == True) ? 1 : -1);
				}
			}
	}

	//image compressé (RLE) avec palette
	else if(buf == 9)
	{
		Int32 y;
		UInt32 pos;
		ofs = 0;

		for ( y = 0 ; y<(Int32)m_height ; ++y )
		{
			Int32 size = (Int32)m_width;

			if (top) pos = y * (Int32)m_width * 4;
			else     pos = ((Int32)m_height - y - 1 ) * (Int32)m_width * 4;

			while (size > 0)
			{
				UInt8 packedheader = _data[ofs++];
				UInt8 packedsize = 1 + (packedheader & 0x7f);

				if(packedheader & 0x80)
				{
					for (Int32 i = 0 ; i < packedsize ; ++i)
					{
						UInt32 n = _data[ofs]*palbpp;

						m_data[pos+i*4+2] = palette[n];
						m_data[pos+i*4+1] = palette[n+1];
						m_data[pos+i*4]   = palette[n+2];
						if (m_bpp==32)
							m_data[pos+i*4+3] = palette[n+3];
						else
							m_data[pos+i*4+3] = 255;
					}
					++ofs;
				}
				else
				{
					for (Int32 i = 0 ; i < packedsize ; ++i)
					{
						UInt32 n = _data[ofs+i]*palbpp;

						m_data[pos+i*4+2] = palette[n];
						m_data[pos+i*4+1] = palette[n+1];
						m_data[pos+i*4]   = palette[n+2];

						if (m_bpp==32)
							m_data[pos+i*4+3] = palette[n+3];
						else
							m_data[pos+i*4+3] = 255;
					}
					ofs += packedsize;
				}
				pos  += packedsize * 4;
				size -= packedsize;
			}
		}

		// si right coded, pour optimiser le code vu que ce cas n'est pas courant, on inverse
		if (!left)
		{
			m_bpp = 4;
			m_size = m_width * m_height * 4;
			hFlip();
		}
	}
	//image compressé (RLE) sans palette (RGB)
	else if(buf == 10)
	{
		Int32 y;
		UInt32 pos;
		ofs = 0;

		if(m_bpp == 24) for(pos=3 ; pos<(m_width*m_height*4) ; pos+=4) m_data[pos] = 255;

		for ( y = 0 ; y<(Int32)m_height ; ++y )
		{
			Int32 len = (Int32)m_width;

			if (top) pos = y * (Int32)m_width * 4;
			else     pos = ((Int32)m_height - y - 1 ) * (Int32)m_width * 4;

			while (len > 0)
			{
				UInt8 packedheader = _data[ofs++];
				UInt8 packedsize = 1 + (packedheader & 0x7f);

				if(packedheader & 0x80)
				{
					for (Int32 i = 0 ; i < packedsize ; ++i)
					{
						UInt32 ofs_dst = pos + i*4;

						m_data[ofs_dst+2] = _data[ofs];
						m_data[ofs_dst+1] = _data[ofs+1];
						m_data[ofs_dst]   = _data[ofs+2];

						if (m_bpp==32)
							m_data[ofs_dst+3] = _data[ofs+3];
						else
							m_data[ofs_dst+3] = 255;
					}
					ofs += (m_bpp>>3);
				}
				else
				{
					for (Int32 i = 0 ; i < packedsize ; ++i)
					{
						UInt32 ofs_dst = pos + i*4;
						UInt32 ofs_src = ofs + i*(m_bpp>>3);

						m_data[ofs_dst+2] = _data[ofs_src];
						m_data[ofs_dst+1] = _data[ofs_src+1];
						m_data[ofs_dst]   = _data[ofs_src+2];

						if (m_bpp==32)
							m_data[ofs_dst+3] = _data[ofs_src+3];
						else
							m_data[ofs_dst+3] = 255;
					}
					ofs += packedsize * (m_bpp>>3);
				}
				pos += packedsize * 4;
				len -= packedsize;
			}
		}

		// si right coded, pour optimiser le code vu que ce cas n'est pas courant, on inverse
		if (!left)
		{
			m_bpp = 4;
			m_size = m_width * m_height * 4;
			hFlip();
		}
	}
	//image compressé (RLE) en mode de gris
	else if(buf == 11)
	{
		Int32 y;
		UInt32 pos;
		ofs = 0;

		for(pos=3 ; pos<(m_width*m_height*4) ; pos+=4) m_data[pos] = 255;

		for ( y = 0 ; y<(Int32)m_height ; ++y )
		{
			Int32 len = (Int32)m_width;

			if (top) pos = y * (Int32)m_width * 4;
			else     pos = (m_height - y - 1 ) * (Int32)m_width * 4;

			while (len > 0)
			{
				UInt8 packedheader = _data[ofs++];
				UInt8 packedsize = 1 + (packedheader & 0x7f);

				if(packedheader & 0x80)
				{
					for (Int32 i = 0 ; i < packedsize ; ++i)
						memset(m_data+pos+i*4,_data[ofs],3);
					++ofs;
				}
				else
				{
					for (Int32 i = 0 ; i < packedsize ; ++i)
						memset(m_data+pos+i*4,_data[ofs+i],3);
					ofs += packedsize;
				}
				pos += packedsize * 4;
				len -= packedsize;
			}
		}

		// si right coded, pour optimiser le code vu que ce cas n'est pas courant, on inverse
		if (!left)
		{
			m_bpp = 4;
			m_size = m_width * m_height * 4;
			hFlip();
		}
	}

	//si le fichier ne contient pas de données
	else
	{
		deleteArray(palette);
		deleteArray(m_data);
		deleteArray(_data);

        O3D_ERROR(E_InvalidFormat("Invalid TGA color format"));
	}

	deleteArray(palette);
	deleteArray(_data);

	m_bpp = 4;
	m_size = m_width * m_height * 4;

	m_state = True;
	return True;
}

/*---------------------------------------------------------------------------------------
  invertion selon l'axe verticale Y. Renvoi True si succé.
---------------------------------------------------------------------------------------*/
Bool Tga::hFlip()
{
	UInt8 *exdata = new UInt8[m_size];

	// copie de l'ancien buffer pour pouvoir remplace les data de la classe
	memcpy(exdata,m_data,m_size);


	UInt32 lx,ly;
	UInt32 lx2;						// on inverse les ligne de haut en bas
	UInt32 x = m_width*m_bpp;				// taille d'une ligne en octet

	if (m_bpp != 1)
	{
		for ( ly = 0 ; ly < m_height ; ++ly )	// pour chaque ligne de 0..sizeY
		{
			lx2 = x - m_bpp;

			// on lit chaque (RGB) de la ligne
			for ( lx = 0 ; lx < m_width * m_bpp ; lx += m_bpp )
			{
				if (m_bpp == 4)	// ALPHA
					m_data[lx+ly*x+3] = exdata[lx2+ly*x+3];

				m_data[lx+ly*x]   = exdata[lx2+ly*x];		//R
				m_data[lx+ly*x+1] = exdata[lx2+ly*x+1];	//G
				m_data[lx+ly*x+2] = exdata[lx2+ly*x+2];	//B

				lx2 -= m_bpp;
			}
		}
	}
	else
	{
		for ( ly = 0 ; ly < m_height ; ++ly )	// pour chaque ligne de 0..sizeY
		{
			lx2 = x - 1;

			// on lit chaque (RGB) de la ligne
			for ( lx = 0 ; lx < m_width  ; ++lx )
			{
				m_data[lx+ly*x] = exdata[lx2+ly*x];		//index

				lx2--;
			}
		}
	}

	deleteArray(exdata);
	return True;
}

