/**
 * @file vectornd.h
 * @brief N dimensional vectors.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2003-07-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_VECTORND_H
#define _O3D_VECTORND_H

#include <math.h>
#include "base.h"
#include "memorydbg.h"

namespace o3d {

class InStream;
class OutStream;

//---------------------------------------------------------------------------------------
//! @class VectorND
//-------------------------------------------------------------------------------------
//! N dimensional vector
//---------------------------------------------------------------------------------------
class O3D_API VectorND
{
public:

	//! constructor
	VectorND() { V = NULL; dim = 0; }
	VectorND(UInt32 Dim) { V = NULL; dim = 0; reset(Dim); }
	//! copy constructor
	VectorND(const VectorND& vec)
	{
		if (vec.getData() == NULL)
		{
			deleteArray(V);
			dim = 0;
		}
		else
		{
			dim = vec.getDim();
			deleteArray(V);
			V = new Float[dim];
			memcpy(V,vec.getData(),dim*sizeof(Float));
		}
	}
	//! build from a float*
	VectorND(const Float* vec,UInt32 Dim)
	{
		if (vec == NULL || Dim == 0)
		{
			deleteArray(V);
			dim = 0;
		}
		else
		{
			dim = Dim;
			deleteArray(V);
			V = new Float[dim];
			memcpy(V,vec,dim*sizeof(Float));
		}
	}

	~VectorND() { deleteArray(V); }

	//! redimensionne la table du vecteur
	inline void reset(UInt32 Dim)
	{
		if (Dim == 0) return;

  		if (Dim != dim)
		{
			deleteArray(V);
			V = new Float[Dim];
			dim = Dim;
		}

		zero();
	}

	//! renvoie le nombre d'élement du vecteur
	inline UInt32 getDim()const { return dim; }

	//! set un élement i avec val
	inline void setData(UInt32 i,Float val) { if (i >= dim) return; V[i] = val; }
	//! set tout les élements de V avec val
	void setValue(Float val);

	//! renvoie une des composantes du vecteur
	inline Float getData(UInt32 i)const { if (i >= dim) return 0; return V[i]; }
	//! renvoi le vecteur sous forme de pointeur
	inline Float* getData() { return V; }
	inline const Float* getData()const { return V; }

	//! renvoie la longueur du vecteur (norme)
	Float length()const;

	//! renvoie la longueur du vecteur au carré (norme²)
	Float squareLength()const;

	//! renvoie la norme infinie du vecteur (plus grande valeur absolue)
	Float normInf()const;

	//! renvoie la norme absolue du vecteur (somme absolue de ses élements)
	Float normAbs()const;

	//! copie le vecteur à this
	inline VectorND& operator= (const VectorND& vec)
	{
		if (vec.getData() == NULL)
		{
			deleteArray(V);
			dim = 0;
		}
		else
		{
			dim = vec.getDim();
			deleteArray(V);
			V = new Float[dim];
			memcpy(V,vec.getData(),dim*sizeof(Float));
		}
		return (*this);
	}

	//! ajoute un vecteur à this
	VectorND& operator+= (const VectorND& vec);

	//! soustrait un vecteur à this
	VectorND& operator-= (const VectorND& vec);

	//! scalairiser this*=scal
	VectorND& operator*= (const Float scal);

	//! division scalaire this/=scal
	VectorND& operator/= (const Float scal);

	//! produit scalaire this*vec
	Float operator* (const VectorND& v)const;

	//! produit scalaire vec*scal
	VectorND operator* (const Float scal)const;

	//! division scalaire vec/scal
	VectorND operator/ (const Float scal)const;

	//! addition vectorielle v1 + v2
	VectorND operator+ (const VectorND& v)const;

	//! soustraction vectorielle v1 - v2
	VectorND operator- (const VectorND& v)const;

	//! inverse le vecteur this et le renvoi (toutes ses composante sons inversées (-))
	VectorND operator- ()const;

	//! place le vector à 0
	inline void zero() { memset(V,0,dim*sizeof(Float)); }

	//! Serialisation
    Bool writeToFile(OutStream& os)const;
    Bool readFromFile(InStream& is);

protected:

	Float *V;		//!< vector data
	UInt32 dim;		//!< number of data
};

} // namespace o3d

#endif // _O3D_VECTORND_H

