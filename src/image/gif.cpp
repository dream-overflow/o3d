/**
 * @file gif.cpp
 * @brief Load of Gif 87a/89a.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-09
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/image/precompiled.h"
#include "o3d/image/gif.h"

#include "o3d/core/instream.h"
#include "o3d/core/outstream.h"

using namespace o3d;

/*---------------------------------------------------------------------------------------
   check the first bytes of the file for see his format
---------------------------------------------------------------------------------------*/
Bool Gif::checkFormat(InStream& is)
{
	Char id[6];
    is.read(id, 6);
    is.seek(-6);

    if (!strncmp(id,"GIF87a",6) || !strncmp(id,"GIF89a",6))
		return True;

	return False;
}

/*---------------------------------------------------------------------------------------
  Lire un code dans le fichier avec nbits.
---------------------------------------------------------------------------------------*/
UInt16 Gif::Reader::readCode(Int16 nbits)
{
	// masque de 12 bits (c'est le nombre max dans le GIF)
	static UInt16 code_mask[13] =
	{
		0L,
		0x0001L, 0x0003L,
		0x0007L, 0x000FL,
		0x001FL, 0x003FL,
		0x007FL, 0x00FFL,
		0x01FFL, 0x03FFL,
		0x07FFL, 0x0FFFL
	};

	UInt16 ret;				// code à renvoyer
	Int16  i;				// compteur

	if (!nbits_left)					// pas de bits restants ?
	{
		if (navail_bytes <= 0)			// pas d'octets restant dans le buffer ?
		{
			pbytes = byte_buff;
            navail_bytes = is.readInt8();	// lit le nombre d'octets à lire
			if (navail_bytes)
				for (i=0;i<navail_bytes;++i)		// et les copis dans le buffer
                    byte_buff[i] = is.readInt8();
		}
		buf = *pbytes++;				// lit un octet dans le buffer
		nbits_left = 8;					// reste 8 bits non lus
		--navail_bytes;					// et 1 octets de moins dans le buffer
	}
	ret = buf >> (8 - nbits_left);		// traduit la valeur

	while (nbits>nbits_left)			// tant que le nombre de bits d'un code > nbr de bit restant
	{
		if (navail_bytes<=0)			// si plus d'octets dans le buffer
		{
			pbytes = byte_buff;
            navail_bytes = is.readInt8();	// lit le nombre d'octets à lire
			if (navail_bytes)
				for (i=0;i<navail_bytes;++i)	// et les copis dans le buffer
                    byte_buff[i] = is.readInt8();
		}
		buf = *pbytes++;				// lit le premier octet
		ret |= buf << nbits_left;		// et rajoute les bits manquant
		nbits_left += 8;				// il reste donc 8 bits en plus disponibles
		--navail_bytes;					// et 1 octet de moins
	}
	nbits_left -= nbits;				// et on soustrait le nombre de bits lus

	return (ret & code_mask[nbits]);	// et on renvoi le code en masquant les bits inutilisé
}

PixelFormat Gif::getPixelFormat() const
{
	switch (m_bpp)
	{
		case 3:
			return PF_RGB_U8;
		case 4:
            return PF_RGBA_U8;
		default:
			O3D_ERROR(E_InvalidFormat("Unsupported pixel format"));
	}
}

Bool Gif::load(InStream &is)
{
    return loadRgb24(is);
}

Bool Gif::load(InStream &is, PixelFormat pixelFormat)
{
	switch (pixelFormat)
	{
		case PF_RGB_U8:
            return loadRgb24(is);
		case PF_RGBA_U8:
            return loadRgba32(is);
		default:
			O3D_ERROR(E_InvalidParameter("Unsupported convert format"));
	}
}

Bool Gif::save(OutStream &is, PixelFormat pixelFormat)
{
	O3D_ERROR(E_InvalidOperation("Not yet implemented"));
}

Bool Gif::loadRgb24(InStream &is)
{
	// check the file format
    if (!checkFormat(is))
        O3D_ERROR(E_InvalidFormat("Invalid GIF token. Support only GIF87a and GIF89a"));

	UInt8 buf;
	//UInt16 _bpp;
	UInt16 palsize;
	//Bool entrelaced = False;	// image intrelacée
	UInt32 palcol;

    is.seek(6);

	UInt16 u_short;
    is >> u_short; m_width = u_short;   // dimensions de l'image
    is >> u_short; m_height = u_short;
    is >> buf;                        // packed fields

    is.seek(2);

	//_bpp = ((buf&0x70)>>4)+1;	// nombre de bits pour chaque composantes de la palette
	palcol = 2<<(buf&0x7);		// taille de la palette en octet et nombre de couleurs
	palsize = UInt16(3 * palcol);

	UInt8 *palette = new UInt8[palsize];

	if (buf & 0x80)						// si bit de poids fort alors palette globale
	{
        is.read(palette, palsize);	// lecture de la palette globale
	}

	buf = 0;					// recherche de 0x2C (,) debut des info d'une image
    while (buf != 0x2C) buf = is.readInt8();

    is.seek(4);

    is >> u_short; m_width = u_short;   // taille de l'image
    is >> u_short; m_height = u_short;

    is >> buf;                // indicateur (bit7 == 1 -> palette locale)
								// bit 0..3 + 1, color bits

	if (buf&0x80)				// palette local (alors la charger)
	{
		palcol = 2<<(buf&0x7);		// taille de la palette en octet et nombre de couleurs
		palsize = UInt16(3 * palcol);
		//entrelaced = (buf&0x40)?True:False;

		deleteArray(palette);

		palette = new UInt8[palsize];

        is.read(palette, palsize);	// lecture de la palette locale
	}

	m_data = new UInt8[m_width*m_height*3];	// alloue les données de l'image

	UInt8  *stack =  new UInt8[GIF_MAX_CODES + 1];
	UInt16 *prefix = new UInt16[GIF_MAX_CODES + 1];
	UInt8  *suffix = new UInt8[GIF_MAX_CODES + 1];


    Int16 _size = (Int16)is.readInt8();		// nombre de bits au début
	// décompression de l'image
	UInt16 code;
	UInt16 newcodes;				// nouveaux codes
	UInt16 oldcode = 0;				// ancien code
	UInt16 fcode = 0;				// futur code
	Int16 nbits = _size + 1;		// nombre de bit d'un code
	UInt16 top_slot = 1 << nbits;	// limite
	UInt16 clear = 1<<_size;		// code d'effacement
	UInt16 ending = clear + 1;		// nombre max de code avec 9bits (max = 2^nbit - 1), code de fin
	UInt16 slot = newcodes = ending + 1;
	UInt8  *pstack = stack;
	UInt16 c;						// code lu

	UInt8 *pdata = m_data;				// pointeur sur les données de l'image
    Reader reader(is);

	while((c=reader.readCode(nbits)) != ending)	// tant que le code lu n'est pas celui de fin (en général ending == 257)
	{
		if(c==clear)						// code d'effacement (en général 256)
		{
	        nbits = _size + 1;				// remet par défaut
			slot = newcodes;				//
			top_slot = 1 << nbits;			//
			while ((c=reader.readCode(nbits)) == clear) {}	// tant que code == clear
			if(c==ending)					// si code fin on termine
				break;
			if(c>=slot)						// si code > slot, alors c le code 0
				c=0;
			oldcode=fcode=c;				// ancien code = code lu
			*pdata++=palette[(UInt8)c*3];// ajoute la donné décompressée
			*pdata++=palette[(UInt8)c*3+1];
			*pdata++=palette[(UInt8)c*3+2];
		}
		else								// autre code
		{
			code=c;							// code courant = code lu
			if(code>=slot)					// code >= slot ?
			{
				code=oldcode;				// alors on reprends l'ancien code
				*pstack++=(UInt8)fcode; // et on rajoute la valeur décodée
			}
			while(code>=newcodes)			// tant que code lu > newcodes
			{
				*pstack++=suffix[code];		// alors le rajoute au suffix
				code=prefix[code];			// et le code est lu dans le préfix
			}
			*pstack++=(UInt8)code;		// ajoute la valeur décodé
			if(slot<top_slot)				//
			{
				fcode=code;					//
				suffix[slot]=(UInt8)fcode;	// ajoute au suffix
                prefix[slot++]=oldcode;		//
				oldcode=c;					//
			}
			if(slot>=top_slot && nbits<12)	//
			{
				top_slot<<=1;				//
				++nbits;					// on rajoute un bit de code
			}
			while(pstack>stack)				//
			{
				--pstack;					//
				*pdata++=palette[*pstack*3];// insert le pixel
				*pdata++=palette[*pstack*3+1];
				*pdata++=palette[*pstack*3+2];
			}
		}
	}

	deleteArray(stack);
	deleteArray(suffix);
	deleteArray(prefix);

	deleteArray(palette);

 	m_bpp = 3;	// 3 octets
	m_state = True;

	return True;
}

Bool Gif::loadRgba32(InStream &is)
{
	// check the file format
    if (!checkFormat(is))
        O3D_ERROR(E_InvalidFormat("Invalid GIF token. Support only GIF87a and GIF89a"));

	UInt8 buf;
	//UInt16 _bpp;
	UInt16 palsize;
	//Bool entrelaced = False;	// image intrelacée
	UInt32 palcol;

    is.seek(6);

	UInt16 u_short;
    is >> u_short; m_width = u_short;   // dimensions de l'image
    is >> u_short; m_height = u_short;
    is >> buf;                        // packed fields

    is.seek(2);

	//_bpp = ((buf&0x70)>>4)+1;	// nombre de bits pour chaque composantes de la palette
	palcol = 2<<(buf&0x7);		// taille de la palette en octet et nombre de couleurs
	palsize = UInt16(3 * palcol);

	UInt8 *palette = new UInt8[palsize];

	if (buf & 0x80)						// si bit de poids fort alors palette globale
	{
        is.read(palette, palsize);	// lecture de la palette globale
	}

	buf = 0;					// recherche de 0x2C (,) debut des info d'une image
    while (buf != 0x2C) buf = is.readInt8();

    is.seek(4);

    is >> u_short; m_width = u_short;   // taille de l'image
    is >> u_short; m_height = u_short;

    is >> buf;                // indicateur (bit7 == 1 -> palette locale)
								// bit 0..3 + 1, color bits

	if (buf&0x80)				// palette local (alors la charger)
	{
		palcol = 2<<(buf&0x7);		// taille de la palette en octet et nombre de couleurs
		palsize = UInt16(3 * palcol);
		//entrelaced = (buf&0x40)?True:False;

		deleteArray(palette);

		palette = new UInt8[palsize];

        is.read(palette, palsize);	// lecture de la palette locale
	}

	m_data = new UInt8[m_width*m_height*4];	// alloue les données de l'image

	UInt8  *stack =  new UInt8[GIF_MAX_CODES + 1];
	UInt16 *prefix = new UInt16[GIF_MAX_CODES + 1];
	UInt8  *suffix = new UInt8[GIF_MAX_CODES + 1];

    Int16 _size = (Int16)is.readInt8();		// nombre de bits au début
	// décompression de l'image
	UInt16 code;
	UInt16 newcodes;				// nouveaux codes
	UInt16 oldcode = 0;				// ancien code
	UInt16 fcode = 0;				// futur code
	Int16 nbits = _size + 1;		// nombre de bit d'un code
	UInt16 top_slot = 1 << nbits;	// limite
	UInt16 clear = 1<<_size;		// code d'effacement
	UInt16 ending = clear + 1;		// nombre max de code avec 9bits (max = 2^nbit - 1), code de fin
	UInt16 slot = newcodes = ending + 1;
	UInt8  *pstack = stack;
	UInt16 c;						// code lu

	UInt8 *pdata = m_data;				// pointeur sur les données de l'image
    Reader reader(is);

	while((c=reader.readCode(nbits)) != ending)	// tant que le code lu n'est pas celui de fin (en général ending == 257)
	{
		if(c==clear)						// code d'effacement (en général 256)
		{
	        nbits = _size + 1;				// remet par défaut
			slot = newcodes;				//
			top_slot = 1 << nbits;			//
			while ((c=reader.readCode(nbits)) == clear) {}	// tant que code == clear
			if(c==ending)					// si code fin on termine
				break;
			if(c>=slot)						// si code > slot, alors c le code 0
				c=0;
			oldcode=fcode=c;				// ancien code = code lu
			*pdata++=palette[(UInt8)c*3];// ajoute la donné décompressée
			*pdata++=palette[(UInt8)c*3+1];
			*pdata++=palette[(UInt8)c*3+2];
			*pdata++=255;
		}
		else								// autre code
		{
			code=c;							// code courant = code lu
			if(code>=slot)					// code >= slot ?
			{
				code=oldcode;				// alors on reprends l'ancien code
				*pstack++=(UInt8)fcode; // et on rajoute la valeur décodée
			}
			while(code>=newcodes)			// tant que code lu > newcodes
			{
				*pstack++=suffix[code];		// alors le rajoute au suffix
				code=prefix[code];			// et le code est lu dans le préfix
			}
			*pstack++=(UInt8)code;		// ajoute la valeur décodé
			if(slot<top_slot)				//
			{
				fcode=code;					//
				suffix[slot]=(UInt8)fcode;	// ajoute au suffix
                prefix[slot++]=oldcode;		//
				oldcode=c;					//
			}
			if(slot>=top_slot && nbits<12)	//
			{
				top_slot<<=1;				//
				++nbits;					// on rajoute un bit de code
			}
			while(pstack>stack)				//
			{
				--pstack;					//
				*pdata++=palette[*pstack*3];// insert le pixel
				*pdata++=palette[*pstack*3+1];
				*pdata++=palette[*pstack*3+2];
				*pdata++=255;
			}
		}
	}

	deleteArray(stack);
	deleteArray(suffix);
	deleteArray(prefix);

	deleteArray(palette);

 	m_bpp = 4;	// 4 octets
	m_state = True;

	return True;
}

