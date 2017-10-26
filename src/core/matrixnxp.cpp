/**
 * @file matrixnxp.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2002-07-24
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/matrixnxp.h"

#include "o3d/core/instream.h"
#include "o3d/core/outstream.h"
#include <math.h>

using namespace o3d;

#define ACCESS(i,j) (j)+(i)*cols

/*---------------------------------------------------------------------------------------
  matrice transpose (transpose this (M))
---------------------------------------------------------------------------------------*/
void MatrixNxP::transpose()
{
	MatrixNxP temp(rows,cols);
	Float* R = temp.getData();

	for (UInt32 i = 0 ; i < rows ; ++i)
		for (UInt32 j = 0 ; j < cols ; ++j)
			temp.setData(i,j,M[ACCESS(j,i)]);

	memcpy(M,R,rows*cols*sizeof(Float));
}

/*---------------------------------------------------------------------------------------
  matrice transpose (transpose de this (M)) renvoyee M n'est pas modifi�e
---------------------------------------------------------------------------------------*/
MatrixNxP MatrixNxP::transposeTo()const
{
	MatrixNxP temp(rows,cols);
//	Float* R = temp.getData();

	for (UInt32 i = 0 ; i < rows ; ++i)
		for (UInt32 j = 0 ; j < cols ; ++j)
			temp.setData(i,j,M[ACCESS(j,i)]);

	return temp;
}

/*---------------------------------------------------------------------------------------
  produit matriciel M = M * _M (colonne major)
---------------------------------------------------------------------------------------*/
MatrixNxP& MatrixNxP::operator*= (const MatrixNxP& _M)
{
	if (cols != _M.getNbrRows()) return (*this);

	MatrixNxP temp(rows,_M.getNbrCols());
	Float* R = temp.getData();

	Float val;

	for (UInt32 i = 0 ; i < rows ; ++i)
		for (UInt32 j = 0 ; j < _M.getNbrCols(); ++j)
		{
			val = 0;

			for (UInt32 k = 0 ; k < cols ; ++k)
				val += M[ACCESS(i,k)] * _M.getData(k,j);

			temp.setData(i,j,val);
		}

	memcpy(M,R,rows*cols*sizeof(Float));
	return (*this);
}

/*---------------------------------------------------------------------------------------
  renvoi le produit matriciel de deux matrice M et _M (colonne major)
---------------------------------------------------------------------------------------*/
MatrixNxP MatrixNxP::operator* (const MatrixNxP& _M)const
{
	if (cols != _M.getNbrRows()) return (*this);

	MatrixNxP temp(rows,_M.getNbrCols());
//	Float* R = temp.getData();

	Float val;

	for (UInt32 i = 0 ; i < rows ; ++i)
		for (UInt32 j = 0 ; j < _M.getNbrCols(); ++j)
		{
			val = 0;

			for (UInt32 k = 0 ; k < cols ; ++k)
				val += M[ACCESS(i,k)] * _M.getData(k,j);

			temp.setData(i,j,val);
		}

	return temp;
}

/*---------------------------------------------------------------------------------------
  ajoute la matrice _M � this (M) , M = M + _M
---------------------------------------------------------------------------------------*/
MatrixNxP& MatrixNxP::operator+= (const MatrixNxP& _M)
{
	if (cols != _M.getNbrCols() || rows != _M.getNbrRows())
		return (*this);

	MatrixNxP temp(rows,cols);
	Float* R = temp.getData();

	for (UInt32 i = 0 ; i < rows; ++i)
		for (UInt32 j = 0 ; j < cols ; ++j)
			temp.setData(i,j,M[ACCESS(i,j)]+_M.getData(i,j));

	memcpy(M,R,rows*cols*sizeof(Float));
	return (*this);
}

/*---------------------------------------------------------------------------------------
  renvoi la somme matricielle de deux matrice M et _M
---------------------------------------------------------------------------------------*/
MatrixNxP MatrixNxP::operator+ (const MatrixNxP& _M)const
{
	if (cols != _M.getNbrCols() || rows != _M.getNbrRows())
		return (*this);

	MatrixNxP temp(rows,cols);
//	Float* R = temp.getData();

	for (UInt32 i = 0 ; i < rows; ++i)
		for (UInt32 j = 0 ; j < cols ; ++j)
			temp.setData(i,j,M[ACCESS(i,j)]+_M.getData(i,j));

	return temp;
}

/*---------------------------------------------------------------------------------------
  soustrait la matrice _M � this (M) , M = M - _M
---------------------------------------------------------------------------------------*/
MatrixNxP& MatrixNxP::operator-= (const MatrixNxP& _M)
{
	if (cols != _M.getNbrCols() || rows != _M.getNbrRows())
		return (*this);

	MatrixNxP temp(rows,cols);
	Float* R = temp.getData();

	for (UInt32 i = 0 ; i < rows; ++i)
		for (UInt32 j = 0 ; j < cols ; ++j)
			temp.setData(i,j,M[ACCESS(i,j)]-_M.getData(i,j));

	memcpy(M,R,rows*cols*sizeof(Float));
	return (*this);
}

/*---------------------------------------------------------------------------------------
  renvoi la soustraction matricielle de deux matrice M et _M
---------------------------------------------------------------------------------------*/
MatrixNxP MatrixNxP::operator- (const MatrixNxP& _M)const
{
	if (cols != _M.getNbrCols() || rows != _M.getNbrRows())
		return (*this);

	MatrixNxP temp(rows,cols);
//	Float* R = temp.getData();

	for (UInt32 i = 0 ; i < rows; ++i)
		for (UInt32 j = 0 ; j < cols ; ++j)
			temp.setData(i,j,M[ACCESS(i,j)]-_M.getData(i,j));

	return temp;
}

/*---------------------------------------------------------------------------------------
  matrice oppos�e de M, mais M (this) n'est pas modifi�e
---------------------------------------------------------------------------------------*/
MatrixNxP MatrixNxP::operator- ()const
{
	MatrixNxP temp(rows,cols);
//	Float* R = temp.getData();

	for (UInt32 i = 0 ; i < rows; ++i)
		for (UInt32 j = 0 ; j < cols ; ++j)
			temp.setData(i,j,-M[ACCESS(i,j)]);

	return temp;
}

/*---------------------------------------------------------------------------------------
  produit vecteur = matrice * vecteur, v = M * vec
---------------------------------------------------------------------------------------*/
VectorND MatrixNxP::operator* (const VectorND& vec)
{
	if (cols != vec.getDim())
		return VectorND();

	VectorND temp(rows);

	Float val;

	for (UInt32 i = 0 ; i < rows ; ++i)
	{
		val = 0;
		for (UInt32 j = 0 ; j < cols ; ++j)
			val += M[ACCESS(i,j)]*vec.getData(j);

		temp.setData(i,val);
	}

	return temp;
}

/*---------------------------------------------------------------------------------------
  r�solution de Gauss-Seidel : M*x = b
---------------------------------------------------------------------------------------*/
void MatrixNxP::gaussSeidelStep(VectorND& x,const VectorND& b)const
{
	UInt32 n = cols;
	if (rows != n) return;
	if (x.getDim() != n) return;
	if (b.getDim() != n) return;

	Float temp;
	UInt32 i,j;

	for ( i = 0 ; i < n ; ++i)
	{
		temp = b.getData(i);

		for ( j = 0 ; j < i ; ++j)
			temp -= M[ACCESS(i,j)] * x.getData(j);
		for ( j = i+1 ; j < n ; ++j)
			temp -= M[ACCESS(i,j)] * x.getData(j);

		if (o3d::abs(M[ACCESS(i,i)]) > o3d::Limits<Float>::epsilon())
		{
			temp /= M[ACCESS(i,i)];
			x.setData(i,temp);
		}
	}
}

/*---------------------------------------------------------------------------------------
  �crit/lit dans un fichier
---------------------------------------------------------------------------------------*/
Bool MatrixNxP::writeToFile(OutStream &os)const
{
    os << rows << cols;

	for (UInt32 i = 0 ; i < rows*cols ; ++i)
        os << M[i];

	return True;
}

Bool MatrixNxP::readFromFile(InStream &is)
{
    is >> rows >> cols;
	reset(rows,cols);

	for (UInt32 i = 0 ; i < rows*cols ; ++i)
        is >> M[i];

	return True;
}

#undef ACCESS

