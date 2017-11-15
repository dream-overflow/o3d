/**
 * @file xmldoc.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/xmldoc.h"
#include "o3d/core/filemanager.h"
#include "o3d/core/debug.h"

using namespace o3d;

// constructor
XmlDoc::XmlDoc(const String &docName) :
    m_doc(nullptr),
    m_curNode(nullptr)
{
	m_doc = new TiXmlDocument(docName.toUtf8().getData());
}

// destructor
XmlDoc::~XmlDoc()
{
	deletePtr(m_doc);
}

// parse the file. Condense the white spaces is default
void XmlDoc::readData(const Char* data, Bool condense)
{
	if (!data)
		O3D_ERROR(E_InvalidParameter("Data is null"));

	if (!m_doc)
		m_doc = new TiXmlDocument("from data document");

	m_doc->SetCondenseWhiteSpace(condense);

	if (!m_doc->Parse(data, 0, TIXML_DEFAULT_ENCODING/*TIXML_ENCODING_UTF8*/))
		O3D_ERROR(E_InvalidResult("Unable to parse the document"));
}

void XmlDoc::read(const String &filename, Bool condense)
{
	String fullFileName = FileManager::instance()->getFullFileName(filename);

	if (!m_doc)
        m_doc = new TiXmlDocument("mydoc");

	m_doc->SetCondenseWhiteSpace(condense);

    InStream *is = FileManager::instance()->openInStream(fullFileName);

    Int32 len = is->getAvailable();

    Char *pData = new Char[len * sizeof(Char) + 1];

    // read the data and close the file
    is->read(pData, len);
    pData[len * sizeof(Char)] = 0; // terminal zero

    deletePtr(is);

    if (m_doc->Parse(pData,0,TIXML_DEFAULT_ENCODING/*TIXML_ENCODING_UTF8*/))
    {
        deleteArray(pData);
    }
    else
    {
        deleteArray(pData);
        O3D_ERROR(E_InvalidResult(fullFileName + String(" ") + String(m_doc->ErrorDesc())));
    }
}

void XmlDoc::read(InStream &is, Bool condense)
{
	if (!m_doc)
        m_doc = new TiXmlDocument("mydoc");

	m_doc->SetCondenseWhiteSpace(condense);

    Int32 len = is.getAvailable();

    Char *data = new Char[len * sizeof(Char) + 1];

    // read the data
    is.read(data, len * sizeof(Char));
    data[len * sizeof(Char)] = 0; // terminal zero

    m_doc->Parse(data, 0, TIXML_DEFAULT_ENCODING);//TIXML_ENCODING_UTF8);
    deleteArray(data);

    if (m_doc->Error())
        O3D_ERROR(E_InvalidResult(m_doc->ErrorDesc()));
}

// write the file
void XmlDoc::write(const String &filename, Bool condense)
{
	String fullFileName = FileManager::instance()->getFullFileName(filename);

	if (!m_doc)
		O3D_ERROR(E_InvalidPrecondition("document is not opened"));

	m_doc->SetCondenseWhiteSpace(condense);

    if (!m_doc->SaveFile(fullFileName.toUtf8().getData()))
        O3D_ERROR(E_InvalidResult(fullFileName + String(" ") + String(m_doc->ErrorDesc())));
}

void XmlDoc::write(OutStream &os, Bool condense)
{
	if (!m_doc)
		O3D_ERROR(E_InvalidPrecondition("Document is not opened"));

	m_doc->SetCondenseWhiteSpace(condense);

    TiXmlPrinter printer;
    m_doc->Accept(&printer);
    m_doc->Accept(nullptr);

    os.write(printer.CStr(), printer.Size());

    if (m_doc->Error())
        O3D_ERROR(E_InvalidResult(m_doc->ErrorDesc()));
}

// destroy the pDoc data (all parsed data)
void XmlDoc::destroy()
{
	deletePtr(m_doc);
}

