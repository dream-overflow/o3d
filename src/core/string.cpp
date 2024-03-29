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

    for (;;) {
		lc1 = towlower(*s1);
		lc2 = towlower(*s2);

		diff = lc1 - lc2;
        if (diff) {
			return diff;
        }

        if (!lc1) {
			return 0;
        }

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
CString::CString(Int32 strlen) :
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
CString::CString(const Char *copy, Int32 strlen) :
	m_data()
{
    if (copy && strlen) {
		m_data = SmartArrayChar(copy, strlen+1);
    }
}

// Construct using a valid string and len. Own it or copy it.
CString::CString(Char *source, Int32 strlen, Bool own) :
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

Bool CString::operator<=(const CString &s) const
{
    if (isNull()) {
        // O3D_ERROR(E_NullPointer("left string is null"));
        return True;
    }

    if (s.isNull()) {
        // O3D_ERROR(E_NullPointer("right string is null"));
        return False;
    }

    return strcmp(m_data.getData(), s.m_data.getData()) <= 0;
}

Bool CString::operator<=(const Char *s) const
{
    if (isNull()) {
        // O3D_ERROR(E_NullPointer("left string is null"));
        return True;
    }

    if (!s) {
        // O3D_ERROR(E_NullPointer("right string is null"));
        return False;
    }

    return strcmp(m_data.getData(), s) <= 0;
}

Bool CString::operator>(const CString &s) const
{
    if (isNull()) {
        // O3D_ERROR(E_NullPointer("left string is null"));
        return True;
    }

    if (s.isNull()) {
        // O3D_ERROR(E_NullPointer("right string is null"));
        return False;
    }

    return strcmp(m_data.getData(), s.m_data.getData()) > 0;
}

Bool CString::operator>(const Char *s) const
{
    if (isNull()) {
        // O3D_ERROR(E_NullPointer("left string is null"));
        return True;
    }

    if (!s) {
        // O3D_ERROR(E_NullPointer("right string is null"));
        return False;
    }

    return strcmp(m_data.getData(), s) > 0;
}

Bool CString::operator>=(const CString &s) const
{
    if (isNull()) {
        // O3D_ERROR(E_NullPointer("left string is null"));
        return True;
    }

    if (s.isNull()) {
        // O3D_ERROR(E_NullPointer("right string is null"));
        return False;
    }

    return strcmp(m_data.getData(), s.m_data.getData()) >= 0;
}

Bool CString::operator>=(const Char *s) const
{
    if (isNull()) {
        // O3D_ERROR(E_NullPointer("left string is null"));
        return True;
    }

    if (!s) {
        // O3D_ERROR(E_NullPointer("right string is null"));
        return False;
    }

    return strcmp(m_data.getData(), s) >= 0;
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
String::String(const TemplateArray<Char, -1> &copy, Int32 ithreshold) :
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
	if (m_data)	{
		/*O3D_FREE*/free(m_data);
        m_data = nullptr;
	}

	m_size = m_maxsize = 0;
}

// Initialise la taille de la chaine a newSize
void String::setCapacity(Int32 newSize)
{
    ++newSize;		// zero terminal

	// test si la nouvelle taille est plus grande que l'actuelle
	// ou si la nouvelle taille est plus petite que l'actuelle d'au moins 1*m_threshold
    if (newSize > m_maxsize || (m_maxsize > m_threshold && newSize < m_maxsize - m_threshold)) {
		// calcule la nouvelle taille (plus proche multiple de m_threshold)
        newSize = ((newSize + m_threshold - 1) / m_threshold) * m_threshold;

        if (!newSize) {
            // si la nouvelle taille est nulle
            if (m_data) {
				/*O3D_FREE*/free(m_data);
                m_data = nullptr;
                m_size = m_maxsize = 0;
			}
        } else {
            if (m_data) {
				// sinon realloue
                m_data = (WChar*)/*O3D_REALLOC*/realloc(m_data, newSize*sizeof(WChar));
            } else {
				// alloue et met un zero terminal
                m_data = (WChar*)/*O3D_MALLOC*/malloc(newSize*sizeof(WChar));
				m_data[0] = 0;
			}

            if (!m_data) {
                O3D_ERROR(E_InvalidAllocation(""));
            }

            m_maxsize = newSize;

            // test si la taille utilisée est supérieure à la taille allouée
            if (m_size > m_maxsize) {
                m_size = m_maxsize;	    // redimensionne à m_maxsize
                m_data[m_size-1] = 0;   // et replace le zéro terminal
            }

            // init for args
            if (m_size < m_maxsize) {
                m_data[m_maxsize-1] = 0x8000;
            }
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

void String::set(const String &copy, Int32 pos)
{
    Int32 len = copy.m_size - pos - 1;
    if (len < 0) {
		destroy();
		return;
	}

	// resize if necessary
    if (m_maxsize < (copy.m_size - pos)) {
		setCapacity(len);
    }

	// copy
    memmove(m_data, copy.m_data + pos, len*sizeof(WChar));

	m_data[len] = 0;

	m_size = len+1;
}

void String::set(const Char* src, Int32 pos)
{
    if (src == nullptr) {
		destroy();
		return;
	}

    Int32 srclen = (Int32)strlen(src);
    Int32 len = srclen - pos;
    if (len < 0) {
		return;
    }

	// resize if necessary
    if (m_maxsize < (srclen-pos+1)) {
		setCapacity(len);
    }

	// copy
    for (Int32 i = 0; i < len; ++i)	{
        m_data[i] = static_cast<UInt8>(*(src+pos+i));
	}

	m_data[len] = 0;
	m_size = len+1;
}

void String::set(const WChar* src, Int32 pos)
{
    if (src == nullptr)	{
		destroy();
		return;
	}

    Int32 srclen = (Int32)wcslen(src);
    Int32 len = srclen-pos;
    if (len < 0) {
		return;
    }

	// resize if necessary
    if (m_maxsize < (srclen-pos+1)) {
		setCapacity(len);
    }

	// copy
    memmove(m_data, src+pos, len*sizeof(WChar));

	m_data[len] = 0;

	m_size = len+1;
}

// Set the string size and fill with a given character
void String::setFill(WChar c, Int32 n)
{
	setCapacity(n);
	m_size = n+1;

    for (Int32 i = 0 ; i < n ; ++i) {
		m_data[i] = c;
	}

	m_data[m_size-1] = 0;
}

// Fill the string with a given character
void String::fill(WChar c)
{
    for (Int32 i = 0 ; i < (m_size-1) ; ++i) {
		m_data[i] = c;
	}

	m_data[m_size-1] = 0;
}

// Count the number of occurrence of a given char
Int32 String::count(WChar c) const
{
    if (m_size == 0) {
		return 0;
    }

    Int32 count=0;

    for (Int32 i = 0; i < m_size; ++i)	{
        if (m_data[i] == c) {
			++count;
        }
	}

	return count;
}

// Check if the string start by a given string.
Bool String::startsWith(const String &start) const
{
    Int32 len = start.length();

	// this is to lower
    if (this->length() < len) {
        return False;
    }

    for (Int32 i = 0; i < len; ++i) {
        if (m_data[i] != start[i]) {
            return False;
        }
	}

    return True;
}

// Check if the string ends by a given string.
Bool String::endsWith(const String &end) const
{
    Int32 len = end.length();

	// this is to lower
    if (this->length() < len) {
        return False;
    }

    for (Int32 i = 0, j = m_size - len - 1; j < m_size; ++i, ++j) {
        if (m_data[j] != end[i]) {
            return False;
        }
	}

    return True;
}

// delete an eventually char/string at the end of the string
Bool String::trimRight(WChar _char, Bool _repeat)
{
	if (m_size > 1)	{
        if (_repeat) {
            for (Int32 k = (Int32)m_size-2 ; k >= 0 ; --k) {
                if (m_data[k] != _char) {
                    if (k != (Int32)m_size - 2) {
						truncate(k+1);
                    }

                    return True;
				}
			}
        } else if (m_data[m_size-2] == _char) {
			truncate(m_size-2);
            return True;
		}
	}
    return False;
}

Bool String::trimRight(const String& str)
{
	// source length lesser than search string
    if (m_size < str.m_size) {
        return False;
    }

    Int32 searchPos = m_size - str.m_size;
    Int32 pos = sub(str, searchPos);

    if ((pos != -1) && (pos == searchPos)) {
		truncate(pos);
        return True;
	}

    return False;
}

WChar String::trimRight()
{
    if (length()) {
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
    if (_charList.isNull()) {
        return False;
    }

    Int32 lCounter = 0;
    const WChar * lChar = nullptr;
    const WChar * const lCharEnd = &_charList.m_data[_charList.m_size-1];

    const WChar * lCharSrc = &m_data[m_size-2];

    for (Int32 k = m_size-1 ; k > 0 ; --k) {
		lChar = _charList.m_data;

        while (lChar != lCharEnd) {
            if (*lCharSrc == *lChar) {
				break;
            }

			++lChar;
		}

        if (lChar == lCharEnd) {
			break;
        }

		--lCharSrc;
		++lCounter;
	}

    if (lCounter > 0) {
		truncate(m_size - lCounter - 1);
        return True;
	}
    return False;
}

// Trim char from the start of the string
Bool String::trimLeftChars(const String & _charList)
{
    if (_charList.isNull()) {
        return False;
    }

    Int32 lCounter = 0;
    const WChar * lChar = nullptr;
    const WChar * const lCharEnd = &_charList.m_data[_charList.m_size-1];

    const WChar * lCharSrc = m_data;

    for (Int32 k = m_size-1 ; k > 0 ; --k) {
		lChar = _charList.m_data;

        while (lChar != lCharEnd) {
			if (*lCharSrc == *lChar)
				break;

			++lChar;
		}

        if (lChar == lCharEnd) {
			break;
        }

		++lCharSrc;
		++lCounter;
	}

    if (lCounter > 0) {
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
    if (m_size > 1) {
        if (rep) {
            Int32 count = 0;

            while (m_data[count] == c) {
				count++;
			}

            memmove(m_data, m_data+count, sizeof(WChar)*(m_size-count));
			m_size = m_size-count;
			setCapacity(m_size);

            return True;
        } else {
            if (m_data[0] == c) {
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
    if (length()) {
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
    if (src.m_size <= 1) {
		return *this;
    }

    if (m_size == 0) {
		m_size = 1;
    }

    Int32 newlen = (m_size - 1) + (src.m_size - 1);

    if (m_maxsize < newlen+1) {
		setCapacity(newlen);
    }

    memmove(m_data+m_size-1, src.m_data, (src.m_size-1)*sizeof(WChar));
	m_data[newlen] = 0;

	m_size = newlen + 1;
	return *this;
}

String& String::operator+= (WChar c)
{
    if (m_size == 0) {
		m_size = 1;
    }

    if (m_maxsize < m_size+1) {
		setCapacity(m_size+1);
    }

	m_data[m_size-1] = c;
	m_data[m_size]   = 0;

	++m_size;
	return *this;
}

// ajoute s1 et s2 et le renvoi return = s1 + s2
String String::operator+ (const String &s)const
{
    if (m_size == 0) {
		return String(s);
    }

	String str(*this);

    if (s.m_size <= 1) {
		return str;
    }

    Int32 newlen = (str.m_size-1) + (s.m_size-1);

    if (str.m_maxsize < newlen+1) {
		str.setCapacity(newlen);
    }

    if (str.m_size == 0) {
		str.m_size = 1;
    }

    memmove(str.m_data+m_size-1, s.m_data, (s.m_size-1)*sizeof(WChar));
	str.m_data[newlen] = 0;

	str.m_size += s.m_size - 1;

	return str;
}

//! Append src to this and return it as a new string
String String::operator+ (WChar c) const
{
	// this is empty
    if (m_size == 0) {
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

    return wcscmp(m_data, s.m_data) == 0;
}

Bool String::operator!=(const String &s) const
{
    if (isNull() && s.isNull()) {
        return False;
    } else if (isNull() || s.isNull()) {
        return True;
    }

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

    if (_type == String::CASE_SENSITIVE) {
        return wcscmp(getData(), s.getData());
    } else {
        return o3d::wcscasecmp(getData(), s.getData());
    }
}

//! Case sensitive comparison.
Bool String::operator> (const String &s) const
{
    if (isNull()) {
        return False;
    }

    if (s.isNull()) {
        return True;
    }

    return wcscmp(m_data, s.m_data) > 0;
}

//! Case sensitive comparison.
Bool String::operator< (const String &s) const
{
    if (isNull()) {
        return True;
    }

    if (s.isNull()) {
        return False;
    }

    return wcscmp(m_data, s.m_data) < 0;
}

Bool String::operator>=(const String &s) const
{
    if (isNull() && s.isNull()) {
        return True;
    }

    if (isNull()) {
        return False;
    }

    if (s.isNull()) {
        return True;
    }

    return wcscmp(m_data, s.m_data) >= 0;
}

Bool String::operator<=(const String &s) const
{
    if (isNull() && s.isNull()) {
        return True;
    }

    if (isNull()) {
        return True;
    }

    if (s.isNull()) {
        return False;
    }

    return wcscmp(m_data, s.m_data) <= 0;
}

//  renvoie le charactère placé à la position Index
WChar& String::operator[] (Int32 index)
{
    if (index >= m_size) {
		O3D_ERROR(E_IndexOutOfRange(""));
    }

    return m_data[index];
}

const WChar& String::operator[] (Int32 index) const
{
    if (index >= m_size) {
		O3D_ERROR(E_IndexOutOfRange(""));
    }

    return m_data[index];
}

// Lowercase the entire string
String& String::lower()
{
#ifdef _MSC_VER
	_wcslwr(m_data);
#else
    Int32 i;
    for (i = 0; i < m_size-1; i++) {
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
    Int32 i;
    for (i = 0; i < m_size-1; i++) {
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

    Int32 w = 1;
    Int32 j, l = WideChar::ponctuationsPlusWs().m_size - 1;
    for (Int32 i = 0; i < m_size; ++i) {
        for (j = 0; j < l; ++j) {
            if (WideChar::ponctuationsPlusWs().m_data[j] == res.m_data[i]) {
                w = 0;
                break;
            }
        }

        // not a punctuation neither a white space, and start a new word => upper case
        if (w == 1) {
            res.m_data[i] = towupper(m_data[i]);
        }

        ++w;
    }

    return res;
}

Int32 String::sub(const String& str, Int32 pos) const
{
    Int32 len = str.m_size - 1;  // taille de la sous chaine a comparer
    Int32 i, j = 0;              // position dans la sous chaine

    if ((m_size - pos) < len) {
		return -1;	// chaine trop petite pour accepter sous chaine
    }

    for (i = pos; i < m_size; ++i) {
        if (m_data[i] == str.m_data[j]) {
			++j;
        } else {
			j = 0;
        }

        if (j == len) {
			break;
        }
	}

    if (j == len) {
		return (i-len+1);
    }

    return -1;
}

// Insert a char
String& String::insert(WChar c, Int32 pos)
{
    if (m_size == 0) {
		set(c);
		return *this;
	}

    if (pos >= m_size) {
		return *this;
    }

    Int32 NewSize = m_size; // + 1 - 1

    if (NewSize >= m_maxsize) {
		setCapacity(NewSize);
    }

    memmove(m_data + pos + 1, m_data + pos, (m_size-pos)*sizeof(WChar));
	m_data[pos] = c;

	++m_size;

	return *this;
}

String& String::insert(const String &str, Int32 pos)
{
    if (str.isEmpty()) {
		return *this;
    }

    if (m_size == 0)//m_data == nullptr)
	{
		set(str,0);
		return *this;
	}

    if (pos >= m_size) {
		return *this;
    }

    Int32 len = str.m_size - 1;
    Int32 NewSize = len + m_size - 1;

    if (NewSize >= m_maxsize) {
		setCapacity(NewSize);
    }

    memmove(m_data + pos + len, m_data + pos, (m_size-pos)*sizeof(WChar));
    memmove(&m_data[pos], str.m_data, len*sizeof(WChar));

	m_size += len;

	return *this;
}

// Reverse the string
String& String::reverse()
{
    Int32 i;
    WChar swapChar;

    for (i=0; i<m_size/2; i++) {
		swapChar = m_data[i];
		m_data[i] = m_data[m_size-i-1];
		m_data[m_size-i-1] = swapChar;
	}

	return *this;
}

// Remove many characters
String& String::remove(Int32 pos, Int32 n)
{
    if (pos >= m_size) {
		return *this;
    }

    if (n >= m_size - pos) {
		n = m_size - pos - 1;
    }

    memmove(m_data + pos, m_data + pos + n, (m_size-pos)*sizeof(WChar));
	m_data[(m_size -= n)-1] = 0;

    return *this;
}

String String::extract(Int32 pos, Int32 n)
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
        n = m_size - start + 1;
    } else {
        n = end - start + 1;
    }

    result.setCapacity(n);

    // copy
    memcpy(result.m_data, m_data + start, n*sizeof(WChar));
    result.m_size = n + 1;
    result.m_data[result.m_size-1] = '\0';

    return result;
}

// Find for a character
Int32 String::find(WChar c, Int32 pos, Bool reverse) const
{
    if (pos >= m_size) {
		return -1;
    }

    Int32 i;

    if (!reverse) {
        for (i = pos ; i < m_size ; ++i) {
            if (m_data[i] == c) {
				break;
            }
		}

        return (i < (m_size-1) ? i : -1);
    } else {
		i = pos;
        for (i = pos ; i > -1 ; --i) {
            if (m_data[i] == c) {
				break;
            }
		}

//		if ((i == 0) && (m_data[0] != c))
//			return -1;

		return i;
	}
}

Int32 String::find(const String &s, Int32 pos, Bool reverse, Bool last) const
{
    if (pos >= m_size) {
        return -1;
    }

    Int32 i;
    Int32 j;

    if (!reverse) {
        if (last) {
            Int32 match = -1;
            Bool found = False;

            for (i = pos ; i < (Int32)m_size ; ++i) {
                found = False;

                for (j = 0; j < s.m_size; ++j) {
                    if (m_data[i] == s.m_data[j]) {
                        match = i;
                        found = True;
                        break;
                    }
                }

                if (match != -1 && !found) {
                    return match;
                }
            }
        } else {
            for (i = pos ; i < (Int32)m_size ; ++i) {
                for (j = 0; j < s.m_size; ++j) {
                    if (m_data[i] == s.m_data[j]) {
                        return i;
                    }
                }
            }
        }

        return -1;
    } else {
        if (last) {
            Int32 match = -1;
            Bool found = False;

            i = pos;
            for (i = pos ; i > -1 ; --i) {
                found = False;

                for (j = 0; j < s.m_size; ++j) {
                    if (m_data[i] == s.m_data[j]) {
                        match = i;
                        found = True;
                        break;
                    }
                }

                if (match != -1 && !found) {
                    return match;
                }
            }
        } else {
            i = pos;
            for (i = pos ; i > -1 ; --i) {
                for (j = 0; j < s.m_size; ++j) {
                    if (m_data[i] == s.m_data[j]) {
                        return i;
                    }
                }
            }
        }

        return -1;
    }
}

// Truncate the string
String& String::truncate(Int32 pos)
{
    if (pos >= m_size) {
		return *this;
    }

	setCapacity(pos);
	m_size = pos + 1;
	m_data[m_size-1] = 0;

	return *this;
}

// Get a part of the string given a range
String String::sub(Int32 _start, Int32 _end) const
{
    if (m_size == 0) {
		return String("");
    }

    // relative end to absolute
    if (_end < 0) {
        _end = m_size - 1;
    } else {
        _end = o3d::min<Int32>(_end, m_size-1);
    }

    // relative start to absolute
    if (_start < 0) {
        _start = 0;
    } else {
        _start = o3d::min<Int32>(_start, m_size-1);
    }

    if (_end <= _start) {
		return String("");
    }

    const Int32 lSize = _end - _start;

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

    while ((dstPos = sub(find, dstPos)) != -1) {
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
    if (m_size == 0) {
		return *this;
    }

    for (Int32 i = 0; i < m_size; ++i) {
        if (m_data[i] == c) {
			m_data[i] = n;
        }
	}

	return *this;
}

// Replace each occurrence of a string by another string
String& String::replace(const String& find, const String& by)
{
    if (m_size == 0 || find.m_size == 0 || by.m_size == 0) {
		return *this;
    }

    Int32 dstPos = 0; // starting at position 0
    Int32 srcPos;

    while ((dstPos = sub(find, dstPos)) != -1) {
		srcPos = dstPos + find.m_size - 1;

        if (by.m_size > find.m_size) {
			setCapacity(m_size-1+ by.m_size-find.m_size);
        }

        memmove(m_data+dstPos+by.m_size-1, m_data+srcPos, sizeof(WChar)*(m_size-srcPos));
        memmove(m_data+dstPos, by.m_data, sizeof(WChar)*(by.m_size-1));

		m_size += by.m_size - find.m_size;
	}

	// resize the m_maxsize if necessary, only when 'by' is lower than 'find'
    if (by.m_size < find.m_size) {
		setCapacity(m_size-1);
    }

	return *this;
}

// Replace a part of the string starting at a position
String& String::replace(const String &str, Int32 n)
{
    if (str.m_size == 0 || m_size == 0) {
		return *this;
    }

    if (n >= m_size) {
		return *this;
    }

    Int32 add = str.m_size - 1;

    if ((add + n) > (m_size - 1)) {
		add -= (add + n) - (m_size - 1);
    }

    memmove(m_data+n, str.m_data, add*sizeof(WChar));

	return *this;
}

// Replace a char at a given position
String& String::replaceChar(WChar c, Int32 n)
{
    if (n >= m_size) {
		return *this;
    }

	m_data[n] = c;
	return *this;
}

// Concat a base 10 integer
void String::concat(Int32 i, Int32 radix)
{
    WChar str[16] = { 0 };
#ifdef _MSC_VER
    _itow(i, str, radix);
#else
    if (radix == 16 ) {
        swprintf(str, 16, L"%x", i);
    } else {
        swprintf(str, 16, L"%i", i);
    }
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
    if (radix == 16 ) {
        swprintf(str, 16, L"%x", i);
    } else {
        swprintf(str, 16, L"%u", i);
    }
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
    if (radix == 16 ) {
        swprintf(str, 32, L"%llx", i);
    } else {
        swprintf(str, 32, L"%lld", i);
    }
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
    if (radix == 16 ) {
        swprintf(str, 32, L"%llx", i);
    } else {
        swprintf(str, 32, L"%llu", i);
    }
#endif
	(*this) += str;
}

// Concat a float
void String::concat(Float f, Int32 decimals)
{
    WChar str[16] = { 0 };
    if (decimals >= 10) {
        decimals = o3d::min(16, decimals);

        WChar format[] = L"%.xxf";
        format[2] = decimals / 10 + '0';
        format[3] = (decimals - ((decimals / 10) * 10)) + '0';

        swprintf(str, 16, format, f);
    } else if (decimals >= 0) {
        WChar format[] = L"%.xf";
        format[2] = o3d::min(9, decimals) + '0';

        swprintf(str, 16, format, f);
    } else {
        swprintf(str, 16, L"%g", f);
    }

	(*this) += str;
}

// Concat a double
void String::concat(Double d, Int32 decimals)
{
    WChar str[32] = { 0 };
    if (decimals >= 10) {
        decimals = o3d::min(32, decimals);

        WChar format[] = L"%.xxf";
        format[2] = decimals / 10 + '0';
        format[3] = (decimals - ((decimals / 10) * 10)) + '0';

        swprintf(str, 32, format, d);
    } else if (decimals >= 0) {
        WChar format[] = L"%.xf";
        format[2] = o3d::min(9, decimals) + '0';

        swprintf(str, 32, format, d);
    } else {
        swprintf(str, 32, L"%g", d);
    }

    (*this) += str;
}

//
// arg
//

inline Bool hasArg(const WChar *data, Int32 dataSize, Int32 dataMaxSize)
{
    return ((dataSize+1 <= dataMaxSize) && ((data[dataMaxSize-1] & 0x80ff) >= 0x8000));
}

void String::allocateArg()
{
    // need 1 more extra char outsides of the string data
    if (m_size+1 < m_maxsize) {
        setCapacity(m_size-1+1);  // minus 1
    }

    m_data[m_maxsize-1] = 0x8000;
}

inline Int32 currentArg(const WChar *data, Int32 dataMaxSize)
{
    return data[dataMaxSize-1] & 0x00ff;
}

inline void incArg(WChar *data, Int32 dataMaxSize, Int32 arg)
{
    if (arg < 99) {
        ++arg;
    }

    data[dataMaxSize-1] = (WChar)(0x8000 | arg);
}

inline Int32 subArg(const WChar *arg, Int32 argSize, WChar *data, Int32 dataSize, Int32 pos)
{
    Int32 len = argSize - 1;  // taille de la sous chaine a comparer
    Int32 i, j = 0;           // position dans la sous chaine

    if ((dataSize - pos) < len) {
        return -1;	// chaine trop petite pour accepter sous chaine
    }

    for (i = pos; i < dataSize; ++i) {
        if (data[i] == arg[j]) {
            ++j;
        } else {
            j = 0;
        }

        if (j == len) {
            break;
        }
    }

    if (j == len) {
        return (i-len+1);
    }

    return -1;
}

void String::replaceArg(const WChar *arg, Int32 argSize, const String &by)
{
    Int32 dstPos = 0; // starting at position 0
    Int32 srcPos;

    while ((dstPos = subArg(arg, argSize, m_data, m_size, dstPos)) != -1) {
        srcPos = dstPos + argSize - 1;

        if (by.m_size > argSize) {
            setCapacity(m_size-1+1+by.m_size-argSize);  // conserve arg in data
        }

        memmove(m_data+dstPos+by.m_size-1, m_data+srcPos, sizeof(WChar)*(m_size-srcPos));
        memmove(m_data+dstPos, by.m_data, sizeof(WChar)*(by.m_size-1));

        m_size += by.m_size - argSize;
    }

    // resize the m_maxsize if necessary, only when 'by' is lower than 'arg'
    if (by.m_size < argSize) {
        setCapacity(m_size-1+1);  // conserve arg in data
    }

    m_data[m_size-1] = 0;
}

String& String::arg(const Char &c)
{   
    if (isEmpty()) {
        return *this;
    }

    if (!hasArg(m_data, m_size, m_maxsize)) {
        allocateArg();
    }

    Int32 arg = currentArg(m_data, m_maxsize);
    WChar placeholder[5] = {'{', '}', '}', '}', '\0'};
    Int32 placeholderSize = 4;

    if (arg < 10) {
        placeholder[1] = WChar(arg + '0');
        placeholder[3] = '\0';
    } else {
        placeholder[1] = WChar((arg / 10) + '0');
        placeholder[2] = WChar(arg - ((arg / 10) * 10) + '0');
        ++placeholderSize;
    }

    String s;
    s.concat(c);

    replaceArg(placeholder, placeholderSize, s);

    incArg(m_data, m_maxsize, arg);
    return *this;
}

String& String::arg(const WChar &w)
{
    if (isEmpty()) {
        return *this;
    }

    if (!hasArg(m_data, m_size, m_maxsize)) {
        allocateArg();
    }

    Int32 arg = currentArg(m_data, m_maxsize);
    WChar placeholder[5] = {'{', '}', '}', '}', '\0'};
    Int32 placeholderSize = 4;

    if (arg < 10) {
        placeholder[1] = WChar(arg + '0');
        placeholder[3] = '\0';
    } else {
        placeholder[1] = WChar((arg / 10) + '0');
        placeholder[2] = WChar(arg - ((arg / 10) * 10) + '0');
        ++placeholderSize;
    }

    String s;
    s.concat(w);

    replaceArg(placeholder, placeholderSize, s);

    incArg(m_data, m_maxsize, arg);
    return *this;
}

String& String::arg(const Int32 &i, Int32 fieldWidth, Int32 base, WChar fillChar)
{
    if (isEmpty()) {
        return *this;
    }

    if (!hasArg(m_data, m_size, m_maxsize)) {
        allocateArg();
    }

    Int32 arg = currentArg(m_data, m_maxsize);
    WChar placeholder[5] = {'{', '}', '}', '}', '\0'};
    Int32 placeholderSize = 4;

    if (arg < 10) {
        placeholder[1] = WChar(arg + '0');
        placeholder[3] = '\0';
    } else {
        placeholder[1] = WChar((arg / 10) + '0');
        placeholder[2] = WChar(arg - ((arg / 10) * 10) + '0');
        ++placeholderSize;
    }

    String s;
    s.concat(i, base);

    if (fieldWidth > 0 && s.length() < fieldWidth) {
        String p;
        p.setFill(fillChar, fieldWidth - s.length());
        s = p + s;
    }

    replaceArg(placeholder, placeholderSize, s);

    incArg(m_data, m_maxsize, arg);
    return *this;
}

String& String::arg(const UInt32 &i, Int32 fieldWidth, Int32 base, WChar fillChar)
{
    if (isEmpty()) {
        return *this;
    }

    if (!hasArg(m_data, m_size, m_maxsize)) {
        allocateArg();
    }

    Int32 arg = currentArg(m_data, m_maxsize);
    WChar placeholder[5] = {'{', '}', '}', '}', '\0'};
    Int32 placeholderSize = 4;

    if (arg < 10) {
        placeholder[1] = WChar(arg + '0');
        placeholder[3] = '\0';
    } else {
        placeholder[1] = WChar((arg / 10) + '0');
        placeholder[2] = WChar(arg - ((arg / 10) * 10) + '0');
        ++placeholderSize;
    }

    String s;
    s.concat(i, base);

    if (fieldWidth > 0 && s.length() < fieldWidth) {
        String p;
        p.setFill(fillChar, fieldWidth - s.length());
        s = p + s;
    }

    replaceArg(placeholder, placeholderSize, s);

    incArg(m_data, m_maxsize, arg);
    return *this;
}

String& String::arg(const Int64 &i, Int32 fieldWidth, Int32 base, WChar fillChar)
{
    if (isEmpty()) {
        return *this;
    }

    if (!hasArg(m_data, m_size, m_maxsize)) {
        allocateArg();
    }

    Int32 arg = currentArg(m_data, m_maxsize);
    WChar placeholder[5] = {'{', '}', '}', '}', '\0'};
    Int32 placeholderSize = 4;

    if (arg < 10) {
        placeholder[1] = WChar(arg + '0');
        placeholder[3] = '\0';
    } else {
        placeholder[1] = WChar((arg / 10) + '0');
        placeholder[2] = WChar(arg - ((arg / 10) * 10) + '0');
        ++placeholderSize;
    }

    String s;
    s.concat(i, base);

    if (fieldWidth > 0 && s.length() < fieldWidth) {
        String p;
        p.setFill(fillChar, fieldWidth - s.length());
        s = p + s;
    }

    replaceArg(placeholder, placeholderSize, s);

    incArg(m_data, m_maxsize, arg);
    return *this;
}

String& String::arg(const UInt64 &i, Int32 fieldWidth, Int32 base, WChar fillChar)
{
    if (isEmpty()) {
        return *this;
    }

    if (!hasArg(m_data, m_size, m_maxsize)) {
        allocateArg();
    }

    Int32 arg = currentArg(m_data, m_maxsize);
    WChar placeholder[5] = {'{', '}', '}', '}', '\0'};
    Int32 placeholderSize = 4;

    if (arg < 10) {
        placeholder[1] = WChar(arg + '0');
        placeholder[3] = '\0';
    } else {
        placeholder[1] = WChar((arg / 10) + '0');
        placeholder[2] = WChar(arg - ((arg / 10) * 10) + '0');
        ++placeholderSize;
    }

    String s;
    s.concat(i, base);

    if (fieldWidth > 0 && s.length() < fieldWidth) {
        String p;
        p.setFill(fillChar, fieldWidth - s.length());
        s = p + s;
    }

    replaceArg(placeholder, placeholderSize, s);

    incArg(m_data, m_maxsize, arg);
    return *this;
}

String& String::arg(const Float &f, Int32 decimals, WChar separator)
{
    if (isEmpty()) {
        return *this;
    }

    if (!hasArg(m_data, m_size, m_maxsize)) {
        allocateArg();
    }

    Int32 arg = currentArg(m_data, m_maxsize);
    WChar placeholder[5] = {'{', '}', '}', '}', '\0'};
    Int32 placeholderSize = 4;

    if (arg < 10) {
        placeholder[1] = WChar(arg + '0');
        placeholder[3] = '\0';
    } else {
        placeholder[1] = WChar((arg / 10) + '0');
        placeholder[2] = WChar(arg - ((arg / 10) * 10) + '0');
        ++placeholderSize;
    }

    String s;
    s.concat(f, decimals);  // @todo separator or local

    replaceArg(placeholder, placeholderSize, s);

    incArg(m_data, m_maxsize, arg);
    return *this;
}

String& String::arg(const Double &d, Int32 decimals, WChar separator)
{
    if (isEmpty()) {
        return *this;
    }

    if (!hasArg(m_data, m_size, m_maxsize)) {
        allocateArg();
    }

    Int32 arg = currentArg(m_data, m_maxsize);
    WChar placeholder[5] = {'{', '}', '}', '}', '\0'};
    Int32 placeholderSize = 4;

    if (arg < 10) {
        placeholder[1] = WChar(arg + '0');
        placeholder[3] = '\0';
    } else {
        placeholder[1] = WChar((arg / 10) + '0');
        placeholder[2] = WChar(arg - ((arg / 10) * 10) + '0');
        ++placeholderSize;
    }

    String s;
    s.concat(d, decimals);  // @todo separator or local

    replaceArg(placeholder, placeholderSize, s);

    incArg(m_data, m_maxsize, arg);
    return *this;
}

String& String::arg(const String &s, Int32 fieldWidth, WChar fillChar)
{
    if (isEmpty()) {
        return *this;
    }

    if (!hasArg(m_data, m_size, m_maxsize)) {
        allocateArg();
    }

    Int32 arg = currentArg(m_data, m_maxsize);
    WChar placeholder[5] = {'{', '}', '}', '}', '\0'};
    Int32 placeholderSize = 4;

    if (arg < 10) {
        placeholder[1] = WChar(arg + '0');
        placeholder[3] = '\0';
    } else {
        placeholder[1] = WChar((arg / 10) + '0');
        placeholder[2] = WChar(arg - ((arg / 10) * 10) + '0');
        ++placeholderSize;
    }

    if (fieldWidth > 0 && s.length() < fieldWidth) {
        String p;
        p.setFill(fillChar, fieldWidth - s.length());
        p = p + s;

        replaceArg(placeholder, placeholderSize, p);
    } else {
        replaceArg(placeholder, placeholderSize, s);
    }

    incArg(m_data, m_maxsize, arg);
    return *this;
}

//
// convert to
//

Char String::toChar(Int32 pos) const
{
    return static_cast<Char>(operator[](pos));
}

WChar String::toWChar(Int32 pos) const
{
	return operator[](pos);
}

Int32 String::toInt32(Int32 pos) const
{
    if (m_size <= 1) {
        return 0;
    }

    WChar *end;
	return wcstol(&m_data[pos], &end, 10);
}

UInt32 String::toUInt32(Int32 pos) const
{
    if (m_size <= 1) {
        return 0;
    }

    WChar *end;
	return wcstoul(&m_data[pos], &end, 10);
}

Int64 String::toInt64(Int32 pos) const
{
    if (m_size <= 1) {
        return 0;
    }

    WChar *end;
    return wcstol(&m_data[pos], &end, 10);
}

UInt64 String::toUInt64(Int32 pos) const
{
    if (m_size <= 1) {
        return 0;
    }

    WChar *end;
    return wcstoul(&m_data[pos], &end, 10);
}

Float String::toFloat(Int32 pos) const
{
    if (m_size <= 1) {
        return 0.f;
    }

    WChar *end;
    return (Float)wcstod(&m_data[pos], &end);
}

Double String::toDouble(Int32 pos) const
{
    if (m_size <= 1) {
        return 0.0;
    }

    WChar *end;
	return wcstod(&m_data[pos], &end);
}

//--------------------------------------------------------------------------------------
// String conversion
//--------------------------------------------------------------------------------------

CString String::toAscii() const
{
	// null string
    if (!m_data) {
		return CString();
    }

	// empty string
    if (m_size == 1) {
		return CString("");
    }

	CString result(m_size);

    for (Int32 i = 0; i < m_size; ++i) {
        result[i] = static_cast<Char>(m_data[i]);
	}

	return result;
}

CString String::toUtf8() const
{
	// null string
    if (!m_data) {
		return CString();
    }

	// empty string
    if (m_size == 1) {
		return CString("");
    }

#ifndef O3D_WINAPI_SYS
	ArrayChar temp(m_size*2, m_size);
    WChar c;

	// WIDECHAR size is 4
    for (Int32 i = 0; i < m_size; ++i) {
		c = m_data[i];

        if (c < 0x80) {
			// 0xxxxxxx
            temp.push(static_cast<Char>(c));
        } else if (c < 0x800) {
			// C0          80
			// 110xxxxx 10xxxxxx
            temp.push(static_cast<Char>(0xC0 | (c >> 6)));
            temp.push(static_cast<Char>(0x80 | (c & 0x3F)));
        } else if (c < 0x8000) {
			// E0       80       80
			// 1110xxxx 10xxxxxx 10xxxxxx
            temp.push(static_cast<Char>(0xE0 | (c >> 12)));
            temp.push(static_cast<Char>(0x80 | (c >> 6 & 0x3F)));
            temp.push(static_cast<Char>(0x80 | (c & 0x3F)));
        } else {
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
#endif // O3D_WINAPI_SYS
}

void String::fromUtf8(const Char* utf8, Int32 maxSize)
{
	// The string must be destroyed if the specified pointer is null
	destroy();

    // null or empty input string
    if (utf8 == nullptr) {
		return;
    }

    Int32 utf8Len;
    if (maxSize == 0) {
		utf8Len = strlen(utf8);
    } else {
		utf8Len = maxSize;
    }

#ifndef O3D_WINAPI_SYS
	//WIDECHAR size is 4
    for (Int32 i = 0; i < utf8Len; ++i) {
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
#endif // O3D_WINAPI_SYS
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
    if (!data.readFromFile(is)) {
        O3D_ERROR(E_StringUnderflow(""));
    }

    if (data.isValid() && data[data.length()] != 0) {
        O3D_ERROR(E_StringOverflow(""));
    }

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
