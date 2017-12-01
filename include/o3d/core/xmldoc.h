/**
 * @file xmldoc.h
 * @brief XML file parsing wrapper of TinyXML
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-09-14
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_PARSEXML_H
#define _O3D_PARSEXML_H

#include <tinyxml/tinyxml.h>
#include "memorydbg.h"
#include "file.h"

namespace o3d {

//! convert TiXmlText* to String
inline String tinyString(const TiXmlElement *str)
{
    return str != nullptr ? str->Value() : String("");
}

//! convert TiXmlText* to String
inline String tinyString(const TiXmlNode *str)
{
    return str != nullptr ? str->Value() : String("");
}

//---------------------------------------------------------------------------------------
//! @class XmlDoc
//-------------------------------------------------------------------------------------
//! XML file parsing wrapper of TinyXML
//---------------------------------------------------------------------------------------
class O3D_API XmlDoc
{
public:

	//! Default constructor.
	XmlDoc(const String &docName);

	//! Virtual destructor.
	virtual ~XmlDoc();

	//! Parse the data buffer. Condense the white spaces is default.
	void readData(const Char *data, Bool condense = True);
	//! Parse the file. Condense the white spaces is default.
	void read(const String &filename, Bool condense = True);
	//! Parse an open file. Condense the white spaces is default.
    void read(InStream &is, Bool condense = True);

	//! Write the file, the write can't be accomplish on an O3DMemFile.
	void write(const String &filename, Bool condense = True);
	//! Write the file, the write can't be accomplish on an O3DMemFile.
	//! Then can't be wrote in a ZIP archive
    void write(OutStream &os, Bool condense = True);

	//! Destroy the TiXmlDocument.
	void destroy();

	//! Get the TiXmlDocument (read only).
	inline const TiXmlDocument* getDoc() const { return m_doc; }
	//! Get the TiXmlDocument.
	inline TiXmlDocument* getDoc() { return m_doc; }

	//! Is document exists.
    inline Bool isValid() const { return m_doc != nullptr; }

	//-----------------------------------------------------------------------------------
	// Element methods
	//-----------------------------------------------------------------------------------

	//! get the first child element with its name element
	inline TiXmlElement* firstChildElement(const String &element)
	{
        O3D_ASSERT(m_doc!=nullptr);
		return m_doc->FirstChildElement(element.toUtf8().getData());
	}

	//! get the first child element
	inline TiXmlElement* firstChildElement()
	{
        O3D_ASSERT(m_doc!=nullptr);
		return m_doc->FirstChildElement();
	}

    //! get the next child element to pPrevNode and element name (null return the first)
	inline TiXmlElement* childElement(const String &element, TiXmlNode *pPrevNode)
	{
        O3D_ASSERT(m_doc!=nullptr);
		return m_doc->IterateChildren(element.toUtf8().getData(), pPrevNode)->ToElement();
	}
    //! get the next child element to pPrevNode (null return the first)
	inline TiXmlElement* childElement(TiXmlNode *pPrevNode)
	{
        O3D_ASSERT(m_doc!=nullptr);
		return m_doc->IterateChildren(pPrevNode)->ToElement();
	}

	//-----------------------------------------------------------------------------------
	// Attribute methods
	//-----------------------------------------------------------------------------------

	//! node element Attribute bool
	inline Bool getEltAttBool(TiXmlNode *pNode, const String &Attribute)
	{
		Int32 tmp;
		pNode->ToElement()->Attribute(Attribute.toUtf8().getData(), &tmp);

		if (tmp)
			return True;
		else
			return False;
	}

	//! element Attribute bool
	inline Bool getEltAttBool(TiXmlElement *pElt, const String &Attribute)
	{
		Int32 tmp;
		pElt->Attribute(Attribute.toUtf8().getData(), &tmp);

		if (tmp)
			return True;
		else
			return False;
	}

	//! node element Attribute float
	inline Float getEltAttFloat(TiXmlNode *pNode, const String &Attribute)
	{
		Double tmp;
		pNode->ToElement()->Attribute(Attribute.toUtf8().getData(), &tmp);

		return (Float)tmp;
	}
	//! element Attribute float
	inline Float getEltAttFloat(TiXmlElement *pElt, const String &Attribute)
	{
		Double tmp;
		pElt->Attribute(Attribute.toUtf8().getData(), &tmp);

		return (Float)tmp;
	}

	//! node element Attribute O3DString
	inline void getEltAttStr(TiXmlNode *pNode, const String &Attribute, String &str)
	{
		const Char *tmp = pNode->ToElement()->Attribute(Attribute.toUtf8().getData());
		if (tmp)
			str.fromUtf8(tmp);
	}

	//! element Attribute O3DString
	inline void getEltAttStr(TiXmlElement *pElt, const String &Attribute, String &str)
	{
		const Char *tmp = pElt->Attribute(Attribute.toUtf8().getData());
		if (tmp)
			str.fromUtf8(tmp);
	}

	//-----------------------------------------------------------------------------------
	// Nodes methods
	//-----------------------------------------------------------------------------------

	//! set/get current node
	inline void setCurrentNode(TiXmlNode *pCurNode) { m_curNode = pCurNode; }
	inline TiXmlNode* getCurrentNode() { return m_curNode; }

	//! set the current node to root to its first child
	inline void setCurNodeRoot()
	{
        O3D_ASSERT(m_doc!=nullptr);
		m_curNode = m_doc->FirstChild();
	}

	//! get the first child with its name element
	inline TiXmlNode* firstChild(const String &value)
	{
        O3D_ASSERT(m_curNode!=nullptr);
		return m_curNode->FirstChild(value.toUtf8().getData());
	}

	//! get the first child
	inline TiXmlNode* firstChild()
	{
        O3D_ASSERT(m_curNode!=nullptr);
		return m_curNode->FirstChild();
	}

    //! get the next child to pPrevNode and element name (nullptr return the first)
	inline TiXmlNode* child(TiXmlNode* pPrevNode, const String &value)
	{
        O3D_ASSERT(m_curNode!=nullptr);
		return m_curNode->IterateChildren(value.toUtf8().getData(), pPrevNode);
	}

    //! get the next child to pPrevNode (nullptr return the first)
	inline TiXmlNode* child(TiXmlNode* pPrevNode)
	{
        O3D_ASSERT(m_curNode!=nullptr);
		return m_curNode->IterateChildren(pPrevNode);
	}

protected:

	TiXmlDocument *m_doc;    //!< the xml document.
	TiXmlNode *m_curNode;    //!< the current working node (default is root).
};

} // namespace o3d

#endif // _O3D_PARSEXML_H

