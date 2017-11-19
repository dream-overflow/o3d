/**
 * @file stringtokenizer.h
 * @brief CString and String tokenizers.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2006-10-22
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_STRINGTOKENIZER_H
#define _O3D_STRINGTOKENIZER_H

#include "memorydbg.h"
#include "string.h"
#include "debug.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class CStringTokenizer
//-------------------------------------------------------------------------------------
//! CString (ASCII) tokenizer.
//---------------------------------------------------------------------------------------
class O3D_API CStringTokenizer
{
public:

	//! Creates a new string tokenizer based on the specified string to be tokenized.
	CStringTokenizer(const CString &str);

	//! Creates a new string tokenizer based on the specified string to be tokenized and a set of delimiters.
	//! The default delimiters are the space, tab, newline, and carriage-return characters.
	CStringTokenizer(
		const CString &str,
		const CString &delimiters);

	//! Creates a new string tokenizer based on the specified string to be tokenized and a set of delimiters
	//! and flag that indicates whether delimiters should be returned as tokens.
	CStringTokenizer(
		const CString &str,
		const CString &delimiters,
		Bool DelimiterAsToken);

	//! destructor
	~CStringTokenizer();

	//! returns the number of remaining tokens
	inline Int32 countTokens() { return m_remainingTokens; }

	//! return true if there are more tokens in the string
	inline Bool hasMoreElements()
	{
		if (m_remainingTokens)
			return True;
		else
			return False;
	}

	//! return true if there are more tokens in the string
	inline Bool hasMoreTokens()
	{
		if (m_remainingTokens)
			return True;
		else
			return False;
	}

	//! return the next element position in the string
	inline UInt32 getNextPosition() const { return m_currentPosition; }
	
	//! return the next element in the string
	inline CString nextElement() { return nextToken(); }

	//! return the next element in the string
	CString nextToken();

	//! return the number of delimiters found before a token
	//! (valid only after NextElement call)
	inline Int32 getNbrWhiteToken() const { return m_whiteToken; }

	//! changes the set of delimiters to the specified string and
	//! then returns the next token in the string
	CString nextToken(const CString &delimiters);

private:

	Int32   m_whiteToken;

	Int32	m_currentPosition;   //!< current index on string
	Int32   m_totalTokens;
	Int32	m_remainingTokens;

	Char*   m_listOfDl;          //!< list of delimiters
	Char*   m_workStr;           //!< temp work string
	Char*   m_origStr;           //!< original string passed

	Bool    m_dlFlag;            //!< delimiter flag

	void prepWorkStr(const Char* delimiters = NULL, Int32 delimitersSize = 0);
};

//---------------------------------------------------------------------------------------
//! @class StringTokenizer
//-------------------------------------------------------------------------------------
//! WideChar string tokenizer.
//---------------------------------------------------------------------------------------
class O3D_API StringTokenizer
{
public:

	//! Creates a new string tokenizer based on the specified string to be tokenized.
	StringTokenizer(const String &str);

	//! Creates a new string tokenizer based on the specified string to be tokenized and a set of delimiters.
	//! The default delimiters are the space, tab, newline, and carriage-return characters.
	StringTokenizer(const String &str, const String &delimiters);

	//! Creates a new string tokenizer based on the specified string to be tokenized and a set of delimiters
	//! and flag that indicates whether delimiters should be returned as tokens.
	StringTokenizer(
		const String &str,
		const String &delimiters,
		Bool delimiterAsToken);

	//! destructor
	~StringTokenizer();

	//! returns the number of remaining tokens
	inline Int32 countTokens() { return m_remainingTokens; }

	//! return true if there are more tokens in the string
	inline Bool hasMoreElements()
	{
		if (m_remainingTokens)
			return True;
		else
			return False;
	}

	//! return true if there are more tokens in the string
	inline Bool hasMoreTokens()
	{
		if (m_remainingTokens)
			return True;
		else
			return False;
	}

	//! return the next element position in the string
	inline UInt32 getNextPosition() const { return m_currentPosition; }

	//! return the next element in the string
	inline String nextElement() { return nextToken(); }

	//! return the next element in the string
	String nextToken();

	//! return the number of delimiters found before a token
	//! (valid only after NextElement call)
	inline Int32 getNumWhiteToken() const { return m_whiteToken; }

	//! changes the set of delimiters to the specified string and
	//! then returns the next token in the string
	String nextToken(const String &delimiters);

private:

	Int32   m_whiteToken;

	Int32	m_currentPosition;   //!< current index on string
	Int32   m_totalTokens;
	Int32	m_remainingTokens;

	WChar*   m_listOfDl;         //!< list of delimiters
	WChar*   m_workStr;          //!< temp work string
	WChar*   m_origStr;          //!< original string passed

	Bool    m_dlFlag;            //!< delimiter flag

    void prepWorkStr(const WChar* delimiters = nullptr, Int32 delimiterSize = 0);
};

} // namespace o3d

#endif // _O3D_STRINGTOKENIZER_H
