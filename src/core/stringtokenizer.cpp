/**
 * @file stringtokenizer.cpp
 * @brief Implementation of StringTokenizer.h
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2006-10-22
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"

#include "o3d/core/templatearray.h"
#include "o3d/core/stringtokenizer.h"

#include <ctype.h>
#include <string.h>

using namespace o3d;

//---------------------------------------------------------------------------------------
// O3DCStringTokenizer
//---------------------------------------------------------------------------------------
CStringTokenizer::CStringTokenizer(const CString &str)
{
	m_currentPosition = m_totalTokens = m_remainingTokens = 0;
	m_dlFlag = False;

	if (str.isEmpty())
	{
		m_origStr = NULL;
		m_workStr = NULL;
		m_listOfDl = NULL;
		return;
	}

	m_origStr = o3d::strdup(str.getData());
	m_workStr = o3d::strdup(str.getData());

	O3D_ASSERT(m_origStr != NULL);
	O3D_ASSERT(m_workStr != NULL);

	prepWorkStr();
}

CStringTokenizer::CStringTokenizer(const CString &str, const CString &delimiters)
{
	m_currentPosition = m_totalTokens = m_remainingTokens = 0;
	m_dlFlag = False;

	if (str.isEmpty())
	{
		m_origStr = NULL;
		m_workStr = NULL;
		m_listOfDl = NULL;
		return;
	}

	m_origStr = o3d::strdup(str.getData());
	m_workStr = o3d::strdup(str.getData());

	O3D_ASSERT(m_origStr != NULL);
	O3D_ASSERT(m_workStr != NULL);

	prepWorkStr(delimiters.getData(), delimiters.length());
}

CStringTokenizer::CStringTokenizer(
	const CString &str,
	const CString &delimiters,
	Bool DelimiterAsToken)
{
	m_currentPosition = m_totalTokens = m_remainingTokens = 0;
	m_dlFlag = DelimiterAsToken;

	if (str.isEmpty())
	{
		m_origStr = NULL;
		m_workStr = NULL;
		m_listOfDl = NULL;
		return;
	}

	m_origStr = o3d::strdup(str.getData());
	m_workStr = o3d::strdup(str.getData());

	O3D_ASSERT(m_origStr != NULL);
	O3D_ASSERT(m_workStr != NULL);

	prepWorkStr(delimiters.getData());
}

// destructor
CStringTokenizer::~CStringTokenizer()
{
	if (m_origStr)
	{
		O3D_FREE(m_origStr);
		m_origStr = NULL;
	}

	if (m_workStr)
	{
		O3D_FREE(m_workStr);
		m_workStr = NULL;
	}

	if (m_listOfDl)
	{
		O3D_FREE(m_listOfDl);
		m_listOfDl = NULL;
	}
}

// Returns the next token in the string
CString CStringTokenizer::nextToken()
{
	O3D_ASSERT(m_remainingTokens != 0);

	m_whiteToken = 0;

	// contiguous tokens
	while (m_workStr[m_currentPosition] == 0)
	{
		m_currentPosition++;
		m_whiteToken++;
	}

	Int32 len = (Int32)strlen(&m_workStr[m_currentPosition]);
	ArrayChar tmpstr(&m_workStr[m_currentPosition], len, 16);

    m_currentPosition = m_currentPosition + len + 1;

	// m_DlFlag indicates whether delimiters should be returned as tokens
	if (m_dlFlag)
	{
		Char e;
		e = m_listOfDl[m_totalTokens - m_remainingTokens];
		tmpstr.push(e);
	}

	m_remainingTokens--;

	tmpstr.push('0');
	return CString(tmpstr.getData());
}

// Changes the set of delimiters to the specified string
CString CStringTokenizer::nextToken(const CString &delimiters)
{
#ifdef _MSC_VER
	strcpy_s(m_workStr, strlen(&m_origStr[m_currentPosition])+1, &m_origStr[m_currentPosition]);
#else
	strcpy(m_workStr, &m_origStr[m_currentPosition]);
#endif

	prepWorkStr(delimiters.getData(), delimiters.length());

	return nextToken();
}

void CStringTokenizer::prepWorkStr(const Char* delimiters, Int32 delimiterSize)
{
	Bool last = False;
	Int32 contigous = 0;

	if (!m_workStr)
	{
		m_listOfDl = NULL;
		O3D_ERROR(E_InvalidPrecondition("string must be valid"));
	}

	m_currentPosition = m_totalTokens = m_remainingTokens = 0;

	// Now, tokenize the temporary work string variable. Put 0's to tokenize replacing the delimiter character
	if (!delimiters)
	{
		for (size_t i = 0, j = strlen(m_workStr) ; i < j ; i++)
		{
			if (isspace(m_workStr[i])) // is a white space
			{
				m_workStr[i] = 0;
				m_totalTokens++;
			}
		}
		m_listOfDl = NULL;
	}
	else
	{
		m_listOfDl = (Char*)O3D_MALLOC((strlen(m_workStr)+1) * sizeof(Char));
		memset(m_listOfDl, 0, (strlen(m_workStr)+1) * sizeof(Char));

		for (size_t i = 0, j = strlen(m_workStr); i < j; i++)
		{
			last = False;
			for (size_t m = 0, n = delimiterSize; m < n; m++)
			{
				if (m_workStr[i] == delimiters[m])
				{
					m_workStr[i] = 0;
					m_listOfDl[m_totalTokens] = delimiters[m];
					m_totalTokens++;

					if ((i > 0) && (m_workStr[i-1] == 0))
						contigous++;
                    else if (i == 0) // if string start by a token
                        contigous++;

					last = True;
					break;
				}
			}
		}
	}

	m_totalTokens++;
	m_totalTokens -= contigous;

	if (last)
		m_totalTokens--;

	m_remainingTokens = m_totalTokens;
}

//---------------------------------------------------------------------------------------
// StringTokenizer
//---------------------------------------------------------------------------------------
StringTokenizer::StringTokenizer(const String &str)
{
	m_currentPosition = m_totalTokens = m_remainingTokens = 0;
	m_dlFlag = False;

	if (str.isEmpty())
	{
		m_origStr = NULL;
		m_workStr = NULL;
		m_listOfDl = NULL;
		return;
	}

	m_origStr = o3d::wcsdup(str.getData());
	m_workStr = o3d::wcsdup(str.getData());

	O3D_ASSERT(m_origStr != NULL);
	O3D_ASSERT(m_workStr != NULL);

	prepWorkStr();
}

StringTokenizer::StringTokenizer(const String &str, const String &delimiters)
{
	m_currentPosition = m_totalTokens = m_remainingTokens = 0;
	m_dlFlag = False;

	if (str.isEmpty())
	{
		m_origStr = NULL;
		m_workStr = NULL;
		m_listOfDl = NULL;
		return;
	}

	m_origStr = o3d::wcsdup(str.getData());
	m_workStr = o3d::wcsdup(str.getData());

	O3D_ASSERT(m_origStr != NULL);
	O3D_ASSERT(m_workStr != NULL);

	prepWorkStr(delimiters.getData(), delimiters.length());
}

StringTokenizer::StringTokenizer(
	const String &str,
	const String &delimiters,
	Bool DelimiterAsToken)
{
	m_currentPosition = m_totalTokens = m_remainingTokens = 0;
	m_dlFlag = DelimiterAsToken;

	if (str.isEmpty())
	{
		m_origStr = NULL;
		m_workStr = NULL;
		m_listOfDl = NULL;
		return;
	}

	m_origStr = o3d::wcsdup(str.getData());
	m_workStr = o3d::wcsdup(str.getData());

	O3D_ASSERT(m_origStr != NULL);
	O3D_ASSERT(m_workStr != NULL);

	prepWorkStr(delimiters.getData(), delimiters.length());
}

// destructor
StringTokenizer::~StringTokenizer()
{
	if (m_origStr)
	{
		O3D_FREE(m_origStr);
		m_origStr = NULL;
	}

	if (m_workStr)
	{
		O3D_FREE(m_workStr);
		m_workStr = NULL;
	}

	if (m_listOfDl)
	{
		O3D_FREE(m_listOfDl);
		m_listOfDl = NULL;
	}
}

// Returns the next token in the string
String StringTokenizer::nextToken()
{
	O3D_ASSERT(m_remainingTokens != 0);

	m_whiteToken = 0;

	// contiguous tokens
	while (m_workStr[m_currentPosition] == 0)
	{
		m_currentPosition++;
		m_whiteToken++;
	}

	String tmpstr(&m_workStr[m_currentPosition]);

    m_currentPosition = m_currentPosition + (Int32)wcslen(&m_workStr[m_currentPosition]) + 1;

	// m_DlFlag indicates whether delimiters should be returned as tokens
	if (m_dlFlag)
	{
		WChar e[2];
		e[0] = m_listOfDl[m_totalTokens - m_remainingTokens];
		e[1] = 0;
		tmpstr += e;
	}

	m_remainingTokens--;
	return tmpstr;
}

// Changes the set of delimiters to the specified string
String StringTokenizer::nextToken(const String &delimiters)
{
#ifdef _MSC_VER
	wcscpy_s(m_workStr, wcslen(&m_origStr[m_currentPosition])+1, &m_origStr[m_currentPosition]);
#else
	wcscpy(m_workStr, &m_origStr[m_currentPosition]);
#endif

	prepWorkStr(delimiters.getData(), delimiters.length());

	return nextToken();
}

void StringTokenizer::prepWorkStr(const WChar* delimiters, Int32 delimiterSize)
{
	Bool last = False;
	Int32 contigous = 0;

	if (!m_workStr)
	{
        m_listOfDl = nullptr;
		O3D_ERROR(E_InvalidPrecondition("string must be valid"));
	}

	m_currentPosition = m_totalTokens = m_remainingTokens = 0;

	// Now, tokenize the temporary work string variable. Put 0's to tokenize replacing the delimiter character
	if (!delimiters)
	{
		for (size_t i = 0, j = wcslen(m_workStr) ; i < j ; i++)
		{
			if (isspace(m_workStr[i])) // is a white space
			{
				m_workStr[i] = 0;
				m_totalTokens++;
			}
		}
        m_listOfDl = nullptr;
	}
	else
	{
		m_listOfDl = (WChar*)O3D_MALLOC((wcslen(m_workStr)+1) * sizeof(WChar));
		memset(m_listOfDl, 0, (wcslen(m_workStr)+1) * sizeof(WChar));

		for (size_t i = 0, j = wcslen(m_workStr); i < j; i++)
		{
			last = False;
			for (size_t m = 0, n = delimiterSize; m < n; m++)
			{
				if (m_workStr[i] == delimiters[m])
				{
					m_workStr[i] = 0;
					m_listOfDl[m_totalTokens] = delimiters[m];
					m_totalTokens++;

                    if ((i > 0) && (m_workStr[i-1] == 0))
						contigous++;
                    else if (i == 0) // if string start by a token
                        contigous++;

					last = True;
					break;
				}
			}
		}
	}

	m_totalTokens++;
	m_totalTokens -= contigous;

	if (last)
		m_totalTokens--;

	m_remainingTokens = m_totalTokens;
}

