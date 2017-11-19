/**
 * @file string.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"

#include <stdarg.h>
#include <stdio.h>

#ifdef _MSC_VER
#include <tchar.h>
#pragma warning(disable: 4996) // Warning about the depreciated function _wcsicmp
#endif

#include "o3d/core/templatearray.h"
#include "o3d/core/string.h"
#include "o3d/core/debug.h"
#include "o3d/core/instream.h"
#include "o3d/core/outstream.h"
#include "o3d/core/char.h"

#include "o3d/core/memory.h"

using namespace o3d;

// Notice we cannot use of O3D_MALLOC... because memory manager should not be used for
// string objects.

namespace o3d
{

#ifdef __APPLE__
//! Perform a lower-case comparison of char strings.
//! @return 0 if equal.
Int32 strcasecmp(const Char *s1, const Char *s2)
{
    Int32 lc1 = 0;
    Int32 lc2 = 0;
    Int32 diff = 0;

	O3D_ASSERT(s1);
	O3D_ASSERT(s2);

	for (;;)
	{
		lc1 = tolower(*s1);
		lc2 = tolower(*s2);

		diff = lc1 - lc2;
		if (diff)
			return diff;

		if (!lc1)
			return 0;

		++s1;
		++s2;
	}
}

// Compare two wchar_t string, insensitive case.
Int32 wcscasecmp(const WChar *s1, const WChar *s2)
{
    Int32 lc1 = 0;
    Int32 lc2 = 0;
    Int32 diff = 0;

	O3D_ASSERT(s1);
	O3D_ASSERT(s2);

	for (;;)
	{
		lc1 = towlower(*s1);
		lc2 = towlower(*s2);

		diff = lc1 - lc2;
		if (diff)
			return diff;

		if (!lc1)
			return 0;

		++s1;
		++s2;
	}
}

// Duplicate a wchar_t string. The new string is allocated with malloc,
WChar *wcsdup(const WChar *str)
{
    WChar *copy;
	size_t len;

    O3D_ASSERT(str != nullptr);

	len = wcslen(str) + 1;
    copy = (WChar*) malloc(len * sizeof(WChar));

	if (!copy)
        return nullptr;

    return (WChar*) wmemcpy(copy, str, len);
}
#endif // __APPLE__
}

//---------------------------------------------------------------------------------------
// CString
//---------------------------------------------------------------------------------------

// Construct an empty cstring.
CString::CString() :
	m_data()
{
}

// Construct an empty cstring full of '0' according to a given size.
CString::CString(UInt32 strlen) :
	m_data(strlen+1)
{
    memset(m_data.getData(), 0, (strlen+1) * sizeof(Char));
}

// Construct a cstring from a source char* (must be 0 terminated).
CString::CString(const Char *copy) :
	m_data()
{
    if (copy) {
        m_data = SmartArrayChar(copy, strlen(copy)+1);
    }
}

CString::CString(const CString &dup) :
    m_data(dup.m_data)
{
}

// Construct using a valid string and len.
CString::CString(const Char *copy, UInt32 strlen) :
	m_data()
{
    if (copy && strlen) {
		m_data = SmartArrayChar(copy, strlen+1);
    }
}

// Construct using a valid string and len. Own it or copy it.
CString::CString(Char *source, UInt32 strlen, Bool own) :
	m_data()
{
    if (source && strlen) {
		m_data = SmartArrayChar(source, strlen+1, own);
    }
}

// Destructor
CString::~CString()
{
}

CString& CString::operator=(const CString &dup)
{
    m_data = dup.m_data;
    return *this;
}

// Destroy
void CString::destroy()
{
    m_data = SmartArrayChar();
}

SmartArrayUInt8 CString::getBytes() const
{
    return SmartArrayUInt8((const UInt8*)m_data.getData(), m_data.getSizeInBytes()-1);
}

Bool CString::operator<(const CString &s) const
{
    if (isNull()) {
        // O3D_ERROR(E_NullPointer("left string is null"));
        return True;
    }

    if (s.isNull()) {
        // O3D_ERROR(E_NullPointer("right string is null"));
        return False;
    }

    return strcmp(m_data.getData(), s.m_data.getData()) < 0;
}

Bool CString::operator<(const Char *s) const
{
    if (isNull()) {
        // O3D_ERROR(E_NullPointer("left string is null"));
        return True;
    }

    if (!s) {
        // O3D_ERROR(E_NullPointer("right string is null"));
        return False;
    }

    return strcmp(m_data.getData(), s) < 0;
}

Bool CString::operator==(const CString &s) const
{
    if (isNull() && s.isNull()) {
        return True;
    } else if (isNull() || s.isNull()) {
        return False;
    }

//    if (isNull()) {
//        O3D_ERROR(E_NullPointer("left string is null"));
//    }

//    if (s.isNull()) {
//        O3D_ERROR(E_NullPointer("right string is null"));
//    }

    return strcmp(m_data.getData(), s.m_data.getData()) == 0;
}

Bool CString::operator==(const Char *s) const
{
    if (isNull() && !s) {
        return True;
    } else if (isNull() || !s) {
        return False;
    }

//    if (isNull()) {
//        O3D_ERROR(E_NullPointer("left string is null"));
//    }

//    if (!s) {
//        O3D_ERROR(E_NullPointer("right string is null"));
//    }

    return strcmp(m_data.getData(), s) == 0;
}

Bool CString::operator!=(const CString &s) const
{
    if (isNull() && s.isNull()) {
        return False;
    } else if (isNull() || s.isNull()) {
        return True;
    }

//    if (isNull()) {
//        O3D_ERROR(E_NullPointer("left string is null"));
//    }

//    if (s.isNull()) {
//        O3D_ERROR(E_NullPointer("right string is null"));
//    }

    return strcmp(m_data.getData(), s.m_data.getData()) != 0;
}

Bool CString::operator!=(const Char *s) const
{
    if (isNull() && !s) {
        return False;
    } else if (isNull() || !s) {
        return True;
    }

//    if (isNull()) {
//        O3D_ERROR(E_NullPointer("left string is null"));
//    }

//    if (!s) {
//        O3D_ERROR(E_NullPointer("right string is null"));
//    }

    return strcmp(m_data.getData(), s) != 0;
}

//---------------------------------------------------------------------------------------
// String
//---------------------------------------------------------------------------------------

//! Copy constructor from a ArrayChar, default assume ArrayChar is UTF8
String::String(const TemplateArray<Char, -1> &copy, UInt32 ithreshold) :
    m_data(nullptr),
    m_size(0),
    m_maxsize(0),
    m_threshold(ithreshold)
{
    fromUtf8(copy.getData(), copy.getSize()-1);
}

// Return an null static string.
const String& String::getNull()
{
	static String emptyString;
	return emptyString;
}

// Destructor
String::~String()
{
	destroy();
}

void String::destroy()
{
	if (m_data)
	{
		/*O3D_FREE*/free(m_data);
        m_data = nullptr;
	}

	m_size = m_maxsize = 0;
}

// Initialise la taille de la chaîne a NewSize
void String::setCapacity(UInt32 NewSize)
{
	++NewSize;		// zero terminal

	// test si la nouvelle taille est plus grande que l'actuelle
	// ou si la nouvelle taille est plus petite que l'actuelle d'au moins 1*m_threshold
	if (NewSize > m_maxsize || (m_maxsize > m_threshold && NewSize < m_maxsize - m_threshold))
	{
		// calcule la nouvelle taille (plus proche multiple de m_threshold)
		NewSize = ((NewSize + m_threshold - 1) / m_threshold) * m_threshold;

		if (!NewSize)		// si la nouvelle taille est nulle
		{
			if (m_data)
			{
				/*O3D_FREE*/free(m_data);
                m_data = nullptr;
			}
		}
		else
		{
			if (m_data)
			{
				// sinon realloue
                m_data = (WChar*)/*O3D_REALLOC*/realloc(m_data,NewSize*sizeof(WChar));
			}
			else
			{
				// alloue et met un zero terminal
                m_data = (WChar*)/*O3D_MALLOC*/malloc(NewSize*sizeof(WChar));
				m_data[0] = 0;
			}
		}

		if (!m_data)
			O3D_ERROR(E_InvalidAllocation(""));

		m_maxsize = NewSize;

		if (m_size > m_maxsize) // test si la taille utilisée est supérieure à la taille allouée
		{
			m_size = m_maxsize;	    // redimmensionne à m_maxsize
			m_data[m_size-1] = 0;   // et replace le zéro terminal
		}
	}
}

void String::set(WChar c)
{
	setCapacity(1);

	m_data[0] = c;
	m_data[1] = 0;

	m_size = 2;
}

void String::set(const String &copy, UInt32 pos)
{
    Int32 len = copy.m_size - pos - 1;
	if (len < 0)
	{
		destroy();
		return;
	}

	// resize if necessary
	if (m_maxsize < (copy.m_size - pos))
		setCapacity(len);

	// copy
    memmove(m_data, copy.m_data + pos, len*sizeof(WChar));

	m_data[len] = 0;

	m_size = len+1;
}

void String::set(const Char* src, UInt32 pos)
{
    if (src == nullptr)
	{
		destroy();
		return;
	}

    UInt32 srclen = (UInt32)strlen(src);
    Int32 len = srclen - pos;
	if (len < 0)
		return;

	// resize if necessary
	if (m_maxsize < (srclen-pos+1))
		setCapacity(len);

	// copy
    for (Int32 i = 0; i < len; ++i)
	{
        m_data[i] = static_cast<UInt8>(*(src+pos+i));
	}

	m_data[len] = 0;
	m_size = len+1;
}

void String::set(const WChar* src, UInt32 pos)
{
    if (src == nullptr)
	{
		destroy();
		return;
	}

    UInt32 srclen = (UInt32)wcslen(src);
    Int32 len = srclen-pos;
	if (len < 0)
		return;

	// resize if necessary
	if (m_maxsize < (srclen-pos+1))
		setCapacity(len);

	// copy
    memmove(m_data, src+pos, len*sizeof(WChar));

	m_data[len] = 0;

	m_size = len+1;
}

// Set the string size and fill with a given character
void String::setFill(WChar c, UInt32 n)
{
	setCapacity(n);
	m_size = n+1;

    for (UInt32 i = 0 ; i < n ; ++i)
	{
		m_data[i] = c;
	}

	m_data[m_size-1] = 0;
}

// Fill the string with a given character
void String::fill(WChar c)
{
    for (UInt32 i = 0 ; i < (m_size-1) ; ++i)
	{
		m_data[i] = c;
	}

	m_data[m_size-1] = 0;
}

// Count the number of occurrence of a given char
UInt32 String::count(WChar c) const
{
	if (m_size == 0)
		return 0;

    Int32 count=0;

    for (UInt32 i = 0; i < m_size; ++i)
	{
		if (m_data[i] == c)
			++count;
	}

	return count;
}

// Check if the string start by a given string.
Bool String::startsWith(const String &start) const
{
    UInt32 len = start.length();

	// this is to lower
	if (this->length() < len)
        return False;

    for (UInt32 i = 0; i < len; ++i)
	{
		if (m_data[i] != start[i])
            return False;
	}

    return True;
}

// Check if the string ends by a given string.
Bool String::endsWith(const String &end) const
{
    UInt32 len = end.length();

	// this is to lower
	if (this->length() < len)
        return False;

    for (UInt32 i = 0, j = m_size - len - 1; j < m_size; ++i, ++j)
	{
		if (m_data[j] != end[i])
            return False;
	}

    return True;
}

// delete an eventually char/string at the end of the string
Bool String::trimRight(WChar _char, Bool _repeat)
{
	if (m_size > 1)
	{
		if (_repeat)
		{
            for (Int32 k = (Int32)m_size-2 ; k >= 0 ; --k)
			{
				if (m_data[k] != _char)
				{
                    if (k != (Int32)m_size - 2)
						truncate(k+1);

                    return True;
				}
			}
		}
		else if (m_data[m_size-2] == _char)
		{
			truncate(m_size-2);
            return True;
		}
	}
    return False;
}

Bool String::trimRight(const String& str)
{
	// source length lesser than search string
	if (m_size < str.m_size)
        return False;

    Int32 searchPos = m_size - str.m_size;
    Int32 pos = sub(str, (UInt32)searchPos);

	if ((pos != -1) && (pos == searchPos))
	{
		truncate(pos);
        return True;
	}

    return False;
}

WChar String::trimRight()
{
	if (length())
	{
        WChar remChar = m_data[length()-1];

		setCapacity(length()-1);
		--m_size;
		m_data[m_size-1] = 0;

		return remChar;
	}
	return 0;
}

// Trim chars from the end of the string
Bool String::trimRightChars(const String & _charList)
{
	if (_charList.isNull())
        return False;

    UInt32 lCounter = 0;
    const WChar * lChar = nullptr;
    const WChar * const lCharEnd = &_charList.m_data[_charList.m_size-1];

    const WChar * lCharSrc = &m_data[m_size-2];

    for (Int32 k = m_size-1 ; k > 0 ; --k)
	{
		lChar = _charList.m_data;

		while (lChar != lCharEnd)
		{
			if (*lCharSrc == *lChar)
				break;

			++lChar;
		}

		if (lChar == lCharEnd)
			break;

		--lCharSrc;
		++lCounter;
	}

	if (lCounter > 0)
	{
		truncate(m_size - lCounter - 1);

        return True;
	}
    return False;
}

// Trim char from the start of the string
Bool String::trimLeftChars(const String & _charList)
{
	if (_charList.isNull())
        return False;

    UInt32 lCounter = 0;
    const WChar * lChar = nullptr;
    const WChar * const lCharEnd = &_charList.m_data[_charList.m_size-1];

    const WChar * lCharSrc = m_data;

    for (Int32 k = m_size-1 ; k > 0 ; --k)
	{
		lChar = _charList.m_data;

		while (lChar != lCharEnd)
		{
			if (*lCharSrc == *lChar)
				break;

			++lChar;
		}

		if (lChar == lCharEnd)
			break;

		++lCharSrc;
		++lCounter;
	}

	if (lCounter > 0)
	{
        memmove(m_data, m_data + lCounter, sizeof(WChar)*(m_size-lCounter));
		m_size = m_size-lCounter;
		setCapacity(m_size);

        return True;
	}
    return False;
}

// Delete an eventually char at the start of the string
Bool String::trimLeft(WChar c, Bool rep)
{
	if (m_size > 1)
	{
		if (rep)
		{
            Int32 count = 0;

			while (m_data[count] == c)
			{
				count++;
			}

            memmove(m_data, m_data+count, sizeof(WChar)*(m_size-count));
			m_size = m_size-count;
			setCapacity(m_size);

            return True;
		}
		else
		{
			if (m_data[0] == c)
			{
                memmove(m_data, m_data+1, sizeof(WChar)*(m_size-1));
				m_size = m_size - 1;
				setCapacity(m_size);

                return True;
			}
		}
	}
    return False;
}

// delete the first character of the string (if it is not empty)
WChar String::trimLeft()
{
	if (length())
	{
        WChar remChar = m_data[0];

        memmove(m_data, m_data+1, sizeof(WChar)*(m_size-1));
		m_size = m_size - 1;
		setCapacity(m_size);

		return remChar;
	}
	return 0;
}

// Build a new String with like sprintf method
String String::print(const Char* str, ...)
{
    O3D_ASSERT(str != nullptr);

	va_list msg;
	va_start(msg, str);

#ifdef O3D_WINDOWS
    Int32 i;
	i = _vscprintf(str, msg);

	ArrayChar data(i+1);

	_vsnprintf(data.getData(), i, str, msg);

	va_end(msg);

	data[i] = 0;
	return String(data.getData());
#else
    Char *data = nullptr;

	vasprintf(&data, str, msg);

	va_end(msg);

	String result(data);

	free(data);
	return result;
#endif
}

// Build a new String with like wsprintf method
String String::print(const WChar* str, ...)
{
    O3D_ASSERT(str != nullptr);

    Int32 i;
	va_list msg;

	va_start(msg,str);

#ifdef _MSC_VER
	i = _vscwprintf(str, msg);
#else // Unix version
	i = vwprintf(str, msg);
#endif

	String newString;
	newString.setCapacity(i);

#ifdef _MSC_VER
	_vsnwprintf(newString.m_data, i, str, msg);
#else
	vswprintf(newString.m_data, i, str, msg);
#endif

	va_end(msg);

    newString.m_data[i] = 0;
	newString.m_size = ++i;

	return newString;
}

// Concat the string src to this
String& String::operator+= (const String &src)
{
	if (src.m_size <= 1)
		return *this;

	if (m_size == 0)
		m_size = 1;

    UInt32 newlen = (m_size - 1) + (src.m_size - 1);

	if (m_maxsize < newlen+1)
		setCapacity(newlen);

    memmove(m_data+m_size-1, src.m_data, (src.m_size-1)*sizeof(WChar));
	m_data[newlen] = 0;

	m_size = newlen + 1;
	return *this;
}

String& String::operator+= (WChar c)
{
	if (m_size == 0)
		m_size = 1;

	if (m_maxsize < m_size+1)
		setCapacity(m_size+1);

	m_data[m_size-1] = c;
	m_data[m_size]   = 0;

	++m_size;
	return *this;
}

// ajoute s1 et s2 et le renvoi return = s1 + s2
String String::operator+ (const String &s)const
{
	if (m_size == 0)
		return String(s);

	String str(*this);

	if (s.m_size <= 1)
		return str;

    UInt32 newlen = (str.m_size-1) + (s.m_size-1);

	if (str.m_maxsize < newlen+1)
		str.setCapacity(newlen);

	if (str.m_size == 0)
		str.m_size = 1;

    memmove(str.m_data+m_size-1, s.m_data, (s.m_size-1)*sizeof(WChar));
	str.m_data[newlen] = 0;

	str.m_size += s.m_size - 1;

	return str;
}

//! Append src to this and return it as a new string
String String::operator+ (WChar c) const
{
	// this is empty
	if (m_size == 0)
	{
		String result;
		result.set(c);
	}

	String result(*this);
	result += c;

	return result;
}

//! Case sensitive comparison.
Bool String::operator== (const String &s) const
{
    if (isNull() && s.isNull()) {
        return True;
    } else if (isNull() || s.isNull()) {
        return False;
    }

//    if (isNull()) {
//        O3D_ERROR(E_NullPointer("left string is null"));
//    }

//    if (s.isNull()) {
//        O3D_ERROR(E_NullPointer("right string is null"));
//    }

//    if (isEmpty() && s.isEmpty()) // both are empty
//        return 0;

    return wcscmp(m_data, s.m_data) == 0;
}

Bool String::operator!=(const String &s) const
{
    if (isNull() && s.isNull()) {
        return False;
    } else if (isNull() || s.isNull()) {
        return True;
    }

//    if (isNull()) {
//        O3D_ERROR(E_NullPointer("left string is null"));
//    }

//    if (s.isNull()) {
//        O3D_ERROR(E_NullPointer("right string is null"));
//    }

//    if (isEmpty() && s.isEmpty()) // both are empty
//        return 1;

    return wcscmp(m_data, s.m_data) != 0;
}

//! Comparison.
Int32 String::compare(const String & s, ComparisonPolicy _type) const
{
    if (isNull() && s.isNull()) {
        return True;
    } else if (isNull() || s.isNull()) {
        return False;
    }

//    if (isNull()) {
//        O3D_ERROR(E_NullPointer("left string is null"));
//    }

//    if (s.isNull()) {
//        O3D_ERROR(E_NullPointer("right string is null"));
//    }

//    if (isEmpty() && s.isEmpty()) // both are empty
//		return 0;

	if (_type == String::CASE_SENSITIVE)
        return wcscmp(getData(), s.getData());
	else
        return o3d::wcscasecmp(getData(), s.getData());
}

//! Case sensitive comparison.
Bool String::operator> (const String &s) const
{
    if (isNull()) {
        // O3D_ERROR(E_NullPointer("left string is null"));
        return False;
    }

    if (s.isNull()) {
        // O3D_ERROR(E_NullPointer("right string is null"));
        return True;
    }

//    if (isEmpty() && s.isEmpty()) // both are empty
//        return 0;

    return wcscmp(m_data, s.m_data) > 0;
}

//! Case sensitive comparison.
Bool String::operator< (const String &s) const
{
    if (isNull()) {
        // O3D_ERROR(E_NullPointer("left string is null"));
        return True;
    }

    if (s.isNull()) {
        // O3D_ERROR(E_NullPointer("right string is null"));
        return False;
    }

//    if (isEmpty() && s.isEmpty()) // both are empty
//        return 0;

    return wcscmp(m_data, s.m_data) < 0;
}

Bool String::operator>=(const String &s) const
{
    if (isNull() && s.isNull()) {
        return True;
    }

    if (isNull()) {
        // O3D_ERROR(E_NullPointer("left string is null"));
        return False;
    }

    if (s.isNull()) {
        // O3D_ERROR(E_NullPointer("right string is null"));
        return True;
    }
//    if (isEmpty() && s.isEmpty()) // both are empty
//        return 0;

    return wcscmp(m_data, s.m_data) >= 0;
}

Bool String::operator<=(const String &s) const
{
    if (isNull() && s.isNull()) {
        return True;
    }

    if (isNull()) {
        // O3D_ERROR(E_NullPointer("left string is null"));
        return True;
    }

    if (s.isNull()) {
        // O3D_ERROR(E_NullPointer("right string is null"));
        return False;
    }

//    if (isEmpty() && s.isEmpty()) // both are empty
//        return 0;

    return wcscmp(m_data, s.m_data) <= 0;
}

//  renvoie le charactère placé à la position Index
WChar& String::operator[] (UInt32 Index)
{
	if (Index >= m_size)
		O3D_ERROR(E_IndexOutOfRange(""));

	return m_data[Index];
}

const WChar& String::operator[] (UInt32 Index)const
{
	if (Index >= m_size)
		O3D_ERROR(E_IndexOutOfRange(""));

	return m_data[Index];
}

// Lowercase the entire string
String& String::lower()
{
#ifdef _MSC_VER
	_wcslwr(m_data);
#else
    UInt32 i;
	for (i = 0; i < m_size-1; i++)
	{
		m_data[i] = towlower(m_data[i]);
	}
#endif
	return *this;
}

// Uppercase the entire string
String& String::upper()
{
#ifdef _MSC_VER
	_wcsupr(m_data);
#else
    UInt32 i;
	for (i = 0; i < m_size-1; i++)
	{
		m_data[i] = towupper(m_data[i]);
	}
#endif
    return *this;
}

String String::toLower() const
{
    String res(*this);
    res.lower();
    return res;
}

String String::toUpper() const
{
    String res(*this);
    res.upper();
    return res;
}

String String::capitalize() const
{
    String res(*this);

    if (res.m_size > 1)
        res.m_data[0] = towupper(res.m_data[0]);

    return res;
}

String String::title() const
{
    String res(*this);

    UInt32 w = 1;
    UInt32 j, l = WideChar::ponctuationsPlusWs().m_size - 1;
    for (UInt32 i = 0; i < m_size; ++i)
    {
        for (j = 0; j < l; ++j)
        {
            if (WideChar::ponctuationsPlusWs().m_data[j] == res.m_data[i])
            {
                w = 0;
                break;
            }
        }

        // not a punctuation neither a white space, and start a new word => upper case
        if (w == 1)
            res.m_data[i] = towupper(m_data[i]);

        ++w;
    }

    return res;
}

Int32 String::sub(const String& str, UInt32 pos) const
{
    UInt32 len = str.m_size - 1;	// taille de la sous chaine a comparer
    UInt32 i,j = 0;					// position dans la sous chaine

	if ((m_size - pos) < len)
		return -1;	// chaine trop petite pour accepter sous chaine

	for (i = pos; i < m_size; ++i)
	{
		if (m_data[i] == str.m_data[j])
			++j;
		else
			j = 0;

		if (j == len)
			break;
	}

	if (j == len)
		return (i-len+1);

	return (-1);
}

// Insert a char
String& String::insert(WChar c, UInt32 pos)
{
	if (m_size == 0)
	{
		set(c);
		return *this;
	}

	if (pos >= m_size)
		return *this;

    UInt32 NewSize = m_size; // + 1 - 1

	if (NewSize >= m_maxsize)
		setCapacity(NewSize);

    memmove(m_data + pos + 1, m_data + pos, (m_size-pos)*sizeof(WChar));
	m_data[pos] = c;

	++m_size;

	return *this;
}

String& String::insert(const String &str, UInt32 pos)
{
	if (str.isEmpty())
		return *this;

    if (m_size == 0)//m_data == nullptr)
	{
		set(str,0);
		return *this;
	}

	if (pos >= m_size)
		return *this;

    UInt32 len = str.m_size - 1;
    UInt32 NewSize = len + m_size - 1;

	if (NewSize >= m_maxsize)
		setCapacity(NewSize);

    memmove(m_data + pos + len, m_data + pos, (m_size-pos)*sizeof(WChar));
    memmove(&m_data[pos], str.m_data, len*sizeof(WChar));

	m_size += len;

	return *this;
}

// Reverse the string
String& String::reverse()
{
    UInt32 i;
    WChar swapChar;

	for (i=0; i<m_size/2; i++)
	{
		swapChar = m_data[i];
		m_data[i] = m_data[m_size-i-1];
		m_data[m_size-i-1] = swapChar;
	}

	return *this;
}

// Remove many characters
String& String::remove(UInt32 pos, UInt32 n)
{
	if (pos >= m_size)
		return *this;

	if (n >= m_size - pos)
		n = m_size - pos - 1;

    memmove(m_data + pos, m_data + pos + n, (m_size-pos)*sizeof(WChar));
	m_data[(m_size -= n)-1] = 0;

    return *this;
}

String String::extract(UInt32 pos, UInt32 n)
{
    String result;

    if (pos >= m_size) {
        return result;
    }

    if (n >= m_size - pos) {
        n = m_size - pos - 1;
    }

    result.setCapacity(n);

    // copy
    memcpy(result.m_data, m_data + pos, n*sizeof(WChar));
    result.m_size = n + 1;
    result.m_data[result.m_size-1] = '\0';

    // cut
    memmove(m_data + pos, m_data + pos + n, (m_size-pos)*sizeof(WChar));
    m_data[(m_size -= n)-1] = 0;

    return result;
}

String String::slice(Int32 start, Int32 end) const
{
    String result;

    if (start >= (Int32)m_size) {
        return result;
    }

    if (start < 0) {
        start = 0;
    }

    Int32 n = 0;
    if (end >= (Int32)m_size || end < 0) {
        n = m_size - start - 1;
    } else {
        n = start - end - 1;
    }

    result.setCapacity(n);

    // copy
    memcpy(result.m_data, m_data + start, n*sizeof(WChar));
    result.m_size = n + 1;
    result.m_data[result.m_size-1] = '\0';

    return result;
}

// Find for a character
Int32 String::find(WChar c, UInt32 pos, Bool reverse) const
{
	if (pos >= m_size)
		return -1;

    Int32 i;

	if (!reverse)
	{
        for (i = pos ; i < (Int32)m_size ; ++i)
		{
			if (m_data[i] == c)
				break;
		}

        return (i < (Int32)(m_size-1) ? i : -1);
	}
	else
	{
		i = pos;
        for (i = pos ; i > -1 ; --i)
		{
			if (m_data[i] == c)
				break;
		}

//		if ((i == 0) && (m_data[0] != c))
//			return -1;

		return i;
	}
}

Int32 String::find(const String &s, UInt32 pos, Bool reverse, Bool last) const
{
    if (pos >= m_size)
        return -1;

    Int32 i;
    UInt32 j;

    if (!reverse)
    {
        if (last)
        {
            Int32 match = -1;
            Bool found = False;

            for (i = pos ; i < (Int32)m_size ; ++i)
            {
                found = False;

                for (j = 0; j < s.m_size; ++j)
                {
                    if (m_data[i] == s.m_data[j])
                    {
                        match = i;
                        found = True;
                        break;
                    }
                }

                if (match != -1 && !found)
                    return match;
            }
        }
        else
        {
            for (i = pos ; i < (Int32)m_size ; ++i)
            {
                for (j = 0; j < s.m_size; ++j)
                {
                    if (m_data[i] == s.m_data[j])
                        return i;
                }
            }
        }

        return -1;
    }
    else
    {
        if (last)
        {
            Int32 match = -1;
            Bool found = False;

            i = pos;
            for (i = pos ; i > -1 ; --i)
            {
                found = False;

                for (j = 0; j < s.m_size; ++j)
                {
                    if (m_data[i] == s.m_data[j])
                    {
                        match = i;
                        found = True;
                        break;
                    }
                }

                if (match != -1 && !found)
                    return match;
            }
        }
        else
        {
            i = pos;
            for (i = pos ; i > -1 ; --i)
            {
                for (j = 0; j < s.m_size; ++j)
                {
                    if (m_data[i] == s.m_data[j])
                        return i;
                }
            }
        }

        return -1;
    }
}

// Truncate the string
String& String::truncate(UInt32 pos)
{
	if (pos >= m_size)
		return *this;

	setCapacity(pos);
	m_size = pos + 1;
	m_data[m_size-1] = 0;

	return *this;
}

// Get a part of the string given a range
String String::sub(UInt32 _start, UInt32 _end) const
{
	if (m_size == 0)
		return String("");

    _end = o3d::min<UInt32>(_end, m_size-1);
    _start = o3d::min<UInt32>(_start, m_size-1);

	if (_end <= _start)
		return String("");

    const UInt32 lSize = _end - _start;

	String lRet(lSize, 256);
    memcpy(lRet.m_data, m_data+_start, lSize * sizeof(WChar));
	lRet.m_size = lSize+1;
	lRet.m_data[lSize] = '\0';

	return lRet;
}

// Remove a substring if found
String& String::remove(const String &find)
{
	if ((m_size == 0) || (find.m_size == 0))
		return *this;

    Int32 dstPos = 0; // starting at position 0
    Int32 srcPos;

    while ((dstPos = sub(find, (UInt32)dstPos)) != -1)
	{
		srcPos = dstPos + find.m_size - 1;

        memmove(m_data+dstPos, m_data+srcPos, sizeof(WChar)*(m_size-srcPos));
		m_size -= find.m_size - 1;
	}

	setCapacity(m_size-1);
	return *this;
}

// Replace each occurrence of a char by another char
String& String::replace(WChar c, WChar n)
{
	if (m_size == 0)
		return *this;

    for (UInt32 i = 0; i < m_size; ++i)
	{
		if (m_data[i] == c)
			m_data[i] = n;
	}

	return *this;
}

// Replace each occurrence of a string by another string
String& String::replace(const String& find, const String& by)
{
	if (m_size == 0 || find.m_size == 0 || by.m_size == 0)
		return *this;

    Int32 dstPos = 0; // starting at position 0
    Int32 srcPos;

	while ((dstPos = sub(find, dstPos)) != -1)
	{
		srcPos = dstPos + find.m_size - 1;

		if (by.m_size > find.m_size)
			setCapacity(m_size-1+ by.m_size-find.m_size);

        memmove(m_data+dstPos+by.m_size-1, m_data+srcPos, sizeof(WChar)*(m_size-srcPos));
        memmove(m_data+dstPos, by.m_data, sizeof(WChar)*(by.m_size-1));

		m_size += by.m_size - find.m_size;
	}

	// resize the m_maxsize if necessary, only when 'by' is lower than 'find'
	if (by.m_size < find.m_size)
		setCapacity(m_size-1);

	return *this;
}

// Replace a part of the string starting at a position
String& String::replace(const String &str, UInt32 n)
{
	if (str.m_size == 0 || m_size == 0)
		return *this;

	if (n >= m_size)
		return *this;

    Int32 add = str.m_size - 1;

	if ((add + n) > (m_size - 1))
		add -= (add + n) - (m_size - 1);

    memmove(m_data+n, str.m_data, add*sizeof(WChar));

	return *this;
}

// Replace a char at a given position
String& String::replaceChar(WChar c, UInt32 n)
{
	if (n >= m_size)
		return *this;

	m_data[n] = c;
	return *this;
}

// Concat a base 10 integer
void String::concat(Int32 i, Int32 radix)
{
    WChar str[16] = { 0 };
#ifdef _MSC_VER
	_itow(i,str,radix);
#else
	swprintf(str, 16, L"%i", i);
#endif
	(*this) += str;
}

// Concat a base 10 unsigned integer
void String::concat(UInt32 i, Int32 radix)
{
    WChar str[16] = { 0 };
#ifdef _MSC_VER
	_ultow(i, str, radix);
#else
	swprintf(str, 16, L"%u", i);
#endif
	(*this) += str;
}

// Concat a base 10 integer
void String::concat(Int64 i, Int32 radix)
{
    WChar str[32] = { 0 };
#ifdef _MSC_VER
	_i64tow(i,str,radix);
#else
	swprintf(str, 32, L"%lld", i);
#endif
	(*this) += str;
}

// Concat a base 10 unsigned integer
void String::concat(UInt64 i, Int32 radix)
{
    WChar str[32] = { 0 };
#ifdef _MSC_VER
	_ui64tow(i, str, radix);
#else
	swprintf(str, 32, L"%llu", i);
#endif
	(*this) += str;
}

// Concat a float
void String::concat(Float f)
{
    WChar str[16] = { 0 };
	swprintf(str, 16, L"%f", f);

	(*this) += str;
}

// Concat a double
void String::concat(Double d)
{
    WChar str[32] = { 0 };
    swprintf(str, 32, L"%f", d);

	(*this) += str;
}

Char String::toChar(UInt32 pos) const
{
    return static_cast<Char>(operator[](pos));
}

WChar String::toWChar(UInt32 pos) const
{
	return operator[](pos);
}

Int32 String::toInt32(UInt32 pos) const
{
    if (m_size <= 1)
        return 0;

    WChar *end;
	return wcstol(&m_data[pos], &end, 10);
}

UInt32 String::toUInt32(UInt32 pos) const
{
    if (m_size <= 1)
        return 0;

    WChar *end;
	return wcstoul(&m_data[pos], &end, 10);
}

Int64 String::toInt64(UInt32 pos) const
{
    if (m_size <= 1)
        return 0;

    WChar *end;
    return wcstol(&m_data[pos], &end, 10);
}

UInt64 String::toUInt64(UInt32 pos) const
{
    if (m_size <= 1)
        return 0;

    WChar *end;
    return wcstoul(&m_data[pos], &end, 10);
}

Float String::toFloat(UInt32 pos) const
{
    if (m_size <= 1)
        return 0.f;

    WChar *end;
    return (Float)wcstod(&m_data[pos], &end);
}

Double String::toDouble(UInt32 pos) const
{
    if (m_size <= 1)
        return 0.0;

    WChar *end;
	return wcstod(&m_data[pos], &end);
}

//--------------------------------------------------------------------------------------
// String conversion
//--------------------------------------------------------------------------------------

CString String::toAscii() const
{
	// null string
	if (!m_data)
		return CString();

	// empty string
	if (m_size == 1)
		return CString("");

	CString result(m_size);

    for (UInt32 i = 0; i < m_size; ++i)
	{
        result[i] = static_cast<Char>(m_data[i]);
	}

	return result;
}

CString String::toUtf8() const
{
	// null string
	if (!m_data)
		return CString();

	// empty string
	if (m_size == 1)
		return CString("");

#ifndef O3D_WIN32_SYS
	ArrayChar temp(m_size*2, m_size);
    WChar c;

	// WIDECHAR size is 4
    for (UInt32 i = 0; i < m_size; ++i)
	{
		c = m_data[i];

		if (c < 0x80)
		{
			// 0xxxxxxx
            temp.push(static_cast<Char>(c));
		}
		else if (c < 0x800)
		{
			// C0          80
			// 110xxxxx 10xxxxxx
            temp.push(static_cast<Char>(0xC0 | (c >> 6)));
            temp.push(static_cast<Char>(0x80 | (c & 0x3F)));
		}
		else if (c < 0x8000)
		{
			// E0       80       80
			// 1110xxxx 10xxxxxx 10xxxxxx
            temp.push(static_cast<Char>(0xE0 | (c >> 12)));
            temp.push(static_cast<Char>(0x80 | (c >> 6 & 0x3F)));
            temp.push(static_cast<Char>(0x80 | (c & 0x3F)));
		}
		else
		{
			// F0      80       80       80
			//11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
            temp.push(static_cast<Char>(0xF0 | (c >> 12) >> 6));
            temp.push(static_cast<Char>(0x80 | (c >> 12 & 0x3F)));
            temp.push(static_cast<Char>(0x80 | (c >> 6 & 0x1F)));
            temp.push(static_cast<Char>(0x80 | (c & 0x3F)));
		}
	}

	temp.push(0);

	// Copy to a CString
	return CString(temp.getData());
#else
    Int32 len = ::WideCharToMultiByte(CP_UTF8, 0, m_data, m_size-1, nullptr, 0, nullptr, nullptr);
	O3D_ASSERT(len != 0);

	CString utf8String(len);
    len = ::WideCharToMultiByte(CP_UTF8, 0, m_data, m_size-1, utf8String.getData(), len+1, nullptr, nullptr);
	O3D_ASSERT(len != 0);

	return utf8String;
#endif // O3D_WIN32_SYS
}

void String::fromUtf8(const Char* utf8, UInt32 maxSize)
{
	// The string must be destroyed if the specified pointer is null
	destroy();

	// NULL or empty input string
    if (utf8 == nullptr)
		return;
    UInt32 utf8Len;
	if (maxSize == 0)
		utf8Len = strlen(utf8);
	else
		utf8Len = maxSize;

#ifndef O3D_WIN32_SYS
	//WIDECHAR size is 4
    for (UInt32 i = 0; i < utf8Len; ++i)
	{
		// F0      80       80       80
		//11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
		if ((utf8[i] & 0xFE) == 0xFE)
		{
            this->operator+= (static_cast<WChar>(
						((utf8[i] & 7) << 18) |
						((utf8[i+1] & 0x3F) << 12) |
						((utf8[i+2] & 0x3F) << 6) |
						(utf8[i+3] & 0x3F)));

			i += 3;
		}
		// E0       80       80
		// 1110xxxx 10xxxxxx 10xxxxxx
		else if ((utf8[i] & 0xE0) == 0xE0)
		{
            this->operator+= (static_cast<WChar>(
						((utf8[i] & 0x0F) << 12) |
						((utf8[i+1] & 0x3F) << 6) |
						(utf8[i+2] & 0x3F)));

			i += 2;
		}
		// C0       80
		// 110xxxxx 10xxxxxx
		else if ((utf8[i] & 0xC0) == 0xC0)
		{
            this->operator+= (static_cast<WChar>(
						((utf8[i] & 0x1F) << 6) |
						(utf8[i+1] & 0x3F)));

			i += 1;
		}
		// 0xxxxxxx
		else// if(utf8[i] < 0x80)
		{
            this->operator+= (static_cast<WChar>(utf8[i]));
		}
	}
#else
    Int32 len = ::MultiByteToWideChar(CP_UTF8, 0, utf8, utf8Len, nullptr, 0);
	// If the line was empty, "len" is now equal to 0

	setCapacity(len); // If len is 0, capacity will be set to 1 since '\0' must be included
	len = ::MultiByteToWideChar(CP_UTF8, 0, utf8, utf8Len, m_data, len+1);

	m_data[len] = 0;
	m_size = len + 1;
#endif // O3D_WIN32_SYS
}

//--------------------------------------------------------------------------------------
// String serialization
//--------------------------------------------------------------------------------------

Bool String::writeToFile(OutStream &os) const
{
	CString data = this->toUtf8();
    return data.writeToFile(os);
}

Bool String::writeToFile(OutStream &os)
{
	CString data = this->toUtf8();
    return data.writeToFile(os);
}

Bool String::readFromFile(InStream &is)
{
	CString data;
    if (!data.readFromFile(is))
        O3D_ERROR(E_StringUnderflow(""));

	if (data[data.length()] != 0)
        O3D_ERROR(E_StringOverflow(""));

	this->fromUtf8(data.getData());

    return True;
}

#include "o3d/core/stringlist.h"
#include "o3d/core/stringtokenizer.h"

T_StringList split(const String &content, const String &delimiters)
{
    T_StringList parts;

    StringTokenizer tokenizer(content, delimiters);
    while (tokenizer.hasMoreElements()) {
        parts.push_back(tokenizer.nextElement());
    }

    return parts;
}
