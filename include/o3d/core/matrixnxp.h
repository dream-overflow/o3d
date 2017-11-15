/**
 * @file matrixnxp.h
 * @brief NxP Matrix definition.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2002-07-24
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_MATRIXNXP_H
#define _O3D_MATRIXNXP_H

#include "memorydbg.h"
#include "vectornd.h"

namespace o3d {

#define ACCESS(i,j) (j)+(i)*cols

//---------------------------------------------------------------------------------------
//! @class MatrixNxP
//-------------------------------------------------------------------------------------
//! Row major NxP matrix.
//!   _                                  _
//!  |                                    |
//!  | a00   a10   a20   .....  a(n-1)0   |
//!  | a01   a11   a21   .....  a(n-1)1   |
//!  | a02   a12   a22   .....  a(n-1)2   |
//!  |  .     .     .      .     .        |
//!  |  .     .     .      .     .        |
//!  | a0p-1 a1p-1 a2p-1 .....  a(n-1)p-1 |
//!  |_                                  _|
//---------------------------------------------------------------------------------------
class MatrixNxP
{
public:

	//! Default constructor
	inline MatrixNxP()	// M null
	{
		M = NULL;
		rows = 0;
		cols = 0;
	}

	//! Initialisation constructor
	inline MatrixNxP(UInt32 NbrRows,UInt32 NbrCols)
	{
		M = NULL;
		rows = 0;
		cols = 0;

        reset(NbrRows,NbrCols);
	}

	//! Copy constructor
	inline MatrixNxP(const MatrixNxP& _M)
	{
		if (_M.getData() == NULL)
		{
			deleteArray(M);
			rows = cols = 0;
		}
		else
		{
			rows = _M.getNbrRows();
			cols = _M.getNbrCols();

			deleteArray(M);
			M = new Float[rows*cols];
			memcpy(M,_M.getData(),rows*cols*sizeof(Float));
		}
	}

	//! Destructor
	~MatrixNxP() { deleteArray(M); }

	//! initialise le tableau de la matrice
	inline void reset(UInt32 NbrRows,UInt32 NbrCols)
	{
		if (NbrRows*NbrCols != rows*cols)
		{
			deleteArray(M);
			M = new Float[NbrRows*NbrCols];
			rows = NbrRows;
			cols = NbrCols;
		}

		zero();
	}

	//! renvoie le nombre de ligne/colones
	UInt32 getNbrRows()const { return rows; }
	UInt32 getNbrCols()const { return cols; }

	//! place toutes les composantes de la matrice à 0
	inline void zero() { memset(M,0,rows*cols*sizeof(Float)); }

	//! M devient une matrice d'identitée (diagonal de 1)
	inline void identity()
	{
		// @todo
	}

	//! matrice transposee (transpose this (M))
	void transpose();
	//! matrice transposee (transposé de this (M)) renvoyée. M n'est pas modifiée
	MatrixNxP transposeTo()const;

	//! affecte la matrice _M à this (M) , M = _M
	inline MatrixNxP& operator= (const MatrixNxP& _M)
	{
		if (_M.getData() == NULL)
		{
			deleteArray(M);
			rows = cols = 0;
		}
		else
		{
			rows = _M.getNbrRows();
			cols = _M.getNbrCols();

			deleteArray(M);
			M = new Float[rows*cols];
			memcpy(M,_M.getData(),rows*cols*sizeof(Float));
		}
		return (*this);
	}

	//! ajoute la matrice _M à this (M) , M = M + _M
	MatrixNxP& operator+= (const MatrixNxP& _M);

	//! soustrait la matrice _M à this (M) , M = M - _M
	MatrixNxP& operator-= (const MatrixNxP& _M);

	//! produit matriciel M = M * _M
	MatrixNxP& operator*= (const MatrixNxP& _M);

	//! matrice opposée de M, mais M (this) n'est pas modifiée
	MatrixNxP operator- ()const;

	//! renvoi le produit matriciel de deux matrice M et _M
	MatrixNxP operator* (const MatrixNxP& _M)const;

	//! produit vecteur = matrice * vecteur, v = M * vec
	VectorND operator* (const VectorND& vec);

	//! renvoi la somme matricielle de deux matrice M et _M
	MatrixNxP operator+ (const MatrixNxP& _M)const;

	//! renvoi la soustraction matricielle de deux matrice M et _M
	MatrixNxP operator- (const MatrixNxP& _M)const;

	//! change une entrée de la matrice
	inline void setData(UInt32 i,UInt32 j,Float x)
	{
		if (i >= rows || j >= cols) return;
		M[ACCESS(i,j)] = x;
	}

	//! renvoie les données de la matrice sous forme d'un pointeur d'Float
	inline Float* getData()
	{
		return M;
	}
	inline const Float* getData()const
	{
		return M;
	}

	//! renvoie une entrée de la matrice
	inline Float getData(UInt32 i,UInt32 j)const
	{
		if (i >= rows || j >= cols) return 0.0f;
		return M[ACCESS(i,j)];
	}

	//! resolution de Gauss-Seidel : M*x = b
	void gaussSeidelStep(VectorND& x,const VectorND& b)const;


	//! Serialisation
	Bool writeToFile(OutStream &os)const;
	Bool readFromFile(InStream &is);

protected:

	Float *M;			//!< matrix data

	UInt32 rows;		//!< rows number
	UInt32 cols;		//!< columns number
};

#undef ACCESS

} // namespace o3d

#endif // _O3D_MATRIXNXP_H

