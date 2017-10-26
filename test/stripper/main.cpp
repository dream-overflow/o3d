//---------------------------------------------------------------------------------------
//! @file main.cpp
//! Main entry of the stripper test.
//! @date 2004-01-01
//! @author Frederic SCHERMA
//-------------------------------------------------------------------------------------
//- Objective-3D Copyright (C) 2001-2009 Revolutioning Entertainment
//- mailto:contact@revolutioning.com
//- http://o3d.revolutioning.com
//-------------------------------------------------------------------------------------
// This file is part of Objective-3D.
// Objective-3D is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Objective-3D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Objective-3D.  If not, see <http://www.gnu.org/licenses/>.
//---------------------------------------------------------------------------------------

#include <o3d/engine/Misc/Stripper.h>

#include <iostream>
#include <iomanip>

const char * EnumString[] = { "STRIPPER_AUTO", "STRIPPER_INVALID", "STRIPPER_NVTRISTRIP", "STRIPPER_TRISTRIPPER" };

int main(int argc, char * argv[])
{
	O3D_USHORT UShortIndexBuffer[12] = { 0, 2, 1, 2, 3, 1, 2, 4, 3, 4, 5, 3 }; 
	O3D_UINT32 UIntIndexBuffer[12] = { 0, 2, 1, 2, 3, 1, 2, 4, 3, 4, 5, 3 }; 

	O3DStripper::O3D_T_UShortIndex UShortIndex;
	UShortIndex.assign(UShortIndexBuffer, UShortIndexBuffer + 12);

	O3DStripper::O3D_T_UIntIndex UIntIndex;
	UIntIndex.assign(UIntIndexBuffer, UIntIndexBuffer + 12);

	std::cout << "Creation d'un maillage �l�mentaire :" << std::endl;
	std::cout << "Faces : 4" << std::endl;
	std::cout << "Vertices : 12" << std::endl;
	std::cout << std::endl;

	for (O3D_UINT32 m = 0 ; m < UShortIndex.size()/3 ; ++m)
		std::cout << "(" << UShortIndexBuffer[3*m+0] << "," << UShortIndexBuffer[3*m+1] << "," << UShortIndexBuffer[3*m+2] << ")  " << std::flush;

#pragma

	std::cout << std::endl;
	std::cout << std::endl;

	{
		std::cout << "========== TEST DU STRIPPER NVTRISTRIP ===========" << std::endl;
		O3DStripper nvStripper(O3DStripper::STRIPPER_NVTRISTRIP);

		O3DStripper::O3D_T_UShortListArray UShortResult;

		std::cout << std::endl;
		std::cout << "----- Generation d'un maillage OPTIMIZED_TRIANGLES ------" << std::endl;
		nvStripper.SetUseCache(O3D_TRUE);
		nvStripper.SetCacheSize(16); /* Default */

		std::cout << "UseCache = " << std::boolalpha << nvStripper.GetUseCache() << std::endl;
		std::cout << "CacheSize = " << O3D_UINT32(nvStripper.GetCacheSize()) << std::endl;

		UShortResult.clear();
		if (!nvStripper.start(UShortIndex, O3DStripper::RESULT_OPTIMIZED_TRIANGLES, UShortResult))
		{
			std::cout << "Stripper Test : " << std::setw(56) << "FAILED" << std::endl;
			std::cout << "Error msg : " << nvStripper.ErrorString().GetData() << std::endl;
		}
		else
		{
			std::cout << "Stripper Test : " << std::setw(56) << "OK" << std::endl;
			std::cout << "Resultats : " << std::endl;
			std::cout << "Nombre de listes : " << UShortResult.size() << std::endl;

			O3D_UINT32 counter = 0;
			for (O3D_UINT32 k = 0 ; k < UShortResult.size() ; ++k)
			{
				std::cout << "--------- Liste " << std::setw(3) << k << " : " << std::setw(5) << UShortResult[k].size()/3 << " faces ---------" << std::endl;

				for (O3D_UINT32 m = 0 ; m < UShortResult[k].size()/3 ; ++m)
					std::cout << "(" << UShortResult[k][3*m+0] << "," << UShortResult[k][3*m+1] << "," << UShortResult[k][3*m+2] << ")  " << std::flush;

				counter += UShortResult[k].size()/3;
			}

			std::cout << "\nNombre de triangles total : " << counter << "\n" << std::endl;
		}

		std::cout << std::endl;
		std::cout << "----- Generation d'un maillage SINGLE_STRIP ------" << std::endl;
		nvStripper.SetUseCache(O3D_TRUE);
		nvStripper.SetCacheSize(16); /* Default */

		std::cout << "UseCache = " << nvStripper.GetUseCache() << std::endl;
		std::cout << "CacheSize = " << O3D_UINT32(nvStripper.GetCacheSize()) << std::endl;

		UShortResult.clear();
		if (!nvStripper.start(UShortIndex, O3DStripper::RESULT_SINGLE_TRIANGLE_STRIP, UShortResult))
		{
			std::cout << "Stripper Test : " << std::setw(56) << "FAILED" << std::endl;
			std::cout << "Error msg : " << nvStripper.ErrorString().GetData() << std::endl;
		}
		else
		{
			if (UShortResult.size() > 1)
			{
				std::cout << "Stripper Test : " << std::setw(56) << "FAILED" << std::endl;
				std::cout << "Le result contient plus d'une liste" << std::endl;
			}
			else
			{
				std::cout << "Stripper Test : " << std::setw(56) << "OK" << std::endl;
				std::cout << "Resultats : " << std::endl;
				std::cout << "Nombre de listes : " << UShortResult.size() << std::endl;

				O3D_UINT32 counter = 0;
				for (O3D_UINT32 k = 0 ; k < UShortResult.size() ; ++k)
				{
					std::cout << "--------- Liste " << std::setw(3) << k << " : " << std::setw(5) << UShortResult[k].size() << " indices ---------" << std::endl;

					for (O3D_UINT32 m = 0 ; m < UShortResult[k].size() ; ++m)
						std::cout << UShortResult[k][m] << ", " << std::flush;

					counter += UShortResult[k].size();
				}

				std::cout << "\nNombre d'indices total : " << counter << "\n" << std::endl;
			}
		}

		std::cout << std::endl;
		std::cout << "----- Generation d'un maillage TRIANGLE_STRIPS ------" << std::endl;
		nvStripper.SetUseCache(O3D_TRUE);
		nvStripper.SetCacheSize(16); /* Default */

		std::cout << "UseCache = " << nvStripper.GetUseCache() << std::endl;
		std::cout << "CacheSize = " << O3D_UINT32(nvStripper.GetCacheSize()) << std::endl;

		UShortResult.clear();
		if (!nvStripper.start(UShortIndex, O3DStripper::RESULT_TRIANGLE_STRIPS, UShortResult))
		{
			std::cout << "Stripper Test : " << std::setw(56) << "FAILED" << std::endl;
			std::cout << "Error msg : " << nvStripper.ErrorString().GetData() << std::endl;
		}
		else
		{
			std::cout << "Stripper Test : " << std::setw(56) << "OK" << std::endl;
			std::cout << "Resultats : " << std::endl;
			std::cout << "Nombre de listes : " << UShortResult.size() << std::endl;

			O3D_UINT32 counter = 0;
			for (O3D_UINT32 k = 0 ; k < UShortResult.size() ; ++k)
			{
				std::cout << "--------- Liste " << std::setw(3) << k << " : " << std::setw(5) << UShortResult[k].size() << " indices ---------" << std::endl;

				for (O3D_UINT32 m = 0 ; m < UShortResult[k].size() ; ++m)
					std::cout << UShortResult[k][m] << ", " << std::flush;

				counter += UShortResult[k].size();
			}

			std::cout << "\nNombre d'indices total : " << counter << "\n" << std::endl;
		}
	}

	{
		std::cout << "========== TEST DU STRIPPER TRI STRIPPER (USHORT) ===========" << std::endl;
		O3DStripper triStripper(O3DStripper::STRIPPER_TRISTRIPPER);

		std::cout << "----- Generation d'un maillage SINGLE_STRIP -----" << std::endl;

		triStripper.SetUseCache(O3D_TRUE);
		triStripper.SetCacheSize(16);
		std::cout << "UseCache = " << triStripper.GetUseCache() << std::endl;
		std::cout << "CacheSize = " << O3D_UINT32(triStripper.GetCacheSize()) << std::endl;

		O3DStripper::O3D_T_UShortListArray UShortResult;

		UShortResult.clear();
		if (!triStripper.start(UShortIndex, O3DStripper::RESULT_SINGLE_TRIANGLE_STRIP, UShortResult))
		{
			std::cout << "Stripper Test : " << std::setw(56) << "FAILED" << std::endl;
			std::cout << "Error msg : " << triStripper.ErrorString().GetData() << std::endl;
		}
		else
		{
			std::cout << "Stripper Test : " << std::setw(56) << "OK" << std::endl;
			std::cout << "Resultats : " << std::endl;
			std::cout << "Nombre de listes : " << UShortResult.size() << std::endl;

			O3D_UINT32 counter = 0;
			for (O3D_UINT32 k = 0 ; k < UShortResult.size() ; ++k)
			{
				std::cout << "--------- Liste " << std::setw(3) << k << " : " << std::setw(5) << UShortResult[k].size() << " indices ---------" << std::endl;

				for (O3D_UINT32 m = 0 ; m < UShortResult[k].size() ; ++m)
					std::cout << UShortResult[k][m] << ",  " << std::flush;

				counter += UShortResult[k].size();
			}

			std::cout << "\nNombre d'indices total : " << counter << "\n" << std::endl;
		}

		std::cout << "----- Generation d'un maillage TRIANGLE_STRIPS -----" << std::endl;
		triStripper.SetUseCache(O3D_TRUE);
		triStripper.SetCacheSize(16);
		std::cout << "UseCache = " << triStripper.GetUseCache() << std::endl;
		std::cout << "CacheSize = " << O3D_UINT32(triStripper.GetCacheSize()) << std::endl;

		UShortResult.clear();
		if (!triStripper.start(UShortIndex, O3DStripper::RESULT_TRIANGLE_STRIPS, UShortResult))
		{
			std::cout << "Stripper Test : " << std::setw(56) << "FAILED" << std::endl;
			std::cout << "Error msg : " << triStripper.ErrorString().GetData() << std::endl;
		}
		else
		{
			std::cout << "Stripper Test : " << std::setw(56) << "OK" << std::endl;
			std::cout << "Resultats : " << std::endl;
			std::cout << "Nombre de listes : " << UShortResult.size() << std::endl;

			O3D_UINT32 counter = 0;
			for (O3D_UINT32 k = 0 ; k < UShortResult.size() ; ++k)
			{
				if (k > 0)
					std::cout << std::endl;

				std::cout << "--------- Liste " << std::setw(3) << k << " : " << std::setw(5) << UShortResult[k].size()/3 << " faces ---------" << std::endl;

				for (O3D_UINT32 m = 0 ; m < UShortResult[k].size() ; ++m)
					std::cout << UShortResult[k][m] << ",  " << std::flush;

				counter += UShortResult[k].size();
			}

			std::cout << "\nNombre d'indices total : " << counter << "\n" << std::endl;
		}
	}

	{
		std::cout << std::endl;
		std::cout << "========== TEST DU STRIPPER TRI STRIPPER (UINT) ===========" << std::endl;
		std::cout << std::endl;
		O3DStripper triStripper(O3DStripper::STRIPPER_TRISTRIPPER);

		O3DStripper::O3D_T_UIntListArray UIntResult;

		std::cout << "----- Generation d'un maillage SINGLE_STRIP -----" << std::endl;

		triStripper.SetUseCache(O3D_TRUE);
		triStripper.SetCacheSize(16);
		std::cout << "UseCache = " << triStripper.GetUseCache() << std::endl;
		std::cout << "CacheSize = " << triStripper.GetCacheSize() << std::endl;

		UIntResult.clear();
		if (!triStripper.start(UIntIndex, O3DStripper::RESULT_SINGLE_TRIANGLE_STRIP, UIntResult))
		{
			std::cout << "Stripper Test : " << std::setw(56) << "FAILED" << std::endl;
			std::cout << "Error msg : " << triStripper.ErrorString().GetData() << std::endl;
		}
		else
		{
			std::cout << "Stripper Test : " << std::setw(56) << "OK" << std::endl;
			std::cout << "Resultats : " << std::endl;
			std::cout << "Nombre de listes : " << UIntResult.size() << std::endl;

			O3D_UINT32 counter = 0;
			for (O3D_UINT32 k = 0 ; k < UIntResult.size() ; ++k)
			{
				if (k > 0)
					std::cout << std::endl;

				std::cout << "--------- Liste " << std::setw(3) << k << " : " << std::setw(5) << UIntResult[k].size() << " indices ---------" << std::endl;

				for (O3D_UINT32 m = 0 ; m < UIntResult[k].size() ; ++m)
					std::cout << UIntResult[k][m] << ",  " << std::flush;

				counter += UIntResult[k].size();
			}

			std::cout << "\nNombre d'indices total : " << counter << "\n" << std::endl;
		}

		std::cout << std::endl;
		std::cout << "----- Generation d'un maillage TRIANGLE_STRIPS -----" << std::endl;
	
		triStripper.SetUseCache(O3D_TRUE);
		triStripper.SetCacheSize(16);
		std::cout << "UseCache = " << triStripper.GetUseCache() << std::endl;
		std::cout << "CacheSize = " << triStripper.GetCacheSize() << std::endl;

		UIntResult.clear();
		if (!triStripper.start(UIntIndex, O3DStripper::RESULT_TRIANGLE_STRIPS, UIntResult))
		{
			std::cout << "Stripper Test : " << std::setw(56) << "FAILED" << std::endl;
			std::cout << "Error msg : " << triStripper.ErrorString().GetData() << std::endl;
		}
		else
		{
			std::cout << "Stripper Test : " << std::setw(56) << "OK" << std::endl;
			std::cout << "Resultats : " << std::endl;
			std::cout << "Nombre de listes : " << UIntResult.size() << std::endl;

			O3D_UINT32 counter = 0;
			for (O3D_UINT32 k = 0 ; k < UIntResult.size() ; ++k)
			{
				if (k > 0)
					std::cout << std::endl;

				std::cout << "--------- Liste " << std::setw(3) << k << " : " << std::setw(5) << UIntResult[k].size() << " indices ---------" << std::endl;

				for (O3D_UINT32 m = 0 ; m < UIntResult[k].size() ; ++m)
					std::cout << UIntResult[k][m] << ",  " << std::flush;

				counter += UIntResult[k].size();
			}

			std::cout << "\nNombre d'indices total : " << counter << "\n" << std::endl;
		}
	}

	{
		std::cout << "========== TEST DU STRIPPER AUTO ===========" << std::endl;
		O3DStripper autoStripper(O3DStripper::STRIPPER_AUTO);

		std::cout << "----- AVEC LES PARAMETRES SUIVANTS -----" << std::endl;
		autoStripper.SetUseCache(O3D_TRUE);
		autoStripper.SetCacheSize(16);

		std::cout << "UseCache = " << autoStripper.GetUseCache() << std::endl;
		std::cout << "CacheSize = " << O3D_UINT32(autoStripper.GetCacheSize()) << std::endl;

		std::cout << "OPTIMIZED_TRIANGLES    : " << EnumString[autoStripper.GetStripper(O3DStripper::RESULT_OPTIMIZED_TRIANGLES)] <<  std::endl;
		std::cout << std::setw(56) << (autoStripper.GetStripper(O3DStripper::RESULT_OPTIMIZED_TRIANGLES) == O3DStripper::STRIPPER_NVTRISTRIP ? "OK": "FAILED") << std::endl;
		std::cout << "SINGLE_TRIANGLES_STRIP : " << EnumString[autoStripper.GetStripper(O3DStripper::RESULT_SINGLE_TRIANGLE_STRIP)] <<  std::endl;
		std::cout << std::setw(56) << (autoStripper.GetStripper(O3DStripper::RESULT_SINGLE_TRIANGLE_STRIP) == O3DStripper::STRIPPER_TRISTRIPPER ? "OK": "FAILED") << std::endl;
		std::cout << "TRIANGLE_STRIPS        : " << EnumString[autoStripper.GetStripper(O3DStripper::RESULT_TRIANGLE_STRIPS)] <<  std::endl;
		std::cout << std::setw(56) << (autoStripper.GetStripper(O3DStripper::RESULT_TRIANGLE_STRIPS) == O3DStripper::STRIPPER_TRISTRIPPER ? "OK": "FAILED") << std::endl;

		std::cout << "----- AVEC LES PARAMETRES SUIVANTS -----" << std::endl;
		autoStripper.SetUseCache(O3D_FALSE);

		std::cout << "UseCache = " << autoStripper.GetUseCache() << std::endl;

		std::cout << "OPTIMIZED_TRIANGLES    : " << EnumString[autoStripper.GetStripper(O3DStripper::RESULT_OPTIMIZED_TRIANGLES)] <<  std::endl;
		std::cout << std::setw(56) << (autoStripper.GetStripper(O3DStripper::RESULT_OPTIMIZED_TRIANGLES) == O3DStripper::STRIPPER_INVALID ? "OK": "FAILED") << std::endl;
		std::cout << "SINGLE_TRIANGLES_STRIP : " << EnumString[autoStripper.GetStripper(O3DStripper::RESULT_SINGLE_TRIANGLE_STRIP)] <<  std::endl;
		std::cout << std::setw(56) << (autoStripper.GetStripper(O3DStripper::RESULT_SINGLE_TRIANGLE_STRIP) == O3DStripper::STRIPPER_TRISTRIPPER ? "OK": "FAILED") << std::endl;
		std::cout << "TRIANGLE_STRIPS        : " << EnumString[autoStripper.GetStripper(O3DStripper::RESULT_TRIANGLE_STRIPS)] <<  std::endl;
		std::cout << std::setw(56) << (autoStripper.GetStripper(O3DStripper::RESULT_TRIANGLE_STRIPS) == O3DStripper::STRIPPER_TRISTRIPPER ? "OK": "FAILED") << std::endl;

	}
	system("PAUSE");

	return 0;
}
