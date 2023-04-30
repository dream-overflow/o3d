/**
 * @file string.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_STRING_H
#define _O3D_STRING_H

#include "smartarray.h"
#include "memorydbg.h"

#ifdef O3D_WINDOWS
#include <stdlib.h>
#include <stdio.h>
#else
#include <unistd.h>
#endif

#include <wchar.h>

namespace o3d
{

class InStream;
class OutStream;
class StringArg;

#ifdef __APPLE__ // APPLE
//! Perform a lower-case comparison of char strings.
//! @return 0 if equal.
Int32 strcasecmp(const Char *s1, const Char *s2);

//! Perform a lower-case comparison of wchar_t strings.
//! @return 0 if equal.
Int32 wcscasecmp(const WChar *s1, const WChar *s2);

//! Duplicate a char string. The new string is allocated with malloc,
//! that means it can be deleted using free.
inline Char *strdup(const Char *str) { return ::strdup(str); }

//! Duplicate a wchar_t string. The new string is allocated with malloc,
//! that means it can be deleted using free.
WChar *wcsdup(const WChar *str);

#elif defined O3D_WINDOWS
//! Perform a lower-case comparison of char strings.
//! @return 0 if equal.
inline Int32 strcasecmp(const Char *s1, const Char *s2) { return ::_stricmp(s1, s2); }

//! Perform a lower-case comparison of wchar_t strings.
//! @return 0 if equal.
inline Int32 wcscasecmp(const WChar *s1, const WChar *s2) { return ::_wcsicmp(s1, s2); }

//! Duplicate a char string. The new string is allocated with malloc,
//! that means it can be deleted using free.
inline Char *strdup(const Char *str) { return ::_strdup(str); }

//! Duplicate a wchar_t string. The new string is allocated with malloc,
//! that means it can be deleted using free.
inline WChar *wcsdup(const WChar *str) { return ::_wcsdup(str); }

#else // GNU/Linux, BSD...
//! Perform a lower-case comparison of char strings.
//! @return 0 if equal.
inline Int32 strcasecmp(const Char *s1, const Char *s2) { return ::strcasecmp(s1, s2); }

//! Perform a lower-case comparison of wchar_t strings.
//! @return 0 if equal.
inline Int32 wcscasecmp(const WChar *s1, const WChar *s2) { return ::wcscasecmp(s1, s2); }

//! Duplicate a char string. The new string is allocated with malloc,
//! that means it can be deleted using free.
inline Char *strdup(const Char *str) { return ::strdup(str); }

//! Duplicate a wchar_t string. The new string is allocated with malloc,
//! that means it can be deleted using free.
inline WChar *wcsdup(const WChar *str) { return ::wcsdup(str); }

#endif // __APPLE__

/**
 * @brief The C style mutable string container. It use of a sharable data container.
 * @note To manipulate a string prefers the usage of o3d::String.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2012-01-01
 * They are mutable, it means a change to the content of this string change any other
 * references to it and to its internal shared array.
 * This class is mainly used to works with C style string. Conversions from o3d::String
 * and o3d::CString are given, and it can be done from and to UTF-8 or ASCII formats.
 * CString can be used in an unordered map. @see struct hash<o3d::CString>.
 */
class O3D_API CString
{
public:

	//! Construct an empty cstring.
	CString();

	//! Construct an empty cstring full of '0' according to a given size.
	//! @param strlen Length of the string to create (don't count terminal zero).
    explicit CString(Int32 strlen);

	//! Construct a cstring from a source char* (must be 0 terminated).
	//! @param source The string to copy.
    CString(const Char *copy);

    //! Copy constructor.
    //! @param dup
    CString(const CString &dup);

	//! Construct using a valid string and len.
	//! @param source The string to copy.
	//! @param strlen Length of the string to copy/own (don't count terminal zero).
	//! @param ref If TRUE the source is not copied, only refer it, and delete it.
    CString(const Char *copy, Int32 strlen);

	//! Construct using a valid string and len.
	//! @param source The string to copy or to own.
	//! @param strlen Length of the string to copy/own (don't count terminal zero).
	//! @param own If TRUE the source is not copied, only refer it, and delete it.
    CString(Char *source, Int32 strlen, Bool own);

	//! Destructor.
	~CString();

    //! Assign operator.
    //! @param dup
    CString& operator=(const CString &dup);

	//! Destroy the content.
	void destroy();

    //! Get string data.
    inline Char* getData() { return m_data.getData(); }

	//! Get string data (read only).
    inline const Char* getData() const { return m_data.getData(); }

    //! Get the string data as a new SmartArrayUInt8 without terminal zero.
    SmartArrayUInt8 getBytes() const;

	//! Get data at specified index (read only).
    inline const Char& operator[] (Int32 index) const { return m_data[index]; }

	//! Get data at specified index.
    inline Char& operator[] (Int32 index) { return m_data[index]; }

	//! Length of the string.
    inline Int32 length() const { return (m_data.getNumElt() == 0 ? 0 : m_data.getNumElt()-1); }

	//! Check if the string content is not empty.
    inline Bool isValid() const { return (m_data.getSize() > 1); }

	//! Check if the string content is empty.
    inline Bool isEmpty() const { return (m_data.getSize() <= 1); }

	//! Check if the string in not allocated.
    inline Bool isNull() const { return (m_data.getSize() == 0); }

    //! Case sensitive comparison.
    //! @return True if s < *this otherwise false.
    Bool operator< (const CString& s) const;

    //! Case sensitive comparison.
    //! @return True if s < *this otherwise false.
    Bool operator< (const Char* s) const;

    //! Case sensitive comparison.
    //! @return True if s < *this otherwise false.
    Bool operator<= (const CString& s) const;

    //! Case sensitive comparison.
    //! @return True if s <= *this otherwise false.
    Bool operator<= (const Char* s) const;

    //! Case sensitive comparison.
    //! @return True if s > *this otherwise false.
    Bool operator> (const CString& s) const;

    //! Case sensitive comparison.
    //! @return True if s > *this otherwise false.
    Bool operator> (const Char* s) const;

    //! Case sensitive comparison.
    //! @return True if s > *this otherwise false.
    Bool operator>= (const CString& s) const;

    //! Case sensitive comparison.
    //! @return True if s > *this otherwise false.
    Bool operator>= (const Char* s) const;

    //! Case sensitive comparison.
    //! @return True if s == *this otherwise false.
    Bool operator== (const CString& s) const;

    //! Case sensitive comparison.
    //! @return True if s == *this otherwise false.
    Bool operator== (const Char* s) const;

    //! Case sensitive comparison.
    //! @return True if s != *this otherwise false.
    Bool operator!= (const CString& s) const;

    //! Case sensitive comparison.
    //! @return True if s != *this otherwise false.
    Bool operator!= (const Char* s) const;

	// serialization
    Bool writeToFile(OutStream &os) const { return m_data.writeToFile(os); }
    Bool readFromFile(InStream &is) { return m_data.readFromFile(is); }

private:

	SmartArrayChar m_data;
};

template <class T, Int32 Align>
class O3D_API_TEMPLATE TemplateArray;

/**
 * @brief Mutable and dynamic size wide char string for manipulations using a shared object.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2002-01-01
 * String can be used in an unordered map with @see struct hash<o3d::String>.
 * @note On Windows String are UCS2, on others UCS4 is used.
 * @todo See for a support for deep and shallow copy or a IString class.
 */
class O3D_API String
{
public:

	//! Define what type of comparison must be applied.
	enum ComparisonPolicy
	{
		CASE_SENSITIVE = 0,
		CASE_INSENSITIVE
	};

	//! Default constructor (use a 0 default maxsize, and a threshold of 256 bytes).
	String() :
        m_data(nullptr),
		m_size(0),
		m_maxsize(0),
        m_threshold(256)
    {
    }

	//! Define a base maxsize and threshold.
    String(Int32 isize, Int32 ithreshold) :
        m_data(nullptr),
		m_size(0),
		m_maxsize(0),
        m_threshold(ithreshold)
	{
		setCapacity(isize);
	}

	//! Copy constructor.
	String(const String &copy) :
        m_data(nullptr),
		m_size(0),
		m_maxsize(0),
        m_threshold(copy.m_threshold)
	{
		set(copy,0);
    }

    //! Copy constructor from a ArrayChar, default assume ArrayChar is UTF8
    String(const TemplateArray<Char, -1> &copy, Int32 ithreshold = 256);

    //! Copy constructor from a CString, default assume CString is UTF8
    String(const CString &copy, Int32 ithreshold = 256) :
        m_data(nullptr),
        m_size(0),
        m_maxsize(0),
        m_threshold(ithreshold)
    {
        fromUtf8(copy.getData(), copy.length());
    }

	//! Build the string from char* (must be 0 terminated).
    String(const Char* copy, Int32 ithreshold = 256) :
        m_data(nullptr),
		m_size(0),
		m_maxsize(0),
        m_threshold(ithreshold)
	{
		set(copy,0);
	}

	//! Build the string from wide char* (must be 0 terminated).
    String(const WChar* copy, Int32 ithreshold = 256) :
        m_data(nullptr),
		m_size(0),
		m_maxsize(0),
        m_threshold(ithreshold)
	{
		set(copy,0);
	}

    //! Destructor.
    ~String();

    //
    // Static methods
    //

    //! Return an null static string.
    static const String& getNull();

	//! Build a new String with like sprintf method.
    static String print(const Char* str, ...);

	//! Build a new String with like wsprintf method.
    static String print(const WChar* str, ...);

    //
    // String methods
    //

    //! Resize the string, +1 is added to count the zero terminal.
    void setCapacity(Int32 newSize);

    //! Destroy the string content and reset its size.
	void destroy();

	//! Get the length of the string (don't count the terminal zero).
    inline Int32 length() const { return (m_size == 0) ? 0 : m_size-1; }

	//! Check if the string content is not empty.
    inline Bool isValid() const { return (m_size > 1); }

	//! Check if the string content is empty.
    inline Bool isEmpty() const { return (m_size <= 1); }

	//! Check if the string in not allocated.
    inline Bool isNull() const { return (m_size == 0); }

	//! Convert the string to ASCII as best as possible.
	CString toAscii() const;

	//! Convert the string to UTF8.
	CString toUtf8() const;

	//! Return the const wchar* data (const version).
    inline const WChar* getData() const { return m_data; }

	//! Return the const wchar* data.
    inline WChar* getData() { return m_data; }

	//! Initialize from an UTF8 char*.
    void fromUtf8(const Char* utf8, Int32 maxSize = 0);

	//! Set the string with a character
    void set(WChar c);

	//! Set the string given another string, copying it starting from a given position.
	//! @param src Source string
	//! @param pos Start position into the source string
	//! @note It is easier to make a truncate at the start of the string using this methods.
	//! e.g. String x = "1234567"; x.set(x,3); -> x == "4567" && x.length() == 4.
    void set(const String &src, Int32 pos);

	//! Set the string given another string, copying it starting from a given position.
	//! @param src source string (C string)
	//! @param pos Start position into the source string
	//! @note It is easier to make a truncate at the start of the string using this methods.
	//! e.g. String x = "1234567"; x.set(x,3); -> x == "4567" && x.length() == 4.
    void set(const Char *src, Int32 pos);

	//! Set the string given another string, copying it starting from a given position.
	//! @param src source string (WChar string)
	//! @param pos Start position into the source string
	//! @note It is easier to make a truncate at the start of the string using this methods.
	//! e.g. String x = "1234567"; x.set(x,3); -> x == "4567" && x.length() == 4.
    void set(const WChar *src, Int32 pos);

	//! Set the string size and fill with a given character
	//! @param n The size to set
	//! @param c The char to fill with
    void setFill(WChar c, Int32 n);

	//! Fill the string with a given character
    void fill(WChar c);

	//! change brutally the content size of the string (be-careful when using it)
    inline void setSize(Int32 newsize) { m_size = newsize; }

    //
    // Construction operators.
    //

	//! Assign the content of src to this
	inline String& operator= (const String &src)
	{
		set(src, 0);
		return *this;
	}

	//! Assign the content of src to this
    inline String& operator= (const Char* src)
	{
		set(src, 0);
		return *this;
	}

	//! Assign the content of src to this
    inline String& operator= (const WChar* src)
	{
		set(src, 0);
		return *this;
	}

	//! Assign the content of src to this
    inline String& operator= (WChar src)
	{
		set(src);
		return *this;
	}

	//! Append src to this
	String& operator+= (const String &src);

	//! Append src to this
    String& operator+= (WChar c);

	//! Append src to this and return it as a new string
	String operator+ (const String &s) const;

	//! Append src to this and return it as a new string
    String operator+ (WChar c) const;

    inline friend /*O3D_API*/ String operator+ (const Char* s1, const String &s2)
	{
		String s(s1);
		return s + s2;
	}

    //
	// Compare
    //

	//! Case sensitive comparison.
	//! @return True if the two string are equals.
    Bool operator== (const String& s) const;

	//! Comparison.
	//! @param _type O3DString::CaseSensitive or String::CaseInsensitive.
	//! @return 0 if equals, otherwise -1 or +1.
    Int32 compare(
		const String & _string,
		ComparisonPolicy _type = String::CASE_SENSITIVE) const;

	//! Case sensitive comparison.
    //! @return True if the two string are differents.
    Bool operator!= (const String& s) const;

	//! Case sensitive comparison.
	//! @return True if s > *this otherwise false.
    Bool operator> (const String& s) const;

	//! Case sensitive comparison.
	//! @return True if s < *this otherwise false.
    Bool operator< (const String& s) const;

	//! Case sensitive comparison.
	//! @return True if s >= *this otherwise false.
    Bool operator>= (const String &s) const;

	//! Return i if s1 <= s2 else return 0
    Bool operator<= (const String& s) const;

    //
    // String manipulation
    //


    //! Delete an eventually char at the end of the string
    Bool trimRight(WChar c, Bool repeat = False);

    //! Delete an eventually string at the end of the string
    Bool trimRight(const String& str);

    //! delete the last character of the string (if it is not empty)
    //! @return the removed char value (0 mean empty string)
    WChar trimRight();

    //! Remove all chars from the end of the string which are contained in
    //! the provided string. You must considered it as a array of chars instead of
    //! a real string.
    //! @param _charList the list of chars
    //! @return TRUE if chars were removed
    Bool trimRightChars(const String &charList);

    //! Remove all chars at the beginning of the string which are contained in
    //! the provided string. You must considered it as a array of chars instead of
    //! a real string.
    //! @param _charList the list of characters
    //! @return TRUE if chars were removed
    Bool trimLeftChars(const String &charList);

    //! Delete an eventually char at the start of the string,
    //! @param repeat to true for repeat until data[0] == c
    //! @return TRUE if one or more occurrence of the character are removed.
    Bool trimLeft(WChar c, Bool repeat = False);

    //! delete the first character of the string (if it is not empty)
    //! @return the removed char value (0 mean empty string)
    WChar trimLeft();

	//! Find for a substring occurrence, starting at a given position.
	//! @param str The substring to find
	//! @param pos The starting position for search
	//! @return The position of the first occurrence of the substring, or -1 if not found
    Int32 sub(const String& str, Int32 pos) const;

	//! Find a char into the string.
	//! @param c The char to find
	//! @param reverse Start by the beginning of the string if false, and by the end if true
	//! @return The position of the first occurrence of the char found, or -1 if not found
    Int32 find(WChar c, Int32 pos = 0, Bool reverse = False) const;

    //! Find a char into the string according to a string containing a list of characters.
    //! @param s The string containing the list of possible characters to look for.
    //! @param reverse Start by the beginning of the string if false, and by the end if true
    //! @param last If True continue for searching until there is contigues found characeters.
    //! @return The position of the first occurrence of the char found, or -1 if not found
    Int32 find(const String &s, Int32 pos = 0, Bool reverse=False, Bool last=False) const;

	//! Find a char starting by the end of the string.
	//! @param c The char to find
	//! @return The position of the first occurrence of the char found, or -1 if not found
    inline Int32 reverseFind(WChar c) const
	{
        return find(c, m_size-1, True);
	}

	//! Count the number of occurrence of a given char.
	//! @param c The char to find
	//! @return The number of occurrence found
    Int32 count(WChar c) const;

	//! Check if the string starts by a given string.
	//! @param start String to search at the start
	//! @return True if the string starts by the string given in parameter
    Bool startsWith(const String &start) const;

	//! Check if the string ends by a given string.
	//! @param start String to search at the end
	//! @return True if the string ends by the string given in parameter
    Bool endsWith(const String &end) const;

    //! Check the validity of a position into the string
    inline Bool isIndex(Int32 index) const { return (index < m_size); }

    //! Get the char given an index position
    WChar& operator[] (Int32 index);

    //! get the char given an index position (const version)
    const WChar& operator[] (Int32 Index) const;

	//! Convert the string to its lower case
    String& lower();
	//! Convert the string to its upper case
    String& upper();

    //! Returns a new string containing the lower case of this.
    String toLower() const;
    //! Returns a new string containing the upper case of this.
    String toUpper() const;

    /**
     * @brief capitalize Returns a new string with the first char in upper case.
     * @return
     */
    String capitalize() const;

    /**
     * @brief title Returns a new string with first letter of each word in upper case.
     * @return
     * @note Separators are Char::punctuation().
     */
    String title() const;

	//! Insert a character at the given position @em pos
    String& insert(WChar c, Int32 pos);

	//! Insert a string at the given position @em pos
    String& insert(const String &str, Int32 pos);

	//! Insert a character at the first position
    inline String& prepend(WChar c) { return insert(c, 0); }

	//! Insert a character at the last position
    inline String& append(WChar c) { return *this += c; }

	//! Remove @em n character starting at position @em pos
    String& remove(Int32 pos, Int32 n);

    //! Copy and remove n character starting at position pos and returns the new string.
    String extract(Int32 pos, Int32 n);

    /**
     * @brief Get a part of the string
     * @param start Starting postion
     * @param end Ending position or -1 for end
     * @return A new string
     */
    String slice(Int32 start, Int32 end) const;

    /**
     * @brief sub Get a substring given a range of position.
     * @param start The starting position
     * @param end The ending position
     * @return The substring. Can be empty.
     */
    String sub(Int32 start, Int32 end = -1) const;

	//! Replace all occurrence of the character @em c by the character @em n
    String& replace(WChar c, WChar n);

	//! replace a substring (find) by another substring (by)
	String& replace(const String& find, const String& by);

    //! Replace the substring located at n by str, for length of str and keeping the actual length
    String& replace(const String& str, const Int32 n);

	//! Replace the character at the position n by the character given by c
    String& replaceChar(WChar c, Int32 n);

	//! Remove a substring
	String& remove(const String &find);

	//! Reverse the content of the string
	String& reverse();

	//! Truncate the string at a given position
    String& truncate(Int32 pos);

    //
    // Concatenate to this.
    //

	//! Append an integer32.
    void concat(Int32 i, Int32 radix = 10);

	//! Append an unsigned integer32.
    void concat(UInt32 u, Int32 radix = 10);

	//! Append an integer64.
    void concat(Int64 i, Int32 radix = 10);

	//! Append an unsigned integer64.
    void concat(UInt64 u, Int32 radix = 10);

	//! Append a float
    void concat(Float f, Int32 decimals = -1);

	//! Append a double
    void concat(Double d, Int32 decimals = -1);

	//! Append a char.
    inline void concat(Char c) { *this += (WChar)static_cast<UInt8>(c); }

	//! Append a char
    inline void concat(const WChar c) { *this += c; }

	//! Append a char* string
    inline void concat(const WChar *str) { *this += str; }

	//! Append another string
	inline void concat(const String& str) { *this += str; }

    //
    // Serialization << operator.
    //

    //! Operator concat a char to the string.
    String& operator<< (const Char &c)
    {
        concat(c);
        return *this;
    }

    //! Operator concat a wide char to the string.
    String& operator<< (const WChar &w)
    {
        concat(w);
        return *this;
    }

    //! Operator concat a base 10 32 bits integer to the string.
    String& operator<< (const Int32 &i)
    {
        concat(i, 10);
        return *this;
    }

    //! Operator concat a base 10 32 bits unsigned integer to the string.
    String& operator<< (const UInt32 &u)
    {
        concat(u, 10);
        return *this;
    }

    //! Operator concat a base 10 64 bits integer to the string.
    String& operator<< (const Int64 &i)
    {
        concat(i, 10);
        return *this;
    }

    //! Operator concat a base 10 64 bits unsigned integer to the string.
    String& operator<< (const UInt64 &u)
    {
        concat(u, 10);
        return *this;
    }

    //! Operator concat a float to the string.
    String& operator<< (const Float &f)
    {
        concat(f);
        return *this;
    }

    //! Operator concat an double to the string.
    String& operator<< (const Double &d)
    {
        concat(d);
        return *this;
    }

    //! Operator concat another string to the string.
    String& operator<< (const String& s)
    {
        concat(s);
        return *this;
    }   

    //
    // Concatenation arg methods using placeholders {0} to {99} from the string constructor.
    //

    //! Set the nth argument of type char.
    String& arg(const Char &c);

    //! Set the nth argument of type wide char.
    String& arg(const WChar &w);

    /**
     * @brief Set the nth argument of type 32 bits integer, with default as base 10.
     * @param i 32 bits integer value
     * @param fieldWidth Width of the value (add some fill char to complete)
     * @param base Integer base conversion (10 by default)
     * @param fillChar Character used for filling
     */
    String& arg(const Int32 &i, Int32 fieldWidth = 0, Int32 base = 10, WChar fillChar = 0);

    /**
     * @brief Set the nth argument of type unsigned 32 bits integer, with default as base 10.
     * @param i 32 bits unsigned integer value
     * @param fieldWidth Width of the value (add some fill char to complete)
     * @param base Integer base conversion (10 by default)
     * @param fillChar Character used for filling
     */
    String& arg(const UInt32 &i, Int32 fieldWidth = 0, Int32 base = 10, WChar fillChar = 0);

    /**
     * @brief Set the nth argument of type 64 bits integer, with default as base 10.
     * @see arg for Int32
     */
    String& arg(const Int64 &i, Int32 fieldWidth = 0, Int32 base = 10, WChar fillChar = 0);

    /**
     * @brief Set the nth argument of type integer, with default as base 10.
     * @see arf for UInt32
     */
    String& arg(const UInt64 &i, Int32 fieldWidth = 0, Int32 base = 10, WChar fillChar = 0);

    //! Set the nth argument of type float.
    String& arg(const Float &f, Int32 decimals = -1, WChar separator = '.');

    //! Set the nth argument of type double.
    String& arg(const Double &d, Int32 decimals = -1, WChar separator = '.');

    //! Set the nth argument of type string.
    String& arg(const String& s, Int32 fieldWidth = 0, WChar fillChar = 0);

    //
    // To <type> conversion.
    //

	//! Convert the value at pos to a char
    Char toChar(Int32 pos = 0) const;

	//! Convert the value at pos to a widechar
    WChar toWChar(Int32 pos = 0) const;

	//! Convert the value at pos to an int
    Int32 toInt32(Int32 pos = 0) const;

	//! Convert the value at pos to an unsigned int
    UInt32 toUInt32(Int32 pos = 0) const;

    //! Convert the value at pos to an long int (int64)
    Int64 toInt64(Int32 pos = 0) const;

    //! Convert the value at pos to an unsigned long int (uint64)
    UInt64 toUInt64(Int32 pos = 0) const;

	//! Convert the value at pos to a float
    Float toFloat(Int32 pos = 0) const;

	//! Convert the value at pos to a double
    Double toDouble(Int32 pos = 0) const;

    //
    // Serialization
    //

    Bool writeToFile(OutStream &os) const;
    Bool writeToFile(OutStream &os);
    Bool readFromFile(InStream &is);

protected :

    WChar *m_data;        //!< String data
    Int32 m_size;        //!< String content size
    Int32 m_maxsize;     //!< String memory allocation size
    Int32 m_threshold;   //!< Reallocation size threshold

    //
    // arg management helpers
    //

    void allocateArg();
    void replaceArg(const WChar *arg, Int32 argSize, const String &by);
};

} // namespace o3d

namespace std {
//namespace tr1 {
template<>
struct O3D_API hash<o3d::CString> {
    enum
    {
        bucket_size = 8,
        min_buckets = 16
    };

    bool operator()(const o3d::CString &s1, const o3d::CString &s2) const
    {
        return memcmp(
                    s1.getData(),
                    s2.getData(),
                    s1.length() < s2.length() ? s1.length() : s2.length()) < 0;
    }

    size_t operator()(const o3d::CString &s1) const
    {
        size_t  h = 0;

        if (s1.isNull()) {
            return 0;
        }

        o3d::Char *p = const_cast<o3d::Char*>(s1.getData());
        o3d::Char zero = '\0';

        while ( *p != zero ) h = 31 * h + (*p++);

        return h;
    }

    /*std::size_t operator()(o3d::CString &c) const
            {
                std::hash<o3d::Char*> hash;
                if (c.isValid())
                    return hash(c.getData());
                else
                    return 0;
            }*/
};

template<>
struct O3D_API hash<o3d::String> {
    enum
    {
        bucket_size = 8,
        min_buckets = 16
    };

    bool operator()(const o3d::String &s1, const o3d::String &s2) const
    {
        return s1 < s2;
    }

    size_t operator()(const o3d::String &s1) const
    {
        size_t  h = 0;

        if (s1.isNull()) {
            return 0;
        }

        wchar_t *p = const_cast<o3d::WChar*>(s1.getData());
        wchar_t zero = L'\0';

        while ( *p != zero ) h = 31 * h + (*p++);

        return h;
    }

    /*std::size_t operator()(const o3d::String &c) const
            {
                //std::hash<o3d::WChar*> hash;
                //if (c.isValid())
                //	return hash(c.getData());
                //else
                    return 0;
            }*/
};
//}
}

#endif // _O3D_STRING_H
