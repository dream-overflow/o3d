/**
 * @file i18n.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/gui/i18n.h"
#include "o3d/core/filemanager.h"

using namespace o3d;

I18n::I18n() :
	m_currentLang(DEFAULT_LANG)
{
	// default add message box button text for EN.
	m_strings["msgBoxOk"] = "Ok";
	m_strings["msgBoxRetry"] = "Retry";
	m_strings["msgBoxCancel"] = "Cancel";
	m_strings["msgBoxNo"] = "No";
	m_strings["msgBoxYes"] = "Yes";
}

I18n::~I18n()
{
}

void I18n::setLang(I18n::Languages lang)
{
	if (m_currentLang != lang)
	{
		// reload each folder
		for (String &folder : m_folders)
		{
			loadLang(folder, lang);
		}

		m_currentLang = lang;
	}
}

void I18n::addFolder(const String &path)
{
	m_folders.push_back(path);
	loadLang(path, m_currentLang);
}

String I18n::get(const String &key) const
{
	StringMap<String>::CIT cit = m_strings.find(key);
	if (cit != m_strings.end())
		return cit->second;
	else
		return key;
}

String I18n::get(const String &key, Int32 _1) const
{
    String data;
    String res;

    StringMap<String>::CIT cit = m_strings.find(key);
    if (cit != m_strings.end())
        data = cit->second;
    else
        return key;

    String val1 = String::print("%i", _1);
    Bool plural = o3d::abs(_1) > 1;

    Int32 p = data.sub("{}", 0);
    if (p >= 0)
        res = data.sub(0, p) + val1 + data.sub(p+2, -1);
    else
        return key;

    p = 0;
    while ((p = res.sub("{", p)) != -1)
    {
        Int32 p2 = res.sub("}", p+2);
        if (p2 > p)
        {
            if (plural)
            {
                // set the value between {}
                res.remove(p, 1);
                res.remove(p2-1, 1);
            }
            else
            {
                // remove the value between {} and the {}
                res.remove(p, p2-p+1);
            }
        }
    }

    return res;
}

String I18n::get(const String &key, const String &_1) const
{
	String data;
	String res;

	StringMap<String>::CIT cit = m_strings.find(key);
	if (cit != m_strings.end())
		data = cit->second;
	else
		return key;

	Int32 p = data.sub("{}", 0);
	if (p >= 0)
		return data.sub(0, p) + _1 + data.sub(p+2, -1);
	else
		return key;
}

String I18n::get(const String &key, const String &_1, const String &_2) const
{
	String data;
	String res;

	StringMap<String>::CIT cit = m_strings.find(key);
	if (cit != m_strings.end())
		data = cit->second;
	else
		return key;

	Int32 p1 = data.sub("{}", 0);
	Int32 p2 = data.sub("{}", p1+2);

	if (p1 >= 0)
		res = data.sub(0, p1) + _1 + data.sub(p1+2, p2);

	if (p2 >= 0)
		res += _2 + data.sub(p2+2, -1);

	return res;
}

String I18n::get(const String &key, const String &_1, const String &_2, const String &_3) const
{
	String data;
	String res;

	StringMap<String>::CIT cit = m_strings.find(key);
	if (cit != m_strings.end())
		data = cit->second;
	else
		return key;

	Int32 p1 = data.sub("{}", 0);
	Int32 p2 = data.sub("{}", p1+2);
	Int32 p3 = data.sub("{}", p2+2);

	if (p1 >= 0)
		res = data.sub(0, p1) + _1 + data.sub(p1+2, p2);

	if (p2 >= 0)
		res += _2 + data.sub(p2+2, p3);

	if (p3 >= 0)
		res += _3 + data.sub(p3+2, -1);

	return res;
}

void I18n::loadLang(const String &path, I18n::Languages lang)
{
	String filename = "en_US.lang";

	switch (lang) {
		case LANG_EN_US:
//		case LANG_EN_GB:
//		case LANG_EN_CA:
			filename = "en_US.lang";
			break;
		case LANG_FR_FR:
//		case LANG_FR_CA:
			filename = "fr_FR.lang";
			break;
		case LANG_ES_ES:
//		case LANG_ES_AR:
//		case LANG_ES_CO:
//		case LANG_ES_MX:
			filename = "es_ES.lang";
			break;
		case LANG_IT_IT:
			filename = "it_IT.lang";
			break;
		case LANG_GR_GR:
			filename = "gr_GR.lang";
			break;
		case LANG_RU_RU:
			filename = "ru_RU.lang";
			break;
		default:
			filename = "en_US.lang";
			break;
	}

    InStream *is = FileManager::instance()->openInStream(path + "/" + filename);
	String line;
	Int32 equalPos;
	String key, value;

    while (is->readLine(line) != -1)
	{
		line.trimLeftChars(" \t");
		line.trimRightChars(" \t");

		// comment or empty line
		if (line.startsWith("#") || line.isEmpty())
			continue;

		// find the first = and split at
		equalPos = line.find('=');
		if (equalPos > 0)
		{
			key = line.sub(0, equalPos);
			key.trimRight(' ');

            value = line.sub(equalPos+1, -1);
            value.trimLeft(' ');

            m_strings[key] = value;
		}
	}

    deletePtr(is);
}

