//---------------------------------------------------------------------------------------
//! @file main.cpp
//! Main entry of the O3DConfigFile test.
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

#include <o3d/core/ConfigFile.h>
#include <o3d/core/DiskFile.h>

#include <iostream>
#include <iomanip>
#include <algorithm>

#define CHECK_ASSERTION(X) \
	if (!(X)) std::cout << "Check failed <" << __FUNCTION__ << ":" << __LINE__ << "> : " << std::setw(50) << #X << std::endl;

#define LAUNCH_TEST(X) \
	{ \
	std::cout << #X << " : BEGIN" << std::endl; \
	X(); \
	std::cout << #X << " : END" << std::endl; \
	}


// Test des fonctions d'�tats d'un objet
void Test1()
{
	O3DConfigFile lFile;
	CHECK_ASSERTION(lFile.IsNull());

	lFile.SetFileName("un_fichier");
	CHECK_ASSERTION(lFile.GetFileName() == String("un_fichier"));
	CHECK_ASSERTION(lFile.HasFileName());
	CHECK_ASSERTION(lFile.IsEmpty());
	CHECK_ASSERTION(!lFile.IsNull());

	lFile.Destroy();
	CHECK_ASSERTION(lFile.GetFileName().IsEmpty());
	CHECK_ASSERTION(lFile.IsNull());

	CHECK_ASSERTION(lFile.GetGroup() == String());
	CHECK_ASSERTION(lFile.GetFullPath() == String("/"));
}

// Test des fonctions li�es � l'ajout/lecture/recherche/suppression des clefs
void Test2()
{
	O3DConfigFile lFile;

	CHECK_ASSERTION(!lFile.FindKey("test"));

	std::vector<String> lArray;
	CHECK_ASSERTION(!lFile.FindKey("test", lArray));

	CHECK_ASSERTION(!lFile.RemoveKey("test"));
	CHECK_ASSERTION(lFile.SetValue("testBOOL", O3D_FALSE));

	CHECK_ASSERTION(lFile.SetValue("testINT", 14));
	CHECK_ASSERTION(lFile.SetValue("testDOUBLE", 3.141592653));
	CHECK_ASSERTION(lFile.SetValue("testSTRING", String("une chaine")));

	CHECK_ASSERTION(lFile.GetValue("testBool", O3D_TRUE) == O3D_FALSE);
	CHECK_ASSERTION(lFile.GetValue("testINT", 10) == 14);
	CHECK_ASSERTION(lFile.GetValue("testDOUBLE", 4.2) == 3.141592653);
	CHECK_ASSERTION(lFile.GetValue("testSTRING", String("test")) == String("une chaine"));

	CHECK_ASSERTION(lFile.GetValue("TESTBOOL", O3D_TRUE) == O3D_FALSE);
	CHECK_ASSERTION(lFile.GetValue("TESTINT", 10) == 14);
	CHECK_ASSERTION(lFile.GetValue("TESTDOUBLE", 4.2) == 3.141592653);
	CHECK_ASSERTION(lFile.GetValue("TESTSTRING", String("test")) == String("une chaine"));

	CHECK_ASSERTION(lFile.GetValue("TESTBOOL2", O3D_TRUE) == O3D_TRUE);
	CHECK_ASSERTION(lFile.GetValue("TESTINT2", 10) == 10);
	CHECK_ASSERTION(lFile.GetValue("TESTDOUBLE2", 4.2) == 4.2);
	CHECK_ASSERTION(lFile.GetValue("TESTSTRING2", String("test")) == String("test"));

	CHECK_ASSERTION(lFile.RemoveKey("testBool"));
	CHECK_ASSERTION(lFile.RemoveKey("TESTINT"));

	CHECK_ASSERTION(!lFile.RemoveKey("TESTINT2"));
	CHECK_ASSERTION(!lFile.RemoveKey("TESTSTRING2"));
}

// Test des fonctions ajout/suppression/recherche et d�placement dans les groupes
void Test3()
{
	O3DConfigFile lFile;

	CHECK_ASSERTION(!lFile.FindGroup("FG"));
	CHECK_ASSERTION(lFile.InsertGroup("/fG"));
	CHECK_ASSERTION(lFile.FindGroup("FG"));

	CHECK_ASSERTION(lFile.InsertGroup("sG"));
	CHECK_ASSERTION(lFile.FindGroup("sG"));
	CHECK_ASSERTION(lFile.RemoveGroup("sG"));
	CHECK_ASSERTION(lFile.InsertGroup("sG"));

	CHECK_ASSERTION(!lFile.FindGroup("/fG/subG"));
	CHECK_ASSERTION(lFile.InsertGroup("/fG/subG"));
	CHECK_ASSERTION(lFile.FindGroup("/fG/subG"));
	CHECK_ASSERTION(!lFile.FindGroup("subG"));

	CHECK_ASSERTION(!lFile.FindGroup("/sG/subG2"));
	CHECK_ASSERTION(lFile.InsertGroup("/sG/subG2"));
	CHECK_ASSERTION(lFile.FindGroup("/sG/subG2"));
	CHECK_ASSERTION(!lFile.FindGroup("subG2"));

	CHECK_ASSERTION(lFile.Enter("fG"));
	CHECK_ASSERTION(lFile.GetGroup().Compare(String("fG"), String::CASE_INSENSITIVE) == 0);
	CHECK_ASSERTION(lFile.GetFullPath().Compare(String("/fG/"), String::CASE_INSENSITIVE) == 0);
	CHECK_ASSERTION(lFile.FindGroup("subG"));
	lFile.Leave();
	CHECK_ASSERTION(lFile.GetGroup().IsEmpty());
	CHECK_ASSERTION(lFile.GetFullPath() == String("/"));

	CHECK_ASSERTION(lFile.Enter("fG/subG"));
	CHECK_ASSERTION(lFile.GetGroup().Compare(String("subG"), String::CASE_INSENSITIVE) == 0);
	CHECK_ASSERTION(lFile.GetFullPath().Compare(String("/fG/subG/"), String::CASE_INSENSITIVE) == 0);

	lFile.ToRoot();
	CHECK_ASSERTION(lFile.GetGroup().IsEmpty());
	CHECK_ASSERTION(lFile.GetFullPath() == String("/"));

	CHECK_ASSERTION(lFile.Enter("sg/subG2"));
	lFile.ToRoot();
	CHECK_ASSERTION(lFile.Enter("/sg/subG2/"));
	lFile.ToRoot();
	CHECK_ASSERTION(lFile.Enter("sg//subG2"));
	lFile.ToRoot();
	CHECK_ASSERTION(lFile.Enter("sg/   /subG2"));
	lFile.ToRoot();
}

// Test des fonctions de recherche de clefs
void Test4()
{
	O3DConfigFile lFile;

	std::vector<String> lArray;

	CHECK_ASSERTION(!lFile.FindKey("key"));
	CHECK_ASSERTION(!lFile.FindKey("key", lArray));
	CHECK_ASSERTION(lFile.SetValue("key", O3D_FALSE));
	CHECK_ASSERTION(lFile.FindKey("key"));
	CHECK_ASSERTION(lFile.FindKey("key", lArray));
	CHECK_ASSERTION(lArray.size() == 1);
	if (lArray.size() == 1)
		CHECK_ASSERTION(lArray.front() == String(""));

	CHECK_ASSERTION(lFile.InsertGroup("my_group"));
	CHECK_ASSERTION(lFile.InsertGroup("my_group/sub_group"));

	CHECK_ASSERTION(lFile.Enter("my_group"));
	CHECK_ASSERTION(lFile.SetValue("key", O3D_FALSE));

	CHECK_ASSERTION(lFile.FindKey("key", lArray));
	CHECK_ASSERTION(lArray.size() == 1);
	if (lArray.size() == 1)
		CHECK_ASSERTION(lArray.front() == String(""));

	lFile.ToRoot();
	CHECK_ASSERTION(lFile.Enter("my_group/sub_group"));
	CHECK_ASSERTION(lFile.SetValue("key", 14));

	lFile.Leave(1);
	CHECK_ASSERTION(lFile.FindKey("key", lArray));
	CHECK_ASSERTION(lArray.size() == 2);
	if (lArray.size() == 2)
	{
		CHECK_ASSERTION(std::find(lArray.begin(), lArray.end(), String("sub_group/")) != lArray.end());
		CHECK_ASSERTION(std::find(lArray.begin(), lArray.end(), String("")) != lArray.end());
	}

	lFile.Leave(1);
	CHECK_ASSERTION(lFile.FindKey("key", lArray));
	CHECK_ASSERTION(lArray.size() == 3);
	if (lArray.size() == 3)
	{
		CHECK_ASSERTION(std::find(lArray.begin(), lArray.end(), String("")) != lArray.end());
		CHECK_ASSERTION(std::find(lArray.begin(), lArray.end(), String("my_group/")) != lArray.end());
		CHECK_ASSERTION(std::find(lArray.begin(), lArray.end(), String("my_group/sub_group/")) != lArray.end());
	}
}

void Test5()
{
	{
		O3DConfigFile lFile;
		DiskFile lSrcFile("Test5_File0.txt", "rt");

		CHECK_ASSERTION(lFile.readFromFile(lSrcFile));
		CHECK_ASSERTION(lFile.FindKey("gk1"));
		CHECK_ASSERTION(lFile.FindKey("0keybool"));
		CHECK_ASSERTION(!lFile.FindKey("keyBool1"));
		CHECK_ASSERTION(!lFile.FindKey("keyBOOL0"));

		O3D_BOOL lConversion;
		CHECK_ASSERTION(lFile.GetValue("gk1", 10, &lConversion) == 14);
		CHECK_ASSERTION(lConversion);

		CHECK_ASSERTION(lFile.GetValue("gk1", O3D_TRUE, &lConversion) == O3D_TRUE);
		CHECK_ASSERTION(!lConversion);
		CHECK_ASSERTION(lFile.GetValue("gk1", String("coucou"), &lConversion) == String("coucou"));
		CHECK_ASSERTION(!lConversion);

		CHECK_ASSERTION(lFile.GetValue("0KEYBOOL", O3D_FALSE, &lConversion) == O3D_TRUE);
		CHECK_ASSERTION(lConversion);

		CHECK_ASSERTION(lFile.FindGroup("G1"));

		std::vector<String> lArray;
		CHECK_ASSERTION(lFile.FindKey("keyBool1", lArray));
		CHECK_ASSERTION(lArray.size() == 1);
		CHECK_ASSERTION(lArray.front().Compare(String("G1/"), String::CASE_INSENSITIVE) == 0);

		CHECK_ASSERTION(lFile.Enter("G1"));
		CHECK_ASSERTION(lFile.GetValue("keyBool1", O3D_FALSE, &lConversion) == O3D_TRUE);
		CHECK_ASSERTION(lConversion);
		CHECK_ASSERTION(lFile.GetValue("keyBool0", O3D_TRUE, &lConversion) == O3D_FALSE);
		CHECK_ASSERTION(lConversion);
		CHECK_ASSERTION(lFile.GetValue("keyInt", 10, &lConversion) == 14);
		CHECK_ASSERTION(lConversion);
		CHECK_ASSERTION(lFile.GetValue("keyString", String("test"), &lConversion).Compare(String("une chaine"), String::CASE_INSENSITIVE) == 0);
		CHECK_ASSERTION(lConversion);
	}
	{
		O3DConfigFile lFile;
		DiskFile lSrcFile("Test5_File1.txt", "rt");

		CHECK_ASSERTION(lFile.readFromFile(lSrcFile));

		O3D_BOOL lConversion;
		CHECK_ASSERTION(lFile.GetValue("unbool", O3D_FALSE, &lConversion) == O3D_TRUE);
		CHECK_ASSERTION(lConversion);

		CHECK_ASSERTION(lFile.GetValue("chaine1", String("vide"), &lConversion).Compare(String("coucou #\" ca va #faux commentaire"), String::CASE_INSENSITIVE) == 0);
		CHECK_ASSERTION(lConversion);

		CHECK_ASSERTION(lFile.GetValue("chaine2", String("vide"), &lConversion).Compare(String("#\" ca va #faux commentaire"), String::CASE_INSENSITIVE) == 0);
		CHECK_ASSERTION(lConversion);

		CHECK_ASSERTION(!lFile.FindKey("chaine3"));
	}
}

// Test d'�criture
void Test6()
{
	O3DConfigFile lFile;
	DiskFile lOutFile("Test6_out0.txt", "wt");

	CHECK_ASSERTION(lFile.InsertGroup("test"));
	CHECK_ASSERTION(lFile.InsertGroup("groupe2"));
	CHECK_ASSERTION(lFile.InsertGroup("groupe2/sous_groupe de 2"));
	CHECK_ASSERTION(lFile.SetValue("clef 1", O3D_TRUE));
	CHECK_ASSERTION(lFile.SetValue("clef 2", 14));
	CHECK_ASSERTION(lFile.SetValue("clef 2", String("changement de type")));
	CHECK_ASSERTION(lFile.SetValue("clef 3", 3.14159));
	CHECK_ASSERTION(lFile.SetValue("clef 4", String("la quatri�me")));
	CHECK_ASSERTION(lFile.Enter("groupe2"));
	CHECK_ASSERTION(lFile.SetValue("clef 1", O3D_TRUE));
	CHECK_ASSERTION(lFile.SetValue("clef 2", 14));
	CHECK_ASSERTION(lFile.SetValue("clef 2", String("re changement de type")));
	CHECK_ASSERTION(lFile.SetValue("clef 3", 3.14159));
	CHECK_ASSERTION(lFile.SetValue("clef 4", String("la quatri�me")));
	CHECK_ASSERTION(lFile.writeToFile(lOutFile));
}

void Test7()
{
	O3DConfigFile lFile;
	DiskFile lSrcFile("Test5_File1.txt", "rt");
	DiskFile lOutFile("Test7_File1_cpy.txt", "wt");

	CHECK_ASSERTION(lFile.readFromFile(lSrcFile));
	CHECK_ASSERTION(lFile.writeToFile(lOutFile));
	lOutFile.Close();

	// Analyse du fichier de sortie
	DiskFile lTestFile("Test7_File1_cpy.txt", "rt");
	CHECK_ASSERTION(lFile.readFromFile(lTestFile));

	CHECK_ASSERTION(lSrcFile.FileLength() == lTestFile.FileLength());
}

int main(int argc, char * argv[])
{
	LAUNCH_TEST(Test1);
	LAUNCH_TEST(Test2);
	LAUNCH_TEST(Test3);
	LAUNCH_TEST(Test4);
	LAUNCH_TEST(Test5);
	LAUNCH_TEST(Test6);
	LAUNCH_TEST(Test7);
	
	system("PAUSE");

	return 0;
}
